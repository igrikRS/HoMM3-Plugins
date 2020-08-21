/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////    показ предполагаемого количества убитых монстров при атаке и стрельбе	/////////////////////
_int_ damLo;
_int_ damHi;

int __stdcall Y_Battle_SetHintAttackWillKilled(LoHook* h, HookContext* c)
{
	_int_ fullHealth = 0; 
	_int_ willKilledLo = 0;
	_int_ willKilledHi = 0;

	_int_ lost;

	_BattleStack_* stackD = (_BattleStack_*)c->esi;
	fullHealth = stackD->GetFullHealth(0);

	if (fullHealth > damHi){
		if (stackD->creature.hit_points > 1) {
			lost = (fullHealth - damHi) / stackD->creature.hit_points;
			if ((fullHealth - damHi) % stackD->creature.hit_points > 0)
				lost += 1;
		}
		else lost = fullHealth - damHi;
		willKilledHi = stackD->count_current - lost;
	} else willKilledHi = stackD->count_current; 

	if (damHi != damLo) {
		if (fullHealth > damLo){
			if (stackD->creature.hit_points > 1) {
				lost = (fullHealth - damLo) / stackD->creature.hit_points;
				if ((fullHealth - damLo) % stackD->creature.hit_points > 0)
					lost += 1;
			}
			else lost = fullHealth - damLo;
			willKilledLo = stackD->count_current - lost;
		} else willKilledLo = stackD->count_current;
	}
	
	if (damHi == damLo || willKilledHi == willKilledLo) {
		if (willKilledHi > 9999) {			
			sprintf((char*)c->ecx, "%s%s %dk", c->ecx, WogNDlg_TXT->GetString(22), (willKilledHi / 1000) );
		} else {
			 sprintf((char*)c->ecx, "%s%s %d", c->ecx, WogNDlg_TXT->GetString(22), willKilledHi);
		} 
	} else {
		if (willKilledLo > 9999) {
			sprintf((char*)c->ecx, "%s%s %dk-%dk", c->ecx, WogNDlg_TXT->GetString(22), (willKilledLo / 1000), (willKilledHi / 1000) );
		} else {
			sprintf((char*)c->ecx, "%s%s %d-%d", c->ecx, WogNDlg_TXT->GetString(22), willKilledLo, willKilledHi);
		} 
	}

	return EXEC_DEFAULT;
}

// перехват получения величин урона
int __stdcall Y_Battle_SetHintAttackGetDamage(LoHook* h, HookContext* c)
{
	damLo = c->edi;
	damHi = c->esi;
	return EXEC_DEFAULT;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_ShowKilled(PatcherInstance* _PI)
{
	// показ предполагаемого количества убитых монстров при атаке и стрельбе (подмена строк)
	_PI->WriteDword(0x4925FD +1, (int)&WogNDlg_TXT );	_PI->WriteDword(0x492605 +2, 88); // рукопашная (подмена строк)
	_PI->WriteDword(0x492825 +2, (int)&WogNDlg_TXT );	_PI->WriteDword(0x492837 +2, 92); // стрелять (подмена строк)
	_PI->WriteDword(0x49279C +1, (int)&WogNDlg_TXT );	_PI->WriteDword(0x4927A4 +2, 96); // последний выстрел (подмена строк)
	// показ предполагаемого количества убитых монстров при атаке и стрельбе 
	_PI->WriteLoHook(0x493058, Y_Battle_SetHintAttackGetDamage); // получение урона			
	_PI->WriteLoHook(0x4925FD, Y_Battle_SetHintAttackWillKilled); // рукопашная
	_PI->WriteLoHook(0x492825, Y_Battle_SetHintAttackWillKilled); // стрелять
	_PI->WriteLoHook(0x49279C, Y_Battle_SetHintAttackWillKilled); // последний выстрел
}
