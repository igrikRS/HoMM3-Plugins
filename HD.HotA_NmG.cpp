// Плагин с некоторыми корректировками для HoMM3 HotA 1.42
// автор: [igrik]

#include "stdafx.h" 
#include <stdio.h>

#include "..\..\include\homm3.h"
#include "AutoGrade.cpp"
#include "10SSkills.cpp"
#include "BlackMarket.cpp"
#include "New_SSkill.cpp"

// Объекты patcher_x86.
Patcher* _P;
PatcherInstance* _PI;

// поля битвы для объектов
char* pCmBkUtop = "CmBkUtop.pcx"; // утопия драконов	(25)
char* pCmBkGnom = "CmBkGnom.pcx"; // гномы				(16-1)
char* pCmBkNaga = "CmBkNaga.pcx"; // нага-банк			(16-5)
char* pCmBkUley = "CmBkUley.pcx"; // улей				(16-6)
char* pCmBkOsob = "CmBkOsob.pcx"; // особняк			(16-24)
char* pCmBkAlya = "CmBkAlya.pcx"; // алая башня			(16-26)
char* pCmBkPgst = "CmBkPgst.pcx"; // погост (зомби)		(16-26)
char* pCmBkCehh = "CmBkCehh.pcx"; // опытный цех		(16-30)
char* pCmBkZstv = "CmBkZstv.pcx"; // застава (собаки)	(16-31)

// показ морали и удачи в бою
int __stdcall HookOn_MoralLuckBonus(LoHook* h, HookContext* c)
{
	// отображение единиц морали
	if (*(int*)(c->edi+1256) < 1)  
		sprintf(o_TextBuffer, "%d", *(int*)(c->edi+1256));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->edi+1256));

	int bonMoral = CALL_12 (int, __thiscall, 6014624, CALL_1 (int, __cdecl, 6386834, 80), 46, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, -1, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx, *(int*)(c->ebx+8), 1, &bonMoral); 

	// отображение единиц удачи
	if (*(int*)(c->edi+1260) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->edi+1260));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->edi+1260));

	int bonLuck = CALL_12 (int, __thiscall, 6014624, CALL_1 (int, __cdecl, 6386834, 80), 99, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, -1, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx, *(int*)(c->ebx+8), 1, &bonLuck); 

	return EXEC_DEFAULT;
} 

// показ морали и удачи вне боя
int __stdcall HookOn_MoralLuckBonus2(LoHook* h, HookContext* c)
{
	// отображение единиц морали
	if (*(int*)(c->ebx+104) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->ebx+104));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->ebx+104));	

	int bonMoral = CALL_12 (int, __thiscall, 6014624, CALL_1 (int, __cdecl, 6386834, 80), 46, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, -1, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx+48, *(int*)(c->ebx+56), 1, &bonMoral);

	// отображение единиц удачи
	if (*(int*)(c->ebx+124) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->ebx+124));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->ebx+124));	

	int bonLuck = CALL_12 (int, __thiscall, 6014624, CALL_1 (int, __cdecl, 6386834, 80), 99, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, -1, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx+48, *(int*)(c->ebx+56), 1, &bonLuck);

	return EXEC_DEFAULT;
}


// Автокаст артефактов
int __stdcall ArtSpellAutocast1(LoHook* h, HookContext* c)
{
	int hero = c->edi;
	if( hero > 0) // if _hero_
	{
		if( CALL_2(_bool_, __thiscall, 0x4D9460, (void*) hero, 148) )  // HeroHasArt(_hero_, [щит_морской_славы]) 
			{
				if( CALL_6(_bool_, __thiscall, 0x5A43E0, o_BattleMgr, 27, 3, o_BattleMgr->current_side, 1, 2) )
				{
					CALL_7(void*, __thiscall, 0x5A0140, o_BattleMgr, 27, -1, 2, -1, 3, 50);  // щит
				}

				if( CALL_6(_bool_, __thiscall, 0x5A43E0, o_BattleMgr, 28, 3, o_BattleMgr->current_side, 1, 2) )
				{
					CALL_7(void*, __thiscall, 0x5A0140, o_BattleMgr, 28, -1, 2, -1, 3, 50);  // воздушный щит
				}
			}
	}
	return EXEC_DEFAULT;
} 


// Правильное описание автокаста артефактов
int __stdcall ArtSpellAutocast2(LoHook* h, HookContext* c)
{
	if( c->edx == 27 )	{
		c->eax = /* номер арта */ 148; // щит_морской_славы
		c->return_address = 0x5A8E7F;
		return NO_EXEC_DEFAULT;
	}

	if( c->edx == 28 )	{
		c->eax = /* номер арта */ 148; // щит_морской_славы
		c->return_address = 0x5A8E7F;
		return NO_EXEC_DEFAULT;
	}
	return EXEC_DEFAULT;
} 


