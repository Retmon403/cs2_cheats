#include "draw.h"
#include "player.h"
#include "hacker.h"


ImU32 floatToImU32(float* col) {
    return ImGui::ColorConvertFloat4ToU32({ col[0],col[1],col[2],col[3] });
}

void DrawLine(float x, float y, float xx, float yy, ImU32 color, float thickness) // 绘制直线
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(xx, yy), color, thickness); // 绘制线条
}

void DrawBox(float x, float y, float width, float height, ImU32 color, float thickness) // 绘制矩形
{
    ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), color, 0.0f, 0, thickness);
}

void DrawRound(float x, float y, float radius /* 半径 */, ImU32 Color, float thickness)
{
    ImGui::GetForegroundDrawList()->AddCircle(ImVec2(x, y), radius, Color, 0, thickness);
}

void DrawBox2(float x, float y, float w, float h, ImU32 color, float thickness)
{
    float w1 = 0.f, h1 = 0.f;
    w1 = w / 3.f;
    h1 = h / 4.f;
    // 左上角
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + w1, y), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + h1), color, thickness);

    // 右上角
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 2.f * w1, y), ImVec2(x + w, y), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + h1), color, thickness);

    // 左下角
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + 3.f * h1), ImVec2(x, y + h), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + w1, y + h), color, thickness);

    // 右下角
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w1 * 2.f, y + h), ImVec2(x + w, y + h), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h), ImVec2(x + w, y + 3.f * h1), color, thickness);
}


void DrawHP(float x, float y, float w, DWORD hp) {
    ImU32 hp_color = 0;
    if (hp >= 80) {
        hp_color = floatToImU32(g_color_green);
    }
    else if (hp > 50 && hp < 80) {
        hp_color = floatToImU32(g_color_yellow);
    }
    else if (hp <= 50 && hp > 30) {
        hp_color = floatToImU32(g_color_hp);
    }
    else if (hp <= 30) {
        hp_color = floatToImU32(g_color_red);
    }

    float d_w = (float)hp / 100.f * w;

    ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y - 8), ImVec2(x + d_w, y + 0.5f - 8), hp_color, 0.0f, 0, 2.f);
    if (hp != 100) {
        ImGui::GetForegroundDrawList()->AddRect(ImVec2(x + d_w, y - 8),
            ImVec2(x + w, y - 8 + 0.5f), floatToImU32(g_color_text), 0.0f, 0, 2.f);
    }
}

void DrawBackgroundText(float x, float y, char* text, ImU32 background_color, ImU32 text_color) {
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + text_size.x, y + text_size.y), background_color);
    ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y), text_color, text);
}