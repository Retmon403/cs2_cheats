#pragma once
#include "define.h"
#include "hacker.h"


enum EntityType{
	Type_Projectile,//Í¶ÖÀ×´Ì¬
	Type_Gun,
	Type_ProjectileItem,//µòÂäÎï
	Type_C4
};

class Entity
{
public:
	Point3D pos;
	DWORD64 addr;
	CHAR name[128];
	EntityType type;
	bool GetWeaponName();
	bool GetEntityName();

	bool GetEntityPos();
	bool UpdateEntity(DWORD64);
	bool GetEntityType();
private:

};
