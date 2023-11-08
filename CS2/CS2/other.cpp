#include "other.h"
#include "gui.h"
#include "hacker.h"
#include "tool.h"

int g_autofire_thread_status = 0;
int g_jump_thread_status = 0;



DWORD AutoJumpThread(LPVOID lp) {
	//最基本的连跳解决方案 不会被vac
	g_jump_thread_status = STATUS_RUN;
	while (g_jump_thread_status != STATUS_STOP) {
		if (gui.auto_jump && (gui.jump_mode == MEM_JUMP)) {
			UCHAR status = 0;
			if (GetAsyncKeyState(VK_CAPITAL) & 0x8000) {
				if (Controller::GetJumpStatus(&status)) {
					if (status & 1) {
						Sleep(5);
						Controller::SetJumpFlags(65537);
						Sleep(30);
						Controller::SetJumpFlags(256);
					}
				}
			}
		}
		Sleep(gui.auto_jump ? 5 : 100);
	}
	g_jump_thread_status = STATUS_EXIT;
	return 0;
}

DWORD AutoFireThread(LPVOID lp) {
	g_autofire_thread_status = STATUS_RUN;
	while (g_autofire_thread_status != STATUS_STOP) {
		if (gui.auto_fire) {
			//如果窗口被激活
			if (WindowsIsActivat(game.Hwnd)) {
				void AutoFire();
				AutoFire();
			}
		}
		Sleep(gui.auto_fire ? 1 : 100);
	}
	g_autofire_thread_status = STATUS_EXIT;
	return 0;
}

typedef struct {
	DWORD64 v1;
	DWORD64 v2;
}R9;

typedef void (*func)(PCHAR mem, Point3D& start, Point3D& end, R9* r9,DWORD64 v5, DWORD64 v6);


bool VisibilityCheck(Client& self,PDWORD64 result_obj) {
	//检查当前瞄准的对象
	DWORD tmp = 0;
	bool status = ReadGameMemory((LPVOID)(self.player.Address + C_CSPlayerPawnBase::m_iIDEntIndex), &tmp, sizeof(tmp));
	if (!status || tmp == -1) {
		return false;
	}
	DWORD64 ListEntry = 0;
	ReadGameMemory((LPVOID)(game.EntityList + 0x8 * (tmp >> 9) + 0x10), &ListEntry, sizeof(ListEntry));
	if (ListEntry == 0) {
		return false;
	}
	DWORD64 PawnAddress = 0;
	ReadGameMemory((LPVOID)(ListEntry + 0x78 * (tmp & 0x1FF)), &PawnAddress, sizeof(PawnAddress));
	if (!PawnAddress) {
		return false;
	}
	Player player;
	if (!player.UpdatePlayer(PawnAddress) || !player.alive) {
		return false;
	}
	if ((self.player.TeamID == player.TeamID) && gui.auto_fire_no_shot_firend) {
		return false;
	}
	*result_obj = PawnAddress;
	return true;
}

std::mutex g_mutex_autofire;
DWORD64 g_is_autofire_obj = 0;

void AutoFire() {
	Client self;
	self.player.UpdatePlayer(self.local.UpdateController(game.GetLocalPlayerController()));
	if (self.player.fov == 90 && (strcmp("awp", gui.self.player.WeaponName) == 0 || strcmp("ssg08", gui.self.player.WeaponName) == 0)) {
		return;
	}
	DWORD64 obj = 0;
	//这里写的一坨屎 但大概就是 静默自瞄和普通自瞄用的不同方案 
	if (VisibilityCheck(self, &obj)) {
		if (gui.aim_mode == MEM_AIM) {
			Sleep(gui.fire_uptime);
			Controller::SetAttackStatus(true);
			Sleep(gui.fire_shot_time);
			if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
				Controller::SetAttackStatus(false);
			}
			Sleep(gui.fire_nexttime);
		}
		else if(obj && gui.aim_mode == SILENT_AIM){
			Sleep(gui.fire_uptime);

			g_mutex_autofire.lock();
			//静默自瞄会用过锁把 玩家对象给到createmove线程里 
			g_is_autofire_obj = obj;
			g_mutex_autofire.unlock();

			Sleep(gui.fire_shot_time);

			g_mutex_autofire.lock();
			g_is_autofire_obj = 0;
			g_mutex_autofire.unlock();

			Sleep(gui.fire_nexttime);
		}
	}
}



bool CheckAllThreadIsExit() {
	return ((g_autofire_thread_status == STATUS_EXIT) && (g_jump_thread_status == STATUS_EXIT)) ? true : false;
}