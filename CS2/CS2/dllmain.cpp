#include "define.h"
#include "dx11hk.h"
#include "gui.h"
#include "other.h"
#include "tool.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	g_dllbase = hModule;
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		NewThread(MainThread);
	}
	return TRUE;
}