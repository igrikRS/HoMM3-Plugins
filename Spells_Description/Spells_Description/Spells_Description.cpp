#include "stdafx.h"
#include "..\..\..\include\homm3.h"

//#include "..\..\..\include\era.h"
//using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

struct _TXT_;
_TXT_* Spells_Description_TXT;

// получения номера строки с текстом
// в зависимости от локализации игры
int GetString_Localosation(int string_id)
{
	if (isRusLangWoG) 
		string_id += 14;

	return string_id;
}


int WoG_GetResistGolem(int spell_id, int damage, _BattleStack_* stack)
{
	int result = 0;

	if (stack->creature_id >= 174 && stack->creature_id <= 191) 
	{	// сопротивление командиров NPC::Resist(MR_Type, MR_Dam, MR_Mon);
		result = CALL_3(int, __cdecl, 0x76D506, stack->creature_id, damage, stack);
	} 
	else 
	{	
		result = stack->GetResistGolem(spell_id, damage); // SoD
	}
	// опыт стеков: CrExpBon::GolemResist(stack, result, damage, spell_id);
	result = CALL_4(int,  __cdecl, 0x71E766, stack, result, damage, spell_id);

	return result;
}

int BattleStack_Get_Killed_From_Damage(_BattleStack_* stack, int damage, int param)
{
	int killed = 0;

	if (stack && damage > 0) 
	{
		int lost = 0;

		int fullHealth;
		if ( param == 2 ) // нанеcение урона
		{
			fullHealth = stack->GetFullHealth(0);

			if (fullHealth > damage){
				if (stack->creature.hit_points > 1) {
					lost = (fullHealth - damage) / stack->creature.hit_points;
					if ((fullHealth - damage) % stack->creature.hit_points > 0)
						lost += 1;
				}
				else lost = fullHealth - damage;
				killed = stack->count_current - lost;  
			} else killed = stack->count_current; 
		}
		if (param == 3) // воскрешение и т.п.
		{
			fullHealth = (stack->count_at_start * stack->creature.hit_points) - stack->GetFullHealth(0);
			if (fullHealth <= damage) {
				killed = stack->count_at_start - stack->count_current;	
			} else {
				int lost_hp = stack->lost_hp;

				// если стек мертвый, его lost_hp не обнулены
				// поэтому будет считаться неправильно. А мы обнуляем.
				if ( stack->creature.flags & BCF_DIE)
					lost_hp = 0;

				int resurect_hp = damage - lost_hp -1;
				if ( resurect_hp > 0 )
				{
					killed = (resurect_hp / stack->creature.hit_points) +1;
				} 
			}
		}
	}

	return killed;	
}



int __stdcall Y_Battle_Hint_SpellDescr_Prepare(LoHook* h, HookContext* c)
{	
	int spell = DwordAt(c->ebp +8);
	int str_hint_id = 0; // номер строки в текстовом файле (если 0, то хук вообще пропускаем)

	// проверяем нужно ли модифицировать строку
	// в зависимости от заклинания
	switch ( spell )
	{
	case SPL_MAGIC_ARROW: 
	case SPL_ICE_BOLT:
	case SPL_LIGHTNING_BOLT:
	case SPL_IMPLOSION:
	case SPL_CHAIN_LIGHTNING:
	case SPL_TITANS_LIGHTNING_BOLT:
	case SPL_FIREBALL:
	case SPL_INFERNO:
	case SPL_METEOR_SHOWER:
		str_hint_id = 2;
		break;

	case SPL_RESURRECTION:
	case SPL_ANIMATE_DEAD:
		str_hint_id = 3;
		break;

	case SPL_CURE:
		str_hint_id = 4;
		break;

	default: 
		str_hint_id = 0;
		break;
	}
	
	// если строку хинта заклинания модифицируем
	if (str_hint_id) 
	{
		int damage = 0;
		int spell_power = 1;
		int lvl_spell = 0;
		int killed = 0;
		
		_BattleMgr_* bm = o_BattleMgr;
		_BattleHex_* hex = (_BattleHex_*)c->edi;
		_BattleStack_* stack = hex->GetCreature();

		if ( !stack ) 
		{
			// если воскрешение, то мы не сможем получить стек через гекс
			// поэтому пытаемся получить через другие функции
			if ( str_hint_id == 3 ) 
			{
				int hex_id = DwordAt(c->ebp +12);
				if ( spell == SPL_RESURRECTION )
					stack = CALL_4(_BattleStack_*, __thiscall, 0x5A3FD0, bm, bm->current_side, hex_id, 0);
				else stack = CALL_3(_BattleStack_*, __thiscall, 0x5A4260, bm, bm->current_side, hex_id);

			} else return EXEC_DEFAULT; // если всё же не получилось - выход
		}
		
		// если стек НЕ мертв
		if ( ! ( stack->creature.flags & BCF_DIE)  )
		{
			if ( !(stack->CanUseSpell(spell, bm->current_side, 1, 0) ) ) 
			{
				return EXEC_DEFAULT;
			}
		}

		_Hero_* hero = bm->hero[bm->current_side];

		if (hero) 
		{
			lvl_spell = hero->Get_School_Level_Of_Spell(spell, bm->spec_terr_type);
			spell_power = bm->hero_spellPower[bm->current_side];
		}

		damage = spell_power * o_Spell[spell].eff_power + o_Spell[spell].effect[lvl_spell];

		if (hero) 
		{
			if (o_Spell[spell].flags & SPF_DAMAGE )
				damage = (int)hero->Get_Spell_Power_Bonus_Arts_And_Sorcery(spell, damage, stack ); 
			else 
			{	
				damage += hero->GetSpell_Specialisation_PowerBonuses(spell, damage, stack->creature.level);	
			}
		}

		if ( stack )
		{
			if ( !(o_Spell[spell].flags & SPF_FRIENDLY_HAS_MASS) ) 
			{
				int resist = WoG_GetResistGolem(spell, damage, stack);
				damage = stack->GetResistSpellProtection(spell, resist);
			}	
		}

		if (str_hint_id == 4 ) 
		{ // если лечение
			if (stack->lost_hp < damage) 
			{
				damage = stack->lost_hp; // точное кол-во сколько вылечит hp
			}
			// проверяем язык игры и по ней корректируем номер строки
			str_hint_id = GetString_Localosation(str_hint_id);

			sprintf(o_TextBuffer, Spells_Description_TXT->GetString( str_hint_id ), (_cstr_)c->eax, damage );
		} 
		else 
		{
			killed = BattleStack_Get_Killed_From_Damage(stack, damage, str_hint_id);

			// проверяем язык игры и по ней корректируем номер строки
			str_hint_id = GetString_Localosation(str_hint_id);

			sprintf(o_TextBuffer, Spells_Description_TXT->GetString( str_hint_id ), o_Spell[spell].name, (_cstr_)c->eax, damage, killed );
		}


		c->return_address = 0x5A8BBF;
		return NO_EXEC_DEFAULT;
	} 

	else return EXEC_DEFAULT;
} 

