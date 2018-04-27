///////////////////////////////////////////////////////////////////////////////////////////////////
// Диалог повышения уровня героя, состоящий из 3х элементов для выбора (стандартно 2 элемента)   //
//        Третий навык всегда ищется из новых. Если доступных новых навыков больше нет,          //
//                То диалог переключается в стандартный режим (выбор из 2х навыков)              //
//                                                                                               //
//                               Автор: [igrik]                                                  //
//                             Дата:  30.08.2017г.                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

static _bool_ skill_3_on = 0;
static _bool_ click_on = 0;

int Y_HeroLvlUpDlg_Get_3_SSkill(_Hero_* hero, int ss_1, int ss_2)
{
	char ssHero_1 = hero->second_skill[ss_1]; // запомним ss_1
					hero->second_skill[ss_1] = 3;

	char ssHero_2 = hero->second_skill[ss_2]; // запомним ss_2
					hero->second_skill[ss_2] = 3;

	skill_3_on = 0;
	int ss_res = CALL_4 (int, __fastcall, 0x4DAF70, hero, 0, 1, -1); // ищем новый навык (когда есть еще не изученные)
	/* if(ss_res == -1 ) // если таковые отсутствуют
	{
		ss_res = CALL_4 (int, __fastcall, 0x4DAF70, hero, 1, 3, -1); // ищем из существующих для апгрейда
	} */ 
	if(ss_res != -1 ){
		skill_3_on = 1; // указывает что 3тий навык найден и мы включаем окно выбора из 3х навыков
	}

	// тут не хватает еще куска, 
	// где происходит последнее предложение выбора мудрости
	// 0x4DAC2A

	hero->second_skill[ss_1] = ssHero_1; // вернули уровни навыков
	hero->second_skill[ss_2] = ssHero_2;
	return ss_res;
}

_Dlg_* Y_HeroLvlUpDlg_Create_3x(HiHook* hook, _Dlg_* this_, _Hero_* hero, int a3, int a4, int a5)
{	
	click_on = 0; // обнуление переменных
	skill_3_on = 0;

	int SSkill = Y_HeroLvlUpDlg_Get_3_SSkill(hero, (a4-3)/3, (a5-3)/3); // ищем третий навык	

	if (skill_3_on == 0){ // если 3й навык НЕ нашли
		_PI->WriteByte((0x4F9528 + 1), 124);	// стандартные координаты "secskill.def"
		_PI->WriteDword((0x4F9581 + 1), 222);

		_PI->WriteByte((0x4F944F + 1), 122); 	// стандартные координаты обводки
		_PI->WriteDword((0x4F94BB + 1), 220);

		_PI->WriteByte((0x4F95FE + 1), 102);	// стандартные координаты названия навыков снизу
		_PI->WriteDword((0x4F967B + 1), 200);

		CALL_5(_Dlg_*, __thiscall, hook->GetDefaultFunc(), this_,hero,a3,a4,a5);
	}
	else { // если навык нашли
		_PI->WriteByte((0x4F9528 + 1), 90);		// смещаем координаты "secskill.def"
		_PI->WriteDword((0x4F9581 + 1), 174);

		_PI->WriteByte((0x4F944F + 1), 88); 	// смешаем координаты обводки
		_PI->WriteDword((0x4F94BB + 1), 172);

		_PI->WriteByte((0x4F95FE + 1), 70);		// смещаем координаты названия навыков снизу
		_PI->WriteDword((0x4F967B + 1), 152);
		
		CALL_5(_Dlg_*, __thiscall, hook->GetDefaultFunc(), this_, hero, a3, a4, a5);

		this_->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 257, 325, 46, 46, 7013, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		this_->GetItem(7013)->SendCommand(6, 4);
		this_->AddItem(_DlgStaticDef_::Create(258, 326, 7012, "secskill.def", (3 + SSkill*3 + hero->second_skill[SSkill]), 0, 16));

		sprintf(o_TextBuffer, "%s %s", *(int*)((*(int*)((*(int*)0x6A7F68) +32))	+ 4*hero->second_skill[SSkill]), *(int*)(*(int*)0x67DCF0 + SSkill*16)); 
		this_->AddItem(_DlgStaticText_::Create(236, 375, 87, 40, o_TextBuffer, "smalfont.fnt", 1, 7014, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));  
		
		sprintf(o_TextBuffer, "%s либо %s %s.", ((_DlgStaticText_*)this_->GetItem(2005))->text, 
												/* o_GENRLTXT_TXT->GetString(5), */  
												*(int*)((*(int*)((*(int*)0x6A7F68) +32)) + 4*hero->second_skill[SSkill]), 
												*(int*)(*(int*)0x67DCF0 + SSkill*16) );

		((_DlgStaticText_*)this_->GetItem(2005))->SetText(o_TextBuffer);
	}
	
	return this_;
}

