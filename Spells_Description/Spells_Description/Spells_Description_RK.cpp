#include "pch.h"
#include "../../Headers/H3API.h"

Patcher* _P;
PatcherInstance* _PI;

// объявление внешнего файла
H3ColumnTextFile* Spells_Description_TXT;

// игровая переменная проверки языковой локализации
#define isRusLangWoG IntAt(0x80321C)


// получения номера строки с текстом
// в зависимости от локализации игры
INT32 GetString_Localosation(INT32 string_id)
{
	if (isRusLangWoG)
		string_id += 14;

	return string_id;
}

INT32 WoG_GetResistGolem(INT32 spell_id, INT32 damage, H3CombatMonster* stack)
{
	INT32 result = 0;

	if (stack->type >= 174 && stack->type <= 191)
	{	// сопротивление командиров NPC::Resist(MR_Type, MR_Dam, MR_Mon);
		result = CDECL_3(INT32, 0x76D506, stack->type, damage, stack);
	}
	else
	{
		// result = stack->GetResistGolem(spell_id, damage); // SoD
		result = FASTCALL_3(INT32, 0x44B180, damage, spell_id, stack->type);
	}
	// опыт стеков: CrExpBon::GolemResist(stack, result, damage, spell_id);
	result = CDECL_4(INT32, 0x71E766, stack, result, damage, spell_id);

	return result;
}

INT32 BattleStack_Get_Killed_From_Damage(H3CombatMonster* stack, INT32 damage, INT32 param)
{
	INT32 killed = 0;

	if (stack && damage > 0)
	{
		INT32 fullHealth;

		if (param == 2) // нанеcение урона
		{
			fullHealth = stack->GetFullHealth(0);

			INT32 lost = 0;

			if (fullHealth > damage) 
			{
				if (stack->info.hitPoints > 1) 
				{
					lost = (fullHealth - damage) / stack->info.hitPoints;
					if ((fullHealth - damage) % stack->info.hitPoints > 0)
						lost += 1;
				}
				else lost = fullHealth - damage;

				killed = stack->numberAlive - lost;
			}
			else killed = stack->numberAlive;
		}
		if (param == 3) // воскрешение и т.п.
		{
			fullHealth = (stack->numberAtStart * stack->info.hitPoints) - stack->GetFullHealth(0);
			if (fullHealth <= damage) {
				killed = stack->numberAtStart - stack->numberAlive;
			}
			else {
				INT32 lost_hp = stack->healthLost;

				// если стек мертвый, его lost_hp не обнулены
				// поэтому будет считаться неправильно. А мы обнуляем.
				if (stack->info.flags.DIE)
					lost_hp = 0;

				int resurect_hp = damage - lost_hp - 1;
				if (resurect_hp > 0)
				{
					killed = (resurect_hp / stack->info.hitPoints) + 1;
				}
			}
		}
	}

	return killed;
}

_LHF_(Y_Battle_Hint_SpellDescr_Prepare)
{
	INT32 spell_id = DwordAt(c->ebp + 8);
	INT32 str_hint_id = 0; // номер строки в текстовом файле (если 0, то хук вообще пропускаем)

	// проверяем нужно ли модифицировать строку
	// в зависимости от заклинания
	switch (spell_id)
	{
	case H3Spell::MAGIC_ARROW:
	case H3Spell::ICE_BOLT:
	case H3Spell::LIGHTNING_BOLT:
	case H3Spell::IMPLOSION:
	case H3Spell::CHAIN_LIGHTNING:
	case H3Spell::TITANS_LIGHTNING_BOLT:
	case H3Spell::FIREBALL:
	case H3Spell::INFERNO:
	case H3Spell::METEOR_SHOWER:
		str_hint_id = 2;
		break;

	case H3Spell::RESURRECTION:
	case H3Spell::ANIMATE_DEAD:
		str_hint_id = 3;
		break;

	case H3Spell::CURE:
		str_hint_id = 4;
		break;

	default:
		str_hint_id = 0;
		break;
	}


	// если строку хинта заклинания модифицируем
	if (str_hint_id)
	{
		INT32 damage = 0;
		INT32 spell_power = 1;
		INT32 spell_lvl = 0;
		INT32 killed = 0;

		H3CombatManager* cm = P_CombatMgr;
		H3CombatSquare* square = (H3CombatSquare*)c->edi;
		H3CombatMonster* stack = square->GetMonster();

		INT32 side = cm->currentActiveSide;

		if (!stack)
		{
			// если воскрешение, то мы не сможем получить стек через гекс
			// поэтому пытаемся получить через другие функции
			if (str_hint_id == 3)
			{
				INT32 square_id = DwordAt(c->ebp + 12);
				if (spell_id == H3Spell::RESURRECTION)
				{
					stack = cm->GetResurrectionTarget(square_id);
				}
				else stack = cm->GetAnimateDeadTarget(square_id);
			}
			else return EXEC_DEFAULT; // если всё же не получилось - выход
		}

		// если стек НЕ мертв
		if (!(stack->info.flags.DIE))
		{
			if (!( stack->CanApplySpell(spell_id, side, 1, 0 ) ) )
			{
				return EXEC_DEFAULT;
			}
		}

		H3Hero* hero = cm->hero[side];

		if (hero)
		{
			spell_lvl = hero->GetSpellExpertise(spell_id, cm->specialTerrain);
			spell_power = cm->heroSpellPower[side];
		}

		damage = spell_power * P_Spell[spell_id].sp_effect + P_Spell[spell_id].base_value[spell_lvl];

		if (hero)
		{
			if ( P_Spell[spell_id].flags.damage_spell )
				damage = (INT32)hero->GetSorceryEffect(spell_id, damage, stack);
			else
			{
				damage += hero->GetSpellSpecialtyEffect(spell_id, stack->info.level, damage);
			}
		}

		if (stack)
		{
			if (!(P_Spell[spell_id].flags.friendly_mass) )
			{
				INT32 resist = WoG_GetResistGolem(spell_id, damage, stack);

				damage = stack->GetProtectiveSpellEffect(resist, spell_id);
				// damage = STDCALL_3(INT32, 0x5A7EC0, resist, spell_id, stack);
			}
		}

		if (str_hint_id == 4)
		{ // если лечение
			if (stack->healthLost < damage)
			{
				damage = stack->healthLost; // точное кол-во сколько вылечит hp
			}
			// проверяем язык игры и по ней корректируем номер строки
			str_hint_id = GetString_Localosation(str_hint_id);

			sprintf(h3_TextBuffer, Spells_Description_TXT->GetText(str_hint_id), (PCHAR)c->eax, damage);
		}
		else
		{
			killed = BattleStack_Get_Killed_From_Damage(stack, damage, str_hint_id);

			// проверяем язык игры и по ней корректируем номер строки
			str_hint_id = GetString_Localosation(str_hint_id);

			sprintf(h3_TextBuffer, Spells_Description_TXT->GetText(str_hint_id), P_Spell[spell_id].name, (PCHAR)c->eax, damage, killed);
		}


		c->return_address = 0x5A8BBF;
		return NO_EXEC_DEFAULT;
	}

	else return EXEC_DEFAULT;
}

