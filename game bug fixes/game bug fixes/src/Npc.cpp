////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// добавление расчета силы командиров для AI (Fight_Value и AI_Value) /////////////////////////

_int64_ getAIValue_NPC(int heroID) {
    _int64_ value = 50;

    if ( heroID < 0 && heroID >= o_HeroesCount )
        return value; // выход, если номер героя неправильный

    _Npc_* npc = GetNpc(heroID);    // структура командира
    if ( !npc || npc->on < 1 || npc->alive )
        return value;   // выход, если командиров нет (-1), уволен или мертв

    _Hero_* hero = o_GameMgr->GetHero(heroID);

    int attNPC = Get_NpcSkillPower(npc, 0);     attNPC += hero->primary_skill[0];
    int defNPC = Get_NpcSkillPower(npc, 1);     defNPC += hero->primary_skill[1];
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

    // исправление бага блока командира, когда защита падала из-за флага "в защите"
    _PI->WriteCodePatch(0x76E7D7, "%n", 24); // 15 nop 
    _PI->WriteCodePatch(0x76E80B, "%n", 13); // 13 nop
    _PI->WriteHexPatch(0x76E7D7, "8B4D 08 C601 01 C641 02 04");

    // исправление одного из багов Астрального духа
    // убираем WoG сообщение, которое вызывает непонятную ошибку
    _PI->WriteHexPatch(0x76D4B3, "EB17");

    // исправление бага, когда командир после пропуска хода может оказаться мёртвым
    // затираем вызов функции CheckForAliveNPCAfterBattle в функции добавления опыта
    // Она тут не нужна! Этот воговский хук стоит в добавлении опыта!
    _PI->WriteCodePatch(0x76B39E, "%n", 7);

    // расширяем свитч хинтов колдовства для описаний командиров
    // и монстров с номером больше 134
    _PI->WriteHexPatch(0x492A56, "81FF B7000000 90 7747");
    _PI->WriteDword(0x492A63, *(_int_*)0x44825F);
}
