#include "hook.h"
#include "class_offset.h"
#include "hacker.h"
#include "gui.h"
#include "player.h"
#include "MyHook.h"
#include "tool.h"
#include "other.h"

InterFaces interfaces;

namespace ClassDataOffset {
	int m_nValue = 0x4530;
	int m_fCameraInThirdPerson = 0x43f1;

}

void SilentAim(c_user_cmd* cmd, bool shotfire) {
    Client self;
	if (!self.player.UpdatePlayer(self.local.UpdateController(game.GetLocalPlayerController()))) {
		return;
	}
	Player aim_obj;
	float up_distance = 0.f;
	float up_disfront = 0.f;
	for (size_t i = 0; i < 64; i++) {

		Client object;
		DWORD64 EntityAddress = 0;
		ReadGameMemory((LPVOID)(game.GetEntityList() + 0x10), &EntityAddress, sizeof(EntityAddress));
		if (!ReadGameMemory((LPVOID)(EntityAddress + (i + 1) * 0x78), &EntityAddress, sizeof(EntityAddress))) {
			continue;
		}
		if (EntityAddress == game.GetLocalPlayerController()) {
			continue;
		}
		if (!object.player.UpdatePlayer(object.local.UpdateController(EntityAddress))) {
			continue;
		}
		if (!object.player.alive) {
			continue;
		}
		if (object.player.Pos.x == 0.f && object.player.Pos.y == 0.f) {
			continue;
		}
		Point2D fake = { 0 };
		if (!WorldToScreen(object.player.Pos, fake) || fake.x == 0.f || fake.y == 0.f) {
			continue;
		}
		if (!gui.aim_friend && self.player.TeamID == object.player.TeamID) {
			//如果瞄准队友没开 且这个对象是队友 就continue;
			continue;
		}
		Point2D bons_2d;
		WorldToScreen(object.player.BonsPos[gui.aim_pos], bons_2d);
		float disfront = calc2DDistance(bons_2d, { (float)(game.rect.right - game.rect.left) / 2.f,(float)(game.rect.bottom - game.rect.top) / 2.f });
		float distance = CalcPoint3DDistance(self.player.Pos, object.player.Pos);


		//如果在瞄准范围内
		if (disfront < gui.aim_scale_size) {

			if (!shotfire) {
				if (!up_distance) {
					up_distance = distance;
					aim_obj = object.player;
				}
				else if (distance < up_distance) {
					up_distance = distance;
					aim_obj = object.player;
				}
			}
			else {
				if (!up_disfront) {
					up_disfront = disfront;
					aim_obj = object.player;
				}
				else if (disfront < up_disfront) {
					up_disfront = disfront;
					aim_obj = object.player;
				}
			}

			
		}
		
	}
	if (aim_obj.Address) {
		self.player.UpdatePlayer(self.player.Address);
		aim_obj.UpdatePlayer(aim_obj.Address);
		Point2D aim_angle;
		get_aimbot((float*)&self.player.BonsPos[6], (float*)&aim_obj.BonsPos[gui.aim_pos], (float*)&aim_angle);
		if (gui.aim_rcs && self.player.ShotsFired > 1) {
			Point2D rcs = { 0 };
			if (Controller::GetShotFireRCS(self.player.Address, rcs)) {
				aim_angle.x -= rcs.x * 2.f;
				aim_angle.y -= rcs.y * 2.f;
			}
		}
		
		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || shotfire) {
			//如果此时有开枪动作，直接开枪
			cmd->buttons |= BUTTONS::in_attack;
		}
		cmd->set_sub_tick_angles(aim_angle);
		//printf("%.2f,%.2f\n", aim_angle.x, aim_angle.y);
		
	}
}



namespace CInputHook{
	DWORD64 hkCreateMove(DWORD64 a1, int a2, int a3) {
		return 0;
	}


