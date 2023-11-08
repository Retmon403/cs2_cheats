#pragma once
#include "MinHook/MinHook.h"

#ifndef _WIN64
#pragma comment(lib,"MinHook\\libMinHook.x86.lib")
#else
#pragma comment(lib,"MinHook\\libMinHook.x64.lib")
#endif

class MiniHook
{
public:
	bool SetHook(LPVOID pTarget, LPVOID hkFunc, LPVOID* oFunc);
	bool UnHook(LPVOID pTarget);
	void FreeHook();

private:
	bool status;
	bool InitHook();
};

extern MiniHook minihook;