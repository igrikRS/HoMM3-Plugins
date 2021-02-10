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

// фикс вылета: АИ битва (просчёт)
// проверка на скорость монстра и когда он дойдет до защиты стрелка. 
// Убираем из проверки существ с нулевой скоростью и боевые машины
_int_ __stdcall Y_AIMgr_Stack_MinRoundToReachHex(HiHook* hook, _dword_ this_, _BattleStack_* stack, _int_ a3)
{
	if (stack->creature.flags == BCF_CANT_MOVE || stack->creature.speed < 1) 
	{
		return 99; // 99 раундов необходимо, чтобы добраться до стрелка
	}

	return CALL_3(_int_, __thiscall, hook->GetDefaultFunc(), this_, stack, a3);
}

// фикс вылета: нет проверки на наличие стуктуры целевого стека
// но тут не хватает проверки на c->edi 
int __stdcall Y_FixCrash_CastSpell_38(LoHook* h, HookContext* c)
{
   if ( c->edi )
   {
	   c->eax = *(int*)(c->edi + 0x38);
	   c->ecx = *(int*)(c->ebx + 0x132C0);
	   c->return_address = 0x5A1C20;
	  
   } else c->return_address = 0x5A2368;
   return NO_EXEC_DEFAULT;
}

// фикс вылета: при удалении препятствия в битве нет проверки на наличие стуктуры препятствия
// (привет WoG и его стена огня у Огненных Лошадей)
int __stdcall Y_FixCrash_RemoveObstacle(LoHook* h, HookContext* c)
{
   // проверяем на пустую структуру боевого пропятствия
   // чтобы пропустить код обращения к ней и как следствие крит.краш.игры
   if ( !c->ecx || !c->edi )
   {
	   c->return_address = 0x466826;
	   return NO_EXEC_DEFAULT;
	  
   } 
   return EXEC_DEFAULT;
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

    // логическая ошибка SOD:
    // пропускаем показ обновления экрана для ИИ в телепортах:
    // - если мы не видим территорию
    // - если стоит опция "не показывать передвижения противника"
    _PI->WriteByte(0x41DBE8 +1, 0x5C);


    ///////////////////////////////////////////////////////////////////////////
	///////////////////////// Фиксы крит.крашей игры //////////////////////////

	// АИ битва (просчёт)
	// проверка на скорость монстра и когда он дойдет до защиты стрелка. 
	// Убираем из проверки существ с нулевой скоростью и боевые машины
	_PI->WriteHiHook(0x4B3C80, SPLICE_, EXTENDED_, THISCALL_, Y_AIMgr_Stack_MinRoundToReachHex);

	// фикс вылета: нет проверки на на наличие стуктуры целевого стека
	// но тут не хватает проверки на c->edi 
	_PI->WriteLoHook(0x5A1C17, Y_FixCrash_CastSpell_38);

	// фикс вылета: при удалении препятствия в битве, когда его стуктура таблицы равна нуля 
	// (привет WoG и его стена огня у Огненных Лошадей)
	_PI->WriteLoHook(0x46681B, Y_FixCrash_RemoveObstacle);

    ///////////////////////////////////////////////////////////////////////////
	///////////////////////// неиспользуемый функционал ///////////////////////

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
