#pragma once
#include "define.h"


enum THREAD_STATUS {
	STATUS_STOP,
	STATUS_RUN,
	STATUS_EXIT
};

DWORD AutoJumpThread(LPVOID lp);
bool CheckAllThreadIsExit();
DWORD AutoFireThread(LPVOID lp);
extern int g_jump_thread_status;
extern int g_autofire_thread_status;

extern std::mutex g_mutex_autofire;
extern DWORD64 g_is_autofire_obj;