	//如你所见 我没有直接hook createmove这个函数 因为在他的函数下第4个call 里面计算出了一半cmd缓冲区了 
	DWORD64 hkGetUserCmd(DWORD64 a1, DWORD64 a2, DWORD64 a3) {

		c_user_cmd* cmd = (c_user_cmd*)(0x4350 * a2 + 0x250 + 0x70 * (a3 % 0x96) + a1);
		DWORD64 result = interfaces.cinput.o_GetUserCmd(a1, a2, a3);
		//printf("%p,%p\n", cmd, result);
		if (!cmd || !cmd->base) {
			return result;
		}

		if (gui.auto_jump && (gui.jump_mode == MSG_JUMP)) {
			UCHAR jump = 0;
			ReadGameMemory((LPVOID)(game.GetLocalPlayerPawn() + C_BaseEntity::m_fFlags), &jump, sizeof(jump));
			if ((jump & 1) && (GetAsyncKeyState(VK_CAPITAL) & 0x8000)) {
				cmd->buttons |= BUTTONS::in_jump;
			}
		}

		if (gui.aimbot && gui.aim_mode == SILENT_AIM) {


			g_mutex_autofire.lock();
			DWORD64 autofire_obj = g_is_autofire_obj;
			//所以自动开枪时的对象 在这里拿到了 我没有使用他，只不过让SilentAim里来实现开枪 这样的好处是更精准
			//为什么没有把autofire_obj置0 因为我要让这个函数要执行多次 autofire_obj的置0 让autofire线程来做吧
			g_mutex_autofire.unlock();
			if (WindowsIsActivat(game.Hwnd)) {
				if ((GetAsyncKeyState(gui.aim_key) & 0x8000) || autofire_obj) {
					//好吧 我必须告诉你 ，在这里写入输入角度会被vac 我不知道是hook被检查了？也许我应该替换虚表？
					SilentAim(cmd, autofire_obj != 0 ? true : false);
				}
			}
		}
		return result;
	}
}


void CInput::ToThirdPerson(bool is) {
	//这个是源码里有的，不过需要绕过svcheats.getbool的检查 不然没啥用
	this->m_nValue = (int*)(this->rcx + ClassDataOffset::m_nValue);
	*this->m_nValue = is ? 1 : 2;
}

bool CInput::IsThirdPerson() {
	this->m_fCameraInThirdPerson = (int*)(this->rcx + ClassDataOffset::m_fCameraInThirdPerson);
	return (bool)*this->m_fCameraInThirdPerson;
}

void CInput::SetHook(bool is_create) {
	if (is_create)
		minihook.SetHook(interfaces.cinput.GetUserCmd, (LPVOID)(&CInputHook::hkGetUserCmd), (LPVOID*)&interfaces.cinput.o_GetUserCmd);
	else
		minihook.UnHook(interfaces.cinput.GetUserCmd);
}


bool CInput::Init() {
	ULONG64 addr = getrelative_addr((ULONG64)pattern_scan((HMODULE)game.ClientBase, OFFSET_INPUT_CLASS) + 14, 3, 7);
	if (!addr) {
		printf("find OFFSET_INPUT_CLASS failed\n");
		return false;
	}
	interfaces.cinput.rcx = *(PULONG64)addr;
	interfaces.cinput.CreateMove = (_CreateMove)((PULONG64)(*(PULONG64) * (PULONG64)addr))[5];

	addr = (ULONG64)pattern_scan((HMODULE)game.ClientBase, OFFSET_GETUSETCMD);
	if (!addr) {
		printf("find OFFSET_GETUSETCMD failed\n");
		return false;
	}
	interfaces.cinput.GetUserCmd = (_GetUserCmd)(addr + 2);
	

	addr = (ULONG64)pattern_scan((HMODULE)game.ClientBase, OFFSET_TO_THIRD_PERSON);
	if (!addr) {
		printf("find OFFSET_TO_THIRD_PERSON failed\n");
		return false;
	}
	interfaces.cinput.CAM_ToThirdPerson = (_CAM_ToThirdPerson)(addr + 2);

	addr = (ULONG64)pattern_scan((HMODULE)game.ClientBase, OFFSET_TO_THIRD_PERSON);
	if (!addr) {
		printf("find OFFSET_TO_THIRD_PERSON failed\n");
		return false;
	}
	interfaces.cinput.CAM_ToThirdPerson = (_CAM_ToThirdPerson)(addr + 2);
	
	printf("=============================interfaces=============================\n");
	printf("CreateMove:%p\tGetUserCmd:%p\n", interfaces.cinput.CreateMove, interfaces.cinput.GetUserCmd);
	printf("interfaces.cinput:%p\n", interfaces.cinput.rcx);
	printf("=============================interfaces=============================\n");
	return true;
}