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

// правка функции WoG: накопление существ и стражников в двеллингах существ
// теперь:
//   - существа и стражники копятся, только если двеллинг принадлежит одному из игроков
//   - Санта-Гремлины копятся в обычном режиме
//   - Существа 8 уровня (№№ 150-158) копятся в обычном режиме

//#define o_WogAccumCreatures (*(_int_*)0x277193C)
//#define o_WogAccumDefenders (*(_int_*)0x2771940)
//
//int __stdcall SOD_Dwelling_Add_Creatures(HiHook* hook, _Dwelling_* dw, _int_ isBonus)
//{
//    int countCreatures[4];
//    int countDefenders[4];
//
//    if( dw->owner_ix != -1 ) {
//        for (int i=0; i<4; i++) {
//            if (o_WogAccumCreatures) {
//                countCreatures[i] = dw->creature_counts[i];
//        }
//            if (o_WogAccumDefenders) {
//                countDefenders[i] = dw->defenders.count[i];
//            }
//        }
//    }
//
//    int result = CALL_2(int, __thiscall, hook->GetDefaultFunc(), dw, isBonus);
//
//    if( dw->owner_ix != -1 ) {
//        for (int i=0; i<4; i++) {
//            if (o_WogAccumCreatures && countCreatures[i] > 0 && countCreatures[i] <= 4000 ) {
//                dw->creature_counts[i] += countCreatures[i];
//            }
//            if (o_WogAccumDefenders && countDefenders[i] > 0 && countDefenders[i] <= 12000) {
//                dw->defenders.count[i] += countDefenders[i];
//            }
//        }
//    }
//
//    return result;
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//// фикс сетевой разсинхронизации стеков в бою
//void __stdcall Y_SelectNewMonsterToAct(HiHook* hook, _BattleMgr_* bm, int side, int stack_id_in_side)
//{
//    if (o_NetworkGame) {
//        if ( o_GameMgr->IsPlayerHuman(bm->owner_id[1]) ) { 
//            if ( o_GameMgr->IsPlayerHuman(bm->owner_id[0]) ) { 
//                _int32_ net[14197];  // 56748 +40 = 56788 / 4 = 14197
//                net[0] = -1;
//                net[1] = NULL;
//                net[2] = 1987;
//                net[3] = 56788;
//                net[4] = 0;
//                net[5] = side; 
//                net[6] = stack_id_in_side;
//
//                int bmStart = (int)o_BattleMgr +21708 -40;
//                for (int i=10; i<14197; i++) {
//                	net[i] = *(int*)(bmStart +i*4);
//                }
//
//                _int32_ isGood = CALL_4(_int32_, __fastcall, 0x5549E0, (int)&net, *(int*)(0x697790 +4*bm->current_side), 0, 1); 
//                if ( !isGood ) 
//                	CALL_1(char, __thiscall, 0x4F3D20, 0);
//
//                delete[] net;
//            }
//        }
//    }
//    CALL_3(void, __thiscall, hook->GetDefaultFunc(), bm, side, stack_id_in_side);
//}
//
//int __stdcall Y_BM_ReceNetData(LoHook* h, HookContext* c)
//{
//    int id = *(int*)(c->esi +8);
//    if ( id == 1987 ) {
//
//        _int32_ netData = c->esi; // netData содержит прямой адрес переданного массива net[14197] 
//
//        int bmStart = (int)o_BattleMgr +21708; // начало стеков
//        int netDSt = netData +40;
//        for (int i=0; i<42; i++) {
//            for (int k=0; k<1352; k++) { 
//                if (k>=40 && k<44) continue;
//                if (k>=116 && k<172) continue;
//                if (k>=272 && k<404) continue;				
//                if (k>=1056 && k<1108) continue;
//                if (k>=1264) continue;
//
//                *(char*)(bmStart +i*1352 +k) = *(char*)(netDSt +i*1352 +k);
//            }
//        }
//
//        CALL_3(void, __thiscall, 0x464F10, o_BattleMgr, *(int*)(netData +20), *(int*)(netData +24));
//
//        CALL_1(void*, __thiscall, 0x555D00, netData); // деструктор
//    }
//
//    return EXEC_DEFAULT; 
//}

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

    // правка функции WoG: накопление существ и стражников в двеллингах существ, теперь:
    //   - существа и стражники копятся, только если двеллинг принадлежит одному из игроков
    //   - Санта-Гремлины копятся в обычном режиме
    //   - Существа 8 уровня (№№ 150-158) копятся в обычном режиме    
    // сначала восстанавливаем оригинальный код игры   CALL 0x4B8760
    // снимая установку хука WoG                       CALL 0x760BDB
    // _PI->WriteHexPatch(0x4C8795 +1, "C6FFFEFF"); 
    // модифицируем еженедельную прибавку монстрам
    // _PI->WriteHiHook(0x4C8795, CALL_, EXTENDED_, THISCALL_, SOD_Dwelling_Add_Creatures);

    // частичное исправление разсихнронизации 
    // сетевое копирование параметров стеков в битве
    // _PI->WriteHiHook(0x464F10, SPLICE_, EXTENDED_, THISCALL_, Y_SelectNewMonsterToAct);
    // _PI->WriteLoHook(0x473D41, Y_BM_ReceNetData);
}
