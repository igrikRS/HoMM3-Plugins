#include "..\..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

#define BattleSpeed (*(_int_*)0x69883C)
#define BATTON_ID 240

// константы боевых скоростей
const float battleSpeedCoef[10] = { 1.00f, 0.60f, 0.40f, 0.20f, 0.10f, 0.08f, 0.06f, 0.040f, 0.020f, 0.010f };

int __stdcall Y_Dlg_BattleOptions_Proc(HiHook* hook, _EventMsg_* msg)
{
	_Dlg_* dlg = o_CombatOptionsDlg; 

	if (msg->type == MT_MOUSEBUTTON)
	{
		if (msg->subtype == MST_RBUTTONDOWN)
		{
			if (msg->item_id >= BATTON_ID && msg->item_id <= (BATTON_ID +9) )
			{
				// описания скорости кнопок (подгоняем под стандарные 3 описания)
				msg->item_id = ( (msg->item_id - BATTON_ID -1) / 3 +230 );
			}
		}
		if (msg->subtype == MST_LBUTTONDOWN)
		{
			int ID = msg->item_id;
			if ( msg->item_id >= BATTON_ID && msg->item_id <= (BATTON_ID +9) )
			{
				for (int i = 0; i < 10; i++)
				{
					((_DlgStaticDef_*)dlg->GetItem(BATTON_ID + i))->Hide_ButStayEnable();
				}
				BattleSpeed = ID - BATTON_ID;
				((_DlgStaticDef_*)dlg->GetItem(BATTON_ID + BattleSpeed))->Show_ButStayEnable();

				dlg->Redraw();
			}
		}
	}

	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}

_Dlg_* __stdcall Y_Dlg_BattleOptions_Create(HiHook* hook, _Dlg_* dlg) 
{
	dlg = CALL_1(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg);

	// dlg->AddItem(_DlgStaticPcx8_::Create(25, 222, 250, "SYSBS.pcx" ));

	for (int i = 0; i < 10; i++)
	{
		dlg->AddItem(_DlgStaticDef_::Create(29 + 19*i, 225, 18, 36, BATTON_ID +i, "SYSLB.DEF", i, 0, 0));
		((_DlgStaticDef_*)dlg->GetItem(BATTON_ID + i))->Hide_ButStayEnable();  // сразу же скрываем кнопки (но они работают)
	}
	((_DlgStaticDef_*)dlg->GetItem(BATTON_ID + BattleSpeed))->Show_ButStayEnable();

	return dlg;	
}

void StartPlugin()
{
	const int pBSpeed = (int)&battleSpeedCoef;

	_PI->WriteDword(0x43F255 +3, pBSpeed); // Battle_Stack_DrawShot_Bullet
	_PI->WriteDword(0x43F392 +3, pBSpeed); // Battle_Stack_DrawShot_Bullet
	_PI->WriteDword(0x441B2A +3, pBSpeed); // BattleStack_AttackMelle
	_PI->WriteDword(0x441BDC +3, pBSpeed); // BattleStack_AttackMelle
	_PI->WriteDword(0x4466F4 +3, pBSpeed); // BattleStack_DrawMoving
	_PI->WriteDword(0x44670B +3, pBSpeed); // BattleStack_DrawMoving
	_PI->WriteDword(0x466CD4 +3, pBSpeed); // BattleMgr_RemoveDead
	_PI->WriteDword(0x467758 +3, pBSpeed); // BattleMgr_MaybeFlyingArrow
	_PI->WriteDword(0x467BCA +3, pBSpeed); // BattleMgr_DrawMagicMissileMoving
	_PI->WriteDword(0x468093 +3, pBSpeed); // BattleMgr_Unk
	_PI->WriteDword(0x473997 +3, pBSpeed); // BattleMgr_Unk
	_PI->WriteDword(0x473A49 +3, pBSpeed); // BattleMgr_Proc
	_PI->WriteDword(0x494662 +3, pBSpeed); // BattleMgr_AnimationStep
	_PI->WriteDword(0x4B49AC +3, pBSpeed); // BattleStack_0x4B47A0
	_PI->WriteDword(0x5A601C +3, pBSpeed); // BattleStack_RayShooting
	_PI->WriteDword(0x5A6813 +3, pBSpeed); // BattleStack_0x5A6670
	_PI->WriteDword(0x5A7FE2 +3, pBSpeed); // BattleStack_CastSpellEarthquake
	_PI->WriteDword(0x5A8148 +3, pBSpeed); // BattleStack_CastSpellEarthquake

	////// диалог системных опций (id 240 - 249)
	////_PI->WriteDword(0x46F07A +1, bttn_id);		// (начало цикла)
	////_PI->WriteDword(0x46F096 +2, bttn_id +9);		// (конец цикла)
	////_PI->WriteDword(0x46F0A9 +2, bttn_id);		// передать команду элементу

	//// колбек системных опций 1
	//_PI->WriteDword(0x46F3EF +1, BATTON_ID);		// (начало цикла)
	//_PI->WriteDword(0x46F40B +2, BATTON_ID +9);	// (конец цикла)
	//_PI->WriteDword(0x46F41D +1, BATTON_ID);		// передать команду элементу
	//								   
	// // колбек системных опций 2
	//_PI->WriteDword(0x46F76E +1, -BATTON_ID);
	//_PI->WriteDword(0x46F778 +1, BATTON_ID);
	//_PI->WriteDword(0x46F7A7 +2, BATTON_ID);

	// пропускаем создание кнопок в диалоге системных опций битвы
	_PI->WriteHexPatch(0x46E1F0, "E9 F7000000"); // JMP 0046E2EC
	_PI->WriteHexPatch(0x46F07A, "EB 44"); // JMP SHORT 0046F0C0

	// добавление новых кнопок в диалог системных опций
	_PI->WriteHiHook(0x46DF00, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleOptions_Create);
	_PI->WriteHiHook(0x46F300, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleOptions_Proc);

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_DlgMainMenu_Proc(HiHook* hook, _EventMsg_* msg)
{
	if ( msg->type == MT_KEYDOWN) 
	{
		if ( msg->subtype == HK_F6 ) 
		{
			o_PauseVideo();
			_Dlg_* dlg = (_Dlg_*)o_New(104);
			CALL_1(int, __thiscall, 0x46DF00, dlg);
			CALL_1(int, __thiscall, 0x46F2D0, dlg);
			CALL_1(int, __thiscall, 0x46F250, dlg);

			o_WndMgr->result_dlg_item_id = -1;
			o_ContinueVideo();
		}

		//if ( msg->subtype == HK_F6 ) // тестовая кнопка
		//{
		//	int HD_Version = _P->VarValue<_dword_>("HD.Version.Dword");
		//	sprintf(o_TextBuffer, "HD_Version = %d", HD_Version );
		//	b_MsgBox(o_TextBuffer, 1);
		//}
	}
	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	static _bool_ pluginOn = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!pluginOn)
		{
			pluginOn = true;

			_P = GetPatcher();
			_PI = _P->CreateInstance("BattleSpeed");

			//системное меню в главном меню
			_PI->WriteHiHook(0x4FBDA0, SPLICE_, EXTENDED_, THISCALL_, Y_DlgMainMenu_Proc);
			_PI->WriteHiHook(0x4D5B50, SPLICE_, EXTENDED_, THISCALL_, Y_DlgMainMenu_Proc);
			_PI->WriteHiHook(0x456FD0, SPLICE_, EXTENDED_, THISCALL_, Y_DlgMainMenu_Proc);

			StartPlugin();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
