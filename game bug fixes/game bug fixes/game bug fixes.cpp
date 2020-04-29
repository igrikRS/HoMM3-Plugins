#include "..\..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

// by RoseKavalier ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall AI_split_div0(LoHook *h, HookContext *c)
{
    if (c->ebx == 0) // enemy army value of 0?
        c->ebx = 1; // make it 1 to prevent crash
    return EXEC_DEFAULT;
}

// исправление бага посещения банков в которых дают существ 
// вылет в диалоге присоедиения монстров
void __stdcall HH_Show_Hero_Info_Dlg(HiHook *h, int this_hero_id, int a1, int a2)
{
	CALL_3(void, __thiscall, h->GetDefaultFunc(), this_hero_id, a1, a2);
	CALL_4(void, __thiscall, 0x417540, o_AdvMgr, 0, 0, 0); // Mobilize active hero
}

int __stdcall AI_waterwalk_fly(LoHook *h, HookContext *c)
{
    if (c->eax == 0) // no angel wings
    {
        _Hero_* hero = (_Hero_ *)(c->esi);
        if (hero->spell[6] == 0 && hero->spell_level[6] == 0) // this AI hero does not have the means to cast fly (id = 6)
        {
            if (hero->spell[7] != 0 || hero->spell_level[7] != 0) // this AI hero has access to waterwalk (id = 7)
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

// The Castle's Lighthouse building bonus
int __stdcall castleOwnerCheck(LoHook *h, HookContext *c)
{
   _Town_ *town = (_Town_*)(c->ecx);
   _Hero_ *hero = *(_Hero_**)(c->ebp - 4); // _Hero_ is stored in temp variable [LOCAL.1]

   if (hero->owner_id == town->owner_id) // normal
      return EXEC_DEFAULT;
  
   c->return_address = 0x4E4D6C; // skip procedure
   return NO_EXEC_DEFAULT;
}

int __stdcall EarthquakeBug(LoHook *h, HookContext *c)
{
    _BattleStack_ *cre = (_BattleStack_ *)(c->eax - 0x84); // offset is to creature flags
    if (cre->index_on_side == 0) // if creature is in position 0 (1st on the left), then we don't want it to be flagged 0x200000 aka CANNOT_MOVE aka WAR_MACHINE
        return NO_EXEC_DEFAULT;
    return EXEC_DEFAULT;
}

int __stdcall faerie_button(LoHook *h, HookContext *c)
{
    if (c->eax == 15) // item 0xF is spellbook for Faerie Dragon
    {
        c->edi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

int __stdcall faerie_button_RMB(LoHook *h, HookContext *c)
{
   if (c->esi == 15)
   {
      c->esi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
      return NO_EXEC_DEFAULT;
   }
   return EXEC_DEFAULT;
}

// by igrik ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int __stdcall setActStack(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)(int)(c->esi -656);

	// если выстрелов у катапульты или балисты больше нет
	// то делаем пропуски передачи хода этим машинам
	if (mon->creature_id == 145 || mon->creature_id == 146) {
		if ( mon->creature.shots < 1 ) {
			c->return_address = 0x464DAD;
			return NO_EXEC_DEFAULT;
		}	
	}
	// если выстрелов у циклопов больше нет
	// то забираем флаг "катапульта"
	if (mon->creature_id == 94 || mon->creature_id == 95) {
		if ( mon->creature.shots < 1 ) {
			if ( mon->creature.flags & 32 ) {
				mon->creature.flags -= 32;
			}
		}
	}
	return EXEC_DEFAULT;
} 

// создание инкремента боезапаса при атаке осадных стен "катапульта"
int __stdcall catapultaShoot(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)c->edi;
	mon->creature.shots -= 1;

	return EXEC_DEFAULT;
} 

// запрет на второй выстрел, если боезапас закончился
// для баллисты и стрелков
int __stdcall monstreShoot(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)c->esi;

	if (mon->creature.shots < 1) {
		c->return_address = 0x43FFFC;
		return NO_EXEC_DEFAULT;		
	}

	return EXEC_DEFAULT;
} 

_int_ __stdcall Y_FixBagCreatureGredeOfNeutrals(HiHook* hook, _Army_* army, _int_ creature_id)
{
    _int_ count = 0;
    _int_ i = 0;
    _int_ crGrade_id = GetCreatureGrade(creature_id);
    do {
        if (army->type[i] == creature_id || army->type[i] == crGrade_id) {
            count += army->count[i];
        }
        i++;
    } while ( i<7 );

    return count;
}

int __stdcall fixHarpyBinds(LoHook* h, HookContext* c)
{
    if (*(int*)(c->ebx + 696)) {
        c->return_address = 0x478365;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

int __stdcall ERM_Fix_EA_E(HiHook* hook, _BattleStack_* stack )
{
    int ret = 0;
    _int32_ spell_duration[81]; // для сохранения длительности заклинаний
    _int32_ spells_power[81];   // для сохранения силы действия заклинания

    if (stack) {
        for (int i=0; i<80; i++) {
            spell_duration[i] = stack->active_spell_duration[i];
            spells_power[i] = stack->active_spells_power[i];

            if (spell_duration[i] > 0 ) // если заклинание наложено на стек, то сбрасываем его эффект
                CALL_2(int, __thiscall, 0x444230, stack, i); // ResetSpellFromStack 0x444230
        }
    }

    ret = CALL_1(int, __cdecl, hook->GetDefaultFunc(), stack);

    for (int i=0; i<80; i++) {
        if (spell_duration[i] > 0) { // если заклинание ранее было наложено, то восстанавливаем его
            CALL_5(int, __thiscall, 0x444610, stack, i, spell_duration[i], spells_power[i], 0); // ApplySpell 0x444610
        }
    }

    return ret;
}

// не считать кавалерийский бонус при полете
_int_ __stdcall Y_AntiKavalierAndFly(LoHook* h, HookContext* c)
{
    if ( *(_dword_*)(c->ebx +132) >> 1 & 1 ) { // проверить флаг атакующего на полет
        c->return_address = 0x4430A3; // обходим расчет кавалерийского бонуса (он всё равно не работает)
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
} 

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

//#define Wog_FOH_Monstr (*(int*)0x27718CC)

int __stdcall Y_FixWoG_GetCreatureGrade(LoHook* h, HookContext* c)
{
	int ID = *(int*)(c->ebp +8);
	int mon_id = c->ecx; 
	int mon_idGr =  *(int*)(0xA49590 +  mon_id*4);

	if (mon_idGr == -2) {
		return EXEC_DEFAULT;
	} else if (mon_idGr == -1) {
		mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
	}

	int mon_idGr2 = *(int*)(0xA49590 +  mon_idGr*4);

	if (mon_idGr2 == -2) {
		return EXEC_DEFAULT;
	} else if (mon_idGr2 == -1) {
		mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
	}
	
	if (ID == mon_idGr2 ) { c->ecx = mon_idGr; }

	return EXEC_DEFAULT;
}

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

// корректировка WoG ненависти существ
// добавляем и существ 8-го уровня
_int_ __stdcall Y_SetWogHates(LoHook* h, HookContext* c)
{
	int mult = 0; // бонус урона ненависти в процентах
	int amtype = *(int*)(c->ebp -4);
	int dmtype = *(int*)(c->ebp -8);
	 
	switch(amtype){
		case 12:  // Ангел
		case 13:  // Архангел
		case 150: // Верховный Архангел
			if(dmtype == 54 || dmtype == 55 || dmtype == 153) mult = 50; break;
		case 36:  // Джин
		case 37:  // Мастар Джин
			if(dmtype == 52 || dmtype == 53) mult = 50; break; 
		case 41:  // Титан
		case 152: // Громовержец
			if(dmtype == 83 || dmtype == 155) mult = 50; break;
		case 52:  // Ифрит
		case 53:  // Ифрит Султан
			if(dmtype == 36 || dmtype == 37) mult = 50; break;
		case 54:  // Дьявол
		case 55:  // Архидьявол
		case 153: // Барон Ада
			if(dmtype == 12 || dmtype == 13 || dmtype == 150) mult = 50; break;
		case 83:  // Черный Дракон
		case 155: // Тёмный Дракон
			if(dmtype == 41 || dmtype == 152) mult = 50; break;
		default: break;
  }
	// теперь бонус урона нужно занести в EDX
	c->edx = mult;
	c->return_address = 0x766EEB;
	return NO_EXEC_DEFAULT;
} 

void __stdcall Y_SelectNewMonsterToAct(HiHook* hook, _BattleMgr_* bm, int side, int stack_id_in_side)
{
	if (o_NetworkGame) {
		if ( o_GameMgr->IsPlayerHuman(bm->owner_id[1]) ) { 
			if ( o_GameMgr->IsPlayerHuman(bm->owner_id[0]) ) { 
				_int32_ net[14197];  // 56748 +40 = 56788 / 4 = 14197
				net[0] = -1;
				net[1] = NULL;
				net[2] = 1987;
				net[3] = 56788;
				net[4] = 0;
				net[5] = side; 
				net[6] = stack_id_in_side;

				int bmStart = (int)o_BattleMgr +21708 -40;
				for (int i=10; i<14197; i++) {
					net[i] = *(int*)(bmStart +i*4);
				}

				_int32_ isGood = CALL_4(_int32_, __fastcall, 0x5549E0, (int)&net, *(int*)(0x697790 +4*bm->current_side), 0, 1); 
				if ( !isGood ) 
					CALL_1(char, __thiscall, 0x4F3D20, 0);

				delete[] net;
			}
		}
	}
	CALL_3(void, __thiscall, hook->GetDefaultFunc(), bm, side, stack_id_in_side);
}

int __stdcall Y_BM_ReceNetData(LoHook* h, HookContext* c)
{
	int id = *(int*)(c->esi +8);
	if ( id == 1987 ) {

		_int32_ netData = c->esi; // netData содержит прямой адрес переданного массива net[14197] 

		int bmStart = (int)o_BattleMgr +21708; // начало стеков
		int netDSt = netData +40;
		for (int i=0; i<42; i++) {
			for (int k=0; k<1352; k++) { 
				if (k>=40 && k<44) continue;
				if (k>=116 && k<172) continue;
				if (k>=272 && k<404) continue;				
				if (k>=1056 && k<1108) continue;
				if (k>=1264) continue;

				*(char*)(bmStart +i*1352 +k) = *(char*)(netDSt +i*1352 +k);
			}
		}

		CALL_3(void, __thiscall, 0x464F10, o_BattleMgr, *(int*)(netData +20), *(int*)(netData +24));

		CALL_1(void*, __thiscall, 0x555D00, netData); // деструктор
	}

	return EXEC_DEFAULT; 
}

// корректировка описаний заклинаний в книге (не учитывались бонусы специалистов по заклинаниям)
_int64_ __stdcall Y_DlgSpellBook_FixDecription_SpellPower(HiHook* hook, _Hero_ *hero, int spell, signed int damage, _BattleStack_ *stack)
{
	_int64_ power = CALL_4(_int64_, __thiscall, hook->GetDefaultFunc(), hero, spell, damage, stack);
	power += (_int64_)hero->GetSpell_Specialisation_PowerBonuses(spell, (_int_)power, 0);

	return power;
}


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

// фикс неправильного отображения величины урона в окне статуса битвы при касте заклинания Армагеддон
_int_ __stdcall Y_Fix_ReportStatusMsg_CastArmageddonSpell(HiHook* hook, _BattleMgr_ *bm, _int_ damage, _int_ spell, _Hero_ *heroA, _Hero_ *heroD, _BattleStack_ *stack, _int_ a7) 
{
	if ( !heroA ) { // в этом месте структура атакующего героя не подаётся. Поэтому мы создаём её сами
		heroA = bm->hero[bm->current_side];
	}

	_int_ ret = CALL_7(_int_, __thiscall, hook->GetDefaultFunc(), bm, damage, spell, heroA, heroD, stack, a7);
	return ret;
}

// Решение бага ERM: триггер MA:U#/-2 приводил к тому, что любое существо при установке такой команды улучшалось в копейщика
int __stdcall Fix_WoG_GetCreatureGrade_Expo(LoHook* h, HookContext* c)
{
	if ( *(int*)(c->ebp -4) < -1) {
		*(int*)(c->ebp -4) = -1;
	}
	return EXEC_DEFAULT;
} 

int __stdcall Fix_WoG_GetCreatureGrade_Town(LoHook* h, HookContext* c)
{
	if (*(int*)0x27F93B0 < -1) {
		*(int*)0x27F93B0 = -1;
	}
	return EXEC_DEFAULT;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////            ЗАПРЕТЫ ЗАКЛИНАНИЙ             ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// запрет выдачи заклинаний у артефактов, если они запрещены через UN:J0/spell_id/1;
int __stdcall Y_ArtGive_Spell(HiHook* hook, int spells_array, unsigned int spell_id, char enable) 
{
	if (enable && o_GameMgr->disabled_spells[spell_id]) 
	{
		if ( spell_id != SPL_TITANS_LIGHTNING_BOLT )
		{
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

	if (enable && o_GameMgr->disabled_spells[spell_id]) 
	{
		return 0;
	}

	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), spells_array, enable);
}


// фикс переполнения опыта существ (вызов проверки на max опыт 0x71924A)
void __stdcall Y_WoGCrExpoSet_AddExpo(HiHook* hook, int cr_Expo) 
{
	int expo = IntAt(cr_Expo);

	// если опыт по какой то причине перевалил 
	// через предел (2^32)/4 и ушел в отрицательное число
	// ставим максимальный опыт
	if (expo < -1073741824) {
		IntAt(cr_Expo) = 200000;
		expo = IntAt(cr_Expo);
	}

	// если опыт был < 0 && > -1073741824, обнуляем опыт
	if (expo < 0) { 
		IntAt(cr_Expo) = 0; 
	}
	// ориг.функция проверки на превышение допустимого опыта
	return CALL_1(void, __thiscall, hook->GetDefaultFunc(), cr_Expo);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////





// ##############################################################################################################################
// ##############################################################################################################################
// ##############################################################################################################################



void startPlugin(Patcher* _P, PatcherInstance* _PI)
{
// by RoseKavalier ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	_PI->WriteLoHook(0x42DDA6, AI_split_div0); // Prevent crash when AI attacks "ghost" hero			
	_PI->WriteByte(0x49E4EC +1, 99); // работающая кнопка Отмена в Арене

	// ХЗ  что это (возможно операции со стеками)
	_PI->WriteByte(0x49C021, 183);
	_PI->WriteByte(0x4A763F, 183);
	_PI->WriteByte(0x4A9423, 183);
	_PI->WriteByte(0x4AA82E, 183);
	_PI->WriteByte(0x4C9662, 183);
	_PI->WriteByte(0x4FD164, 183);
	_PI->WriteByte(0x505C9F, 183);
	_PI->WriteByte(0x52CD36, 183);
	_PI->WriteJmp(0x5A365D, 0x5A3666);
	_PI->WriteJmp(0x5A37B9, 0x5A37C2);
	_PI->WriteJmp(0x464DF1, 0x464DFB);
	_PI->WriteHexPatch(0x4FD12A, "0F BF 78 24 83 FF FF 0F 84");
	_PI->WriteHexPatch(0x505C75, "0F BF 77 24 83 FE FF 74");
	_PI->WriteHexPatch(0x52CD26, "0F BF 7F 24 83 FF FF 74");
	_PI->WriteHexPatch(0x44AA4F, "90 8B 7C 81 1C");
	_PI->WriteHexPatch(0x44AA58, "90 89 F8 89 44 B2 1C");
	_PI->WriteHexPatch(0x42D922, "90 8B 56 1C 89 45 DC 89 55 E4 90");
	_PI->WriteHexPatch(0x42DA39, "8B 4D E4 90 57 51");
	_PI->WriteHexPatch(0x42DAD9, "8B 4D E4 90 57 51");

	// исправление бага посещения банков в которых дают существ (вылет в диалоге присоедиенния монстров) © RoseKavalier
	_PI->WriteHiHook(0x5D52CA, CALL_, EXTENDED_, THISCALL_, HH_Show_Hero_Info_Dlg); // alternative 2 - should

	////////////////////////// AI BAGs ///////////////////////////////////////
	_PI->WriteLoHook(0x56B344, AI_TP_cursed_check);
	_PI->WriteLoHook(0x43020E, AI_waterwalk_fly);


// by Ben80 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Arrow towers damage bug fix
    // for cases:
    // Armorer sec.skill, Stoning of creature, Air shield spell
    _PI->WriteByte(0x41E3A4, 0x4D);
    _PI->WriteByte(0x41E4DF, 0x4D);
    _PI->WriteByte(0x465944, 0x4D);

	// fix Fortress attack and defense bonuses for defended Hero
    _PI->WriteByte(0x4639FE, 0x77);
    _PI->WriteByte(0x463A31, 0x76);

	// The Castle's Lighthouse building bonus
    // is now only applied to the Castle's owner
    _PI->WriteLoHook(0x4E4D40, castleOwnerCheck);

	// Earthquake Bug will no longer kill creatures and end battle
    _PI->WriteLoHook(0x465656, EarthquakeBug);

// by igrik ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// центрирование изображения по ПКМ в городе на иконке найма войск (ранее уходило сильно влево)
	_PI->WriteHexPatch(0x5D47B3, "0F BF 57 18  8B 4F 24 B8  FF FF FF FF  90");

	// исправить координаты кнопки Сказочных Драконов
	_PI->WriteDword(0x5F3D9F, 235);  // подложка поз.Y
	_PI->WriteByte(0x5F3DA4, 21);    // подложка поз.X
	_PI->WriteDword(0x5F3DF5, 235);  // кнопка   поз.Y
	_PI->WriteByte(0x5F3DFA, 21);    // кнопка   поз.X
    _PI->WriteLoHook(0x5F5320, faerie_button);
    _PI->WriteLoHook(0x5F4C99, faerie_button_RMB);

	// исправление ошибки ERM в командре IF:N1, теперь командра работает 
	// со всеми локальными, глобальными и отрицательными переменными z, а не только с z1
	_PI->WriteByte(0x749093, 0xB0);
	_PI->WriteByte(0x74909C, 0xB0);
	_PI->WriteByte(0x7490B0, 0xB0);
	_PI->WriteByte(0x7490B6, 0xB0);
	_PI->WriteByte(0x7490CD, 0xB0);

	// исправление неправильных иконок героев Инферно (Ксерафакс и Ксерон)
	_PI->WriteDword(0x79984C, 63);
	_PI->WriteDword(0x799850, 57);

	// исправление неправильных кнопок
	// в диалоге таверны
	_PI->WriteDword(0x5D7ACA, 0x682A24); // iCN6432.def
	// в диалоге резделения отрядов
	_PI->WriteDword(0x449A41, 0x682A24); // iCN6432.def

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

	// исправление бага палатки, когда на её ходу невозможно убежать или сделать другие действия
	_PI->WriteByte(0x75C82C, 0xEB);
	// возможность заходить в гильдию магов без наличия книги и денег у героя-гостя
	_PI->WriteHexPatch(0x5CEA83, "EB74");
	_PI->WriteHexPatch(0x5CEACD, "2800");

	// исправление бага с исчезновением стартового героя при переигрывании
	_PI->WriteByte(0x5029C0, 0xEB);

	// исправления стрельбы при отрицат.боезапасе
	// при передаче хода стеку
	_PI->WriteLoHook(0x464D75, setActStack);
	// при стрельбе по стенам (катапульта, циклопы)
	_PI->WriteLoHook(0x445CF9, catapultaShoot);	
	// второй выстрел монстрами
	_PI->WriteLoHook(0x43FF92, monstreShoot);	
	// второй выстрел баллистой
	_PI->WriteLoHook(0x43FFF4, monstreShoot);	

	// Решение бага (еще с SoD) исчезновения улучшенного стека при битве с даунгрейдом нейтралов
	_PI->WriteHiHook(0x4AC5F5, CALL_, EXTENDED_, THISCALL_, Y_FixBagCreatureGredeOfNeutrals);

	// фикс отлета гарпий, когда после удара они связаны корнями дендроидов
    _PI->WriteLoHook(0x47835B, fixHarpyBinds);

	// не считать кавалерийский бонус при полете
	_PI->WriteLoHook(0x44307A, Y_AntiKavalierAndFly);

	// Решение бага Вога, когда в бою накладывается опыт через EA:E и атака, защита, уроны, скорость, боезапасы и т.п. заново пересчитываются.
	// Из-за этого теряются бонусы наложенных заклинаний (например бонус скорости от ускорения)
	_PI->WriteHiHook(0x726DE4, CALL_, EXTENDED_, CDECL_, ERM_Fix_EA_E);

	// Делаем кнопку отмены в Хижине Ведьмы 
	_PI->WriteDword(0x4A7E63 +1, 2);
	_PI->WriteHiHook(0x4A7E8A, CALL_, EXTENDED_, THISCALL_, Y_SetCanselWitchHut);

	// Делаем кнопку отмены у ученого, предлагающего втор.навык
	_PI->WriteLoHook(0x4A4AFE, Y_SetCanselScholarlySS);	

	// Решаем проблему когда бонусы специалистов не считаются Супер существам
	// _PI->WriteHiHook(0x4E64FA, CALL_, EXTENDED_, FASTCALL_, Y_FixWoG_GetCreatureGrade);
	_PI->WriteLoHook(0x4E64D1, Y_FixWoG_GetCreatureGrade);

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

	// исправление созданий WoG'ом корявых пакованых координат
	_PI->WriteHiHook(0x711E7F, SPLICE_, EXTENDED_, CDECL_, Y_WoG_MixedPos_Fix);
	_PI->WriteHiHook(0x711F49, SPLICE_, SAFE_, CDECL_, Y_WoG_UnMixedPos_Fix);

	// корректировка WoG ненависти существ
	// добавляем и существ 8-го уровня
	_PI->WriteLoHook(0x766E4E, Y_SetWogHates);

	// частичное исправление разсихнронизации 
	// сетевое копирование параметров стеков в битве
	//_PI->WriteHiHook(0x464F10, SPLICE_, EXTENDED_, THISCALL_, Y_SelectNewMonsterToAct);
	//_PI->WriteLoHook(0x473D41, Y_BM_ReceNetData);

	// вызовы драконов от артефакта сердце дракона
	// меняем местами номера гексов, 
	// чтобы в банках существ не перекрывался стек №3
	_PI->WriteByte(0x767A05 +1, 55); 
	_PI->WriteByte(0x767A2A +1, 89); 

	// фикс выбора типа атаки при битве ИИ vs человек (человек не мог выбрать тип атаки)
	// суть в том, что была проверка на флаг V997, а должна быть V998
	_PI->WriteByte(0x762601 +3, 0xC5);

	// Решение проблемы отображения некоторых строк (в русской локализации) в диалоге экспы монстров.
	// Суть в подмене типа копирования символов со знакового на беззнаковое ( MOVSX -> MOVZX )
	_PI->WriteByte(0x71F3FC, 0xB6);
	_PI->WriteByte(0x71F5BA, 0xB6);
	_PI->WriteByte(0x71F5D3, 0xB6);
	_PI->WriteByte(0x723657, 0xB6);
	_PI->WriteByte(0x723219, 0xB6);
	_PI->WriteByte(0x7238D8, 0xB6);
	_PI->WriteByte(0x7217BB, 0xB6);
	_PI->WriteByte(0x723CBD, 0xB6);
	_PI->WriteByte(0x721B03, 0xB6);
	_PI->WriteByte(0x722792, 0xB6); 
	_PI->WriteByte(0x723ACB, 0xB6);
	_PI->WriteByte(0x723F1C, 0xB6);

	// корректировка описаний заклинаний в книге (не учитывались бонусы специалистов по заклинаниям)
	_PI->WriteHiHook(0x59BFFD, CALL_, SAFE_, THISCALL_, Y_DlgSpellBook_FixDecription_SpellPower);

	// фиксим неотображение Монолитов и Подземных врат в диалоге заклинания Просмотр Земли и Воздуха		
	_PI->WriteLoHook(0x5FC3EC, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Prepare);
	_PI->WriteLoHook(0x5F8545, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Show);

	// фикс неправильного отображения величины урона в окне статуса битвы при касте заклинания Армагеддон
	_PI->WriteHiHook(0x5A5522, CALL_, EXTENDED_, THISCALL_, Y_Fix_ReportStatusMsg_CastArmageddonSpell);

	// Решение бага ERM: триггер MA:U#/-2 приводил к тому, что любое существо при установке такой команды улучшалось в копейщика
	_PI->WriteDword(0x724A9F, -2);  _PI->WriteLoHook(0x724AC5, Fix_WoG_GetCreatureGrade_Expo);
	_PI->WriteDword(0x74ED27, -2);  _PI->WriteLoHook(0x74ED5C, Fix_WoG_GetCreatureGrade_Town);

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

	// фикс переполнения опыта существ (вызов проверки на max опыт)	
	_PI->WriteHiHook(0x71924A, CALL_, EXTENDED_, THISCALL_, Y_WoGCrExpoSet_AddExpo);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    static _bool_ bug_fixes_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!bug_fixes_On)
        {
			bug_fixes_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("ERA_bug_fixes"); 

			startPlugin(_P, _PI);

		}
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
