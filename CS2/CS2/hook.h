#pragma once
#include "define.h"

class c_cmd_qangle
{
public:
    char pad1[24];
    Point2D angles;
};


class c_sub_tick_cmd
{
public:
    char pad1[0x18];
    c_cmd_qangle* view;
};


class c_sub_tick_container
{
public:
    std::int32_t tick_count;
    char pad1[0x4];
    std::uint8_t* tick_pointer;

    c_sub_tick_cmd* get_tick(std::int32_t index)
    {
        if (index < this->tick_count)
        {
            c_sub_tick_cmd** tick_list = reinterpret_cast<c_sub_tick_cmd**>(this->tick_pointer + 0x8);
            return tick_list[index];
        }

        return nullptr;
    }
};


class c_user_cmd_base
{
public:
    char pad1[64];
    c_cmd_qangle* view;
    char pad2[0x8];
    float forwardmove;
    float sidemove;
};


class c_user_cmd
{
public:
    char pad1[0x20];
    c_sub_tick_container sub_tick_container;
    c_user_cmd_base* base;
    char pad2[0x18];
    std::uint32_t buttons;

    void set_sub_tick_angles(Point2D& angles)
    {
        c_sub_tick_container container = this->sub_tick_container;
        for (std::int32_t i = 0; i < container.tick_count; i++)
        {
            c_sub_tick_cmd* tick = container.get_tick(i);

            if (tick && tick->view)
            {
                tick->view->angles = angles;
            }
        }
    }
};

typedef DWORD64(__fastcall* _CreateMove)(DWORD64, int, int);
typedef DWORD64(__fastcall* _GetUserCmd)(DWORD64, DWORD64, DWORD64);
typedef void (*_CAM_ToThirdPerson)();
typedef void (*_CAM_ToFirstPerson)();


enum BUTTONS : std::uint32_t
{
    in_attack = 1,
    in_jump = 2,
    in_duck = 4,
    in_attack2 = 2048,
    in_moveforward = 8,
    in_moveback = 16,
    in_moveleft = 512,
    in_moveright = 1024
};

class CInput
{
public:
    _GetUserCmd o_GetUserCmd;
    _CreateMove o_CreateMove;
public:
    ULONG64 rcx;
    _GetUserCmd GetUserCmd;
    _CreateMove CreateMove;
    

    _CAM_ToThirdPerson CAM_ToThirdPerson;
    _CAM_ToFirstPerson CAM_ToFirstPerson;

    

    bool GetUserCmdIsHook = false;

    void ToThirdPerson(bool);
    bool IsThirdPerson();
    void SetHook(bool);
    bool Init();
private:
    int* m_nValue;
    int* m_fCameraInThirdPerson;

};


class InterFaces
{
public:
    CInput cinput;

private:

};

extern InterFaces interfaces;;