#pragma once
#include "define.h"





struct C_UTL_VECTOR
{
    DWORD64 Count;
    DWORD64 Data;
};


class Controller
{
public:
    DWORD64 Address;
    int Health;
    int TeamID;
    int AliveStatus;
    char PlayerName[500];
    


    DWORD64 UpdateController(DWORD64);
private:
    bool GetHealth();
    bool GetTeamID();
    bool GetIsAlive();
    bool GetPlayerName();
public:
    static bool SetViewAngles(Point2D&);
    static bool GetViewAngles(Point2D&);
    static bool GetShotFireRCS(DWORD64, Point2D&);
    static bool SetViewAngles(float& x, float& y);
    static bool GetAttackStatus(bool& is_attack);
    static bool SetAttackStatus(bool attack);
    static bool GetJumpStatus(UCHAR* status);
    static bool SetJumpFlags(DWORD flags);
};




class Player
{

public:
	DWORD64 Address;
    Point3D Pos;
    Point2D ScreenPosBottom;
    Point2D ScreenPosTop;
    Point2D ViewAngle;
	int Health;
	int TeamID;
    bool alive;
    DWORD Pawn;
    float Height;
    int fov;
    char WeaponName[500];
    /*
   Í·6
   ÐØ¿Ú4
   ²±×Ó5
   Ñü0
   ÓÒÏ¥¸Ç26
   ÓÒ½Å27
   ×óÏ¥¸Ç23
   ×ó½Å24
   ÓÒ¼ç°ò13
   ÓÒÊÖÏ¥¸Ç14 15 16
   ×ó¼ç°ò8
   ÓÒÊÖÏ¥¸Ç9 10 11
   */
    Point3D BonsPos[30];
    
    int ShotsFired;
    bool SpottedStatus;
    DWORD64 SpottedByMask;
public:
    Player();
    ~Player();

public:
    bool GetTeamID();
    bool GetSpottedState();
    bool UpdatePlayer(DWORD64, bool = true);
private:
    
    bool GetHealth();
    bool GetBonsPos();
    bool GetViewAngle();
    bool GetWeaponName();
    bool GetFov();
    bool GetShotsFired();
    
};

class Client
{
public:
    Player player;
    Controller local;
private:

};
