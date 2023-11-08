#include "entity.h"
#include "tool.h"

bool Entity::UpdateEntity(DWORD64 Address) {
	if (!Address) {
		return false;
	}
	this->addr = Address;
	if (!this->GetEntityName()) {
		return false;
	}
	if (!this->GetEntityType()) {
		return false;
	}
	if (!this->GetWeaponName()) {
		return false;
	}
	if (!this->GetEntityPos()) {
		return false;
	}
	return true;
}


bool Entity::GetWeaponName() {
	char buf[128] = { 0 };
	char* pos_weapon = strstr(this->name, "weapon_");
	char* pos_projectile = strstr(this->name, "projectile");
	if (pos_weapon) {
		RtlCopyMemory(buf, pos_weapon + 7, strlen(this->name) - (pos_weapon - this->name) - 7);
		RtlCopyMemory(this->name, buf, sizeof(buf));
		return true;
	}
	else if (pos_projectile) {
		ZeroMemory(buf, sizeof(buf));
		RtlCopyMemory(buf, this->name, (size_t)pos_projectile - (size_t)this->name - 1);
		RtlCopyMemory(this->name, buf, sizeof(buf));
		return true;
	}
	else {
		return false;
	}
}


bool Entity::GetEntityType() {
	if (strstr(this->name, "weapon_c4")) {
		this->type = Type_C4;
		return true;
	}
	else if (strstr(this->name, "projectile")) {
		this->type = Type_Projectile;
		return true;
	}
	else if (strstr(this->name, "weapon_molotov") || strstr(this->name, "weapon_flashbang") || strstr(this->name, "weapon_smokegrenade")
		|| strstr(this->name, "weapon_decoy") || strstr(this->name, "weapon_hegrenade") || strstr(this->name, "weapon_taser")) {
		this->type = Type_ProjectileItem;
		return true;
	}
	else if (strstr(this->name, "weapon_")) {
		this->type = Type_Gun;
		return true;
	}
	return false;
}

bool Entity::GetEntityName() {
	DWORD64 tmp = 0;
	ReadGameMemory((LPVOID)(this->addr + 0x10), &tmp, sizeof(tmp));
	ReadGameMemory((LPVOID)(tmp + 0x20), &tmp, sizeof(tmp));
	return ReadGameMemory((LPVOID)(tmp), this->name, sizeof(this->name));
}

bool Entity::GetEntityPos() {
	DWORD64 SceneNode = 0;
	if (!ReadGameMemory((LPVOID)(this->addr + C_BaseEntity::m_pGameSceneNode), &SceneNode, sizeof(SceneNode))) {
		return false;
	}
	return ReadGameMemory((LPVOID)(SceneNode + CGameSceneNode::m_vecAbsOrigin), &this->pos, sizeof(this->pos));
}