_LHF_(Y_DlgSpellBook_ModifSpell_Description)
{
	INT32 string = 0; // номер строки в текстовом файле (0 - не модифицировать)

	INT32 spell_pow = c->eax;
	INT32 spell_lvl = DwordAt(c->ebp - 16);
	INT32 spell_id = DwordAt(c->ebp + 12);

	H3Hero* hero = (H3Hero*)DwordAt(c->ebp + 16);

	int damage = spell_pow * P_Spell[spell_id].sp_effect + P_Spell[spell_id].base_value[spell_lvl];

	if (hero)
	{
		if (P_Spell[spell_id].flags.damage_spell)
			damage = (INT32)hero->GetSorceryEffect(spell_id, damage, 0);
		else
			damage += hero->GetSpellSpecialtyEffect(spell_id, 0, damage);
	}

	if (P_Spell[spell_id].flags.damage_spell) {
		string = 6;
	}

	switch (spell_id)
	{
	case H3Spell::FIRE_WALL:
		string = 6;
		break;

	case H3Spell::CURE:
		string = 7;
		break;

	case H3Spell::HYPNOTIZE:
		string = 8;
		break;

	case H3Spell::LAND_MINE:
		string = 9;
		break;

	case H3Spell::RESURRECTION:
	case H3Spell::ANIMATE_DEAD:
		string = 10;
		break;

	case H3Spell::CHAIN_LIGHTNING:
		string = 11;
		break;

	case H3Spell::FIRE_ELEMENTAL:
	case H3Spell::EARTH_ELEMENTAL:
	case H3Spell::WATER_ELEMENTAL:
	case H3Spell::AIR_ELEMENTAL:
		string = 12;
		if (hero)
			damage *= hero->primarySkill[2];
		break;

	default:
		if (string != 6)
			string = 0;
		break;
	}

	// если строку модифицировали
	if (string)
	{
		// проверяем язык игры и по ней корректируем номер строки
		string = GetString_Localosation(string);

		c->Push(damage); // вталкиваем push eax (см. 0x59C002)
		c->edx = (INT32)Spells_Description_TXT->GetText(string);

		// указываем адрес возврата (если модифицированно описание)
		c->return_address = 0x59C011;
	}
	else
	{
		// указываем адрес возврата (если НЕ модифицированно описание)
		c->return_address = 0x59C084;
	}

	return NO_EXEC_DEFAULT;
}


int __stdcall Y_LoadAllTXTinGames(LoHook* h, HookContext* c)
{
	Spells_Description_TXT = H3ColumnTextFile::Load("Spells_Description.txt");
	return EXEC_DEFAULT;
}


void StartPlugin()
{
	// создаем загрузку необходимых тектовиков
	_PI->WriteLoHook(0x4EDD65, Y_LoadAllTXTinGames);

	_PI->WriteLoHook(0x5A8B90, Y_Battle_Hint_SpellDescr_Prepare);
	_PI->WriteByte(0x5A8C31, 6); // модиф.свитч SPL_FIREBALL
	_PI->WriteByte(0x5A8C32, 6); // модиф.свитч SPL_INFERNO
	_PI->WriteByte(0x5A8C33, 6); // модиф.свитч SPL_METEOR_SHOWER

	// модификация указания силы некоторых заклинаний в книге
	_PI->WriteCodePatch(0x59BFBE, (char*)"%n", 12); // убираем проверку на флаг заклинания (потом проверяем сами)
	_PI->WriteLoHook(0x59BFE7, Y_DlgSpellBook_ModifSpell_Description);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	static BOOL pluginOn = FALSE;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!pluginOn)
		{
			_P = GetPatcher();
			_PI = _P->CreateInstance("Test_Plugin");


			StartPlugin();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
