#include "gui.h"
#include "dx11hk.h"
#include "hacker.h"
#include "tool.h"
#include "hook.h"


GUI_DATA gui = { 0 };

void initgui() {
    gui.imgui = true;

    gui.show_menu = true;
    gui.show_debug = true;
    gui.debug_info = false;

    gui.esp = true;
    gui.esp_mod = 0;

    gui.esp_line = false;
    gui.esp_entity = true;
    gui.dx_hk = false;
    gui.esp_friend = false;
    gui.bons = true;
    gui.show_info = true;

    gui.aimbot = true;
    gui.aim_mode = MEM_AIM;
    gui.aim_rcs = true;
    gui.aim_auto_moveangle = false;
    gui.aim_auto_switch = true;
    gui.aim_select_mode = 0;
    gui.aim_friend = false;
    gui.aim_key = VK_MENU;
    gui.aim_size = 200;
    
    gui.aim_scale_size = 0;

    gui.aim_pos = 6;

    gui.auto_jump = true;
    gui.jump_mode = MEM_JUMP;

    gui.no_flashbang = false;
    gui.multicolour_smoke = false;

    gui.aim_speed = 8;
    gui.aim_rand_num = false;


    gui.auto_fire = true;
    gui.auto_fire_no_shot_firend = true;
    gui.fire_uptime = 10;
    gui.fire_shot_time = 110;
    gui.fire_nexttime = 220;
    
    gui.third_person = false;

}

void SetUiStyle() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.f;
    style.GrabMinSize = 15.f;
    style.GrabRounding = 2.f;
    style.FramePadding =ImVec2(8.f,4.f);
    style.ItemSpacing = ImVec2(5.f, 8.f);
}

