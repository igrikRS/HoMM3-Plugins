//////////////////////////////////////////////////////////
// Плагин автоматического улучшения существ в городе	//
// Автор: [igrik]										//
// Дата: 09.02.2017										//
/////////////////////////////////////////////////////////

#include "..\..\..\include\homm3.h"
#include "..\..\..\include\WoG_CrExpo.cpp"

#define WOG_Option_174 (*(_int_*)0x4C6988)

// GetCreatureGradeWOG	воговская функция
// GetCreatureGrade		содовская функция

// Объекты patcher_x86.
Patcher* _P;
PatcherInstance* _PI;

int cost_res[7];

void ResetCostRes()
{
	for (int i = 0; i < 7; i++ ) {
		cost_res[i] = 0;
	}
	return;
}



// улучшение всех стеков у героя или в городе
_bool_ Y_AutoGradeMonInTownFunc(_Town_* town, char type){

_Player_* me = o_GameMgr->GetMe();

if (!me->IsActive()) return false;

	ResetCostRes();

	if (type != 0 ) // если не город 
	{
		_Hero_* hero = 0;
		

		if (type == 1)
			 hero = o_GameMgr->GetHero(town->up_hero_id);
		else hero = o_GameMgr->GetHero(town->down_hero_id);

		if (me->id != hero->owner_id) return false;

		for (int i = 0; i < 7; i++ )
		{
			if( hero->army.type[i] >= 0 )
			{
				if ( GetCreatureGrade(hero->army.type[i]) != -1 && hero->army.type[i] != 158 && hero->army.type[i] != 56)  
				{
					if( town->IsBuildingBuilt(37 + o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].level, 1) )	
					{
						if ( (WOG_Option_174 == 260179083 && town->type == o_pCreatureInfo[hero->army.type[i]].town) || WOG_Option_174 == 75648139 )
						{
							cost_res[0] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.wood * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.wood * hero->army.count[i]);
							cost_res[1] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.mercury * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.mercury * hero->army.count[i]);
							cost_res[2] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.ore * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.ore * hero->army.count[i]);
							cost_res[3] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.sulfur * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.sulfur * hero->army.count[i]);
							cost_res[4] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.crystal * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.crystal * hero->army.count[i]);
							cost_res[5] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.jems * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.jems * hero->army.count[i]);
							cost_res[6] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.gold * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.gold * hero->army.count[i]);

							if (me->resourses.wood >= cost_res[0] && me->resourses.mercury >= cost_res[1] && me->resourses.ore >= cost_res[2] && me->resourses.sulfur >= cost_res[3] && me->resourses.crystal >= cost_res[4] && me->resourses.jems >= cost_res[5] && me->resourses.gold >= cost_res[6])
							{
								me->resourses.wood    -= cost_res[0];
								me->resourses.mercury -= cost_res[1];
								me->resourses.ore     -= cost_res[2];
								me->resourses.sulfur  -= cost_res[3];
								me->resourses.crystal -= cost_res[4];
								me->resourses.jems    -= cost_res[5];
								me->resourses.gold    -= cost_res[6];
								
								int expo_deGrade = CrExpoSet_GetExp(&hero->army, i);					
								hero->army.type[i] = GetCreatureGrade(hero->army.type[i]);
								if ( expo_deGrade > 0 ) { 
									CrExpoSet_SetExp(&hero->army, i, expo_deGrade *75 /100);
								}								
							}
						}
					}				
				}
			}
		}
		return true;
	}
	else 
	{
		if (me->id != town->owner_id) return false;

		for (int i = 0; i < 7; i++ )
		{
			if( town->guards.type[i] >= 0 )
			{
				if ( GetCreatureGrade(town->guards.type[i]) != -1 && town->guards.type[i] != 158 && town->guards.type[i] != 56) 
				{
					if( town->IsBuildingBuilt(37 + o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].level, 1) )	
					{
						if ( (WOG_Option_174 == 260179083 && town->type == o_pCreatureInfo[town->guards.type[i]].town) || WOG_Option_174 == 75648139 )
						{
							cost_res[0] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.wood * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.wood * town->guards.count[i]);
							cost_res[1] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.mercury * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.mercury * town->guards.count[i]);
							cost_res[2] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.ore * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.ore * town->guards.count[i]);
							cost_res[3] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.sulfur * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.sulfur * town->guards.count[i]);
							cost_res[4] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.crystal * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.crystal * town->guards.count[i]);
							cost_res[5] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.jems * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.jems * town->guards.count[i]);
							cost_res[6] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.gold * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.gold * town->guards.count[i]);

							if (me->resourses.wood >= cost_res[0] && me->resourses.mercury >= cost_res[1] && me->resourses.ore >= cost_res[2] && me->resourses.sulfur >= cost_res[3] && me->resourses.crystal >= cost_res[4] && me->resourses.jems >= cost_res[5] && me->resourses.gold >= cost_res[6])
							{
								me->resourses.wood    -= cost_res[0];
								me->resourses.mercury -= cost_res[1];
								me->resourses.ore     -= cost_res[2];
								me->resourses.sulfur  -= cost_res[3];
								me->resourses.crystal -= cost_res[4];
								me->resourses.jems    -= cost_res[5];
								me->resourses.gold    -= cost_res[6];

								int expo_deGrade = CrExpoSet_GetExp(&town->guards, i);					
								town->guards.type[i] = GetCreatureGrade(town->guards.type[i]);
								if ( expo_deGrade > 0 ) { 
									CrExpoSet_SetExp(&town->guards, i, expo_deGrade *75 /100);
								}
							}
						}
					}				
				}
			}
		}
		return true;
	}

