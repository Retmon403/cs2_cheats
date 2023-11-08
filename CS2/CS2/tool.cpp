#include "tool.h"
#include <TlHelp32.h>

void get_rand_str(char s[], int num) {
	//定义随机生成字符串表
	char str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	size_t i, lstr;
	char ss[2] = { 0 };
	lstr = strlen(str);//计算字符串长度
	srand((unsigned int)time((time_t*)NULL));//使用系统时间来初始化随机数发生器
	for (i = 1; i <= num; i++) {//按指定大小返回相应的字符串
		sprintf(ss, "%c", str[(rand() % lstr)]);//rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
		strcat_s(s, num, ss);//将随机生成的字符串连接到指定数组后面
	}
}

int get_rand_num(int minimum, int max) {
	return (rand() % (max - minimum + 1) + minimum);
}

DWORD GetProcessIdByName(const wchar_t* ProcessName) {
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		return 0;
	}
	PROCESSENTRY32 info = { 0 };
	info.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snap, &info)) {
		CloseHandle(snap);
		return 0;
	}
	do
	{
		if (!wcscmp(ProcessName, info.szExeFile)) {
			CloseHandle(snap);
			return info.th32ProcessID;
		}
	} while (Process32Next(snap, &info));
	CloseHandle(snap);
	return 0;
}

HWND GetHwndByPid(DWORD dwProcessID) {
	HWND h = GetTopWindow(0);
	HWND retHwnd = NULL;
	while (h)
	{
		DWORD pid = 0;
		if (GetWindowThreadProcessId(h, &pid) != 0) {
			if (pid == dwProcessID && GetParent(h) == NULL && ::IsWindowVisible(h)) {
				retHwnd = h;    //会有多个相等值
			}
		}
		h = GetNextWindow(h, GW_HWNDNEXT);
	}
	return retHwnd;
}

void SetConsoleStatus(bool iscreate) {
	static HWND debug_window = 0;
	if (!iscreate) {
		if (!debug_window) {
			return;
		}
		FreeConsole();
		EnableMenuItem(GetSystemMenu(debug_window, false), SC_CLOSE, MF_ENABLED | MF_BYCOMMAND);
		SendMessage(debug_window, WM_CLOSE, 0, 0);
		return;
	}
	FILE* file = nullptr;
	AllocConsole();
	freopen_s(&file, "CON", "r", stdin);
	freopen_s(&file, "CON", "w", stdout);
	SetConsoleTitleA("debug window");
	debug_window = GetConsoleWindow();
	EnableMenuItem(GetSystemMenu(debug_window, false), SC_CLOSE, MF_GRAYED | MF_BYCOMMAND);
}

bool WindowsIsActivat(HWND hwnd) {
	WINDOWINFO info = { 0 };
	return GetWindowInfo(hwnd, &info) && info.dwWindowStatus == 1;
}

//如你所见 调用敏感的api使用的是nt函数 其实我可以自己syscall进内核 但需要得到ssdt的调用号 每个版本的汇编也不一样 但我又懒得去动态解析ntdll的pe文件
//就直接调用nt函数将就用吧
bool NewThread(PVOID func, LPVOID p) {
	static _NtCreateThreadEx NtCreateThreadEx = nullptr;
	if (!NtCreateThreadEx) {
		
		NtCreateThreadEx = (_NtCreateThreadEx)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
		if (!NtCreateThreadEx) {
			return false;
		}
	}
	HANDLE h = 0;
	NTSTATUS status = NtCreateThreadEx(&h, THREAD_ALL_ACCESS, NULL, GetCurrentProcess(),
		(LPTHREAD_START_ROUTINE)func, p, 0, 0, 0x10000, 0x100000, NULL);
	if (NT_SUCCESS(status)) {
		CloseHandle(h);
		return true;
	}
	else {
		return false;
	}
}





	/*static _NtReadVirtualMemory NtReadVirtualMemory = nullptr;
	if (!NtReadVirtualMemory) {
		NtReadVirtualMemory = (_NtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
		if (!NtReadVirtualMemory) {
			return false;
		}
	}
	SIZE_T tmp = 0;
	return NT_SUCCESS(NtReadVirtualMemory(GetCurrentProcess(), ptr, buf, len, &tmp));*/



	/*static _NtWriteVirtualMemory NtWriteVirtualMemory = nullptr;
	if (!NtWriteVirtualMemory) {
		NtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");
		if (!NtWriteVirtualMemory) {
			return false;
		}
	}
	SIZE_T tmp = 0;
	return NT_SUCCESS(NtWriteVirtualMemory(GetCurrentProcess(), ptr, buf, len, &tmp));*/



