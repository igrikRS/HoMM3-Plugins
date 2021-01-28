////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// добавление расчета силы командиров для AI (Fight_Value и AI_Value) /////////////////////////

_int64_ getAIValue_NPC(int heroID){

    if ( heroID < 0 && heroID > 155 )
        return 0; // выход, если номер героя неправильный

    _Npc_* npc = GetNpc(heroID);	// структура командира
    if ( npc->on < 1 || npc->alive ) 
        return 0;	// выход, если командиров нет (-1), уволен или мертв

    _Hero_* hero = o_GameMgr->GetHero(heroID);

    _int64_ value = 0;

    int attNPC = Get_NpcSkillPower(npc, 0);		attNPC += hero->attack;	
    int defNPC = Get_NpcSkillPower(npc, 1);		defNPC += hero->defence;
    int hpNPC = Get_NpcSkillPower(npc, 2);	
    int damageNPC = Get_NpcSkillPower(npc, 3);	
    int speedNPC = Get_NpcSkillPower(npc, 5);								
    int isShoot = npc->specBon[0] & 0x10 ? 2 : 1;

    value = (_int64_)((attNPC * damageNPC * speedNPC * isShoot + defNPC * hpNPC) >> 2); 

    return value;
}

bool check_AIValue_isNotHero = false;

_int_ __stdcall get_AIValue_Hook(HiHook* hook, int army)
{
    _int_ value;

    int armyNPC = army;

    value = CALL_1(_int_, __thiscall, hook->GetDefaultFunc(), army);

    if (!check_AIValue_isNotHero) {
        int heroID = *(int*)(armyNPC -119);

        if ( heroID >= 0 && heroID < 156)
            value += (_int_)getAIValue_NPC(heroID);

    } 

    check_AIValue_isNotHero = false;

    if ( value < 0 || value >= (INT_MAX -500) ) { 
        int random = Randint(550, 600);
        value = INT_MAX - random;
    }

    return value; 
}


int __stdcall get_AIValue_And_NPC_Error(LoHook* h, HookContext* c)
{
    check_AIValue_isNotHero = true;
    return EXEC_DEFAULT;
} 

int __stdcall get_Fight_Value_Hook(LoHook* h, HookContext* c)
{
    _Hero_* hero = o_BattleMgr->hero[c->ebx]; 

    if (!hero) 
        return EXEC_DEFAULT; 

    c->ecx += getAIValue_NPC(hero->id);

    return EXEC_DEFAULT;
} 



// фикс переполнения AI_Value, когда армия у ИИ слишком большая
_int32_ __stdcall AI_ArmyExchanging_GetAddStack_Value_And_Place(HiHook* h, int this_, int a2, __int16 a3, int a4, char a5)
{
    _int32_ value = CALL_5(_int32_, __thiscall, h->GetDefaultFunc(), this_, a2, a3, a4, a5);

    //if (value < -(2 << 20) ) {
    //    //if ( value > -(INT_MAX >> 2) )
    //    //    value = 0;
    //    //else {
    //    //    int random = Randint(550, 600);
    //    //    value = INT_MAX - random;
    //    //}

    //    int random = Randint(1000, 2500);
    //    value = -random;

    //    //if (o_ActivePlayer->selected_hero_id == 24)
    //    //{
    //    //    countCalls++;
    //    //    sprintf(MyString, "%d) value: %d", countCalls, value);
    //    //    b_MsgBox(MyString, 5);
    //    //}
    //}

    return value;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Npc(PatcherInstance* _PI)
{
    // добавляем расчет командиров в проверку Fight_Value
    _PI->WriteLoHook(0x41EAD2, get_Fight_Value_Hook);
    // добавляем в расчет AI_Value и расчет командиров
     _PI->WriteHiHook(0x44A950, SPLICE_, EXTENDED_, THISCALL_, get_AIValue_Hook);
    // ставим лоухук, чтобы понять что идет расчет AI_Value без наличия героя
    _PI->WriteLoHook(0x5C1867, get_AIValue_And_NPC_Error);
    _PI->WriteLoHook(0x42758F, get_AIValue_And_NPC_Error);
    _PI->WriteLoHook(0x42CA6B, get_AIValue_And_NPC_Error);
    _PI->WriteLoHook(0x52846A, get_AIValue_And_NPC_Error);	

    // фикс переполнения AI_Value, когда армия у ИИ слишком большая
    _PI->WriteHiHook(0x42C830, SPLICE_, EXTENDED_, THISCALL_, AI_ArmyExchanging_GetAddStack_Value_And_Place);

    // исправление бага блока командира, когда защита падала из-за флага "в защите"
    _PI->WriteCodePatch(0x76E7D7, "%n", 24); // 15 nop 
    _PI->WriteCodePatch(0x76E80B, "%n", 13); // 13 nop
    _PI->WriteHexPatch(0x76E7D7, "8B4D 08 C601 01 C641 02 04");	

    // исправление одного из багов Астрального духа
    // убираем WoG сообщение, которое вызывает непонятную ошибку
    _PI->WriteHexPatch(0x76D4B3, "EB17");

    // расширяем свитч хинтов колдовства для описаний командиров
    // и монстров с номером больше 134
    _PI->WriteHexPatch(0x492A56, "81FF B7000000 90 7747");
    _PI->WriteDword(0x492A63, *(_int_*)0x44825F);
}
