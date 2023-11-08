#pragma once
#include "define.h"

ImU32 floatToImU32(float* col);
void DrawLine(float x, float y, float xx, float yy, ImU32 color, float thickness = 1.0f); // 绘制直线
void DrawBox(float x, float y, float width, float height, ImU32 color, float thickness = 1.f); // 绘制矩形
void DrawRound(float x, float y, float radius /* 半径 */, ImU32 Color, float thickness = 1.0f);
void DrawBox2(float x, float y, float w, float h, ImU32 color, float thickness);


void DrawHP(float x, float y, float w, DWORD hp);
void DrawBackgroundText(float x, float y, char* text, ImU32 background_color, ImU32 text_color);