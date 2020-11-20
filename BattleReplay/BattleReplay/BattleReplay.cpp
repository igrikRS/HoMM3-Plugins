#include "..\..\..\include\homm3.h"

#include "..\..\..\include\era.h"
using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

#define hdv(type, name) _P->VarValue<type>((_cstr_)(name))

#define BACall_Day (*(_int_*)0x79F0B8)
#define BACall_Turn (*(_int_*)0x79F0BC)

#define o_WinBattleType (*(_int_*)0x695064)
#define o_Sound_BattleResult (*(char**)0x6701A8)

#define o_TimeClick (*(_int_*)0x6977D4)
_bool_ isNeedReplay, ifCanReplay;

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
		
		// непосредственный вызов битвы        
		ret = CALL_11(int, __thiscall, hook->GetDefaultFunc(), advMng, MixedPos, HrA, MArrA, OwnerD, townD, HrD, MArrD, Pv3, Pv2, Pv1);

		if ( isNeedReplay ) {
			FireEvent("OnAfterBattleReplay", NULL, 0);
			ERM_FU_CALL(870530); 
		}
	} while ( isNeedReplay );

    ifCanReplay = false; 

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

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

#define o_Dlg_BattleResults (*(_int_*)0x69500C)

_Dlg_* __stdcall Y_Dlg_BattleResults_Create(HiHook* hook, _Dlg_* dlg, _Hero_* heroA, _Hero_* heroD, _int_ dlgDef, _int_ sideWin, _int_ a6, _int_ allHPLoserLose)
{
	CALL_7(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg, heroA, heroD, dlgDef, sideWin, a6, allHPLoserLose);

	if ( ifCanReplay ) {
		dlg->AddItem(_DlgStaticPcx8_::Create(20, 506, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(21, 507, 64, 30, 30723, "iCancel.def", 0, 1, 0, 1, 2)); 
	}
	return dlg;
}

int __stdcall Y_Dlg_BattleResults_Proc(HiHook* hook, _EventMsg_* msg)
{
	if ( ifCanReplay ) {
		isNeedReplay = false;
		if (msg->type == MT_MOUSEBUTTON && msg->type != MT_KEYDOWN) {
			if (msg->subtype == MST_LBUTTONCLICK) {
				if (msg->item_id == 30723) {
					o_TimeClick = 15000; 
					msg->item_id == 30722;
					isNeedReplay = true;
				}
			}
		}
	}
	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}


////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

int __stdcall Y_SkipRedrawAdvMap(LoHook* h, HookContext* c)
{ // 0x4173E2 -> 0x41742D
	if (isNeedReplay) {
        c->return_address = 0x41742D;
        return NO_EXEC_DEFAULT;
	}
	return EXEC_DEFAULT; 
}


int __stdcall Y_AfterBattle_SkipAll(LoHook* h, HookContext* c)
{  // 0x4ADFE8 -> 0x4AE67C
	if (isNeedReplay) {
		c->return_address = 0x4AE67C;
		return NO_EXEC_DEFAULT;
	}
	return EXEC_DEFAULT; 
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 


void My_Dlg_BattleResults(_BattleMgr_* bm, int sideWinner, int sideLooser, int time)
{
    int townID = 0;
    if (bm->town)
        townID = bm->town->id; 

    // создаём список забранных артефактов
    _List_<_int32_> artsList;
    artsList.Creation = sideWinner;

    // считаем опыт за победу
    int expirience = 0;
    if ( sideWinner != -1 && bm->hero[sideWinner] ) {
        // BattleMgr_Side_Get_AfterBattleMgr_Experience
        CALL_3(int, __thiscall, 0x469F30, bm, sideWinner, (int)&expirience);

        // BattleMgr_LearnSpellsByEagleEye
        CALL_2(int, __thiscall, 0x469BC0, bm, sideWinner);

        // BattleMgr_AfterBattleMgr_GiveArts_AndCreateList        
        CALL_3(char, __thiscall, 0x469C50, bm, sideWinner, &artsList);
    }

    // выделяем память под диалог результатов битвы        
    _ptr_ dlg = o_New(0x50);

    // создаём диалог результатов битвы   Dlg_BattleResults_Create
    CALL_7(_dword_, __thiscall, 0x46FE20, dlg, bm->hero[0], bm->hero[1], sideLooser, sideWinner, townID, expirience);

    // играем звук победы или поражения
    CALL_4(void, __thiscall, 0x59AFB0, o_SoundMgr, (*(char**)(0x6701A8 + 4*o_WinBattleType)), 1, 0);

    // запускаем диалог результатов битвы Dlg_BattleResults_Run(&dlg)
    CALL_1(_dword_, __thiscall, 0x4716C0, dlg);

    // деструктор диалога результатов битвы Dlg_BattleResults_Destruct(&dlg)
    CALL_1(_dword_, __thiscall, 0x4715C0, dlg);

    // показываем выученные заклинания от орлинного глаза
    // и список забранных артефактов у побеждённого героя
    if ( !isNeedReplay && sideWinner != -1 && sideWinner == sideLooser && bm->hero[sideWinner] ) {

        // BattleMgr_EagleEye_ShowGiveSpell
        CALL_3(void, __thiscall, 0x476910, bm, sideWinner, time); 

        // BattleMgr_TakeArtsFromKilledHero
        CALL_2(void, __stdcall, 0x476BE0, &artsList, time);

        // Hero_GiveExperience
        CALL_4(int, __thiscall, 0x4E3620, bm->hero[sideWinner], expirience, bm->isHuman[sideWinner] == 0, 1);
        
    }

    // очищаем список артефактов
    artsList.DeleteAll();

    // и уничтожаем диалог полностью 
    o_Delete(dlg);
}


void __stdcall Y_BattleMgr_SetWinner(HiHook* hook, _BattleMgr_* bm, int sideWinner)
{
    // StopBINKVideoAndWait_AIL
    CALL_0(void, __cdecl, 0x4635C0);  

    if ( !bm->ShouldNotRenderBattle() )
        CALL_4(void, __thiscall, 0x4729D0, bm->dlg, o_NullString, 0, 0);


    // работаем с курсором мыши
    o_MouseMgr->Field<int>(0x38) = 0;
    o_MouseMgr->SetMouseCursor(6, 2);
    o_MouseMgr->MouseON(0);    

    int time = 15000;
    if ( !o_NetworkGame || *(int*)0x6977D8 )  {
        o_TimeClick = 0;
        time = 0;
    } else o_TimeClick = o_GetTime() + time;

    if ( o_AutoSolo )
        o_TimeClick = o_GetTime() + 2000;

    if ( sideWinner != -1 && bm->owner_id[sideWinner] != -1 
        && o_GameMgr->PlayerIsInGameHuman(bm->owner_id[sideWinner]) )
    {
        My_Dlg_BattleResults(bm, sideWinner, sideWinner, time);
    } else if ( !o_AutoSolo && bm->isHuman[0] ) {
        My_Dlg_BattleResults(bm, sideWinner, 1 - sideWinner, time);
    }  
  
    if ( !isNeedReplay )
        CALL_2(void, __thiscall, hook->GetDefaultFunc(), bm, sideWinner);

    // обнуляем время показа диалога
    o_TimeClick = 0;  
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

void HooksInit()
{
    // делаем кнопку ОТМЕНА и управляем ею диалоге результатов битвы
    _PI->WriteHiHook(0x46FE20, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Create);
    _PI->WriteHiHook(0x4716E0, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Proc);

    // создаём сами диалог результатов битвы, если isNeedReplay = true
    _PI->WriteHiHook(0x476DA0, SPLICE_, EXTENDED_, THISCALL_, Y_BattleMgr_SetWinner);

    // так как мы сами реализовали показ диалога результатов битвы
    // вырезаем его из игры вообще
    _PI->WriteJmp(0x4770F7, 0x477316);

    // пропускаем обновление мира сразу после битвы
    _PI->WriteLoHook(0x4173E2, Y_SkipRedrawAdvMap);  

    // пропускаем всё сразу после битвы
    _PI->WriteLoHook(0x4ADFE8, Y_AfterBattle_SkipAll);  

    // убираем воговский вызов CALL_BATTLE (0x4AD160)
    _PI->WriteHexPatch(0x75AEB0, "E8 AB22D5FF 90 90");

    // вызываем CALL_BATTLE сами столько раз, сколько нужно
    // попутно сохраняем структуры нейтралов, героев и командиров
    _PI->WriteHiHook(0x75AEB0, CALL_, SAFE_, THISCALL_, Y_ReplayBattle);				

    // Есть баг: Астральный дух возвращает существ перед первой переигровкой
    // исправить так: JMP в 0x76C632 -> 0x76C72B
    // а пропущенный код выполнить (придется писать ручками) после всех переигровок
    // АЛЬТЕРНАТИВНОЕ РЕШЕНИЕ (как по мне - лучше)
    // hihook на функцию 0x76C616 c полным пропуском оной во время переигровок
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
			_PI = _P->CreateInstance("igrik.BattleReplay"); 

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
