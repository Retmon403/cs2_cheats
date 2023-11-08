#pragma once
#include "define.h"



class PlantedC4
{
public:
	DWORD64 c4_object;
	bool activated;
	Point3D c4_pos;
	float explosion_time;

	bool UpdateC4();
	
private:
	bool GetC4Object();
	bool GetC4Activated();
	bool GetC4Pos();
	bool GetExplosionTime();
};
