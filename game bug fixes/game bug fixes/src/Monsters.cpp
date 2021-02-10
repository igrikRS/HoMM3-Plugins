/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

_int_ __stdcall Y_FixBagCreatureGredeOfNeutrals(HiHook* hook, _Army_* army, _int_ creature_id)
{
    _int_ count = 0;
    _int_ i = 0;
    _int_ crGrade_id = GetCreatureUprade(creature_id);
    do {
        if (army->type[i] == creature_id || army->type[i] == crGrade_id) {
            count += army->count[i];
        }
        i++;
    } while ( i<7 );

    return count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall fixHarpyBinds(LoHook* h, HookContext* c)
{
    if (*(int*)(c->ebx + 696)) {
        c->return_address = 0x478365;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// не считать кавалерийский бонус при полете
_int_ __stdcall Y_AntiKavalierAndFly(LoHook* h, HookContext* c)
{
    if ( *(_dword_*)(c->ebx +132) >> 1 & 1 ) { // проверить флаг атакующего на полет
        c->return_address = 0x4430A3; // обходим расчет кавалерийского бонуса (он всё равно не работает)
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//// фикс переполнения опыта существ (фикс проверки на max опыт)	
//_int_ __stdcall Y_WoGCrExpoSet_AddExpo(LoHook* h, HookContext* c)
//{
//	int cr_Expo = DwordAt(c->ebp -0x8);
//
//	if ( cr_Expo ) {
//		int expoOld = IntAt(cr_Expo);
//		int expoAdd = IntAt(c->ebp -0xC);
//
//		if ( expoAdd < 0 || expoAdd > 200000 ) 
//			expoAdd = 0;
//
//		// если опыт по какой то причине перевалил 
//		// через предел (2^32)/4 и ушёл в отрицательное число
//		// ставим максимальный опыт
//		if (expoOld < -1073741824) {
//			IntAt(cr_Expo) = 200000;
//		}
//
//		// если опыт был < 0 && > -1073741824, обнуляем опыт
//		if (expoOld < 0) { 
//		   IntAt(cr_Expo) = expoAdd; 
//		}
//	}
//
//    return EXEC_DEFAULT;
//} 

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//#define Wog_FOH_Monstr (*(int*)0x27718CC)
#define WoG_CreatureUpgradeTable (*(int*)0x724A95)

// Решаем проблему когда бонусы специалистов не считаются Супер существам
int __stdcall Y_FixWoG_GetCreatureGrade(LoHook* h, HookContext* c)
{
    int ID = *(int*)(c->ebp +8);
    int mon_id = c->ecx; 
    int mon_idGr =  *(int*)(WoG_CreatureUpgradeTable + mon_id*4);

    if (mon_idGr == -2) {
        return EXEC_DEFAULT;
    } else if (mon_idGr == -1) {
        mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
    }

    int mon_idGr2 = *(int*)(WoG_CreatureUpgradeTable + mon_idGr*4);

    if (mon_idGr2 == -2) {
        return EXEC_DEFAULT;
    } else if (mon_idGr2 == -1) {
        mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
    }

    if (ID == mon_idGr2 ) { c->ecx = mon_idGr; }

    return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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
        if(dmtype == 54 || dmtype == 55 || dmtype == 153) 
            mult = 50; 
            break;

        case 36:  // Джин
        case 37:  // Мастар Джин
        if(dmtype == 52 || dmtype == 53) 
            mult = 50; 
            break; 

        case 41:  // Титан
        case 152: // Громовержец
        if(dmtype == 83 || dmtype == 155) 
            mult = 50; 
            break;

        case 52:  // Ифрит
        case 53:  // Ифрит Султан
        if(dmtype == 36 || dmtype == 37) 
            mult = 50; 
            break;

        case 54:  // Дьявол
        case 55:  // Архидьявол
        case 153: // Барон Ада
        if(dmtype == 12 || dmtype == 13 || dmtype == 150) 
            mult = 50; 
            break;

        case 83:  // Черный Дракон
        case 155: // Тёмный Дракон
        if(dmtype == 41 || dmtype == 152) 
            mult = 50; 
            break;

        default: 
        break;
    }
    // теперь бонус урона нужно занести в EDX
    c->edx = mult;
    c->return_address = 0x766EEB;
    return NO_EXEC_DEFAULT;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#define WOG_HeroEntered2Object (*(_Hero_**)0x803300)

// показ лычек опыта в диалоге присоедиения монстров (он же диалог гарнизона)
int __stdcall Y_Dlg_AddCreatures_Init_Add(HiHook* hook, _Hero_* hero, int creatureType, int creatureCount)
{
    if ( hero ) 
        WOG_HeroEntered2Object = hero;

    return CALL_3(int, __fastcall, hook->GetDefaultFunc(), hero, creatureType, creatureCount);
}

// показ лычек опыта в диалоге оставления монстров на карте (он же диалог гарнизона)
int __stdcall Y_Dlg_AddCreatures_Init_Leave(HiHook* hook, _Hero_* hero, _Army_* army, int creatureType)
{
    if ( hero ) 
        WOG_HeroEntered2Object = hero;

    return CALL_3(int, __fastcall, hook->GetDefaultFunc(), hero, army, creatureType);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void Monsters(PatcherInstance* _PI)
{

    // исправления стрельбы при отрицат.боезапасе
    // при передаче хода стеку
    _PI->WriteLoHook(0x464D75, setActStack);
    // при стрельбе по стенам (катапульта, циклопы)
    _PI->WriteLoHook(0x445CF9, catapultaShoot);	
    // второй выстрел монстрами
    _PI->WriteLoHook(0x43FF92, monstreShoot);	
    // второй выстрел баллистой
    _PI->WriteLoHook(0x43FFF4, monstreShoot);	


    // исправление бага палатки, когда на её ходу невозможно убежать или сделать другие действия
    _PI->WriteByte(0x75C82C, 0xEB);

    // фикс бага блока существ (от опыта мностров), заключается в том
    // что существа бесконечно получают ход в текущем раунде и игра виснет
    _PI->WriteJmp(0x71C685, 0x71C69D); // убираем установку флага "в защите"

    // Решение бага (еще с SoD) исчезновения улучшенного стека при битве с даунгрейдом нейтралов
    _PI->WriteHiHook(0x4AC5F5, CALL_, EXTENDED_, THISCALL_, Y_FixBagCreatureGredeOfNeutrals);

    // фикс отлета гарпий, когда после удара они связаны корнями дендроидов
    _PI->WriteLoHook(0x47835B, fixHarpyBinds);

    // не считать кавалерийский бонус при полете
    _PI->WriteLoHook(0x44307A, Y_AntiKavalierAndFly);

    // Решение бага Вога, когда в бою накладывается опыт через EA:E и атака, защита, уроны, скорость, боезапасы и т.п. заново пересчитываются.
    // Из-за этого теряются бонусы наложенных заклинаний (например бонус скорости от ускорения)
    _PI->WriteHiHook(0x726DE4, CALL_, EXTENDED_, CDECL_, ERM_Fix_EA_E);

    // фикс переполнения опыта существ (вызов проверки на max опыт)	
	// _PI->WriteLoHook(0x71924A, Y_WoGCrExpoSet_AddExpo);

    // корректировка WoG ненависти существ
    // добавляем и существ 8-го уровня
    _PI->WriteLoHook(0x766E4E, Y_SetWogHates);

    // вызовы драконов от артефакта сердце дракона
    // меняем местами номера гексов, 
    // чтобы в банках существ не перекрывался стек №3
    _PI->WriteByte(0x767A05 +1, 55); 
    _PI->WriteByte(0x767A2A +1, 89); 

    // Решение бага ERM: триггер MA:U#/-2 приводил к тому, что любое существо при установке такой команды улучшалось в копейщика
    _PI->WriteDword(0x724A9F, -2);  _PI->WriteLoHook(0x724AC5, Fix_WoG_GetCreatureGrade_Expo);
    _PI->WriteDword(0x74ED27, -2);  _PI->WriteLoHook(0x74ED5C, Fix_WoG_GetCreatureGrade_Town);

    // Решаем проблему когда бонусы специалистов не считаются Супер существам
    // _PI->WriteHiHook(0x4E64FA, CALL_, EXTENDED_, FASTCALL_, Y_FixWoG_GetCreatureGrade);
    _PI->WriteLoHook(0x4E64D1, Y_FixWoG_GetCreatureGrade);


    // показ лычек опыта в диалоге присоединения и оставления монстров на карте
    _PI->WriteHiHook(0x5D15D0, SPLICE_, EXTENDED_, FASTCALL_, Y_Dlg_AddCreatures_Init_Add); 
    _PI->WriteHiHook(0x5D16B0, SPLICE_, EXTENDED_, FASTCALL_, Y_Dlg_AddCreatures_Init_Leave);

    // убираем отображение двух ошибок от опыта существ
    // 0x717B37 TError(15, 222, aCrexpoUnknownS);
    // 0x717FAB TError(15, 286, aCrexpoUnknow_0);    
    _PI->WriteCodePatch(0x717B37, "%n", 20); // 20 nops
    _PI->WriteCodePatch(0x717FAB, "%n", 20); // 20 nops

    // меняем картинки оставления монстров и для эстетики
    // и для того, чтобы визуально понимать, что мы вносили правку
    _PI->WriteByte(0x757365 +1, 35); // 35 - Меч Правосудия
    _PI->WriteByte(0x757369 +1, 13); // 13 - Архангел
}
