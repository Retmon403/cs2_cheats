#pragma once
#include "define.h"
#include "player.h"
#include "entity.h"

class World
{
public:
    static void SetNoFlashBnag(Player& self);
    static void SetSmokeColor(Entity&,float[3]);

private:

};
