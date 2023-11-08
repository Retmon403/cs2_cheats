#pragma once
#include "define.h"


enum AIM_MODE {
    MEM_AIM,
    SILENT_AIM,
};

enum JUMP_MODE {
    MEM_JUMP,
    MSG_JUMP,
};


struct AIMBOT_DATA {
    Point3D bons_3d;
    Point2D bons_2d;
    float up_dis_front;//距离准星最近
    float up_dis_tance;
    DWORD64 up_obj;//合适的对象
};

class Game
{
public:
    HWND Hwnd;
    DWORD Pid;
    RECT rect;

    DWORD64 ClientBase = 0;
    DWORD64 EntityList;
    DWORD64 LocalPlayerController;
    DWORD64 LocalPlayerPawn;

public:
    float mex[4][4];
public:
    bool InitGameBase();
    DWORD64 GetEntityList();
    DWORD64 GetLocalPlayerController();
    DWORD64 GetLocalPlayerPawn();
    void GetRect();
public:
    bool UpDateMex();
private:

};

#define PI 3.14159265358979323846f
#define angEyeAngles 0x1500
#define OFFSET_WeaponName 0x1290

extern float g_color_black[4];
extern float g_color_red[4];
extern float g_color_yellow[4];
extern float g_color_green[4];
extern float g_color_hp[4];
extern float g_color_white[4];
extern float g_color_blue[4];
extern float g_color_text[4];
extern float g_color_purple[4];

extern Game game;

void get_aimbot(float* self, float* obj, float* ret);
float CalcPoint3DDistance(Point3D& my, Point3D& obj);
float calc2DDistance(Point2D p1, Point2D p2);

bool WorldToScreen(Point3D&, Point2D&);

ImU32 floatToImU32(float* col);
void ESP();