#include "tool.hpp"
#include "VMP/VMProtectSDK.h"

#ifndef _WIN64
#pragma comment(lib,"VMP\\VMProtectSDK32.lib")
#else
#pragma comment(lib,"VMP\\VMProtectSDK64.lib")
#endif


#pragma warning(disable : 4996)

void error(char const* const _Format, ...) {
	printf(_Format);
	Sleep(1500);
	exit(0);
}

int main() {
	//VMProtectBegin("main");
	DWORD pid = GetProcessIdByName(L"cs2.exe");
	if (!pid) {
		error("process not run...\n");
	}
	CHAR path[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, path);
	strcat_s(path, "\\CS2.dll");
	if (_access(path, 0) == -1) {
		error("the dll does not exist...\n");
	}
	

	DWORD fsize = 0;
	PVOID pdata = ReadFileToBit(path, &fsize);
	if (pdata && fsize) {
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if (!hProc) {
			free(pdata);
			error("openprocess failed...\n");
		}
		//kpmap大佬开源的内存注入，shellcode拉伸dll修复重定向，可以使用seh异常.
		bool status = ManualMapDll(hProc, (BYTE*)pdata, (SIZE_T)fsize);
		CloseHandle(hProc);
		free(pdata);
		error(status ? "successful...\n" : "injector failed...\n");
	}
	error("read file failed...");
	//VMProtectEnd();
}