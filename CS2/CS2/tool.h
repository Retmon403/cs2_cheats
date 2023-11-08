#pragma once
#include "define.h"


typedef PVOID POBJECT_ATTRIBUTES;
typedef NTSTATUS(NTAPI* _NtCreateThreadEx)(
	PHANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	HANDLE ProcessHandle,
	PVOID ThreadProcedure,
	PVOID ParameterData,
	ULONG CreateSuspended,
	SIZE_T StackZeroBits,
	SIZE_T SizeOfStackCommit,
	SIZE_T SizeOfStackReserve,
	PVOID BytesBuffer);

typedef NTSTATUS(*_NtReadVirtualMemory)(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	OUT PVOID               Buffer,
	IN SIZE_T                NumberOfBytesToRead,
	OUT PSIZE_T              NumberOfBytesReaded OPTIONAL);

typedef NTSTATUS(*_NtWriteVirtualMemory)(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	OUT PVOID               Buffer,
	IN SIZE_T                NumberOfBytesToRead,
	OUT PSIZE_T              NumberOfBytesReaded OPTIONAL);


#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

void get_rand_str(char s[], int num);
int get_rand_num(int minimum, int max);
DWORD GetProcessIdByName(const wchar_t* ProcessName);
HWND GetHwndByPid(DWORD dwProcessID);

void SetConsoleStatus(bool iscreate);
bool WindowsIsActivat(HWND hwnd);

bool NewThread(PVOID func, LPVOID p = nullptr);;

inline bool ReadGameMemory(LPVOID ptr, LPVOID buf, DWORD len) {
	__try {
		RtlCopyMemory(buf, ptr, len);
		return true;
	}
	__except (1) {
		return false;
	}

}
inline bool WriteGameMemory(LPVOID ptr, LPVOID buf, DWORD len) {
	__try {
		RtlCopyMemory(ptr, buf, len);
		return true;
	}
	__except (1) {
		return false;
	}
}

PUCHAR SearchHex(PVOID start_addr, SIZE_T size, PUCHAR search_code, SIZE_T search_size);
std::uint8_t* pattern_scan(HMODULE modulebase, const char* signature);
ULONG64 getrelative_addr(ULONG64 code_addr, ULONG offset, ULONG code_size);

PVOID getjump_addr(PVOID addr);