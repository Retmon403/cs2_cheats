#include "MyHook.h"

MiniHook minihook;

bool MiniHook::SetHook(LPVOID pTarget, LPVOID hkFunc, LPVOID* oFunc) {
	if (!this->InitHook()) {
		return false;
	}
	//尝试创建hook
	MH_STATUS status = MH_CreateHook(pTarget, hkFunc, oFunc);
	if (status == MH_ERROR_ALREADY_CREATED) {
		//如果已经创建 则启用它
		return (MH_EnableHook(pTarget) == MH_OK);
	}
	else if (status != MH_OK) {
		//如果也不等于ok 就失败
		return false;
	}
	if (MH_EnableHook(pTarget) != MH_OK) {
		return false;
	}
	return true;
}

bool MiniHook::UnHook(LPVOID pTarget) {
	return (MH_DisableHook(pTarget) == MH_OK);
}

void MiniHook::FreeHook() {
	if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) {
		return;
	}
	MH_Uninitialize();
}

bool MiniHook::InitHook() {
	if (!this->status) {
		this->status = (MH_Initialize() == MH_OK);
	}
	return this->status;
}