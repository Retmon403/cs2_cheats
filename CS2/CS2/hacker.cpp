#include "hacker.h"
#include "player.h"
#include "dx11hk.h"
#include "draw.h"
#include "tool.h"
#include "gui.h"
#include "c4.h"
#include "entity.h"
#include "hook.h"
#include "world.h"

float g_color_black[4] = { 0.f, 0.f, 0.f, 0.7f };
float g_color_red[4] = { 1.f,0.f,0.f,0.85f };
float g_color_yellow[4] = { 0.789f,0.996f,0.f,0.85f };
float g_color_green[4] = { 0.f,0.745f,0.f,1.f };
float g_color_hp[4] = { 1.f,0.5f,0.f,1.f };
float g_color_white[4] = { 1.0f,1.0f,1.0f,1.f };
float g_color_blue[4] = { 0.0f,0.975f,0.918f,0.784f };
float g_color_text[4] = { 0.72f,0.77f,0.75f,1.f };
float g_color_purple[4] = { 0.421f,0.327f,0.966f,1.f };


void get_aimbot(float* self, float* obj, float* ret) {
    float x = self[0] - obj[0];
    float y = self[1] - obj[1];
    float z = self[2] - obj[2];
    ret[0] = (float)atan(z / sqrt(x * x + y * y)) / PI * 180.0f;
    ret[1] = (float)atan(y / x);
    if (x >= 0.0f && y >= 0.0f) {
        ret[1] = ret[1] / PI * 180.0f - 180.0f;
    }
    else if (x < 0.0f && y >= 0.0f) {
        ret[1] = ret[1] / PI * 180.0f;
    }
    else if (x < 0.0f && y < 0.0f) {
        ret[1] = ret[1] / PI * 180.0f;
    }
    else if (x >= 0.0f && y < 0.0f) {
        ret[1] = ret[1] / PI * 180.0f + 180.0f;
    }
}

bool WorldToScreen(Point3D& IN pos, Point2D& OUT topos) {
    game.UpDateMex();
    float View = 0.f;
    float SightX = (float)(game.rect.right / 2), SightY = (float)(game.rect.bottom / 2);
    View = game.mex[3][0] * pos.x + game.mex[3][1] * pos.y + game.mex[3][2] * pos.z + game.mex[3][3];
    if (View <= 0.01)
        return false;
    topos.x = SightX + (game.mex[0][0] * pos.x + game.mex[0][1] * pos.y + game.mex[0][2] * pos.z + game.mex[0][3]) / View * SightX;
    topos.y = SightY - (game.mex[1][0] * pos.x + game.mex[1][1] * pos.y + game.mex[1][2] * pos.z + game.mex[1][3]) / View * SightY;
    return true;
}


float CalcPoint3DDistance(Point3D& my, Point3D& obj) {
    return sqrt((obj.x - my.x) * (obj.x - my.x) + (obj.y - my.y) * (obj.y - my.y) + (obj.z - my.z) * (obj.z - my.z));
}