return false;
}

// улучшение одного стека у героя
_bool_ Y_AutoGradeMonInTownOne_H(_Town_* town, int type, int i)
{
	_Player_* me = o_GameMgr->GetMe();
	if (!me->IsActive()) return false;

	_Hero_* hero = 0;
	ResetCostRes();

	if (type == 1)
			hero = o_GameMgr->GetHero(town->up_hero_id);
	else hero = o_GameMgr->GetHero(town->down_hero_id);

	if (me->id != hero->owner_id || hero->army.type[i] == -1 ) return false;

	if ( GetCreatureGrade(hero->army.type[i]) != -1 && hero->army.type[i] != 158 )  
	{
		if( town->IsBuildingBuilt(37 + o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].level, 1) )	
		{
			if ( (WOG_Option_174 == 260179083 && town->type == o_pCreatureInfo[hero->army.type[i]].town) || WOG_Option_174 == 75648139 )
			{
				cost_res[0] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.wood * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.wood * hero->army.count[i]);
				cost_res[1] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.mercury * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.mercury * hero->army.count[i]);
				cost_res[2] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.ore * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.ore * hero->army.count[i]);
				cost_res[3] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.sulfur * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.sulfur * hero->army.count[i]);
				cost_res[4] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.crystal * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.crystal * hero->army.count[i]);
				cost_res[5] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.jems * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.jems * hero->army.count[i]);
				cost_res[6] = (o_pCreatureInfo[GetCreatureGrade(hero->army.type[i])].cost.gold * hero->army.count[i]) - (o_pCreatureInfo[hero->army.type[i]].cost.gold * hero->army.count[i]);

				if (me->resourses.wood >= cost_res[0] && me->resourses.mercury >= cost_res[1] && me->resourses.ore >= cost_res[2] && me->resourses.sulfur >= cost_res[3] && me->resourses.crystal >= cost_res[4] && me->resourses.jems >= cost_res[5] && me->resourses.gold >= cost_res[6])
				{
					me->resourses.wood    -= cost_res[0];
					me->resourses.mercury -= cost_res[1];
					me->resourses.ore     -= cost_res[2];
					me->resourses.sulfur  -= cost_res[3];
					me->resourses.crystal -= cost_res[4];
					me->resourses.jems    -= cost_res[5];
					me->resourses.gold    -= cost_res[6];

					int expo_deGrade = CrExpoSet_GetExp(&hero->army, i);					
					hero->army.type[i] = GetCreatureGrade(hero->army.type[i]);
					if ( expo_deGrade > 0 ) { 
						CrExpoSet_SetExp(&hero->army, i, expo_deGrade *75 /100);
					}
					return true;
				}
			}
		}
	}
 return false;
}

