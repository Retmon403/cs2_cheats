#include "c4.h"
#include "hacker.h"
#include "tool.h"


//c4对象非常有趣 很可惜我没有一个好的办法来检查c4是否被激活 即使已经爆炸 还是可以读出来
bool PlantedC4::UpdateC4() {
	if (!this->GetC4Object()) {
		return false;
	}
	if (!this->GetC4Activated()) {
		return false;
	}
	if (!this->GetC4Pos()) {
		return false;
	}
	if (!this->GetExplosionTime()) {
		return false;
	}
	return true;
}

bool PlantedC4::GetC4Object() {
	DWORD64 PlantedC4 = 0;
	bool status = ReadGameMemory((LPVOID)(game.ClientBase + client_dll::dwPlantedC4), &PlantedC4, sizeof(PlantedC4));
	if (!status || !PlantedC4) {
		return false;
	}
	status = ReadGameMemory((LPVOID)(PlantedC4), &PlantedC4, sizeof(PlantedC4));
	this->c4_object = PlantedC4;
	return (status && PlantedC4);
}

bool PlantedC4::GetC4Activated() {
	bool Activated = false;
	DWORD SpottedState = 0;
	ReadGameMemory((LPVOID)(this->c4_object + C_PlantedC4::m_bC4Activated), &Activated, sizeof(Activated));
	ReadGameMemory((LPVOID)(this->c4_object + C_PlantedC4::m_entitySpottedState), &SpottedState, sizeof(SpottedState));
	this->activated = ((SpottedState == 1) && Activated);
	return true;
}

bool PlantedC4::GetC4Pos() {
	DWORD64 SceneNode = 0;
	ReadGameMemory((LPVOID)(this->c4_object + C_BaseEntity::m_pGameSceneNode), &SceneNode, sizeof(SceneNode));
	return ReadGameMemory((LPVOID)(SceneNode + CGameSceneNode::m_vecAbsOrigin), &this->c4_pos, sizeof(this->c4_pos));
}

bool PlantedC4::GetExplosionTime() {
	float a, b; bool status_a, status_b;
	status_a = ReadGameMemory((LPVOID)(this->c4_object + C_PlantedC4::m_flC4Blow), &a, sizeof(a));
	status_b = ReadGameMemory((LPVOID)(this->c4_object + C_PlantedC4::m_flNextBeep), &b, sizeof(b));
	if (status_a && status_b) {
		this->explosion_time = a - b + 1.2f;
		return true;
	}
	else {
		return false;
	}
}