float calc2DDistance(Point2D p1, Point2D p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

void GetBonePos(DWORD64 obj, int n, Point3D* ret) {

   

    DWORD64 gamescene = 0;
    ReadGameMemory((LPVOID)(obj + C_BaseEntity::m_pGameSceneNode), &gamescene, sizeof(gamescene));

    DWORD64 bonearray = 0;
    ReadGameMemory((LPVOID)(gamescene + CSkeletonInstance::m_modelState + CGameSceneNode::m_vecOrigin), &bonearray, sizeof(bonearray));

    bonearray += n * 0x20;
    ReadGameMemory((LPVOID)(bonearray), ret, sizeof(float) * 3);

}

void DrawBons_AB(Point3D a, Point3D b, ImU32 color) {
    Point2D a_2d, b_2d;
    if (WorldToScreen(a, a_2d) && WorldToScreen(b, b_2d)) {
        if (a_2d.x == 0.f || a_2d.y == 0.f || b_2d.x == 0.f || b_2d.y == 0.f) {
            return;
        }
        DrawLine(a_2d.x, a_2d.y, b_2d.x, b_2d.y, color, 2.f);
    }
}

void DrawBons(Player& object, ImU32 color) {

    DrawBons_AB(object.BonsPos[6], object.BonsPos[5], color);
    DrawBons_AB(object.BonsPos[4], object.BonsPos[5], color);
    DrawBons_AB(object.BonsPos[4], object.BonsPos[2], color);
    DrawBons_AB(object.BonsPos[2], object.BonsPos[0], color);

    DrawBons_AB(object.BonsPos[13], object.BonsPos[5], color);
    DrawBons_AB(object.BonsPos[8], object.BonsPos[5], color);
    DrawBons_AB(object.BonsPos[13], object.BonsPos[14], color);
    DrawBons_AB(object.BonsPos[8], object.BonsPos[9], color);
    DrawBons_AB(object.BonsPos[9], object.BonsPos[10], color);
    DrawBons_AB(object.BonsPos[10], object.BonsPos[11], color);
    DrawBons_AB(object.BonsPos[14], object.BonsPos[15], color);
    DrawBons_AB(object.BonsPos[16], object.BonsPos[15], color);

    DrawBons_AB(object.BonsPos[0], object.BonsPos[25], color);
    DrawBons_AB(object.BonsPos[25], object.BonsPos[26], color);
    DrawBons_AB(object.BonsPos[26], object.BonsPos[27], color);
    DrawBons_AB(object.BonsPos[0], object.BonsPos[22], color);
    DrawBons_AB(object.BonsPos[23], object.BonsPos[22], color);
    DrawBons_AB(object.BonsPos[23], object.BonsPos[24], color);
    

    Point2D head = { 0 }, neck = { 0 };
    if (WorldToScreen(object.BonsPos[5], neck) && WorldToScreen(object.BonsPos[6], head)) {
        if (head.x != 0.f && head.y != 0.f && neck.x != 0.f && neck.y != 0.f) {
            DrawRound(head.x, head.y - 5, calc2DDistance(head, neck), floatToImU32(g_color_white), 2.f);
        }
    }
}

void Debug_DrawAllBons(DWORD64 object) {
    for (int i = 0; i < 30; i++) {
        Point3D bons_pos = { 0 };
        Point2D bons_screen = { 0 };
        GetBonePos(object, i, &bons_pos);
        if (WorldToScreen(bons_pos, bons_screen)) {
            char num[50] = { 0 };
            ::sprintf(num, "%d\n", i);
            ImGui::GetForegroundDrawList()->AddText(ImVec2(bons_screen.x, bons_screen.y), floatToImU32(g_color_red),num);
        }
        
    }

}
DWORD64 g_c4_player = 0;

void DrawC4() {
    PlantedC4 c4;
    if (c4.UpdateC4()) {
        g_c4_player = 0;
        Point2D c4_screen = { 0 };
        if (WorldToScreen(c4.c4_pos, c4_screen) && c4_screen.x != 0.f && c4_screen.y != 0.f) {
            Point2D c4_draw_screen = { c4_screen.x - 15, c4_screen.y - 15 };
            DrawBox(c4_draw_screen.x, c4_draw_screen.y, 30, 30, floatToImU32(g_color_red), 2.5f);
            CHAR c4_info[100] = { 0 };
            ::sprintf(c4_info, u8"倒计时:%.2f", c4.explosion_time);
            DrawBackgroundText(c4_draw_screen.x, c4_draw_screen.y - 20, c4_info,  floatToImU32(c4.explosion_time <= 10.f ? g_color_red:g_color_black),
                floatToImU32(g_color_text));
        }
    }
}

void EntutyCallBack(Entity& entity) {
    if (entity.type == Type_Projectile) {
        bool status = false;
        WriteGameMemory((LPVOID)(entity.addr + C_SmokeGrenadeProjectile::m_bDidSmokeEffect), &status, sizeof(status));
    }
}

DWORD BackFlashBang(LPVOID lp) {
    float y = *(float*)lp;
    free(lp);


    Point2D myangles = { 0 };
    Controller::GetViewAngles(myangles);

    int mode = get_rand_num(0, 1);
    for (size_t i = 0; i < 180; i++) {
        myangles = { myangles.x,mode ? y + i * 1.f : y - i * 1.f };
        Controller::SetViewAngles(myangles);
        Sleep(2);
    }

    return 0;
}


void DrawEntity(Client& self,vector<Player>& arr_player) {
    if (gui.show_info) {
        //很愚蠢的方式遍历实体 但可以使用 因为我没有更好的解决方案
        for (int i = 64; i < 1024; i++) {
            DWORD64 tmp = 0;
            ReadGameMemory((LPVOID)(game.GetEntityList() + 8LL * ((i & 0x7FFF) >> 9) + 16), &tmp, sizeof(tmp));
            if (!tmp) {
                continue;
            }
            ReadGameMemory((LPVOID)(tmp + 120LL * (i & 0x1FF)), &tmp, sizeof(tmp));
            if (!tmp) {
                continue;
            }
            Entity entity;
            if (!entity.UpdateEntity(tmp)) {
                continue;
            }
            EntutyCallBack(entity);
            if (CalcPoint3DDistance(entity.pos, self.player.Pos) < 15) {
                continue;
            }
            bool jmp = false;
            for (auto itr = arr_player.begin(); itr != arr_player.end(); itr++) {
                if (CalcPoint3DDistance(entity.pos, itr._Ptr->Pos) < 15) {
                    if (entity.type == Type_C4) {
                        //如果这个玩家离实体很近 且 实体是C4 他就持有C4
                        g_c4_player = itr._Ptr->Address;

                    }
                    jmp = true;
                    break;
                }
                else if (entity.type == Type_C4) {
                    g_c4_player = 0;
                }
            }
            if (jmp) {
                continue;
                //printf("%p\n", entity.addr);
            }

            //绘制实体
            if (gui.esp_entity) {
                Point2D entity_screen = { 0 };
                if (WorldToScreen(entity.pos, entity_screen) && entity_screen.x != 0.f && entity_screen.y != 0.f) {

                    //如果调试 则绘制实体的地址
                    if (gui.debug_info) {
                        ::sprintf(entity.name, "%s,%p", entity.name, entity.addr);
                    }
                    ImU32 color = { 0 };
                    if (!g_c4_player && entity.type == Type_C4) {
                        color = floatToImU32(g_color_red);
                    }
                    else if (entity.type == Type_Gun) {
                        color = floatToImU32(g_color_purple);
                    }
                    else if (entity.type == Type_ProjectileItem) {
                        color = floatToImU32(g_color_green);
                    }
                    else if (entity.type == Type_Projectile) {
                        color = floatToImU32(g_color_red);
                    }

                    DrawBackgroundText(entity_screen.x, entity_screen.y, entity.name, floatToImU32(g_color_black), color);
                }
            }

            if (gui.no_flashbang && entity.type == Type_Projectile && strstr(entity.name, "flashbang")) {
                //写内存来无视闪光会不会太危险了 vac 我害怕...
                World::SetNoFlashBnag(self.player);
            }
            if (gui.multicolour_smoke && entity.type == Type_Projectile && strstr(entity.name, "smokegrenade")) {
                float color[3] = { 1.f,0.f,0.f };
                World::SetSmokeColor(entity, color);
            }
            
            //很愚蠢的背闪方式 解除注释 依旧可以使用 
            //if (gui.no_flashbang && entity.type == Type_Projectile && strstr(entity.name, "flashbang")) {
            //    Point2D entity_screen = { 0 };
            //    if (WorldToScreen(entity.pos, entity_screen) && entity_screen.x != 0.f && entity_screen.y != 0.f
            //        && CalcPoint3DDistance(self.player.Pos, entity.pos) < 800.f) {
            //        Point2D angles = { 0 };
            //        get_aimbot((float*)&self.player.Pos, (float*)&entity.pos, (float*)&angles);


            //        static ULONG64 first_get = 0;
            //        static ULONG64 first_back = 0;

            //        if (!first_get) {
            //            first_get = GetTickCount64();
            //        }
            //        else if ((GetTickCount64() - first_get) > 900) {
            //            if (!first_back) {
            //                //如果第一次进来 就背闪 然后赋值启动时间
            //                void* mem = malloc(sizeof(angles.y));
            //                *(float*)mem = angles.y;
            //                NewThread(BackFlashBang, (LPVOID)mem);
            //                first_back = GetTickCount64();
            //            }
            //            first_get = 0;
            //        }

            //        if (first_back != 0 && (GetTickCount64() - first_back) > 1000) {
            //            //如果不是first_back不等于空 且 时间已经过去了3s 就清空
            //            first_back = 0;
            //        }
            //    }

            //}
        }
    }
}

void ESP() {
    static DWORD64 aim_obj = 0;//已经被瞄准的对象
    game.GetRect();
    vector<Player> arr_player;
    //更新自己的信息
    Client self;
    if (!self.player.UpdatePlayer(self.local.UpdateController(game.GetLocalPlayerController()))) {
        return;
    }
    //根据fov调整自瞄范围
    gui.aim_scale_size = gui.aim_size + (95 - self.player.fov);

    if (gui.debug_info) {
        gui.self = self;
    }
    
    //保存自己的self信息，在调试模式使用
    int self_index = 1;
    for (int i = 0; i < 64; i++) {
        
        Client object;
        DWORD64 EntityAddress = 0;

        ReadGameMemory((LPVOID)(game.GetEntityList() + 0x10), &EntityAddress, sizeof(EntityAddress));
        if (!ReadGameMemory((LPVOID)(EntityAddress + (i + 1) * 0x78), &EntityAddress, sizeof(EntityAddress))) {
            continue;
        }
        if (EntityAddress == game.GetLocalPlayerController() ) {
            self_index = i;
            continue;
        }
        if (!object.player.UpdatePlayer(object.local.UpdateController(EntityAddress))) {
            continue;
        }
        if (!object.player.alive) {
            if (aim_obj == object.player.Address && gui.aim_auto_switch) {
                aim_obj = 0;
            }
            continue;
        }
        if (object.player.Pos.x == 0.f && object.player.Pos.y == 0.f) {
            continue;
        }
        Point2D fake = { 0 };
        if (!WorldToScreen(object.player.Pos, fake) || fake.x == 0.f || fake.y == 0.f) {
            continue;
        }
        

        //设置该对象对自己的可见性
        object.player.SpottedStatus = (object.player.SpottedByMask & (DWORD64(1) << (self_index))) ||
            (object.player.SpottedByMask & (DWORD64(1) << (i)));

        //将有效对象存入容器
        arr_player.push_back(object.player);

        //计算方框位置
        WorldToScreen(object.player.Pos, object.player.ScreenPosBottom);
        Point3D neck_head = object.player.BonsPos[6];
        neck_head.z += (object.player.BonsPos[6].z - object.player.BonsPos[5].z) * 2 / 1.5f;
        WorldToScreen(neck_head, object.player.ScreenPosTop);
        float DrawH = object.player.ScreenPosBottom.y - object.player.ScreenPosTop.y;
        float DrawW = DrawH / 2;
        float box_x = object.player.ScreenPosBottom.x - DrawW / 2;
        float box_y = object.player.ScreenPosTop.y;

        bool is_friend = (self.player.TeamID == object.player.TeamID);

        if (is_friend && gui.esp_friend) {
            d_box:
            if (gui.esp) {
                if (gui.esp_mod == 0) {
                    DrawBox2(box_x, box_y, DrawW, DrawH + 5, floatToImU32(is_friend ? g_color_green : g_color_red), 2.5f);
                }
                else if (gui.esp_mod == 1) {
                    DrawBox(box_x, box_y, DrawW, DrawH + 5, floatToImU32(is_friend ? g_color_green : g_color_red), 2.5f);
                }
                
                DrawHP(box_x, box_y, DrawW, object.player.Health);
            }
            if (gui.bons) {
                DrawBons(object.player, floatToImU32(is_friend ? g_color_blue : g_color_yellow));
            }
            if (gui.esp_line) {
                DrawLine((float)(game.rect.right / 2), 0.f, object.player.ScreenPosBottom.x, object.player.ScreenPosTop.y, floatToImU32(is_friend ? g_color_white : g_color_red), 1.5f);
            }
        }
        else if (!is_friend) {
            goto d_box;
        }
        if (gui.show_info) {
            float distance = (float)CalcPoint3DDistance(self.player.Pos, object.player.Pos);
            CHAR player_name[500] = { 0 };
            ::sprintf(player_name, object.player.fov == 90 ? u8"%s | %1.fm" : u8"%s | %1.fm | 开镜瞄准中...", object.local.PlayerName, distance / 100);
            if (is_friend && gui.esp_friend) {
            draw_info:
                DrawBackgroundText(box_x, box_y - 30, player_name,
                    floatToImU32(object.player.fov == 90 ? g_color_black : g_color_red), floatToImU32(g_color_text));

                DrawBackgroundText(box_x, box_y - 50, object.player.WeaponName,
                    floatToImU32(g_color_black), floatToImU32(g_color_text));
                if (g_c4_player == object.player.Address) {
                    CHAR player_c4_info[] = u8"持有C4炸弹";
                    DrawBackgroundText(box_x, box_y - 70, player_c4_info, floatToImU32(g_color_black), floatToImU32(g_color_red));
                }
            }
            else if (!is_friend) {
                goto draw_info;
            }

            
        }

        if (gui.debug_info) {
            char szBuf[500] = { 0 };
            RtlZeroMemory(szBuf, 0);
            ::sprintf(szBuf, u8"obj:0x%p,local:0x%p", object.player.Address, object.local.Address);
            ImGui::GetForegroundDrawList()->AddText(ImVec2(box_x, box_y + DrawH + 20), floatToImU32(g_color_text), szBuf);


            RtlZeroMemory(szBuf, 0);
            ::sprintf(szBuf, u8"SpottedState:%d", object.player.SpottedStatus);
            ImGui::GetForegroundDrawList()->AddText(ImVec2(box_x, box_y + DrawH + 40), floatToImU32(g_color_text), szBuf);


            Debug_DrawAllBons(object.player.Address);
            
        }
    }
    
    
    //绘制实体
    DrawEntity(self, arr_player);
    //绘制c4信息
    DrawC4();

    if (gui.aimbot) {
        //画辅助线条

        DrawRound((float)(game.rect.right - game.rect.left) / 2.f, (float)(game.rect.bottom - game.rect.top) / 2.f, (float)gui.aim_scale_size, floatToImU32(g_color_text),1.5f);
        DrawRound((float)(game.rect.right - game.rect.left) / 2.f, (float)(game.rect.bottom - game.rect.top) / 2.f, (float)5, floatToImU32(g_color_red));

        //如果自瞄方式不是这种就返回
        if (gui.aim_mode != MEM_AIM) {
            return;
        }

        AIMBOT_DATA aim = { 0 };
        for (auto itr = arr_player.begin(); itr != arr_player.end(); itr++) {
            //如果不瞄准队友
            if (!gui.aim_friend && self.player.TeamID == itr._Ptr->TeamID) {
                continue;
            }
            Point2D bons_2d;
            WorldToScreen(itr._Ptr->BonsPos[gui.aim_pos], bons_2d);
            float disfront = calc2DDistance(bons_2d, { (float)(game.rect.right - game.rect.left) / 2.f,(float)(game.rect.bottom - game.rect.top) / 2.f });
            float distance = CalcPoint3DDistance(self.player.BonsPos[gui.aim_pos], itr._Ptr->BonsPos[gui.aim_pos]);


            if (gui.aim_auto_moveangle) {
                if (!itr._Ptr->SpottedStatus) {
                    aim_obj = 0;
                    continue;
                }
                else {
                    if ((disfront > (float)gui.aim_scale_size) && itr._Ptr->Address == aim_obj) {
                        aim_obj = 0;
                        continue;
                    }
                }
            }
            
            //如果当前对象在瞄准返回内 记录下来
            if (disfront < gui.aim_scale_size) {
                //如果有锁定对象 但没更新到他 且 自动锁定下一个敌人是开的 就下一次循环
                if (aim_obj && aim_obj != itr._Ptr->Address && gui.aim_auto_switch) {
                    continue;
                }
                //如果循环到锁定对象 且对象还活着 就更新他的信息
                if (aim_obj == itr._Ptr->Address && itr._Ptr->alive) {
                    aim.up_dis_front = disfront;
                    goto updata;
                }
                if (!aim.up_dis_front || !aim.up_dis_tance) {
                    aim.up_dis_front = disfront;
                    aim.up_dis_tance = distance;
                    goto updata;
                }
                else {
                    //如果当前已经有锁定对象 且锁定对象是现在遍历到了的这个 就更新这个对象的信息
                    if (gui.aim_select_mode == 0) {
                        if (disfront <= aim.up_dis_front) {
                            aim.up_dis_front = disfront;
                            goto updata;
                        }
                    }
                    else if (gui.aim_select_mode == 1) {
                        if (distance <= aim.up_dis_tance) {
                            aim.up_dis_tance = distance;
                            goto updata;
                        }
                    }

                }
                if (false) {
                updata:
                    //更新被自瞄敌人的数据
                    aim.bons_2d = bons_2d;
                    aim.bons_3d = itr._Ptr->BonsPos[gui.aim_pos];
                    aim.up_obj = itr._Ptr->Address;

                }
            }

        }

        if (aim.up_obj) {
            //绘制当前锁定对象的连接线条
            DrawLine((float)(game.rect.right - game.rect.left) / 2.f, (float)(game.rect.bottom - game.rect.top) / 2.f, aim.bons_2d.x, aim.bons_2d.y, floatToImU32(g_color_red), 2.0f);
            Client obj;
            obj.player.UpdatePlayer(aim.up_obj);
            DrawBons(obj.player, (self.player.TeamID != obj.player.TeamID) ? floatToImU32(g_color_red) : floatToImU32(g_color_white));


            if (!gui.aim_auto_moveangle) {
                if (GetAsyncKeyState(gui.aim_key) & 0x8000) {
                    //如果记录上一次的范围 和对象都是空的
                    if (!aim_obj) {
                        aim_obj = aim.up_obj;
                        goto aim;
                    }
                    else if (aim_obj == aim.up_obj) {
                    aim:
                        void AutoAimLockMmeory(Player&, Point3D&, Point3D&, int);
                        AutoAimLockMmeory(self.player, self.player.BonsPos[6], aim.bons_3d, gui.aim_speed);
                    }
                }
                else {
                    aim_obj = 0;
                }
            }
            else {
                if (!aim_obj) {
                    aim_obj = aim.up_obj;
                    goto aim2;
                }
                else if (aim_obj == aim.up_obj) {
                aim2:
                    void AutoAimLockMmeory(Player&, Point3D&, Point3D&, int);
                    AutoAimLockMmeory(self.player, self.player.BonsPos[6], aim.bons_3d, gui.aim_speed);
                }
            }
        }
    }
    
}

void AutoAimLockMmeory(Player& self_obj, Point3D& self, Point3D& fuck, int speed) {
    //我知道我的平滑自瞄算法写的不好 不过是按照我的思路想出来的 我知道有更好的代码  但这样也可以运行的很好
    Point2D aim_angle;
    Point2D self_angle;

    get_aimbot((float*)&self, (float*)&fuck, (float*)&aim_angle);

    if (gui.aim_rcs && self_obj.ShotsFired > 1) {
        //如果要压枪 就 对写入的视角做处理
        Point2D rcs = { 0 };
        if (Controller::GetShotFireRCS(self_obj.Address, rcs)) {
            aim_angle.x -= rcs.x * 2.f;
            aim_angle.y -= rcs.y * 2.f;
        }
    }

    if (speed == 0) {
        Controller::SetViewAngles(aim_angle);
    }
    else {
        Controller::GetViewAngles(self_angle);

        int x_speed = get_rand_num(speed, speed * 10);
        int y_speed = get_rand_num(speed, speed * 10);
        //你可以给平滑自瞄插入随机值 
        float distance = 0.f;
        float t = 0.01f;

        {
            //x 处理
            
            distance = abs(aim_angle.x - self_angle.x);
            if (distance > 0.1f) {

                if (gui.aim_rand_num) {
                    speed = x_speed;
                    //printf("x==> distance:%2.f,rand:%d\n", abs(aim_angle.y - self_angle.y), speed);
                    if (distance < 1.5f) {
                        speed = get_rand_num(2, 5);
                    }

                }
                else {
                    if (distance < 1.f) {
                        t = 0.02f;
                    }
                    else {
                        t = 0.08f;
                    }
                }
                
                if (self_angle.x > aim_angle.x) {
                    self_angle.x -= speed * t;
                }
                else {
                    self_angle.x += speed * t;
                }
            }
        }

        {
            //y 处理
            
            distance = abs(aim_angle.y - self_angle.y);
            if (distance > 0.1f) {

                if (gui.aim_rand_num) {
                    speed = y_speed;
                    //printf("y==> distance:%2.f,rand:%d\n", abs(aim_angle.y - self_angle.y), speed);
                    if (distance < 1.5f) {
                        speed = get_rand_num(2, 5);
                    }

                }
                else {
                    if (distance < 1.f) {
                        t = 0.02f;
                    }
                    else {
                        t = 0.08f;
                    }
                }
               

                if (self_angle.y > 0 && aim_angle.y < 0) {
                    if (self.x > fuck.x && self.y > fuck.y) {
                        self_angle.y += speed * t;
                    }
                    else {
                        self_angle.y -= speed * t;
                    }
                }
                if (self_angle.y < 0 && aim_angle.y < 0) {
                    if (self_angle.y > aim_angle.y) {
                        self_angle.y -= speed * t;
                    }
                    else {
                        self_angle.y += speed * t;
                    }

                }

                if (self_angle.y < 0 && aim_angle.y > 0) {
                    if (self.x < fuck.x && self.y < fuck.y) {
                        self_angle.y += speed * t;
                    }
                    else {
                        self_angle.y -= speed * t;
                    }


                }
                if (self_angle.y > 0 && aim_angle.y > 0) {
                    if (self_angle.y > aim_angle.y) {
                        self_angle.y -= speed * t;
                    }
                    else {
                        self_angle.y += speed * t;
                    }

                }
            }

        }
        
        
        
        Controller::SetViewAngles(self_angle);
    }
}

Game game;

bool Game::InitGameBase() {
    this->ClientBase = (DWORD64)GetModuleHandleA("client.dll");
    return this->ClientBase ? true : false;
}

DWORD64 Game::GetEntityList() {
    ReadGameMemory((LPVOID)(this->ClientBase + client_dll::dwEntityList), &this->EntityList, sizeof(this->EntityList));
    return this->EntityList;
}

DWORD64 Game::GetLocalPlayerController() {
    ReadGameMemory((LPVOID)(this->ClientBase + client_dll::dwLocalPlayerController), &this->LocalPlayerController, sizeof(this->LocalPlayerController));
    return this->LocalPlayerController;
}
DWORD64 Game::GetLocalPlayerPawn() {
    ReadGameMemory((LPVOID)(this->ClientBase + client_dll::dwLocalPlayerPawn), &this->LocalPlayerPawn, sizeof(this->LocalPlayerPawn));
    return this->LocalPlayerPawn;
}

void Game::GetRect() {
    GetClientRect(this->Hwnd, &this->rect);
}


bool Game::UpDateMex() {
    return ReadGameMemory((LPVOID)(this->ClientBase + client_dll::dwViewMatrix), this->mex, sizeof(this->mex));
}