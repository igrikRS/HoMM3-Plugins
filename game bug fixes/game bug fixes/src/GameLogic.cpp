/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
// by igrik /////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_SetCanselWitchHut(HiHook* hook, _Hero_* hero, _int_ skill, _byte_ skill_lvl) 
{
    if (o_WndMgr->result_dlg_item_id == DIID_CANCEL) {
        return 0;
    }

    return CALL_3(int, __thiscall, hook->GetDefaultFunc(), hero, skill, skill_lvl);
}

int __stdcall Y_SetCanselScholarlySS(LoHook* h, HookContext* c)
{
    int sskill = c->edi;
    _Hero_* hero = (_Hero_*)c->ebx;

    if (!b_MsgBoxD(o_ADVEVENT_TXT->GetString(116), 2, 20, hero->second_skill[sskill] +3*sskill +2) ) 
    {
        if (hero->second_skill[sskill] == 1 ) {
            hero->second_skill_count -= 1;
            hero->second_skill_show[sskill] = 0;
        }
        hero->second_skill[sskill] -= 1;
    }
    c->return_address = 0x4A4B86;
    return NO_EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// исправление созданий WoG'ом корявых пакованых координат
_dword_ __stdcall Y_WoG_MixedPos_Fix(HiHook* hook, int x, int y, int z)
{
    _dword_ xyz = b_pack_xyz(x, y, z);

    return xyz; 
}

// исправление созданий WoG'ом корявых разпакованных координат
void __stdcall Y_WoG_UnMixedPos_Fix(HiHook* hook, _dword_ x, _dword_ y, _dword_ z, _dword_ xyz)
{
    *(_dword_*)x = b_unpack_x(xyz);
    *(_dword_*)y = b_unpack_y(xyz);
    *(_dword_*)z = b_unpack_z(xyz);

    return; 
}

// ##############################################################################################################################
// ##############################################################################################################################
// ##############################################################################################################################


void GameLogic(PatcherInstance* _PI)
{	
    // исправление созданий WoG'ом корявых пакованых координат
    _PI->WriteHiHook(0x711E7F, SPLICE_, EXTENDED_, CDECL_, Y_WoG_MixedPos_Fix);
    _PI->WriteHiHook(0x711F49, SPLICE_, SAFE_, CDECL_, Y_WoG_UnMixedPos_Fix);

    // исправление бага с исчезновением стартового героя при переигрывании
    _PI->WriteByte(0x5029C0, 0xEB);

    // Делаем кнопку отмены в Хижине Ведьмы 
    _PI->WriteDword(0x4A7E63 +1, 2);
    _PI->WriteHiHook(0x4A7E8A, CALL_, EXTENDED_, THISCALL_, Y_SetCanselWitchHut);

    // Делаем кнопку отмены у ученого, предлагающего втор.навык
    _PI->WriteLoHook(0x4A4AFE, Y_SetCanselScholarlySS);	


    // фикс выбора типа атаки при битве ИИ vs человек (человек не мог выбрать тип атаки)
    // суть в том, что была проверка на флаг V997, а должна быть V998
    _PI->WriteByte(0x762601 +3, 0xC5);

	// пропускаем показ всем игрокам захват Двеллинга 8-го уровня существ 
	_PI->WriteByte(0x70DB3B +1, 0x37);  

    // отмена фикса по просьбе Berserker'a для ERA 2.8.6 и выше
    //// o_BattleMgr->Round = 1; правка ошибки с номерами раундов SOD.
    //// После тактической фазы первый раунд всегда был = 1
    //// А без тактической фазы первый раунд всегда был = 0
    //// Теперь всегда первый раунд будет = 0
    //_PI->WriteLoHook(0x473E73, Y_FixNewRoundCountInTactics);
    //_PI->WriteLoHook(0x474B79, Y_FixNewRoundCountInTactics);
    //_PI->WriteLoHook(0x4758B3, Y_FixNewRoundCountInTactics);
    //_PI->WriteDword(0x75D125, 0);
    //_PI->WriteLoHook(0x760973, Y_FixRoundCount_WoG);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// отмена фикса по просьбе Berserker'a для ERA 2.8.6 и выше
//_int_ __stdcall Y_FixNewRoundCountInTactics(LoHook* h, HookContext* c)
//{
//	o_BattleMgr->round = -1;
//	return EXEC_DEFAULT;
//}
//#define BACall_Day  (*(_int_*)0x79F0B8)
//#define BACall_Turn  (*(_int_*)0x79F0BC)
//_int_ __stdcall Y_FixRoundCount_WoG(LoHook* h, HookContext* c)
//{
//	BACall_Day = o_BattleMgr->round;
//	if (o_BattleMgr->isTactics) {
//		BACall_Turn--;
//	} else {
//		BACall_Turn = BACall_Day;
//	}
//	c->return_address = 0x76099A;
//	return NO_EXEC_DEFAULT;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
