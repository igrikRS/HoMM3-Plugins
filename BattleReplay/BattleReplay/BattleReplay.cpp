#include "..\..\..\include\homm3.h"

#include "..\..\..\include\era.h"
using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

char* PLUGIN_NAME = "igrik.BattleReplay";

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

char myString[1024];
#define MyString (char*)myString

#define hdv(type, name) _P->VarValue<type>((_cstr_)(name))

#define DLG_OK 30722
#define DLG_CANCEL 30723

#define o_Dlg_BattleResults (*(_int_*)0x69500C)

#define BACall_Day (*(_int_*)0x79F0B8)
#define BACall_Turn (*(_int_*)0x79F0BC)

#define o_TimeClick (*(_int_*)0x6977D4)
_bool_ isNeedReplay, ifCanReplay;

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

int __stdcall Y_SkipAddExp(LoHook* h, HookContext* c)
{  // 0x477254 (первый выполняемый хук)
	if (isNeedReplay) { 
		c->return_address = 0x477303;
		return NO_EXEC_DEFAULT;
	}
	return EXEC_DEFAULT; 
}

int __stdcall Y_SkipRedrawAdvMap(LoHook* h, HookContext* c)
{ // 0x4173E2 (второй выполняемый хук)
    if (isNeedReplay) { 
        c->return_address = 0x41742D;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT; 
}

int __stdcall Y_AfterBattle_SkipAll(LoHook* h, HookContext* c)
{  // 0x4ADFE8 (последний выполняемый хук)
    if (isNeedReplay) { 
        c->return_address = 0x4AE67C;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT; 
}


////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

_Dlg_* __stdcall Y_Dlg_BattleResults_Create(HiHook* hook, _Dlg_* dlg, _Hero_* heroA, _Hero_* heroD, _int_ dlgDef, _int_ sideWin, _int_ a6, _int_ allHPLoserLose)
{
	CALL_7(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg, heroA, heroD, dlgDef, sideWin, a6, allHPLoserLose);

	if (ifCanReplay) {
		dlg->AddItem(_DlgStaticPcx8_::Create(20, 506, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(21, 507, 64, 30, DLG_CANCEL, "iCancel.def", 0, 1, 0, 1, 2)); 
	}
	return dlg;
}

int __stdcall Y_Dlg_BattleResults_Proc(HiHook* hook, _EventMsg_* msg)
{
	if (ifCanReplay) {
		isNeedReplay = false;
		if (msg->type == MT_MOUSEBUTTON && msg->type != MT_KEYDOWN) {
			if (msg->subtype == MST_LBUTTONCLICK) {
				if (msg->item_id == DLG_CANCEL) {
					o_TimeClick = 15000; 
					msg->item_id == DLG_OK;
					isNeedReplay = true;
				}
			}
		}
	}
	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

int __stdcall Y_ReplayBattle(HiHook* hook, _AdvMgr_* advMng, _dword_ MixedPos, _Hero_* HrA, _Army_* MArrA, _int_ OwnerD, _dword_ townD, _Hero_* HrD, _Army_* MArrD, _int_ Pv3, _dword_ Pv2, _dword_ Pv1)
{	
	int ret = 0; 
	ifCanReplay = false;   

	_GameMgr_* gm = o_GameMgr;
	char isAHuman = 0;
	char isDHuman = 0;
	if (HrA) { 
		isAHuman = gm->GetPlayer(HrA->owner_id)->IsHuman(); 
	}

	if (OwnerD >= 0 ) {	
		isDHuman = gm->GetPlayer(OwnerD)->IsHuman(); 
	}

	if ( o_GameMgrType == 2 && isAHuman && isDHuman ) { 
		// b_MsgBox("Переигрывание битвы ОТКЛЮЧЕНО", 1);
		return CALL_11(int, __thiscall, hook->GetDefaultFunc(), advMng, MixedPos, HrA, MArrA, OwnerD, townD, HrD, MArrD, Pv3, Pv2, Pv1);
	} 

	ifCanReplay = true;
	_int_ saveParamQB = o_QuickBattle;

	_Hero_* HrAS = 0;
	_Hero_* HrDS = 0;
	_Npc_* npcA = 0;
	_Npc_* npcAS = 0;
	_bool_ isNpcASCreate = false;
	_bool_ isNpcDSCreate = false;
	_Npc_* npcD = 0;
	_Npc_* npcDS = 0;
	_Army_* armyAS = 0;
	_Army_* armyDS = 0;

	if (HrA) {
        o_AdvMgr->HeroActive_DeMobilize();
		HrAS = (_Hero_*)o_New(1170);
		MemCopy(HrAS, HrA, 1170);
		npcA = GetNpc(HrA->id);                       
		if (npcA->on == 1 && npcA->alive != 1 ) {
			npcAS = (_Npc_*)o_New(296);
			MemCopy(npcAS, npcA, 296);             
			isNpcASCreate = true;
		}
	} else { 
		armyAS = (_Army_*)o_New(56);
		MemCopy(armyAS, MArrA, 56);
	}

	if (HrD) {
		HrDS = (_Hero_*)o_New(1170);
		MemCopy(HrDS, HrD, 1170);                    
		npcD = GetNpc(HrD->id);        
		if (npcD->on == 1 && npcD->alive != 1) {
			npcDS = (_Npc_*)o_New(296);
			MemCopy(npcDS, npcD, 296);             
			isNpcDSCreate = true;
		}
	} else { 
		armyDS = (_Army_*)o_New(56);
		MemCopy(armyDS, MArrD, 56);
	}

	// начало цикла повтора битвы
	do {
		if (HrA) {
			MemCopy(HrA, HrAS, 1170);
			if ( isNpcASCreate ) { 
				if ( npcAS->on == 1 && npcAS->alive != 1 ) {
					MemCopy(npcA, npcAS, 296);
				}
			}
		} else { 
			MemCopy(MArrA, armyAS, 56); 
		}

		if (HrD) {
			MemCopy(HrD, HrDS, 1170);
			if ( isNpcDSCreate ) {
				if ( npcDS->on == 1 && npcDS->alive != 1 ) {
					MemCopy(npcD, npcDS, 296);
				}
			}
		} else {
			MemCopy(MArrD, armyDS, 56);
		}

		if (isNeedReplay) {	//благодоря этой проверке пропускаем первый вызов битвы
			// фикс призывов от опыта стеков (вызов существ в 1м раунде битвы)
			o_BattleMgr->round = 0; 
			BACall_Day = -1;
			BACall_Turn = -1; 

			o_QuickBattle = 0;						
			hdv(_bool_, "HD.QuickCombat") = 0;	
			ERM_FU_CALL(870520); 
			FireEvent("OnBeforeBattleReplay", NULL, 0);
		}
		// нужен, если выходим в главное меню 
        // через кнопку HD в диалоге настроек битвы
        isNeedReplay = false;

		// непосредственный вызов битвы
		ret = CALL_11(int, __thiscall, hook->GetDefaultFunc(), advMng, MixedPos, HrA, MArrA, OwnerD, townD, HrD, MArrD, Pv3, Pv2, Pv1);

		if ( isNeedReplay ) {
			FireEvent("OnAfterBattleReplay", NULL, 0);
			ERM_FU_CALL(870530); 
		}
	} while ( isNeedReplay );

	// очищаем память
	if (HrAS) {	o_Delete(HrAS); }
	if (HrDS) {	o_Delete(HrDS);	}

	if (npcAS) { o_Delete(npcAS); }
	if (npcDS) { o_Delete(npcDS); }

	if (armyAS) { o_Delete(armyAS); }
	if (armyDS) { o_Delete(armyDS);	}

	o_QuickBattle = saveParamQB;
	hdv(_bool_, "HD.QuickCombat") = saveParamQB; 

	ifCanReplay = false;

	return ret; 
}

// Астральный дух возвращает существ только после всех переигровок
void __stdcall Y_NPC_Astral_AfterBattle(HiHook* hook)
{
    if (!isNeedReplay)
        CALL_0(void, __cdecl, hook->GetDefaultFunc() );
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

void HooksInit()
{
    // делаем кнопку ОТМЕНА и управляем ею диалоге результатов битвы
    _PI->WriteHiHook(0x46FE20, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Create);
    _PI->WriteHiHook(0x4716E0, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Proc);

    // пропускаем обновление мира сразу после битвы
    _PI->WriteLoHook(0x4173E2, Y_SkipRedrawAdvMap);  
 
    // пропускаем всё сразу после битвы
    _PI->WriteLoHook(0x4ADFE8, Y_AfterBattle_SkipAll);  

    // пропускаем добавление опыта выигравшему
    _PI->WriteLoHook(0x477254, Y_SkipAddExp);         


    // убираем воговский вызов CALL_BATTLE (0x4AD160)
    _PI->WriteHexPatch(0x75AEB0, "E8 AB22D5FF 90 90");	    
    // вызываем CALL_BATTLE сами столько раз, сколько нужно
    // попутно сохраняем структуры нейтралов, героев и командиров   
    _PI->WriteHiHook(0x75AEB0, CALL_, SAFE_, THISCALL_, Y_ReplayBattle);				

    // Есть баг: Астральный дух возвращает существ перед первой переигровкой
    // С этим хуком Астральный дух возвращает существ только после всех переигровок
    _PI->WriteHiHook(0x76C616, SPLICE_, EXTENDED_, CDECL_, Y_NPC_Astral_AfterBattle);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

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

            _P = GetPatcher();
            _PI = _P->CreateInstance(PLUGIN_NAME); 

            // подтягиваем ERA
            ConnectEra();

            // инициализация всех хуков
            HooksInit();

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
