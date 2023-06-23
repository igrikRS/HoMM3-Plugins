#include "..\..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

#define BattleSpeed (*(_int_*)0x69883C)
#define BUTTON_ID 240
#define DEFAULT_BUTTON_ID 201

// константы боевых скоростей
// const float battleSpeedCoef[10] = { 1.00f, 0.61f, 0.43f, 0.21f, 0.090f, 0.075f, 0.060f, 0.045f, 0.030f, 0.010f };

// * new battle speed coefficients from SOD_SP
float battleSpeedCoef[10] =
{
	1.000f, 0.630f, 0.400f, // original speed coefficients
	0.300f,                 // new speed coefficient
	0.200f, 0.100f,	        // old SoD_SP turbo speed coefficients
	0.075f,                 // new speed coefficient
	0.050f,                 // old SoD_SP turbo speed coefficient
	0.025f, 0.010f          // new speed coefficients
};

int __stdcall Y_Dlg_BattleOptions_Proc(HiHook* hook, _EventMsg_* msg)
{
	_Dlg_* dlg = o_CombatOptionsDlg; 

	if (msg->type == MT_MOUSEBUTTON)
	{
        // ПКМ по элементу диалога 
		if (msg->subtype == MST_RBUTTONDOWN)
		{
			if (msg->item_id >= BUTTON_ID && msg->item_id <= (BUTTON_ID +9) )
			{
				// описания скорости кнопок (подгоняем под стандарные 3 описания)
				msg->item_id = ( (msg->item_id - BUTTON_ID -1) / 3 +230 );
			}
		}

        // ЛКМ по элементу диалога 
		if (msg->subtype == MST_LBUTTONDOWN)
		{
			int ID = msg->item_id;
			if ( ID >= BUTTON_ID && ID <= (BUTTON_ID +9) )
			{
				for (int i = 0; i < 10; i++)
				{
					((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID + i))->Hide_ButStayEnable();
				}
				BattleSpeed = ID - BUTTON_ID;
				((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID + BattleSpeed))->Show_ButStayEnable();

				dlg->Field<byte>(0x4C) = 1; // в диалоге нужно сохранить параметры
				dlg->Redraw();
			}
		}

        // ЛКМ по кнопке диалога 
        if (msg->subtype == MST_LBUTTONCLICK)
        {
            // default button
            if (msg->item_id == DEFAULT_BUTTON_ID) {
				for (int i = 0; i < 10; i++)
				{
					((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID + i))->Hide_ButStayEnable();
				}
				BattleSpeed = 1;
				((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID +1))->Show_ButStayEnable();

				dlg->Field<byte>(0x4C) = 1;
				dlg->Redraw();
            }

        }
	}

	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}

_Dlg_* __stdcall Y_Dlg_BattleOptions_Create(HiHook* hook, _Dlg_* dlg) 
{
	dlg = CALL_1(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg);

	dlg->AddItem(_DlgStaticPcx8_::Create(25, 222, 250, "BattleSpeed.pcx" ));

	for (int i = 0; i < 10; i++)
	{
		dlg->AddItem(_DlgStaticDef_::Create(29 + 19*i, 225, 18, 36, BUTTON_ID +i, "SYSLB.DEF", i, 0, 0));
		((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID + i))->Hide_ButStayEnable();  // сразу же скрываем кнопки (но они работают)
	}
	((_DlgStaticDef_*)dlg->GetItem(BUTTON_ID + BattleSpeed))->Show_ButStayEnable();

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

	_PI->WriteByte(0x50B556 +2, 9); // NormalizeRegistry ( if ( BattleSpeed < 0 || BattleSpeed > 2->9 ))

	// пропускаем создание кнопок в диалоге системных опций битвы
	_PI->WriteHexPatch(0x46E1F0, "E9 F7000000"); // JMP 0046E2EC
	_PI->WriteHexPatch(0x46F07A, "EB 44"); // JMP SHORT 0046F0C0


    // default button
    _PI->WriteDword(0x46F41D +1, BUTTON_ID);
    _PI->WriteDword(0x46F3EF +1, BUTTON_ID);
    _PI->WriteDword(0x46F40B +2, BUTTON_ID +10);

	// добавление новых кнопок в диалог системных опций
	_PI->WriteHiHook(0x46DF00, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleOptions_Create);
	_PI->WriteHiHook(0x46F300, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleOptions_Proc);

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
