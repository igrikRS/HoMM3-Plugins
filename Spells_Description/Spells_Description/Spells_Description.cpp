#include "stdafx.h"
#include "..\..\..\include\homm3.h"

#pragma warning( disable : 4482 )

Patcher* _P;
PatcherInstance* _PI;

struct _TXT_;
_TXT_* SpDescr_TXT;

char myString[1024];
#define MyString  (char*)myString

_bool8_ SacrificeType;
_BattleStack_* SacrificeStack;

enum SPELL_TYPE
{
    BUFF      = 0,
    EMPTY_1   = 1,
    DAMAGE    = 2,
    RECOVERY  = 3,
    CURE      = 4,
    SACRIFICE = 5,
    TELEPORT  = 6,
    REMOVE_OBSTACLE = 7,
};

enum STRING_ID
{
    NONE               = 0,
    NONE_1             = 1,
    BATTLE_DAMAGE      = 2,
    BATTLE_RECOVERY    = 3,
    BATTLE_CURE        = 4,
    BATTLE_SACRIFICE_1 = 5,
    BATTLE_SACRIFICE_2 = 6,
    NONE_4             = 7,
    BOOK_DAMAGE        = 8,
    BOOK_CURE          = 9,
    BOOK_HYPNOTIZE     = 10,
    BOOK_LAND_MINE     = 11,
    BOOK_RECOVERY      = 12,
    BOOK_CHAIN_LIGHTNING = 13,
    BOOK_SUMMON        = 14,
    BATTLE_PIT_LORD    = 15,
    BATTLE_ARCHANGEL   = 16
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// вспомогательные функции ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// получения номера строки с текстом
// в зависимости от локализации игры
int GetString_Localosation(int string_id)
{
    if (isRusLangWoG) 
        string_id += 17;

    return string_id;
}

// перевод тысяч в k или M форматы
char* FormatedNumber(char* source, int number)
{
    if (number >= 10000000)
        sprintf(source, "%dM", number / 1000000);
    else if (number >= 10000)
        sprintf(source, "%dk", number / 1000);
    else
        sprintf(source, "%d", number);   

    return source;
}

// получить номер строки в текстовом файле
int GetSpellType(int spellId)
{
    int result = 0;
    // проверяем нужно ли модифицировать строку
    // в зависимости от заклинания
    switch ( spellId )
    {
        case SPL_MAGIC_ARROW: 
        case SPL_ICE_BOLT:
        //case SPL_FROST_RING:
        case SPL_LIGHTNING_BOLT:
        case SPL_IMPLOSION:
        case SPL_CHAIN_LIGHTNING:
        case SPL_TITANS_LIGHTNING_BOLT:
        case SPL_FIREBALL:
        case SPL_INFERNO:
        case SPL_METEOR_SHOWER:
            result = SPELL_TYPE::DAMAGE;
            break;

        case SPL_RESURRECTION:
        case SPL_ANIMATE_DEAD:
            result = SPELL_TYPE::RECOVERY;
            break;

        case SPL_CURE:
            result = SPELL_TYPE::CURE;
            break;

        case SPL_SACRIFICE:
            result = SPELL_TYPE::SACRIFICE;
            break;

        case SPL_TELEPORT:
            result = SPELL_TYPE::TELEPORT;
            break;

        case SPL_REMOVE_OBSTACLE:
            result = SPELL_TYPE::REMOVE_OBSTACLE;
            break;

        default: 
            result = SPELL_TYPE::BUFF;
            break;
    }

    return result;
}


// получение силы сопростивления
int WoG_GetResistGolem(int spell_id, int damage, _BattleStack_* stack)
{
    int result = 0;

    if (stack->creature_id >= 174 && stack->creature_id <= 191) 
    {   // сопротивление командиров NPC::Resist(MR_Type, MR_Dam, MR_Mon);
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


// получение кол-ва убитых от ударного заклинания
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
                if (stack->creature.flags & BCF_DIE)
                    lost_hp = 0;

                int resurect_hp = damage - lost_hp -1;
                if ( resurect_hp > 0 )
                {
                    killed = (resurect_hp / stack->creature.hit_points) +1;

                    // На заметку! Для оживления мертвецов
                    // когда (stack->count_current < 1) возможно нужно сделать декремент
                    // --killed;
                }
            }
        }
    }

    return killed;
}

// получение силы заклинания монстра WoG
int GetWoGCreatureSpellPower(_BattleStack_* stack)
{
    int result = 0;
    if (stack->creature_id == CID_FAERIE_DRAGON)
    {
        result = stack->count_current * 5;
    }
    else if (stack->creature_id >= CID_COMMANDER_FIRST_A || stack->creature_id <= CID_COMMANDER_LAST_D)
    {
        result = stack->GetNPCMagicPower();
    }
    return result;
}

