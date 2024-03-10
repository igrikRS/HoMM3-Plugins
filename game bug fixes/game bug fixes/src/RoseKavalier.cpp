/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
// by RoseKavalier //////////////////////////////////////////////////////////////////////////////////


// This hook prevents division by 0 during tactics phase when you have no army.
// When AI has tactics or attacks you, they split their shooters...
_LHF_ (AI_split_div0)
{
    if (c->ebx == 0) // enemy army value of 0?
        c->ebx = 1;  // make it 1 to prevent crash
    return EXEC_DEFAULT;
}

// The AI was observed to divide by speed = 0 in "Rise of The Sun King"
_LHF_(AI_combat_div0)
{
	if (c->edi == 0)
		  c->edi = 1;
	return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// исправление бага посещения банков в которых дают существ 
// вылет в диалоге присоедиения монстров
void __stdcall HH_Show_Hero_Info_Dlg(HiHook *h, int this_hero_id, int a1, int a2)
{
    CALL_3(void, __thiscall, h->GetDefaultFunc(), this_hero_id, a1, a2);
    CALL_4(void, __thiscall, 0x417540, o_AdvMgr, 0, 0, 0); // Mobilize active hero
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall AI_waterwalk_fly(LoHook *h, HookContext *c)
{
    if (c->eax == 0) // no angel wings
    {
        _Hero_* hero = (_Hero_ *)(c->esi);
        if (hero->spells_spellbook[6] == 0 && hero->spells_available[6] == 0) // this AI hero does not have the means to cast fly (id = 6)
        {
            if (hero->spells_spellbook[7] != 0 || hero->spells_available[7] != 0) // this AI hero has access to waterwalk (id = 7)
            {
                if (hero->waterwalk_cast_power == -1) // waterwalk is not cast ~ waterwalk field is *(&hero + 0x116) (see 0x4E6040 Cast_waterwalk function)
                    c->return_address = 0x430231; // try to cast waterwalk instead (code checks for Boots of Levitation first...)
                else
                    c->return_address = 0x430540; // skip procedure
                return NO_EXEC_DEFAULT;
            }
        }
    }
    return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall AI_TP_cursed_check(LoHook *h, HookContext *c)
{
    _Hero_* hero = (_Hero_*)c->esi;
    if (hero->GetSpecialTerrain() == 0x15) // 0x15 = cursed ground, 0x4E5130: __thiscall GetSpecialTerrain()
    {
        c->return_address = 0x56B6F4;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

_bool8_ GetCreatureCostGradeText(_dword_ dlg, _bool8_ rmc)
{
    sprintf(o_TextBuffer, "");
    sprintf(MyString, "");
    if (!dlg) return false;

    _int_ monDeGrade = IntAt(dlg + 96);
    _int_ monUpGrade = IntAt(dlg + 144);
    _int_ count = IntAt(dlg + 100);

    if (monDeGrade < 0 && monUpGrade < 0 && count < 1) 
        return false;

    _dword_ ptrText = rmc ? *(int*)0x6A752C : *(int*)0x6A7528;
    char* separator = rmc ? "\n" : " ";

    _Resources_* cost = (_Resources_*)o_New(sizeof(_Resources_));
    cost->GetUpradeCreatureCost(monDeGrade, monUpGrade, count);
    std::string costtext;

    for (int i = 6; i >= 0; --i)
    {
      if (cost->GetByIndex(i) > 0)
      {
     // sprintf(MyString, "{~>TSRESOUR.DEF:%d valign=middle}%d, ", i, cost->GetByIndex(i));
        sprintf(MyString, "%s: %d, ", cost->GetNameByIndex(i), cost->GetByIndex(i));
        costtext.append(MyString);
      }
    }
    o_Delete(cost);    
    if (costtext.length() < 2) return false;

    // стереть 2 последних символа
    costtext.erase(costtext.end()-2, costtext.end());
    
    sprintf(o_TextBuffer, "%s%s(%s)", ptrText, separator, (const char*)costtext.c_str());
    return true;
}

#define UPGRADE_BUTTON 13
#define FAERIE_BUTTON 15

int __stdcall FaerieButton(LoHook *h, HookContext *c)
{
    if (c->eax == FAERIE_BUTTON)
    {
        c->edi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
        return NO_EXEC_DEFAULT;
    }
    else if (c->eax == UPGRADE_BUTTON)
    {
        if (GetCreatureCostGradeText(c->ebx, FALSE))
        {
          c->edi = (int)o_TextBuffer;
          return NO_EXEC_DEFAULT;
        }
    }

    return EXEC_DEFAULT;
}

int __stdcall FaerieButton_RMB(LoHook *h, HookContext *c)
{
    if (c->esi == FAERIE_BUTTON)
    {
        c->esi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
        return NO_EXEC_DEFAULT;
    }
    else if (c->esi == UPGRADE_BUTTON)
    {
        if (GetCreatureCostGradeText(c->ebx, TRUE))
        {
          c->esi = (int)o_TextBuffer;
          return NO_EXEC_DEFAULT;
        }
    }
    return EXEC_DEFAULT;
}


/*
 * Wait Phase Bug [2].
 * Because Wait Phase Bug [1] restores access to a function at all times,
 * portion of its code must be skipped during Wait Phase.
 */

int __stdcall RK_WaitPhaseBug(LoHook *h, HookContext *c)
{
    if (o_BattleMgr->waitPhase) // мы должны предотвратить триггер регена существ или регена элексира жизни
        c->flags.ZF = TRUE;      // потому что он не должен был действовать в этой фазе


    return EXEC_DEFAULT;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void RK(Patcher* _P, PatcherInstance* _PI)
{ 
    PatcherInstance* _RK = _P->GetInstance("ERA_bug_fixes");

    if (!_RK ) 
    {
        // if AI has tactics and shooters, but you have 0 creatures -> crash
        _PI->WriteLoHook(0x42DDA6, AI_split_div0); 

        // divides by speed which shouldn't be 0
        _PI->WriteLoHook(0x42437D, AI_combat_div0);

        // описание кнопки сказочного дракона
        _PI->WriteLoHook(0x5F5320, FaerieButton);
        _PI->WriteLoHook(0x5F4C99, FaerieButton_RMB);

        // исправление бага посещения банков в которых дают существ (вылет в диалоге присоедиенния монстров) © RoseKavalier
        _PI->WriteHiHook(0x5D52CA, CALL_, EXTENDED_, THISCALL_, HH_Show_Hero_Info_Dlg); // alternative 2 - should

        // небольшое исправление блоков на дорогах
        _PI->WriteByte(0x541B6A, 235);

        ////////////////////////// AI BAGs ///////////////////////////////////////
        _PI->WriteLoHook(0x56B344, AI_TP_cursed_check);
        _PI->WriteLoHook(0x43020E, AI_waterwalk_fly);

        // работающая кнопка Отмена в Арене         
        _PI->WriteByte(0x49E4EC +1, 99); 
        
        // функцию регена в фазе ожидания (я не совсем понимаю зачем)
        // _PI->WriteLoHook(0x446BCD, RK_WaitPhaseBug); // Wait Phase Bug part 2 (WTF?)
        // _PI->WriteJmp(0x464DF1, 0x464DFB);  // Wait Phase Bug part 1 (WTF?)

        // ХЗ  что это (возможно операции со стеками)
        _PI->WriteByte(0x49C021, 183);
        _PI->WriteByte(0x4A763F, 183);
        _PI->WriteByte(0x4A9423, 183);
        _PI->WriteByte(0x4AA82E, 183);
        _PI->WriteByte(0x4C9662, 183);
        _PI->WriteByte(0x4FD164, 183);
        _PI->WriteByte(0x505C9F, 183);
        _PI->WriteByte(0x52CD36, 183);      
        
        _PI->WriteHexPatch(0x4FD12A, "0F BF 78 24 83 FF FF 0F 84");
        _PI->WriteHexPatch(0x505C75, "0F BF 77 24 83 FE FF 74");
        _PI->WriteHexPatch(0x52CD26, "0F BF 7F 24 83 FF FF 74");
        _PI->WriteHexPatch(0x44AA4F, "90 8B 7C 81 1C");
        _PI->WriteHexPatch(0x44AA58, "90 89 F8 89 44 B2 1C");
        _PI->WriteHexPatch(0x42D922, "90 8B 56 1C 89 45 DC 89 55 E4 90");
        _PI->WriteHexPatch(0x42DA39, "8B 4D E4 90 57 51");
        _PI->WriteHexPatch(0x42DAD9, "8B 4D E4 90 57 51");
    }
}
