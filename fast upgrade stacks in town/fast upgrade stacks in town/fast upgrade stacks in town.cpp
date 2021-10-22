//////////////////////////////////////////////////////////
// Плагин автоматического улучшения существ в городе    //
// Автор: [igrik]                                       //
// Дата: 09.02.2017                                     //
/////////////////////////////////////////////////////////

#include "..\..\..\include\homm3.h"
#include "..\..\..\include\WoG_CrExpo.cpp"

Patcher* _P;
PatcherInstance* _PI;


_int_ GetCrGrade(_int_ monID)
{
    _int_ result = -1; 

    if ( GetWoGOptionsStatus(174) )
        result = GetCreatureUpradeWOG(monID); // воговская функция
    else
        result = GetCreatureUprade(monID);    // содовская функция

    return result;
}


_bool8_ UpgradeArmyStack(_Army_* army, _Town_* town, _Player_* me, _int_ index, _int_ isUpgradeSkeleton)
{
    _bool8_ result = false;

    _int_ type = army->type[index]; 
    _int_ count = army->count[index]; 

    if ( count < 1 )
        return result;

    if (!isUpgradeSkeleton && type == CID_SKELETON) 
        return result;

    _CreatureInfo_* mon = &o_pCreatureInfo[type];

    int upgType = GetCrGrade(type);
    if ( upgType >= 0 )  
    {
        if( town->IsBuildingBuilt(37 + mon->level, 1) ) 
        {
            if ( GetWoGOptionsStatus(174) || town->type == mon->town )
            {
                _Resources_ costRes;
                mon->GetUpradeCost(type, upgType, count, &costRes); 

                _Resources_* pRes = &me->resourses;

                if (pRes->EnoughResources(&costRes) )
                {                        
                    pRes->RemoveResources(&costRes);
                    int expo_deGrade = CrExpoSet_GetExp(army, index);   
                    army->type[index] = upgType;
                    if ( expo_deGrade > 0 ) 
                       CrExpoSet_SetExp(army, index, (expo_deGrade *3) >> 2 /* 75:100 */ );

                    result = true;                                          
                }
            }
        }
    }
    return result;
}

_bool8_ TryToUpgrade(_Town_* town, _Player_* me, int clickID)
{
    _bool8_ result = false;
    _Army_* army;
    _GameMgr_* gm = o_GameMgr;

    if (clickID >= 115 && clickID <= 121 && town->owner_id == me->id) 
    {   // существа в гарнизоне (сверху)
        if ( _Hero_* hero = gm->GetHero(town->up_hero_id) )         
            result = UpgradeArmyStack(&hero->army, town, me, clickID -115, 1);
        else
            result = UpgradeArmyStack(&town->guards, town, me, clickID -115, 1);
    }
    else if (clickID >= 140 && clickID <= 146)
    {   // существа у героя визитёра (снизу)
        if ( _Hero_* hero = gm->GetHero(town->down_hero_id) )
            if (hero->owner_id == me->id)
                result = UpgradeArmyStack(&hero->army, town, me, clickID -140, 1);
    }
    else if (clickID == 123 && town->owner_id == me->id)
    {   // герой в гарнизоне (сверху)
        if ( _Hero_* hero = gm->GetHero(town->up_hero_id) )
            army = &hero->army;
        else 
            army = &town->guards;
        for (int i = 0; i < 7; i++)
                result += UpgradeArmyStack(army, town, me, i, 0);
    }
    else if (clickID == 125)
    {   // герой визитёр (снизу)
        if ( _Hero_* hero = gm->GetHero(town->down_hero_id) ) 
        {
            if (hero->owner_id == me->id) {
                army = &hero->army;
                for (int i = 0; i < 7; i++)
                    result += UpgradeArmyStack(army, town, me, i, 0);
            }
        }
    }

    return result;
}


// автоулучшение существ в городе по ЛКМ+A
_int_ __stdcall Y_AutoGradeMonInTown(LoHook* h, HookContext* c)
{
    if ( GetKeyState(65)<0 )
    {
        _Player_* me = o_GameMgr->GetMe();
        if (me->IsActive()) 
        {
            _Town_* town = o_TownMgr->town;
            int clickID = c->edi; 

            if (TryToUpgrade(town, me, clickID)) {
                o_TownMgr->UnHighlightArmy();
                o_TownMgr->Redraw();
                c->return_address = 0x5D460F;
                return NO_EXEC_DEFAULT; 
            }
        }
    }
    return EXEC_DEFAULT;
} 

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    static _bool_ plugin_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!plugin_On)
        {
            plugin_On = 1;    

            // Создаём объекты patcher_x86.
            _P = GetPatcher();
            _PI = _P->CreateInstance("AutoGradeMonInTown"); 

            // автоулучшение существ в битве по Ctrl+A
            _PI->WriteLoHook(0x5D45FD, Y_AutoGradeMonInTown);   

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
/* СПРАВКА:
sprintf(o_TextBuffer, "Тип %d, подтип %d", ObjectMain_Type[1], ObjectMain_Type[2]); 
b_MsgBox(o_TextBuffer, 1);

*/