int __stdcall Y_DlgSpellBook_ModifSpell_Description(LoHook* h, HookContext* c)
{
	int string = 0; // номер строки в текстовом файле (0 - не модифицировать)

	int spell_pow = c->eax;
	int spell_lvl = DwordAt(c->ebp -16);
	int spell_id = DwordAt(c->ebp +12);

	_Hero_* hero = (_Hero_*)DwordAt(c->ebp +16);

	int damage = spell_pow * o_Spell[spell_id].eff_power + o_Spell[spell_id].effect[spell_lvl];

	if (hero) 
	{
		if (o_Spell[spell_id].flags & SPF_DAMAGE )
			damage = (int)hero->Get_Spell_Power_Bonus_Arts_And_Sorcery(spell_id, damage, 0 );
		else
			damage += hero->GetSpell_Specialisation_PowerBonuses(spell_id, damage, 0);
	}

	if ( o_Spell[spell_id].flags & SPF_DAMAGE )	{
		string = 6;
	}

	switch (spell_id)
	{
	case SPL_FIRE_WALL:
		string = 6; 
		break;

	case SPL_CURE: 
		string = 7; 
		break;

	case SPL_HYPNOTIZE: 
		string = 8; 
		break;

	case SPL_LAND_MINE: 
		string = 9; 
		break;

	case SPL_RESURRECTION: 
	case SPL_ANIMATE_DEAD:
		string = 10; 
		break;

	case SPL_CHAIN_LIGHTNING: 
		string = 11; 
		break;

	case SPL_FIRE_ELEMENTAL: 
	case SPL_EARTH_ELEMENTAL:
	case SPL_WATER_ELEMENTAL:
	case SPL_AIR_ELEMENTAL:	

		// специальная проверка на ReMagic
		if ( !GetWoGOptionsStatus(726) ) 
		{
			string = 12;
			damage *= (int)(hero->power);
		}
		break;

	default: 
		if ( string != 6 )
			string = 0;
		break;
	}

	// если строку модифицировали
	if ( string )
	{
		// проверяем язык игры и по ней корректируем номер строки
		string = GetString_Localosation(string);

		c->Push(damage); // вталкиваем push eax (см. 0x59C002)
		c->edx = (int)Spells_Description_TXT->GetString(string);

		// Пока вырезаем способ ЭРЫ
		// SetPcharValue(o_TextBuffer, tr("isd.spell_book_damage", {"%d", IntToStr(500}).c_str(), 0x300);
		// lstrcpy(o_TextBuffer, tr("isd.test", {"damage", IntToStr(500}).c_str());
		c->return_address = 0x59C011;
	}
	else 
	{
		c->return_address = 0x59C084;
	}
	
	return NO_EXEC_DEFAULT;
}

int __stdcall Y_LoadAllTXTinGames(LoHook* h, HookContext* c)
{
	Spells_Description_TXT = _TXT_::Load( "Spells_Description.txt" );
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
	_PI->WriteCodePatch(0x59BFBE, "%n", 12); // убираем проверку на флаг заклинания (потом проверяем сами)
	_PI->WriteLoHook(0x59BFE7, Y_DlgSpellBook_ModifSpell_Description);

	return;
}



BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	static _bool_ initialized = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!initialized)
		{
			initialized = 1;

			_P = GetPatcher();
			_PI = _P->CreateInstance("Spells_Description");

			// Era::ConnectEra();

			StartPlugin();
			
		}
		break;

	case DLL_PROCESS_DETACH:
		if (initialized)
			initialized = 0;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