int __stdcall Y_HeroLvlUp_3x_Proc(HiHook* hook, _Struct_* this_, _EventMsg_* msg)
{
	if (skill_3_on == 0) goto EXIT_FUNK;

	_Dlg_* dlg = *(_Dlg_**)0x699684;

	if (msg->type == MT_MOUSEBUTTON)
	{
		if (msg->subtype == MST_LBUTTONDOWN)
		{
			if ( click_on == 1 && msg->item_id == 7012 )
			{
				_DlgItem_* hero_forger = dlg->GetItem(7000); // типо структура героя (обманка) ->width (это смещение +26)
				CALL_3(int, __thiscall, 0x4E2540, o_GameMgr->GetHero(hero_forger->width), (((_DlgStaticDef_*)dlg->GetItem(7012))->def_frame_index -3)/3, 1); 
				return CALL_2(int, __thiscall, hook->GetDefaultFunc(), this_, msg->Set(MT_MOUSEBUTTON, MST_LBUTTONCLICK, 30722) );			
			}
		}

		if (msg->subtype == MST_LBUTTONCLICK)
		{
			if (click_on == 1 && msg->item_id == 30722) // нажата кнопка ок когда выбран 3й навык (в желтой обводке)
			{
				_DlgItem_* hero_forger = dlg->GetItem(7000); // типо структура героя (обманка) ->width (это смещение +26)
				CALL_3(int, __thiscall, 0x4E2540, o_GameMgr->GetHero(hero_forger->width), (((_DlgStaticDef_*)dlg->GetItem(7012))->def_frame_index -3)/3, 1);
				goto EXIT_FUNK;
			}	

			if (msg->item_id == 7012)
			{	
				dlg->GetItem(2012)->SendCommand(6, 4); dlg->GetItem(2012)->Draw();
				dlg->GetItem(2013)->SendCommand(6, 4); dlg->GetItem(2013)->Draw();
				dlg->GetItem(7013)->SendCommand(5, 4); dlg->GetItem(7013)->Draw();
				click_on = 1; // клик в 3й навык TRUE
				dlg->GetItem(30722)->SetEnabled(1);
				dlg->Redraw(TRUE);				
			}

			if (msg->item_id == 2010 || msg->item_id == 2011)
			{				
				dlg->GetItem(7013)->SendCommand(6, 4); dlg->GetItem(7013)->Draw();
				click_on = 0;  // клик в 3й навык FALSE
				dlg->Redraw(TRUE);
			}
		}
		
		if (msg->subtype == MST_RBUTTONDOWN)
		{
			if (msg->item_id == 7012 || msg->item_id == 7013)
			{
				char sSkill_Frame = ((_DlgStaticDef_*)dlg->GetItem(7012))->def_frame_index -3; // получить кадр дефа
				sprintf(o_TextBuffer, "%s", *(int*)(*(int*)0x67DCF0 + (sSkill_Frame / 3)*16 + (sSkill_Frame % 3 +1)*4));
				b_MsgBox(o_TextBuffer, MBX_RMC);
			}
			// sprintf(o_TextBuffer, "id = %d", msg->item_id);
			// b_MsgBox(o_TextBuffer, MBX_RMC);
		} 
	}
	EXIT_FUNK:
	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), this_, msg); 
}

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
			_PI = _P->CreateInstance("HD.HeroLvlUpDlg_3_SSkill");

			// увеличиваем кол-во доступных втор.навыков для изучения
			_PI->WriteByte((0x4DAFC8 + 6), 8); // повышение уровня_1
			_PI->WriteByte((0x4E2564 + 6), 8); // повышение уровня_2
			_PI->WriteByte((0x4A7DCE + 2), 8); // посещение хижины ведьмы

			_PI->WriteHiHook(0x4F9E10, SPLICE_, EXTENDED_, THISCALL_, Y_HeroLvlUp_3x_Proc);
			_PI->WriteHiHook(0x4F8F10, SPLICE_, EXTENDED_, THISCALL_, Y_HeroLvlUpDlg_Create_3x);
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