void GUI() {
    
    if (gui.show_menu) {
        ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 380), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGui::Begin(u8"Cs2 Cheats", NULL, ImGuiWindowFlags_NoCollapse);

        ImGui::StyleColorsClassic();
        SetUiStyle();

        if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None | ImGuiTabItemFlags_SetSelected))
        {
            if (ImGui::BeginTabItem(u8"绘制类"))
            {
                ImGui::Checkbox(u8"绘制方框", &gui.esp);
                ImGui::SameLine();
                const char* t_esp_mode[] = { u8"类型一", u8"类型二" };
                ImGui::Combo(u8"样式", &gui.esp_mod, t_esp_mode, 2);
                ImGui::Separator();

                ImGui::Checkbox(u8"绘制队友", &gui.esp_friend);
                ImGui::SameLine();
                ImGui::Checkbox(u8"绘制骨骼", &gui.bons);
                ImGui::SameLine();
                ImGui::Checkbox(u8"辅助天线", &gui.esp_line);
                ImGui::SameLine();
                ImGui::Checkbox(u8"详细信息", &gui.show_info);
                ImGui::Separator();


                ImGui::Checkbox(u8"绘制实体", &gui.esp_entity);
                ImGui::SameLine();
                if (ImGui::Checkbox(u8"模型透视", &gui.dx_hk)) {
                    SetD3dEsp(gui.dx_hk);
                }
                ImGui::Separator();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(u8"自瞄类")) {

                ImGui::Checkbox(u8"启用自瞄", &gui.aimbot);
                ImGui::SameLine();

                static bool t_show_popup = false;
                const char* t_aim_mode[] = { u8"内存自瞄", u8"静默自瞄" };
                if (ImGui::Combo(u8"方式", &gui.aim_mode, t_aim_mode, 2)) {
                    //如果选择了静默自瞄方式 且没有初始化hook
                    if (gui.aim_mode == SILENT_AIM && !interfaces.cinput.GetUserCmdIsHook) {
                        t_show_popup = true;
                        ImGui::OpenPopup("My Popup");
                    }
                    else {
                        setup:
                        static int mem_aim = 0;
                        if (gui.aim_mode == SILENT_AIM) {
                            mem_aim = gui.fire_shot_time;
                            gui.fire_shot_time = 220;
                        }
                        else if (gui.aim_mode == MEM_AIM) {
                            gui.fire_shot_time = mem_aim;
                        }
                    }
                }
                if (t_show_popup) {
                    ImGui::SetNextWindowSize(ImVec2(300, 100));
                    if (ImGui::BeginPopupModal("My Popup", &t_show_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Text(u8"启用静默自瞄有封号风险，依然开启？");
                        if (ImGui::Button(u8"确定")) {
                            interfaces.cinput.SetHook(true);
                            interfaces.cinput.GetUserCmdIsHook = true;
                            goto setup;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(u8"取消")) {
                            gui.aim_mode = MEM_AIM;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::Separator();

                ImGui::Checkbox(u8"自瞄队友", &gui.aim_friend);
                ImGui::SameLine();
                ImGui::Checkbox(u8"自瞄压枪", &gui.aim_rcs);
                if (gui.aim_mode == MEM_AIM) {
                    ImGui::SameLine();
                    ImGui::Checkbox(u8"可视时自动瞄准", &gui.aim_auto_moveangle);
                }
                
                ImGui::Separator();

                static int t_aim = 2;
                const char* t_aim_item[] = { u8"左键", u8"右键" ,u8"Alt键",u8"鼠标侧键" };
                ImGui::Combo(u8"自瞄热键", &t_aim, t_aim_item, 4);
                static int t_aim_key[] = { VK_LBUTTON ,VK_RBUTTON ,VK_MENU ,VK_XBUTTON1 };
                gui.aim_key = t_aim_key[t_aim];

                if (ImGui::SliderInt(u8"自瞄半径", &gui.aim_size, 1, 550)) {
                    gui.aim_size -= gui.aim_size % 10;
                }
                
                

                static int t_aim_pos = 0;
                const char* t_aim_pos_item[] = { u8"头部", u8"脖子" ,u8"胸口",u8"大吊" };
                ImGui::Combo(u8"瞄准位置", &t_aim_pos, t_aim_pos_item, 4);
                static int t_aim_pos_arr[] = { 6,5,4,0 };
                gui.aim_pos = t_aim_pos_arr[t_aim_pos];

                if (gui.aim_mode == MEM_AIM) {
                    const char* t_aim_mod_item[] = { u8"准星距离优先", u8"敌我距离优先" };
                    ImGui::Combo(u8"选敌方式", &gui.aim_select_mode, t_aim_mod_item, 2);
                }
                
                ImGui::Separator();


                if (gui.aim_mode == MEM_AIM) {
                    ImGui::Checkbox(u8"插入随机值", &gui.aim_rand_num);
                    ImGui::SameLine();
                    ImGui::Checkbox(u8"杀死敌人后自动切换目标", &gui.aim_auto_switch);

                    ImGui::SliderInt(u8"平滑速度", &gui.aim_speed, 0, 10);
                    ImGui::Text(u8"Ps：数值为0将关闭平滑(一帧锁)...");
                    ImGui::Separator();
                }
                
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(u8"自动化类"))
            {
                ImGui::Checkbox(u8"瞄准敌人后自动开枪", &gui.auto_fire);
                ImGui::SameLine();
                ImGui::Checkbox(u8"排除队友", &gui.auto_fire_no_shot_firend);

                if ((gui.aim_mode == SILENT_AIM) && gui.auto_fire) {
                    ImGui::Text(u8"Ps:静默自瞄时,自动开枪也会以静默自瞄的方式射击");
                }
                ImGui::Separator();

                ImGui::SliderInt(u8"开枪前延迟(ms)", &gui.fire_uptime, 0, 300, nullptr, 0);
                ImGui::SliderInt(u8"开枪持续时间(ms)", &gui.fire_shot_time, 0, 5000, nullptr, 0);
                ImGui::SliderInt(u8"开枪后延迟(ms)", &gui.fire_nexttime, 0, 1000, nullptr, 0);
                ImGui::Separator();

                ImGui::Checkbox(u8"自动连跳", &gui.auto_jump);
                ImGui::SameLine();

                if (interfaces.cinput.GetUserCmdIsHook) {
                    const char* t_jump_mode[] = { u8"普通", u8"完美" };
                    ImGui::Combo(u8"方式", &gui.jump_mode, t_jump_mode, 2);
                }
                else {
                    const char* t_jump_mode[] = { u8"普通"};
                    ImGui::Combo(u8"方式", &gui.jump_mode, t_jump_mode, 1);
                }
                ImGui::Text(u8"Ps:连跳快捷键 -> [Caps Lock] 大小写切换键...");
                ImGui::Separator();

                

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(u8"世界类")) {

                ImGui::Checkbox(u8"无视闪光", &gui.no_flashbang);
                ImGui::SameLine();
                ImGui::Checkbox(u8"彩色烟雾", &gui.multicolour_smoke);
                ImGui::SameLine();
                ImGui::Checkbox(u8"第三人称", &gui.third_person);

                if (gui.third_person && !interfaces.cinput.IsThirdPerson()) {
                    interfaces.cinput.ToThirdPerson(true);
                }
                if (!gui.third_person && interfaces.cinput.IsThirdPerson()) {
                    interfaces.cinput.ToThirdPerson(false);
                }

                ImGui::Separator();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(u8"其它")) {
                if (ImGui::Checkbox(u8"调试窗口", &gui.show_debug)) {
                    SetConsoleStatus(gui.show_debug);
                }
                ImGui::SameLine();
                static bool tmp_close_game = false;
                if (ImGui::Checkbox(u8"结束游戏", &tmp_close_game)) {
                    SetConsoleStatus(false);
                    TerminateProcess(GetCurrentProcess(), 0);
                    exit(0);
                }
                ImGui::SameLine();
                static bool tmp_free_menu = false;
                if (ImGui::Checkbox(u8"剥离菜单", &tmp_free_menu)) {
                    NewThread(FreeMenu);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(u8"调试信息", &gui.debug_info));
                ImGui::Separator();
                ImGui::SeparatorText(u8"\tbilibili:美少女昕昕");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }



        ImGui::End();
        if (gui.debug_info) {
            ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.75f);
            ImGui::Begin(u8"debug information", NULL, ImGuiWindowFlags_NoCollapse);

            ImGui::Text(u8"[SelfInfo]");
            ImGui::Text(u8"Addr:%p",gui.self.player.Address);
            ImGui::Text(u8"Fov:%d", gui.self.player.fov);
            ImGui::Text(u8"PlayerName:%s", gui.self.local.PlayerName);
            ImGui::Text(u8"Pos:%.2f,%.2f,%.2f", gui.self.player.Pos.x, gui.self.player.Pos.y, gui.self.player.Pos.z);
            ImGui::Text(u8"WeaponName:%s", gui.self.player.WeaponName);

            Point2D dwViewAngles;
            ReadGameMemory((LPVOID)(game.ClientBase + client_dll::dwViewAngles), &dwViewAngles, sizeof(dwViewAngles));
            ImGui::Text(u8"ViewAngle:%.2f,%.2f", dwViewAngles.x, dwViewAngles.y);
            
            ImGui::End();
        }
    }else {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(20, 10), floatToImU32(g_color_red), u8"'Home'键 显示/隐藏 菜单");
    }
    
    ESP();
}