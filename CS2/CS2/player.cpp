#include "player.h"
#include "tool.h"
#include "hacker.h"

/*I don't know if you are Chinese or American. Thank you very much for checking my open source code. This code is poorly written, but it still runs perfectly*/

DWORD64 GetPlayerPawnAddress(DWORD64 addr) {
	DWORD64 EntityPawnListEntry = 0;
	DWORD64 EntityPawnAddress = 0;

	DWORD Pawn;

	if (!ReadGameMemory((LPVOID)(addr + CCSPlayerController::m_hPlayerPawn), &Pawn, sizeof(Pawn))) {
		return 0;
	}
	if (!ReadGameMemory((LPVOID)(game.GetEntityList() + 0x10 + 8 * ((Pawn & 0x7FFF) >> 9)), &EntityPawnListEntry, sizeof(EntityPawnListEntry))) {
		return 0;
	}
	if (!ReadGameMemory((LPVOID)(EntityPawnListEntry + 0x78 * (Pawn & 0x1FF)), &EntityPawnAddress, sizeof(EntityPawnAddress))) {
		return 0;
	}
	return EntityPawnAddress;
}

//所以 这个cpp文件里基本基本都是抄的已有开源项目 你应该知道是哪个 很感谢前辈的付出 
//也借鉴了很多 unknowcheats里的
DWORD64 Controller::UpdateController(DWORD64 addr) {
	this->Address = addr;

	if (!this->GetHealth()) {
		return 0;
	}
	if (!this->GetTeamID()) {
		return 0;
	}
	if (!this->GetPlayerName()) {
		return 0;
	}
	

	return GetPlayerPawnAddress(addr);;
}




bool Player::UpdatePlayer(DWORD64 addr, bool update_bons) {
	if (!addr) {
		return false;
	}
	this->Address = addr;

	if (!this->GetHealth()) {
		return false;
	}
	if (!this->GetTeamID()) {
		return false;
	}
	if (!this->GetBonsPos()) {
		return false;
	}
	//骨骼29就是对象的坐标
	this->Pos = this->BonsPos[28];
	if (!this->GetViewAngle()) {
		return false;
	}
	if (!this->GetFov()) {
		return false;
	}
	if (!this->GetSpottedState()) {
		return false;
	}
	this->GetWeaponName();
	this->GetShotsFired();
	return true;
}

bool Controller::GetPlayerName() {
	return ReadGameMemory((LPVOID)(this->Address + CBasePlayerController::m_iszPlayerName), this->PlayerName, sizeof(this->PlayerName));
}

bool Controller::GetIsAlive() {
	return ReadGameMemory((LPVOID)(this->Address + CCSPlayerController::m_bPawnIsAlive), &this->AliveStatus, sizeof(this->AliveStatus));
}

bool Controller::GetHealth() {
	return ReadGameMemory((LPVOID)(this->Address + C_BaseEntity::m_iHealth), &this->Health, sizeof(this->Health));
}

bool Controller::GetTeamID() {
	return ReadGameMemory((LPVOID)(this->Address + C_BaseEntity::m_iTeamNum), &this->TeamID, sizeof(this->TeamID));
}

bool Controller::SetViewAngles(Point2D& angle) {
	return WriteGameMemory((LPVOID)(game.ClientBase + client_dll::dwViewAngles), &angle, sizeof(Point2D));
}

bool Controller::SetViewAngles(float& x, float& y) {
	WriteGameMemory((LPVOID)(game.ClientBase + client_dll::dwViewAngles), &x, sizeof(x));
	return WriteGameMemory((LPVOID)(game.ClientBase + client_dll::dwViewAngles + 4), &y, sizeof(y));
}

bool Controller::GetViewAngles(Point2D& angle) {
	return ReadGameMemory((LPVOID)(game.ClientBase + client_dll::dwViewAngles), &angle, sizeof(Point2D));
}

bool Controller::GetShotFireRCS(DWORD64 addr, Point2D& rcs) {
	C_UTL_VECTOR AimPunchCache = { 0 };
	bool status = ReadGameMemory((LPVOID)(addr + C_CSPlayerPawn::m_aimPunchCache), &AimPunchCache, sizeof(AimPunchCache));
	if (!status || (AimPunchCache.Count > 0xFFFF && AimPunchCache.Count <= 0)) {
		return false;
	}
	return ReadGameMemory((LPVOID)(AimPunchCache.Data + (AimPunchCache.Count - 1) * sizeof(Point3D)), &rcs, sizeof(rcs));
}


