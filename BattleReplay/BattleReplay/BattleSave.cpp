#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

char* PLUGIN_NAME = "igrik.BattleSave";

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

// ��������� ���������� ��������� � ���������� �����
static struct _SaveHero_
{
   int init;
   int id;
   int x;
   int y;
   int z;
   int movement;
   int save_x;
   int save_y;
   int save_z;
   int save_movement;
} saveHero;

int __stdcall Y_SetBattleSave(LoHook* h, HookContext* c)
{   
    // �������� ��������� �����
    _Hero_* atacker = *(_Hero_**)0x2860248;

    if (atacker) 
    {
        int meID = o_GameMgr->GetMe()->id;
        int attackerID = atacker->owner_id;

        // ����������, ��� ��� ��� �����
        if ( meID == attackerID ) 
        {        
            // ����� ����������� "����������" ���������� � ���������
            if ( saveHero.init == 1 && atacker->id == saveHero.id )
            {
                if (saveHero.x == atacker->x && saveHero.y == atacker->y && saveHero.z == atacker->z)
                    saveHero.init = 0;

                if ( saveHero.init == 1 )
                {
                    saveHero.save_x = atacker->x;
                    atacker->x = saveHero.x;

                    saveHero.save_y = atacker->y;
                    atacker->y = saveHero.y;

                    saveHero.save_z = atacker->z;
                    atacker->z = saveHero.z;

                    saveHero.save_movement = atacker->movement_points;
                    atacker->movement_points = saveHero.movement;
                }
            }
            // ��������� ����
            CALL_6(char, __thiscall, 0x4BEB60, o_GameMgr, "BATTLE!", 1, 1, 1, 0);
            // o_AdvMgr->HeroActive_Mobilize();

            // ��������������� ���������
            if ( saveHero.init && atacker->id == saveHero.id )
            {
                atacker->x = saveHero.save_x;
                atacker->y = saveHero.save_y;
                atacker->z = saveHero.save_z;
                atacker->movement_points = saveHero.movement;
                
            }
            // ���������� ����������
            saveHero.init = 0;
        }
    }
    // ������������ � ��� ����
    return EXEC_DEFAULT; 
}

// ��� ������ ���� ����� ��������� ��� ���������� � ���������
int __stdcall Y_HeroMakeStep(LoHook* h, HookContext* c)
{   
     _Hero_* hero = (_Hero_*)c->esi;

    if (hero) 
    {
        saveHero.init = 1;
        saveHero.id = hero->id;
        saveHero.x = hero->x;
        saveHero.y = hero->y;
        saveHero.z = hero->z;
        saveHero.movement = hero->movement_points; 
    }
    // ������������ � ��� ����
    return EXEC_DEFAULT; 
}

void StartPlugin()
{   
    // ����� ������ ����� ���� "BATTLE!"
    _PI->WriteLoHook(0x75AE24, Y_SetBattleSave);

    // ��� ������ ���� �����
    _PI->WriteLoHook(0x480239, Y_HeroMakeStep);      
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    static _bool_ initialized = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!initialized)
        {
            initialized = 1;

            _P = GetPatcher();
            _PI = _P->CreateInstance(PLUGIN_NAME);

            StartPlugin();
            
        }
        break;

    case DLL_PROCESS_DETACH:
        if (initialized)
            initialized = 0;
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