// изменение фона битвы в утопии
char* __stdcall changeBackGroundBattle(HiHook* hook, _BattleMgr_* this_)
{
	char* ret = CALL_1(char*, __thiscall, hook->GetDefaultFunc(), this_);

	if (this_->town_fort_type <= 0){
		_int_ obj = this_->Field<_int_>(0x53BC);
		_int_ obj_type1 = CALL_1(int, __thiscall, 0x4FD220, obj);
		_word_ obj_type2 = *(short*)(obj +34);

		if( obj_type1 == 25 ){ // утопия драконов
			ret = pCmBkUtop;
			this_->Field<char>(0x53C5) = 1; // ставим расстановку юнитов как в банке (на случай если герой на героя нападает, когда один сидит в утопии)
		}

		if( obj_type1 == 16 ){ // банк
			switch (obj_type2) {
				case 1:  ret = pCmBkGnom; break;	// гномы
				case 5:  ret = pCmBkNaga; break;	// нага-банк
				case 6:  ret = pCmBkUley; break;	// улей (виверны)
				case 24: ret = pCmBkOsob; break;	// особняк (вампиры)
				case 26: ret = pCmBkAlya; break;	// алая башня (огн.лементали)
				case 29: ret = pCmBkPgst; break;	// погост (зомби)
				case 30: ret = pCmBkCehh; break;	// опытный цех (големы)
				case 31: ret = pCmBkZstv; break;	// застава (собаки)
				default: break;
			}		
		}
	}
	return ret;
} 

// открытие окон героев в битве по ПКМ на дефе героев
void __stdcall ShowHeroScreenInBattle(HiHook* hook, _BattleMgr_* this_, signed int BattleHex)
{
	if( BattleHex == 252 && this_->hero[0] > 0)
		CALL_4(void, __fastcall, 0x4E1A70, this_->hero[0]->id, 1, 1, 1);

	if( BattleHex == 253 && this_->hero[1] > 0 )
		CALL_4(void, __fastcall, 0x4E1A70, this_->hero[1]->id, 1, 1, 1);

	CALL_2(int, __thiscall, hook->GetDefaultFunc(), this_, BattleHex);
} 

// нажатие клавиши на карте приключений
_int_ __stdcall N_AdvMgr_KeyDown(HiHook* hook, _AdvMgr_* this_, _EventMsg_* msg, int x, int y, _ptr_ a5)
	{
		_int_ result = CALL_5(_int_, __thiscall, hook->GetDefaultFunc(), this_, msg, x, y, a5);
		if (result)
		{
			switch(msg->subtype)
			{
				case HK_Q: // или W
				{
					o_QuickBattle = 1 - o_QuickBattle;
					char* log_text="Быстрая битва: {ОТКЛ.}";
					if(o_QuickBattle)
						log_text="Быстрая битва: {ВКЛ.}";

					b_PrintToScreenLog(log_text); // написать в чат IF:L
					break;
				}				
				
				case HK_F6:
				case HK_W:
				{
					CALL_6(char, __thiscall, 0x4BEB60, o_GameMgr, "QuickSave", 1, 0, 1, 0);
					b_PrintToScreenLog("Быстрое сохранение.");
					break;
				}				

				default: 
				// sprintf(o_TextBuffer, "кнопка %d", msg->subtype); 
    			// b_MsgBox(o_TextBuffer, 1);
					break;
			}
		}
		return result;
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
			_PI = _P->CreateInstance("HotA.NmG"); 
 
			_PI->WriteHexPatch((0x4AC420), "E9 8E 00 00 00");	// запрет появления грейдженного стека у нейтралов (JMP 004AC4B3)
   	  	
			// цифровой показ морали и удачи
			_PI->WriteLoHook(0x5F3C43, HookOn_MoralLuckBonus);	// показ морали и удачи в бою
			_PI->WriteLoHook(0x5F439B, HookOn_MoralLuckBonus2);	// показ морали и удачи вне боя

			// отключение бесплатной дипломатии (бесплатно -> за золото)
			_PI->WriteDword((0x417236 + 1), 3);		// при ПКМ на нейтралах на карте приключений
			_PI->WriteHexPatch((0x4A755D), "EB");	// при нападении на нейтралов
			
			// изменение заднего фона битвы (утопия драконов и некоторые банки)
			_PI->WriteHiHook(0x4642B0, SPLICE_, EXTENDED_, THISCALL_, changeBackGroundBattle); 

			// Автокаст артефактов
			_PI->WriteLoHook(0x4650BF, ArtSpellAutocast1);
			// Правильное описание автокаста артефактов
			_PI->WriteLoHook(0x5A8E57, ArtSpellAutocast2);

			// открытие окон героев в битве по ПКМ на дефе героев
			_PI->WriteCodePatch(0x476340, "%n", 13); // 13 nop отменяем ПКМ для heroD
			_PI->WriteCodePatch(0x4763B6, "%n", 13); // 13 nop отменяем ПКМ для heroA
			_PI->WriteHiHook(0x4762F0, SPLICE_, EXTENDED_, THISCALL_, ShowHeroScreenInBattle); 

			// нажатие клавиши на карте приключений (вытянуто из HD-мода)
			_PI->WriteHiHook(0x4089DC, CALL_, EXTENDED_, THISCALL_, N_AdvMgr_KeyDown);	

			// центрирование изображения по ПКМ в городе на иконке найма войск (ранее уходило сильно влево)
			_PI->WriteHexPatch(0x5D47B3, "0F BF 57 18  8B 4F 24 B8  FF FF FF FF  90");

			// автоулучшение существ в городе
			_PI->WriteLoHook(0x5D45FD, Y_AutoGradeMonInTown);

			// 10 вторичных навыков и измененное окно героя
			start_10SecondSkills(_P, _PI);

			// при наступлении новой недели генерируем новые арты у торговцев
			_PI->WriteLoHook(0x4C8446, Y_NewWeek); 
			
			// усиленные вторичные навыки (мистицизм, поместья)
			Y_New_SSkill(_P, _PI);

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}