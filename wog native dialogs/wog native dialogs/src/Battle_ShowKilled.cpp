/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////    показ предполагаемого количества убитых монстров при атаке и стрельбе	/////////////////////
_int_ damLo;
_int_ damHi;

// перехват получения величин урона
int __stdcall Y_Battle_SetHintAttackGetDamage(LoHook* h, HookContext* c)
{
	damLo = c->edi;
	damHi = c->esi;
	return EXEC_DEFAULT;
} 

// вычисление кол-ва убитых и помещение этих данных в строку
_char_* GetStringKilled(char* strCountKilled, _BattleStack_* stackD)
{
	_int_ fullHealth = 0; 
	_int_ willKilledLo = 0;
	_int_ willKilledHi = 0;

	_int_ lost;

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
			sprintf(strCountKilled, "%dk", (willKilledHi / 1000) );
		} else {
			 sprintf(strCountKilled, "%d", willKilledHi);
		} 
	} else {
		if (willKilledLo > 9999) {
			sprintf(strCountKilled, "%dk-%dk", (willKilledLo / 1000), (willKilledHi / 1000) );
		} else {
			sprintf(strCountKilled, "%d-%d", willKilledLo, willKilledHi);
		} 
	}

	return strCountKilled;
}

// расширение стандартной геройской функции показа наносимого урона (добаляем предпологаемый диапазон кол-во убитых)
_HStr_* __stdcall BattleMgr_GetAttackDamage_Hint_LogString(HiHook* hook, _HStr_* H3Str, _BattleStack_* attacker, _BattleStack_* enemy, _byte_ shoot, _int_ stepsTaken) 
{
	_HStr_* H3string;
	H3string = CALL_5(_HStr_*, __fastcall, hook->GetDefaultFunc(), H3Str, attacker, enemy, shoot, stepsTaken);

	char countKilled[512];
	char stringKilled[512];

	// сначала копируем stringKilled = ", убъет: %s" + "количество убитых"
	sprintf((char*)stringKilled, WogNDlg_TXT->GetString(23), GetStringKilled((char*)countKilled, enemy) );

	// потом копируем: "Атаковать/Стрелять в монстр (урон X + stringKilled)" 
	sprintf(o_TextBuffer, "%s %s", H3string->c_str, (char*)stringKilled );

	// создаем новый класс строки H3
	_HStr_* myString = new _HStr_;
	myString->Set(o_TextBuffer);

	// удаляем старый класс строки H3
	H3string->Destruct(1);
	
	return myString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_ShowKilled(PatcherInstance* _PI)
{
	// показ предполагаемого количества убитых монстров при атаке и стрельбе 
	_PI->WriteLoHook(0x493058, Y_Battle_SetHintAttackGetDamage); // перехват величин наносимого урона			
	_PI->WriteHiHook(0x492F50, SPLICE_, EXTENDED_, FASTCALL_, BattleMgr_GetAttackDamage_Hint_LogString);
}