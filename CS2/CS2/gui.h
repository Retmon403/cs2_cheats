#pragma once
#include "define.h"
#include "player.h"



class GUI_DATA
{
public:
	bool imgui;
	bool show_menu;
	bool show_debug;
	bool debug_info;

	bool esp;
	int esp_mod;

	bool esp_line;
	bool esp_entity;
	bool diy_color;
	bool dx_hk;
	bool esp_friend;
	bool bons;
	bool show_info;

	bool aimbot;
	int aim_mode;

	bool aim_rcs;
	bool aim_auto_moveangle;
	bool aim_auto_switch;
	bool aim_friend;
	int aim_select_mode;
	int aim_key;
	int aim_size;
	int aim_scale_size;

	int aim_pos;

	bool auto_jump;
	int jump_mode;

	bool no_flashbang;
	bool multicolour_smoke;

	int aim_speed;
	bool aim_rand_num;

	bool auto_fire;
	bool auto_fire_no_shot_firend;
	int fire_uptime;
	int fire_nexttime;
	int fire_shot_time;


	bool third_person;
	
	Client  self;

private:

};




extern GUI_DATA gui;

void GUI();
void initgui();