// улучшение одного стека в городе без героя
_bool_ Y_AutoGradeMonInTownOne_T(_Town_* town, char type, char i)
{
	_Player_* me = o_GameMgr->GetMe();
	if (!me->IsActive()) return false;

	ResetCostRes();

	if (me->id != town->owner_id || town->guards.type[i] == -1 ) return false;

	if ( GetCreatureGrade(town->guards.type[i]) != -1 && town->guards.type[i] != 158 )  
	{
		if( town->IsBuildingBuilt(37 + o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].level, 1) && town->type == o_pCreatureInfo[town->guards.type[i]].town )	
		{
			cost_res[0] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.wood * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.wood * town->guards.count[i]);
			cost_res[1] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.mercury * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.mercury * town->guards.count[i]);
			cost_res[2] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.ore * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.ore * town->guards.count[i]);
			cost_res[3] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.sulfur * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.sulfur * town->guards.count[i]);
			cost_res[4] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.crystal * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.crystal * town->guards.count[i]);
			cost_res[5] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.jems * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.jems * town->guards.count[i]);
			cost_res[6] = (o_pCreatureInfo[GetCreatureGrade(town->guards.type[i])].cost.gold * town->guards.count[i]) - (o_pCreatureInfo[town->guards.type[i]].cost.gold * town->guards.count[i]);

			if (me->resourses.wood >= cost_res[0] && me->resourses.mercury >= cost_res[1] && me->resourses.ore >= cost_res[2] && me->resourses.sulfur >= cost_res[3] && me->resourses.crystal >= cost_res[4] && me->resourses.jems >= cost_res[5] && me->resourses.gold >= cost_res[6])
			{
				me->resourses.wood    -= cost_res[0];
				me->resourses.mercury -= cost_res[1];
				me->resourses.ore     -= cost_res[2];
				me->resourses.sulfur  -= cost_res[3];
				me->resourses.crystal -= cost_res[4];
				me->resourses.jems    -= cost_res[5];
				me->resourses.gold    -= cost_res[6];

				int expo_deGrade = CrExpoSet_GetExp(&town->guards, i);					
				town->guards.type[i] = GetCreatureGrade(town->guards.type[i]);
				if ( expo_deGrade > 0 ) { 
					CrExpoSet_SetExp(&town->guards, i, expo_deGrade *75 /100);
				}
				return true;
			}	
		}
	}
 return false;
}

// автоулучшение существ в городе по ЛКМ+A
_int_ __stdcall Y_AutoGradeMonInTown(LoHook* h, HookContext* c)
{
	if ( GetKeyState(65)<0 )
	{
		_bool_ done = 0;
		_Town_* town = o_TownMgr->town; 
		int klick_id = c->edi; 

		if(klick_id == 125 && town->down_hero_id != -1) 
			done = Y_AutoGradeMonInTownFunc(town, 2);
		if(klick_id == 123 && town->up_hero_id != -1) 
			done = Y_AutoGradeMonInTownFunc(town, 1);
		if(klick_id == 123 && town->up_hero_id == -1) 
			done = Y_AutoGradeMonInTownFunc(town, 0);		

		if( klick_id >= 140 && klick_id <= 146 && town->down_hero_id != -1 )
			done = Y_AutoGradeMonInTownOne_H(town, 2, klick_id - 140);
		if( klick_id >= 115 && klick_id <= 121 && town->up_hero_id != -1 )
			done = Y_AutoGradeMonInTownOne_H(town, 1, klick_id - 115);
		if( klick_id >= 115 && klick_id <= 121 && town->up_hero_id == -1 )
			done = Y_AutoGradeMonInTownOne_T(town, 1, klick_id - 115);

		// обновить экран города
		if (done)
		{
			CALL_1(void, __thiscall, 0x5D5930, o_TownMgr); 	// отключить жёлтую обводку 
			CALL_1(void, __thiscall, 0x5D5810, o_TownMgr);	// обновить экран города
			c->return_address = 0x5D460F;
			return NO_EXEC_DEFAULT;	
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
			_PI = _P->CreateInstance("WoG_AutoGradeMonInTown"); 

			// автоулучшение существ в битве по ЛКМ+Shift на иконке героев
			_PI->WriteLoHook(0x5D45FD, Y_AutoGradeMonInTown);
			// _PI->WriteHiHook(0x5D3640, SPLICE_, EXTENDED_, THISCALL_, Y_ProcessTownScreen);			

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