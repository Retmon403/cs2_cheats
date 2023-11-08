#include "world.h"
#include "tool.h"


void World::SetNoFlashBnag(Player& self) {
	float max_time = 0.f;
	if (!ReadGameMemory((LPVOID)(self.Address + C_CSPlayerPawnBase::m_flFlashMaxAlpha), &max_time, sizeof(max_time))) {
		return;
	}
	if (max_time == 0.f) {
		return;
	}
	else if (max_time > 0.f) {
		max_time = 0.f;
		WriteGameMemory((LPVOID)(self.Address + C_CSPlayerPawnBase::m_flFlashMaxAlpha), &max_time, sizeof(max_time));
	}
}


void World::SetSmokeColor(Entity& entity, float color[3]) {
	if (entity.type != Type_Projectile) {
		return;
	}
	WriteGameMemory((LPVOID)(entity.addr + C_SmokeGrenadeProjectile::m_vSmokeColor), color, sizeof(float) * 3);
}