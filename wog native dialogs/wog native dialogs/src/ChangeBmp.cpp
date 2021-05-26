

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_ChangeBmp(PatcherInstance* _PI)
{   
    // диалог опыта монстров
    _PI->WriteHexPatch(0x71F0AA, "90 90"); // показ кол-ва знемён (если есть, то 1-4)

    _PI->WriteDword(0x792048, 0); // E_ART.bmp
    int sdv = 7;
    _PI->WriteDword((0x71F86F +sdv), 1);  // E_SHOOTN.bmp
    _PI->WriteDword((0x71F89F +sdv), 2);  // N_SHOOTN.bmp

    _PI->WriteDword((0x71F94C +sdv), 3);  // E_CASTER.bmp
    _PI->WriteDword((0x71F97C +sdv), 4);  // N_CASTER.bmp

    _PI->WriteDword((0x71FA2D +sdv), 5);  // E_FEAR.bmp
    _PI->WriteDword((0x71FA5E +sdv), 6);  // N_FEAR.bmp

    _PI->WriteDword((0x71FB0B +sdv), 7);  // E_RETAIL1.bmp
    _PI->WriteDword((0x71FB3B +sdv), 8);  // N_RETAIL1.bmp

    _PI->WriteDword((0x71FBE8 +sdv), 9);  // E_DEFBON.bmp
    _PI->WriteDword((0x71FC18 +sdv), 10); // N_DEFBON.bmp

    _PI->WriteDword((0x71FCC5 +sdv), 11); // E_DWARF.bmp
    _PI->WriteDword((0x71FCF5 +sdv), 12); // N_DWARF.bmp

    _PI->WriteDword((0x71FDA8 +sdv), 13); // E_GOLEM.bmp
    _PI->WriteDword((0x71FDD8 +sdv), 14); // N_GOLEM.bmp

    _PI->WriteDword((0x71FE89 +sdv), 15); // E_DIST.bmp
    _PI->WriteDword((0x71FEBA +sdv), 16); // N_DIST.bmp

    _PI->WriteDword((0x71FF6B +sdv), 17); // E_OBST.bmp
    _PI->WriteDword((0x71FF9C +sdv), 18); // N_OBST.bmp

    _PI->WriteDword((0x72007B +sdv), 19); // E_FLY.bmp
    _PI->WriteDword((0x7200AC +sdv), 20); // N_FLY.bmp

    _PI->WriteDword((0x720156 +sdv), 21); // E_SHOOT.bmp
    _PI->WriteDword((0x720187 +sdv), 22); // N_SHOOT.bmp

    _PI->WriteDword((0x720231 +sdv), 23); // E_BREATH.bmp
    _PI->WriteDword((0x720262 +sdv), 24); // N_BREATH.bmp

    _PI->WriteDword((0x72030C +sdv), 25); // E_ALIVE.bmp
    _PI->WriteDword((0x72033D +sdv), 26); // N_ALIVE.bmp

    _PI->WriteDword((0x7203E7 +sdv), 27); // E_KING1.bmp
    _PI->WriteDword((0x720418 +sdv), 28); // N_KING1.bmp

    _PI->WriteDword((0x7204C2 +sdv), 29); // E_KING2.bmp
    _PI->WriteDword((0x7204F3 +sdv), 30); // N_KING2.bmp

    _PI->WriteDword((0x72059D +sdv), 31); // E_KING3.bmp
    _PI->WriteDword((0x7205CE +sdv), 32); // N_KING3.bmp

    _PI->WriteDword((0x720678 +sdv), 33); // E_MIND.bmp
    _PI->WriteDword((0x7206A9 +sdv), 34); // N_MIND.bmp

    _PI->WriteDword((0x720753 +sdv), 35); // E_MELEE.bmp
    _PI->WriteDword((0x720784 +sdv), 36); // N_MELEE.bmp

    _PI->WriteDword((0x72082E +sdv), 37); // E_FIRE.bmp
    _PI->WriteDword((0x72085F +sdv), 38); // N_FIRE.bmp

    _PI->WriteDword((0x720909 +sdv), 39); // E_DOUBLE.bmp
    _PI->WriteDword((0x72093A +sdv), 40); // N_DOUBLE.bmp

    _PI->WriteDword((0x7209E4 +sdv), 41); // E_RETAIL.bmp
    _PI->WriteDword((0x720A15 +sdv), 42); // N_RETAIL.bmp

    _PI->WriteDword((0x720ABF +sdv), 43); // E_MORAL.bmp
    _PI->WriteDword((0x720AF0 +sdv), 44); // N_MORAL.bmp

    _PI->WriteDword((0x720B9A +sdv), 45); // E_UNDEAD.bmp
    _PI->WriteDword((0x720BCB +sdv), 46); // N_UNDEAD.bmp

    _PI->WriteDword((0x720C75 +sdv), 47); // E_ROUND.bmp
    _PI->WriteDword((0x720CA6 +sdv), 48); // N_ROUND.bmp

    _PI->WriteDword((0x720D50 +sdv), 49); // E_DRAGON.bmp
    _PI->WriteDword((0x720D81 +sdv), 50); // N_DRAGON.bmp

    _PI->WriteDword((0x720E34 +sdv), 51); // E_HARPY.bmp
    _PI->WriteDword((0x720E65 +sdv), 52); // N_HARPY.bmp

    _PI->WriteDword((0x720F18 +sdv), 53); // E_CHAMP.bmp
    _PI->WriteDword((0x720F49 +sdv), 54); // N_CHAMP.bmp

    _PI->WriteDword((0x720FFC +sdv), 55); // E_MINOT.bmp
    _PI->WriteDword((0x72102D +sdv), 56); // N_MINOT.bmp

    _PI->WriteDword((0x7210E0 +sdv), 57); // E_SHOOTA.bmp
    _PI->WriteDword((0x721111 +sdv), 58); // N_SHOOTA.bmp

    _PI->WriteDword((0x7211C4 +sdv), 59); // E_FEARL.bmp
    _PI->WriteDword((0x7211F5 +sdv), 60); // N_FEARL.bmp

    _PI->WriteDword((0x7212A8 +sdv), 61); // E_REBIRTH.bmp
    _PI->WriteDword((0x7212D9 +sdv), 62); // N_REBIRTH.bmp

    _PI->WriteDword((0x72138C +sdv), 63); // E_UNIC.bmp
    _PI->WriteDword((0x7213BD +sdv), 64); // N_UNIC.bmp

    _PI->WriteDword((0x721484 +sdv), 65); // E_BLOCK.bmp
    _PI->WriteDword((0x7214B4 +sdv), 66); // N_BLOCK.bmp

    _PI->WriteDword((0x72155E +sdv), 67); // E_MANA.bmp
    _PI->WriteDword((0x72158E +sdv), 68); // N_MANA.bmp

    _PI->WriteDword((0x721638 +sdv), 69); // E_DBLOW.bmp
    _PI->WriteDword((0x721668 +sdv), 70); // N_DBLOW.bmp

    _PI->WriteDword((0x721A01 +sdv), 71); // E_HATE.bmp
    _PI->WriteDword((0x721A31 +sdv), 72); // N_HATE.bmp

    _PI->WriteDword((0x721C59 +sdv), 73); // E_TROLL.bmp
    _PI->WriteDword((0x721C89 +sdv), 74); // N_TROLL.bmp

    _PI->WriteDword((0x721E4C +sdv), 75); // E_CAST.bmp
    _PI->WriteDword((0x721E7C +sdv), 76); // N_CAST.bmp

    _PI->WriteDword((0x722037 +sdv), 77); // E_CAST1.bmp
    _PI->WriteDword((0x722067 +sdv), 78); // N_CAST1.bmp

    _PI->WriteDword((0x72224B +sdv), 77); // E_CAST1.bmp
    _PI->WriteDword((0x72227B +sdv), 78); // N_CAST1.bmp

    _PI->WriteDword((0x722436 +sdv), 79); // E_CAST2.bmp
    _PI->WriteDword((0x722466 +sdv), 80); // N_CAST2.bmp

    _PI->WriteDword((0x722643 +sdv), 79); // E_CAST2.bmp
    _PI->WriteDword((0x722673 +sdv), 80); // N_CAST2.bmp

    _PI->WriteDword((0x722A86 +sdv), 81); // E_BLESS.bmp
    _PI->WriteDword((0x722AB6 +sdv), 82); // N_BLESS.bmp

    _PI->WriteDword((0x72303C +sdv), 83); // E_RDEF.bmp
    _PI->WriteDword((0x72306C +sdv), 84); // N_RDEF.bmp

    _PI->WriteDword((0x72311F +sdv), 85); // E_DEATH.bmp
    _PI->WriteDword((0x72314F +sdv), 86); // N_DEATH.bmp

    _PI->WriteDword((0x72341C +sdv), 87); // E_BLOCK1.bmp
    _PI->WriteDword((0x72344C +sdv), 88); // N_BLOCK1.bmp

    _PI->WriteDword((0x7234F6 +sdv), 89); // E_BLOCK2.bmp
    _PI->WriteDword((0x723526 +sdv), 90); // N_BLOCK2.bmp

    _PI->WriteDword((0x723754 +sdv), 91); // E_SGTYPE.bmp
    _PI->WriteDword((0x723784 +sdv), 92); // N_SGTYPE.bmp

    _PI->WriteDword((0x7237D5 +sdv), 93); // E_SGNUM.bmp
    _PI->WriteDword((0x723805 +sdv), 94); // N_SGNUM.bmp

    _PI->WriteDword((0x7239C8 +sdv), 95); // E_SUMMON.bmp
    _PI->WriteDword((0x7239F8 +sdv), 96); // N_SUMMON.bmp

    _PI->WriteDword((0x723BBB +sdv), 97); // E_SUMMON1.bmp
    _PI->WriteDword((0x723BEB +sdv), 98); // N_SUMMON1.bmp

    _PI->WriteDword((0x723DAD +sdv), 99); // E_SUMMON2.bmp
    _PI->WriteDword((0x723DDD +sdv), 100); // N_SUMMON2.bmp 

    int adress = 0x792848;
    for (int i = 0; i < 58; i++){
        _PI->WriteDword(adress, 101+i);
        adress += 4;
    }

    // заменяем длинные названия картинок "артефактов" на кадры дефа "artifact.def"
    _PI->WriteDword(0x76AB3B, 0); // "None" на 0
    adress = 0x7A3B78;
    for (int i = 146; i < 156; i++){
        _PI->WriteDword(adress, i);
        adress += 4;
    }

    // заменяем длинные названия картинок "навыки класса" командира на кадры дефа "Dlg_NPC1.def"
    adress = 0x7A2228;
    for (int i = 0; i < 18; i++){
        _PI->WriteDword(adress, i);
        adress += 4;
    }

    // заменяем длинные названия "Основные навыки командира" ИЗУЧЕННЫЕ на кадры дефа "Dlg_NPC2.def"
    adress = 0x7A26F0;
    for (int i = 0; i < 36; i++){
        _PI->WriteDword(adress, i);
        adress += 4;
    }

    // заменяем длинные названия "Основные навыки командира" КОЛЬЦО СЛАВЫ на кадры дефа "Dlg_NPC2.def"
    adress = 0x7A2840;
    for (int i = 36; i < 42; i++){
        _PI->WriteDword(adress, i);
        adress += 4;
    }

    // 
    adress = 0x7A2CDC;
    for (int i = 42; i < 72; i++){
        if ( i == 47 || i == 52 || i == 57 || i == 62 || i == 67) { // пропускаем некоторые адреса
            adress += 4;
        }
        _PI->WriteDword(adress, i);
        adress += 4;
    }

    // заменеям длинные названия "Спец.умения" Командира на кадры дефа "Dlg_NPC3.def"
    _PI->WriteDword(0x76A92F, 0); // "None" на 0
    _PI->WriteDword(0x7A3790, 1); //  _AT_DF.BMP
    _PI->WriteDword(0x7A3794, 2); //  _AT_DF_0.BMP
    _PI->WriteDword(0x7A37A0, 3); //  _DM_HP.BMP
    _PI->WriteDword(0x7A37A4, 4); //  _DM_HP_P.BMP
    _PI->WriteDword(0x7A37B0, 5); //  _AT_DM.BMP
    _PI->WriteDword(0x7A37B4, 6); //  _AT_DM_P.BMP
    _PI->WriteDword(0x7A37C0, 7); //  _AT_MP.BMP
    _PI->WriteDword(0x7A37C4, 8); //  _AT_MP_P.BMP
    _PI->WriteDword(0x7A37D0, 9); //  _AT_SP.BMP"
    _PI->WriteDword(0x7A37D4, 10); // _AT_SP_P.BMP"
    _PI->WriteDword(0x7A3800, 11); // _DF_HP.BMP"
    _PI->WriteDword(0x7A3804, 12); // _DF_HP_P.BMP"
    _PI->WriteDword(0x7A3810, 13); // _DF_DM.BMP"
    _PI->WriteDword(0x7A3814, 14); // _DF_DM_P.BMP"
    _PI->WriteDword(0x7A3820, 15); // _HP_MP.BMP"
    _PI->WriteDword(0x7A3824, 16); // _HP_MP_P.BMP"
    _PI->WriteDword(0x7A3830, 17); // _DF_SP.BMP"
    _PI->WriteDword(0x7A3834, 18); // _DF_SP_P.BMP"
    _PI->WriteDword(0x7A3870, 19); // _AT_HP.BMP"
    _PI->WriteDword(0x7A3874, 20); // _AT_HP_P.BMP"
    _PI->WriteDword(0x7A3880, 21); // _DF_MP.BMP"   
    _PI->WriteDword(0x7A3884, 22); // _DF_MP_P.BMP" 
    _PI->WriteDword(0x7A3890, 23); // _HP_SP.BMP"
    _PI->WriteDword(0x7A3894, 24); // _HP_SP_P.BMP"
    _PI->WriteDword(0x7A38E0, 25); // _DM_MP.BMP"
    _PI->WriteDword(0x7A38E4, 26); // _DM_MP_P.BMP"
    _PI->WriteDword(0x7A38F0, 27); // _DM_SP.BMP"
    _PI->WriteDword(0x7A38F4, 28); // _DM_SP_P.BMP"
    _PI->WriteDword(0x7A3950, 29); // _MP_SP.BMP"
    _PI->WriteDword(0x7A3954, 30); // _MP_SP_P.BMP"

////////////////////////////////

    // длинные названия изучаемых навыков превращаем в кадры дефа "Dlg_NPC2.def"
    _PI->WriteDword(0x7A3798, 72); // _AT_DF_0.BMP"     
    _PI->WriteDword(0x7A379C, 73); // _AT_DF_1.BMP"
    _PI->WriteDword(0x7A37A8, 74); // _DM_HP_0.BMP"
    _PI->WriteDword(0x7A37AC, 75); // _DM_HP_1.BMP"
    _PI->WriteDword(0x7A37B8, 76); // _AT_DM_0.BMP"
    _PI->WriteDword(0x7A37BC, 77); // _AT_DM_1.BMP"
    _PI->WriteDword(0x7A37C8, 78); // _AT_MP_0.BMP"
    _PI->WriteDword(0x7A37CC, 79); // _AT_MP_1.BMP"
    _PI->WriteDword(0x7A37D8, 80); // _AT_SP_0.BMP"
    _PI->WriteDword(0x7A37DC, 81); // _AT_SP_1.BMP"
    _PI->WriteDword(0x7A37E0, 0); // "NONE"
    _PI->WriteDword(0x7A37E4, 0); // "NONE"
    _PI->WriteDword(0x7A37E8, 0); // "NONE"
    _PI->WriteDword(0x7A37EC, 0); // "NONE"
    _PI->WriteDword(0x7A37F0, 0); // "NONE"
    _PI->WriteDword(0x7A37F4, 0); // "NONE"
    _PI->WriteDword(0x7A37F8, 0); // "NONE"
    _PI->WriteDword(0x7A37FC, 0); // "NONE"
    _PI->WriteDword(0x7A3808, 82); // _DF_HP_0.BMP"
    _PI->WriteDword(0x7A380C, 83); // _DF_HP_1.BMP"
    _PI->WriteDword(0x7A3818, 84); // _DF_DM_0.BMP"
    _PI->WriteDword(0x7A381C, 85); // _DF_DM_1.BMP"
    _PI->WriteDword(0x7A3828, 86); // _HP_MP_0.BMP"
    _PI->WriteDword(0x7A382C, 87); // _HP_MP_1.BMP"
    _PI->WriteDword(0x7A3838, 88); // _DF_SP_0.BMP"
    _PI->WriteDword(0x7A383C, 89); // _DF_SP_1.BMP"
    _PI->WriteDword(0x7A3840, 0); // "NONE"
    _PI->WriteDword(0x7A3844, 0); // "NONE"
    _PI->WriteDword(0x7A3848, 0); // "NONE"
    _PI->WriteDword(0x7A384C, 0); // "NONE"
    _PI->WriteDword(0x7A3850, 0); // "NONE"
    _PI->WriteDword(0x7A3854, 0); // "NONE"
    _PI->WriteDword(0x7A3858, 0); // "NONE"
    _PI->WriteDword(0x7A385C, 0); // "NONE"
    _PI->WriteDword(0x7A3860, 0); // "NONE"
    _PI->WriteDword(0x7A3864, 0); // "NONE"
    _PI->WriteDword(0x7A3868, 0); // "NONE"
    _PI->WriteDword(0x7A386C, 0); // "NONE"
    _PI->WriteDword(0x7A3878, 90); // _AT_HP_0.BMP"
    _PI->WriteDword(0x7A387C, 91); // _AT_HP_1.BMP"
    _PI->WriteDword(0x7A3888, 92); // _DF_MP_0.BMP"
    _PI->WriteDword(0x7A388C, 93); // _DF_MP_1.BMP"
    _PI->WriteDword(0x7A3898, 94); // _HP_SP_0.BMP"
    _PI->WriteDword(0x7A389C, 95); // _HP_SP_1.BMP"
    _PI->WriteDword(0x7A38A0, 0); // "NONE"
    _PI->WriteDword(0x7A38A4, 0); // "NONE"
    _PI->WriteDword(0x7A38A8, 0); // "NONE"
    _PI->WriteDword(0x7A38AC, 0); // "NONE"
    _PI->WriteDword(0x7A38B0, 0); // "NONE"
    _PI->WriteDword(0x7A38B4, 0); // "NONE"
    _PI->WriteDword(0x7A38B8, 0); // "NONE"
    _PI->WriteDword(0x7A38BC, 0); // "NONE"
    _PI->WriteDword(0x7A38C0, 0); // "NONE"
    _PI->WriteDword(0x7A38C4, 0); // "NONE"
    _PI->WriteDword(0x7A38C8, 0); // "NONE"
    _PI->WriteDword(0x7A38CC, 0); // "NONE"
    _PI->WriteDword(0x7A38D0, 0); // "NONE"
    _PI->WriteDword(0x7A38D4, 0); // "NONE"
    _PI->WriteDword(0x7A38D8, 0); // "NONE"
    _PI->WriteDword(0x7A38DC, 0); // "NONE"
    _PI->WriteDword(0x7A38E8, 96); // DATA\\ZVS\\LIB1.RES\\_DM_MP_0.BMP"
    _PI->WriteDword(0x7A38EC, 97); // DATA\\ZVS\\LIB1.RES\\_DM_MP_1.BMP"
    _PI->WriteDword(0x7A38F8, 98); // DATA\\ZVS\\LIB1.RES\\_DM_SP_0.BMP"
    _PI->WriteDword(0x7A38FC, 99); // DATA\\ZVS\\LIB1.RES\\_DM_SP_1.BMP"
    _PI->WriteDword(0x7A3900, 0); // "NONE"
    _PI->WriteDword(0x7A3904, 0); // "NONE"
    _PI->WriteDword(0x7A3908, 0); // "NONE"
    _PI->WriteDword(0x7A390C, 0); // "NONE"
    _PI->WriteDword(0x7A3910, 0); // "NONE"
    _PI->WriteDword(0x7A3914, 0); // "NONE"
    _PI->WriteDword(0x7A3918, 0); // "NONE"
    _PI->WriteDword(0x7A391C, 0); // "NONE"
    _PI->WriteDword(0x7A3920, 0); // "NONE"
    _PI->WriteDword(0x7A3924, 0); // "NONE"
    _PI->WriteDword(0x7A3928, 0); // "NONE"
    _PI->WriteDword(0x7A392C, 0); // "NONE"
    _PI->WriteDword(0x7A3930, 0); // "NONE"
    _PI->WriteDword(0x7A3934, 0); // "NONE"
    _PI->WriteDword(0x7A3938, 0); // "NONE"
    _PI->WriteDword(0x7A393C, 0); // "NONE"
    _PI->WriteDword(0x7A3940, 0); // "NONE"
    _PI->WriteDword(0x7A3944, 0); // "NONE"
    _PI->WriteDword(0x7A3948, 0); // "NONE"
    _PI->WriteDword(0x7A394C, 0); // "NONE"
    _PI->WriteDword(0x7A3958, 100); // _MP_SP_0.BMP"
    _PI->WriteDword(0x7A395C, 101); // _MP_SP_1.BMP"    
}
