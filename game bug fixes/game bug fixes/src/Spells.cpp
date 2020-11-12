/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// фиксим неотображение Монолитов и Подземных врат в диалоге заклинания Просмотр Земли и Воздуха
_bool_ isVisible_Monoliths;
_bool_ isVisible_Gates;

int __stdcall Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Show(LoHook* h, HookContext* c)
{
    c->edi = c->ecx;					// mov edi, ecx
    c->ecx = *(_dword_*)(c->esi +30);	// mov ecx, [esi+1Eh]
    int obj_id = c->ecx; // номер объекта
    _byte_ isVisible = *(_byte_*)(c->ebp +8 +3);

    int type = 0;
    if ( obj_id == 43 || obj_id == 44 || obj_id == 45 ) { // Монолиты
        if ( isVisible_Monoliths || isVisible ) {
            type = 3;
        }
    }

    if ( obj_id == 103 ) { // Подземные врата
        if ( isVisible_Gates || isVisible) {
            type = 4;
        }
    }

    if ( type == 3 || type == 4 ) { // выполняем функцию отображения Монолитов или Подземных Врат при касте Просмотра Земли или Воздуха
        CALL_6(_int_, __fastcall, 0x5F7760, *(int*)0x6AAC88, c->esi, type, *(int*)0x6AAC80 + (*(int*)(c->ebp +0x14) * *(int*)0x68C70C), c->edi +8, *(int*)(c->ebp +0x12) );
    }

    c->return_address = 0x5F854A;
    return NO_EXEC_DEFAULT;
}

int __stdcall Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Prepare(LoHook* h, HookContext* c)
{
    int spell = *(int*)(c->ebp +8);
    int power = *(int*)(c->ebp +12);

    isVisible_Gates = false;
    isVisible_Monoliths = false;

    if ( spell == 5 ) // Просмотр воздуха
    {
        if ( power >= 2 ) { // показываем Подземные Врата
            isVisible_Gates = true;
        }
        if ( power >= 3 ) { // показываем Монолиты
            isVisible_Monoliths = true;
        }
    }

    return EXEC_DEFAULT;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


// корректировка описаний заклинаний в книге (не учитывались бонусы специалистов по заклинаниям)
_int64_ __stdcall Y_DlgSpellBook_FixDecription_SpellPower(HiHook* hook, _Hero_ *hero, int spell, signed int damage, _BattleStack_ *stack)
{
    _int64_ power = CALL_4(_int64_, __thiscall, hook->GetDefaultFunc(), hero, spell, damage, stack);
    power += (_int64_)hero->GetSpell_Specialisation_PowerBonuses(spell, (_int_)power, 0);

    return power;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// фикс неправильного отображения величины урона в окне статуса битвы при касте заклинания Армагеддон
_int_ __stdcall Y_Fix_ReportStatusMsg_CastArmageddonSpell(HiHook* hook, _BattleMgr_ *bm, _int_ damage, _int_ spell, _Hero_ *heroA, _Hero_ *heroD, _BattleStack_ *stack, _int_ a7) 
{
    if ( !heroA ) { // в этом месте структура атакующего героя не подаётся. Поэтому мы создаём её сами
        heroA = bm->hero[bm->current_side];
    }

    _int_ ret = CALL_7(_int_, __thiscall, hook->GetDefaultFunc(), bm, damage, spell, heroA, heroD, stack, a7);
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////            ЗАПРЕТЫ ЗАКЛИНАНИЙ             ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// запрет выдачи заклинаний у артефактов, если они запрещены через UN:J0/spell_id/1;
int __stdcall Y_ArtGive_Spell(HiHook* hook, int spells_array, unsigned int spell_id, char enable) 
{
    if (enable && o_GameMgr->disabled_spells[spell_id]) {
        if ( spell_id != SPL_TITANS_LIGHTNING_BOLT ) {
        	return 0;
        }
    }

    return CALL_3(int, __thiscall, hook->GetDefaultFunc(), spells_array, spell_id, enable);
}

// заполняем таблицы запретов заклинаний при при загрузке карты, если они запрещены через UN:J0/spell_id/1;
int __stdcall Y_ArtGive_LoadSpells(HiHook* hook, int spells_array, char enable) 
{
    int spell_id = *(int*)(spells_array +4);

    o_GameMgr->disabled_shrines[spell_id] = enable;
    o_GameMgr->disabled_spells[spell_id] = enable;

    return CALL_2(int, __thiscall, hook->GetDefaultFunc(), spells_array, enable);
}

// запрет выдачи заклинаний у томов магии и шляпы школяра, если они запрещены через UN:J0/spell_id/1;
int __stdcall Y_ArtGive_AllSpells(HiHook* hook, int spells_array, char enable) 
{
    int spell_id = *(int*)(spells_array +4);

    if (enable && o_GameMgr->disabled_spells[spell_id]) {
        return 0;
    }

    return CALL_2(int, __thiscall, hook->GetDefaultFunc(), spells_array, enable);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void Spells(PatcherInstance* _PI)
{

    // корректировка описаний заклинаний в книге (не учитывались бонусы специалистов по заклинаниям)
    _PI->WriteHiHook(0x59BFFD, CALL_, SAFE_, THISCALL_, Y_DlgSpellBook_FixDecription_SpellPower);

    // фиксим неотображение Монолитов и Подземных врат в диалоге заклинания Просмотр Земли и Воздуха		
    _PI->WriteLoHook(0x5FC3EC, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Prepare);
    _PI->WriteLoHook(0x5F8545, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Show);

    // фикс неправильного отображения величины урона в окне статуса битвы при касте заклинания Армагеддон
    _PI->WriteHiHook(0x5A5522, CALL_, EXTENDED_, THISCALL_, Y_Fix_ReportStatusMsg_CastArmageddonSpell);

    // запрет выдачи заклинаний у артефактов
    _PI->WriteHiHook(0x4D9763, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_Spell);
    _PI->WriteHiHook(0x4D9786, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_Spell);
    _PI->WriteHiHook(0x4D9798, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_Spell);
    // заполняем таблицу при загрузке карты
    _PI->WriteHiHook(0x4C2445, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_LoadSpells);
    // запрет выдачи заклинаний у томов магии и шляпы школяра
    _PI->WriteHiHook(0x4D961F, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_AllSpells);
    _PI->WriteHiHook(0x4D9673, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_AllSpells);
    _PI->WriteHiHook(0x4D9673, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_AllSpells);
    _PI->WriteHiHook(0x4D9720, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_AllSpells);
    //_PI->WriteHiHook(0x4D9673, CALL_, EXTENDED_, THISCALL_, Y_ArtGive_AllSpells);

    // возможность заходить в гильдию магов без наличия книги и денег у героя-гостя
    _PI->WriteHexPatch(0x5CEA83, "EB74");
    _PI->WriteHexPatch(0x5CEACD, "2800");

    // заклинание забывчивось на Продвинутом уровне действует на один целевой отряд (баг СОДа)
    // o_Spell[SPL_FORGETFULNESS].flags |= SPF_HAS_MASS_ON_EXPERT;

}