// создание и запись модифицированной строки в o_TextBuffer
_bool8_ SetModifiedHintTo_TextBuffer(_BattleMgr_* bm, _BattleStack_* stack, _int_ spellId, _int_ spellType, _int_ creatureSpellPower)
{
    if (!stack) return FALSE;

    int spell_power = 1;
    int spell_lvl = 0;
    int killed = 0;
    int damage = 0;
    
    // если стек НЕ мертв
    if ( ! ( stack->creature.flags & BCF_DIE) )
    {
        if ( spellId != SPL_SACRIFICE && !(stack->CanUseSpell(spellId, bm->currentActiveSide, 1, 0) ) ) 
        {
            return FALSE;
        }
    }

    _Hero_* hero = bm->hero[bm->currentActiveSide];

    if (hero)
    {
        spell_lvl = hero->Get_School_Level_Of_Spell(spellId, bm->special_Ground);
        spell_power = bm->heroSpellPower[bm->currentActiveSide];
    }

    // если колдует монстр - затираем данные силы каста от героя
    if (creatureSpellPower)
    {
        spell_lvl = HSSL_ADVANCED;
        spell_power = creatureSpellPower;
    }

    damage = spell_power * o_Spell[spellId].eff_power + o_Spell[spellId].effect[spell_lvl];

    if (hero)
    {
        if (o_Spell[spellId].flags & SPF_DAMAGE )
            damage = (int)hero->Get_Spell_Power_Bonus_Arts_And_Sorcery(spellId, damage, stack );
        else 
        {   
            damage += hero->GetSpell_Specialisation_PowerBonuses(spellId, damage, stack->creature.level);
        }
    }

    if ( !(o_Spell[spellId].flags & SPF_FRIENDLY_HAS_MASS) )
    {
        int resist = WoG_GetResistGolem(spellId, damage, stack);
        damage = stack->GetResistSpellProtection(spellId, resist);
    }

    _cstr_ stackName = GetCreatureName(stack->creature_id, stack->count_current);
    _int_ stringHintId = GetString_Localosation(spellType);
    char text1[128], text2[128];

    if (spellType == SPELL_TYPE::CURE)
    {
        if (stack->lost_hp < damage)
        {
            damage = stack->lost_hp;
        }
        sprintf(o_TextBuffer, SpDescr_TXT->GetString( stringHintId ), stackName, FormatedNumber(text1, damage) );
    }
    else if (spellType == SPELL_TYPE::SACRIFICE)
    {
        if (SacrificeType)
        {
            _int32_ lostHealth = (stack->count_at_start * stack->creature.hit_points) - stack->GetFullHealth(0);
            killed = stack->count_at_start - stack->count_current;
            SacrificeStack = stack;
            sprintf(o_TextBuffer, SpDescr_TXT->GetString( stringHintId ), stackName, FormatedNumber(text1, lostHealth), FormatedNumber(text2, killed) );
        }
        else
        {
            if (!SacrificeStack) return FALSE;

            _int32_ currentHealth = stack->GetFullHealth(0) + damage*stack->count_current;
            _int32_ canResurrectCount = currentHealth / SacrificeStack->creature.hit_points;
            _int32_ lostHealth = (SacrificeStack->count_at_start * SacrificeStack->creature.hit_points) - SacrificeStack->GetFullHealth(0);

            killed = SacrificeStack->count_at_start - SacrificeStack->count_current;
            if (canResurrectCount > killed) canResurrectCount = killed;

            char text3[128], text4[128];
            sprintf(o_TextBuffer, SpDescr_TXT->GetString( stringHintId +1 ), stackName, 
                FormatedNumber(text1, currentHealth), FormatedNumber(text2, lostHealth),
                FormatedNumber(text3, canResurrectCount), FormatedNumber(text4, killed) );
        }
    }
    else
    {
        killed = BattleStack_Get_Killed_From_Damage(stack, damage, spellType);
        sprintf(o_TextBuffer, SpDescr_TXT->GetString( stringHintId ), o_Spell[spellId].name, stackName, FormatedNumber(text1, damage), FormatedNumber(text2, killed) );
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// перехваты игровых функций /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// модификация хинта в бою при применении заклинания
void __stdcall Y_BattleMgr_BattleLog_PrepareSpellMessage(HiHook* hook, _BattleMgr_* bm, _int32_ spellId, _int32_ gexId, _bool8_ type)
{
    if (bm->IsHiddenBattle()) return;

    _BattleHex_ hex = bm->hex[gexId];
    _BattleStack_* stack = hex.GetCreature();
    _int32_ spellType = GetSpellType(spellId);
    _bool8_ isModifedString = false;

    switch (spellType)
    {
        case SPELL_TYPE::DAMAGE:
            isModifedString = true;
            break;
        case SPELL_TYPE::RECOVERY:
            if ( spellId == SPL_RESURRECTION )
                stack = bm->Get_BattleStack_Resurrect(bm->currentActiveSide, gexId, 0);
            else stack = bm->Get_BattleStack_AnimatedDead(bm->currentActiveSide, gexId);
            
            if (stack)
                isModifedString = true;
            break;
        case SPELL_TYPE::CURE:
            isModifedString = true;
            break;
        case SPELL_TYPE::SACRIFICE:
            if (type)
                stack = bm->Get_BattleStack_Resurrect(bm->currentActiveSide, gexId, 0);
            else 
                stack = hex.GetCreature();
            SacrificeType = type;
            isModifedString = true;
            break;
        case SPELL_TYPE::TELEPORT:
            if (o_ChooseHexBySpellTeleport)
            {
                bm->AddHintMessage(o_GENRLTXT_TXT->GetString(27));
                return;
            }
            break;
        case SPELL_TYPE::REMOVE_OBSTACLE:
            bm->AddHintMessage(o_GENRLTXT_TXT->GetString(552));
            return;
            break;

        default:  break;
    }

    if (isModifedString)
    {
        _int_ power = 0;
        // но если колдует монстр, то получаем его силу закла
        if (bm->action == bm->BA_MONSTER_SPELL)
        {
            _BattleStack_* stackActive = o_BattleMgr->GetCurrentStack();
            if (stackActive)
                power = GetWoGCreatureSpellPower(stackActive);
        }
        isModifedString = SetModifiedHintTo_TextBuffer(bm, stack, spellId, spellType, power);
    }

    if (!isModifedString)
    {
        if (stack)
        {
            char* creatureName = GetCreatureName(stack->creature_id, stack->count_current);
            sprintf(o_TextBuffer, o_GENRLTXT_TXT->GetString(29), o_Spell[spellId].name, creatureName); 
        }
        else sprintf(o_TextBuffer, o_GENRLTXT_TXT->GetString(28), o_Spell[spellId].name);
    }
    bm->AddHintMessage(o_TextBuffer);
}



// модификация описания заклинания в книге
int __stdcall Y_DlgSpellBook_ModifSpell_Description(LoHook* h, HookContext* c)
{
    int string = STRING_ID::NONE; // номер строки в текстовом файле (NONE - не модифицировать)

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

    if ( o_Spell[spell_id].flags & SPF_DAMAGE ) {
        string = STRING_ID::BOOK_DAMAGE;
    }

    switch (spell_id)
    {
    case SPL_FIRE_WALL:
        string = STRING_ID::BOOK_DAMAGE; 
        break;

    case SPL_CURE: 
        string = STRING_ID::BOOK_CURE; 
        break;

    case SPL_HYPNOTIZE: 
        string = STRING_ID::BOOK_HYPNOTIZE; 
        break;

    case SPL_LAND_MINE: 
        string = STRING_ID::BOOK_LAND_MINE; 
        break;

    case SPL_RESURRECTION: 
    case SPL_ANIMATE_DEAD:
        string = STRING_ID::BOOK_RECOVERY; 
        break;

    case SPL_CHAIN_LIGHTNING: 
        string = STRING_ID::BOOK_CHAIN_LIGHTNING; 
        break;

    case SPL_FIRE_ELEMENTAL: 
    case SPL_EARTH_ELEMENTAL:
    case SPL_WATER_ELEMENTAL:
    case SPL_AIR_ELEMENTAL: 

        // специальная проверка на ReMagic
        if ( !GetWoGOptionsStatus(726) ) 
        {
            string = STRING_ID::BOOK_SUMMON;
            damage *= (int)(hero->primary_skill[3]);
        }
        break;

    default:
        if ( string != STRING_ID::BOOK_DAMAGE )
            string = STRING_ID::NONE;
        break;
    }

    // если строку модифицировали
    if ( string )
    {
        // проверяем язык игры и по ней корректируем номер строки
        string = GetString_Localosation(string);

        c->Push(damage); // вталкиваем push eax (см. 0x59C002)
        c->edx = (int)SpDescr_TXT->GetString(string);

        c->return_address = 0x59C011;
    }
    else 
    {
        c->return_address = 0x59C084;
    }
    
    return NO_EXEC_DEFAULT;
}

// показ кол-ва воскрешаемых существ Архангелами и Пит-Лордами
int __stdcall Y_Battle_Hint_Prepare_ResurrectArchangel(LoHook* h, HookContext* c)
{
    // получаем активную строну и целевой гекс
    int gex_id = c->eax;
    int side = c->esi;
    
    // получаем структуры активного и целевого стека
    _BattleStack_* stack_active = (_BattleStack_*)c->ebx;
    _BattleStack_* stack_target = o_BattleMgr->Get_BattleStack_Resurrect(side, gex_id, 1);

    if (!stack_target) 
    {
        // если целевой стек не найден, ищем через оригинальную функцию
        stack_target = CALL_3(_BattleStack_*, __thiscall, 0x5A4150, o_BattleMgr, side, gex_id);
    }

    // подготавливаем переменные: кол-во воскрешаемых и их название
    char* mon_name = GetCreatureName(stack_target->creature_id, stack_target->count_current);
    int count = stack_active->Get_Resurrect_Count(stack_target);

    // в зависимости от адреса возврата понимаем какой из двух хуков срабатал
    // и от этого выбираем тип строки из двух возможных 
    int str_id = STRING_ID::BATTLE_ARCHANGEL;
    if ( c->return_address != 4794996 )  // dec: (0x492A6E +6)
        str_id = STRING_ID::BATTLE_PIT_LORD;

    // собираем текст подсказки
    sprintf(o_TextBuffer, SpDescr_TXT->GetString(GetString_Localosation(str_id)), FormatedNumber(myString, count), mon_name);

    // пропускаем стандартный код игры
    c->return_address = 0x492E3B;
    return NO_EXEC_DEFAULT;
}

// фиксим оригинальную функцию игры: ИИ теперь знает о существовании существа с id = 150
int __stdcall Y_Fix_Funk_Get_Resurrect_Count(LoHook* h, HookContext* c)
{
    int mon_id = c->eax;
    if ( mon_id == CID_ARCHANGEL || mon_id == CID_SUPREME_ARCHANGEL ) 
         c->return_address = 0x44705F;
    else c->return_address = 0x447098;

    return NO_EXEC_DEFAULT;
}

// показ при колдовстве Волшебного Дракона (и других существ - модификация WoG'a, например командиры)
int __stdcall Y_Battle_Hint_Prepare_WoG_Creatures(LoHook* h, HookContext* c)
{
    _BattleStack_* stackTarget = (_BattleStack_*)c->eax;
    if (!stackTarget) return EXEC_DEFAULT;
    _BattleStack_* stackActive = o_BattleMgr->GetCurrentStack();

    _int_ power = GetWoGCreatureSpellPower(stackActive);
    if (!power) return EXEC_DEFAULT;

    _int_ spellId = stackActive->faerie_dragon_spell;
    _int_ spellType = GetSpellType(spellId);

    // модифицируем хинт заклинания
    if ( !SetModifiedHintTo_TextBuffer(o_BattleMgr, stackTarget, spellId, spellType, power) )
        return EXEC_DEFAULT;

    c->return_address = 0x492E3B;
    return NO_EXEC_DEFAULT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// инициализации ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_LoadAllTXTinGames(LoHook* h, HookContext* c)
{
    SpDescr_TXT = _TXT_::Load( "SpDescr.txt" );
    return EXEC_DEFAULT;
}


void StartPlugin()
{
    // создаем загрузку необходимых тектовиков
    _PI->WriteLoHook(0x4EDD65, Y_LoadAllTXTinGames);

    _PI->WriteHiHook(0x5A89A0, SPLICE_, EXTENDED_, THISCALL_, Y_BattleMgr_BattleLog_PrepareSpellMessage);

    // модификация указания силы некоторых заклинаний в книге
    _PI->WriteCodePatch(0x59BFBE, "%n", 12); // убираем проверку на флаг заклинания (потом проверяем сами)
    _PI->WriteLoHook(0x59BFE7, Y_DlgSpellBook_ModifSpell_Description);

    // показ кол-ва воскрешаемых существ Архангелами и Пит-Лордами
    _PI->WriteLoHook(0x492A6E, Y_Battle_Hint_Prepare_ResurrectArchangel);
    _PI->WriteLoHook(0x492AF6, Y_Battle_Hint_Prepare_ResurrectArchangel);
    _PI->WriteLoHook(0x44705A, Y_Fix_Funk_Get_Resurrect_Count);

    // показ при колдовстве Волшебного Дракона (и других существ - модификация WoG'a, например командиры)
    _PI->WriteLoHook(0x492B8E, Y_Battle_Hint_Prepare_WoG_Creatures);

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
            _PI = _P->CreateInstance("ERA Spells Description");

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