bool Controller::GetAttackStatus(bool& is_attack) {
	DWORD attack = 0;
	if (!ReadGameMemory((LPVOID)(game.ClientBase + client_dll::dwForceAttack), &attack, sizeof(attack))) {
		return false;
	}
	is_attack = attack == 256 ? false : true;
	return true;
}

bool Controller::SetAttackStatus(bool attack) {
	DWORD status = attack ? 65537 : 256;
	return WriteGameMemory((LPVOID)(game.ClientBase + client_dll::dwForceAttack), &status, sizeof(status));
}



bool Controller::GetJumpStatus(UCHAR* status) {
	return ReadGameMemory((LPVOID)(game.GetLocalPlayerPawn() + C_BaseEntity::m_fFlags), status, sizeof(UCHAR));
}

bool Controller::SetJumpFlags(DWORD flags) {
	DWORD write = flags;
	return WriteGameMemory((LPVOID)(game.ClientBase + client_dll::dwForceJump), &write, sizeof(flags));
}


Player::Player() {
	this->Address = 0;
}

Player::~Player() {

}

bool Player::GetFov() {
	DWORD64 camera = 0;
	ReadGameMemory((LPVOID)(this->Address + C_BasePlayerPawn::m_pCameraServices), &camera, sizeof(camera));
	return ReadGameMemory((LPVOID)(camera + CCSPlayerBase_CameraServices::m_iFOVStart), &this->fov, sizeof(this->fov));
}

bool Player::GetHealth() {
	bool status =  ReadGameMemory((LPVOID)(this->Address + C_BaseEntity::m_iHealth), &this->Health, sizeof(this->Health));
	if (status) {
		this->alive = (this->Health > 0 && this->Health <= 100) ? true : false;
		return true;
	}
	else {
		return false;
	}
}

bool Player::GetTeamID() {
	return ReadGameMemory((LPVOID)(this->Address + C_BaseEntity::m_iTeamNum), &this->TeamID, sizeof(this->TeamID));
}

bool Player::GetBonsPos() {

	for (size_t i = 0; i < sizeof(this->BonsPos) / sizeof(Point3D); i++) {
		DWORD64 gamescene = 0;
		ReadGameMemory((LPVOID)(this->Address + C_BaseEntity::m_pGameSceneNode), &gamescene, sizeof(gamescene));

		DWORD64 bonearray = 0;
		ReadGameMemory((LPVOID)(gamescene + CSkeletonInstance::m_modelState + CGameSceneNode::m_vecOrigin), &bonearray, sizeof(bonearray));

		bonearray += i * 0x20;
		if (!ReadGameMemory((LPVOID)(bonearray), (LPVOID)&this->BonsPos[i], sizeof(Point3D))) {
			return false;
		}
	}

	return true;
}

bool Player::GetViewAngle() {
	return ReadGameMemory((LPVOID)(this->Address + angEyeAngles), &this->ViewAngle, sizeof(this->ViewAngle));
}

bool Player::GetWeaponName() {
	RtlZeroMemory(this->WeaponName, sizeof(this->WeaponName));
	DWORD64 WeaponAddress = 0;
	char Buffer[256] = { 0 };
	ReadGameMemory((LPVOID)(this->Address + C_CSPlayerPawnBase::m_pClippingWeapon), &WeaponAddress, sizeof(WeaponAddress));
	ReadGameMemory((LPVOID)(WeaponAddress + 0x10), &WeaponAddress, sizeof(WeaponAddress));
	ReadGameMemory((LPVOID)(WeaponAddress + 0x20), &WeaponAddress, sizeof(WeaponAddress));
	if (!ReadGameMemory((LPVOID)(WeaponAddress), Buffer, sizeof(Buffer))) {
		return false;
	}
	
	else {
		char* pos = strstr(Buffer, "_");
		if (!pos) {
			RtlCopyMemory(this->WeaponName, Buffer, strlen(Buffer));
		}
		else {
			RtlCopyMemory(this->WeaponName, pos + 1, strlen(Buffer) - (pos - Buffer) - 1);
		}
		return true;
	}
}

bool Player::GetShotsFired() {
	return ReadGameMemory((LPVOID)(this->Address + C_CSPlayerPawnBase::m_iShotsFired), &this->ShotsFired, sizeof(this->ShotsFired));
}


bool Player::GetSpottedState() {
	return ReadGameMemory((LPVOID)(this->Address + C_CSPlayerPawnBase::m_entitySpottedState+ EntitySpottedState_t::m_bSpottedByMask), &this->SpottedByMask, sizeof(this->SpottedByMask));
}
