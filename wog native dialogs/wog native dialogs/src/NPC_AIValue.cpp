////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// добавление расчета силы командиров для AI (Fight_Value и AI_Value) /////////////////////////

_int64_ getAIValue_NPC(int heroID){

	if ( heroID < 0 && heroID > 155 )
		return 0; // выход, если номер героя неправильный

	_Npc_* npc = GetNpc(heroID);				// структура командира
	if ( npc->on < 1 || npc->alive ) return 0;	// выход, если командиров нет (-1), уволен или мертв

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

_int64_ __stdcall get_AIValue_Hook(HiHook* hook, int army)
{
	_int64_ value;

	int armyNPC = army;

	value = CALL_1(_int64_, __thiscall, hook->GetDefaultFunc(), army);

	if (!check_AIValue_isNotHero) {
		int heroID = *(int*)(armyNPC -119);

		if ( heroID >= 0 && heroID < 156)
			value += getAIValue_NPC(heroID);

	} 
	check_AIValue_isNotHero = false;
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

	if (!hero) return EXEC_DEFAULT; 

	c->ecx += getAIValue_NPC(hero->id);

	return EXEC_DEFAULT;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_NPC_AIValue(PatcherInstance* _PI)
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
}