PUCHAR SearchHex(PVOID start_addr, SIZE_T size, PUCHAR search_code, SIZE_T search_size) {
	PUCHAR p = (PUCHAR)start_addr;
	for (size_t i = 0; i < size; i++) {
		ULONG equal = 0;
		for (size_t u = 0; u < search_size; u++) {
			if (search_code[u] == (UCHAR)0) {
				equal++;
				continue;
			}
			if (*(PUCHAR)(p + u) != search_code[u]) {
				break;
			}
			equal++;
		}
		if (equal == search_size) {
			return p;
		}
		p++;
	}
	return NULL;
}

static std::vector<std::uint32_t> pattern_to_byte(const char* pattern) {
	std::vector<std::uint32_t> bytes;
	char* start = const_cast<char*>(pattern);
	char* end = const_cast<char*>(pattern) + std::strlen(pattern);

	for (char* current = start; current < end; current++) {
		if (*current == '?') {
			current++;
			if (*current == '?') {
				current++;
			}
			bytes.push_back(-1);
		}
		else {
			bytes.push_back(std::strtoul(current, &current, 16));
		}
	}
	return bytes;
}

std::uint8_t* pattern_scan(HMODULE modulebase, const char* signature) {
	if (!modulebase) {
		return nullptr;
	}

	PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(modulebase);
	PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(modulebase) + dos_header->e_lfanew);

	std::size_t size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	std::vector<std::uint32_t> pattern_bytes = pattern_to_byte(signature);
	std::uint8_t* image_base = reinterpret_cast<std::uint8_t*>(modulebase);

	std::size_t pattern_size = pattern_bytes.size();
	std::uint32_t* array_of_bytes = pattern_bytes.data();

	for (std::size_t i = 0; i < size_of_image - pattern_size; i++) {
		bool found = true;

		for (std::size_t j = 0; j < pattern_size; j++) {
			if (image_base[i + j] != array_of_bytes[j] && array_of_bytes[j] != -1) {
				found = false;
				break;
			}
		}

		if (found) {
			return &image_base[i];
		}
	}

	return nullptr;
}

ULONG64 getrelative_addr(ULONG64 code_addr, ULONG offset, ULONG code_size) {
	if (!code_addr) {
		return 0;
	}
	//mov rax, [client.dll + 1902348] { (7FFA3E7B9100) }
	//引用地址=偏移+指令地址+指令长度
	LONG offs = *(PULONG)(code_addr + offset);
	return (code_addr + offs + code_size);
}

PVOID getjump_addr(PVOID addr) {
	//获取e9断跳或ff25长跳的目标地址 暂不支持带[]取地址符
	ULONG64 result = 0;
	__try {
		if (*(PUCHAR)addr == 0xe9) {
			LONG offset = 0;
			offset = *(PLONG)((ULONG64)addr + 1);
			result = offset + (ULONG64)addr + 5;
			return (PVOID)result;
		}
		if ((*(PUSHORT)addr == 0x25ff) && (*(PULONG)((ULONG64)addr + 2) == 0)) {
			return (PVOID)(*(PULONG64)((ULONG64)addr + 6));
		}
		return nullptr;
	}
	__except (1) {
		return nullptr;
	}
}