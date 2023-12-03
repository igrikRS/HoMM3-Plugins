#include "stdafx.h"
#include "..\..\..\include\homm3.h"
#include "..\..\..\include\era.h"

using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

///////////////////////////////////////////////////////////////////
// ��� �� �������� ������ �� ������� �����, �� ��� ������-�� ��� ����
char* ButtonName;
char* ButtonHints[10];
char* ButtonRmc[10];
char* AutoBattleQuestion;

#define NONE 0
#define MELEE 1
#define SHOOT 2
#define CAST 3
#define MOOVE 4
#define RETURN 5
#define NO_RETURN 6

static int currentType = NONE;

_byte_ typesArray[10];
_int8_ typesIterator;

#define BTTN_ID 2020

// ������ ���������� ������� ��� ��������
void SendNetData_AttackType(int type);

///////////////////////////////////////////////////////////////////

char* GetJsonStr(const char* json_string_name) {
    return tr(json_string_name);
}

int __stdcall ReadJsonConfig()
{    
    ButtonName = GetJsonStr("choose_attack_type.name");

    ButtonHints[NONE] = GetJsonStr("choose_attack_type.hint.none");
    ButtonRmc[NONE] = GetJsonStr("choose_attack_type.rmc.none");

    ButtonHints[MELEE] = GetJsonStr("choose_attack_type.hint.melee");
    ButtonRmc[MELEE] = GetJsonStr("choose_attack_type.rmc.melee");

    ButtonHints[SHOOT] = GetJsonStr("choose_attack_type.hint.shoot");
    ButtonRmc[SHOOT] = GetJsonStr("choose_attack_type.rmc.shoot");

    ButtonHints[CAST] = GetJsonStr("choose_attack_type.hint.cast");
    ButtonRmc[CAST] = GetJsonStr("choose_attack_type.rmc.cast");

    ButtonHints[MOOVE] = GetJsonStr("choose_attack_type.hint.moove");
    ButtonRmc[MOOVE] = GetJsonStr("choose_attack_type.rmc.moove");

    ButtonHints[RETURN] = GetJsonStr("choose_attack_type.hint.return");
    ButtonRmc[RETURN] = GetJsonStr("choose_attack_type.rmc.return");

    ButtonHints[NO_RETURN] = GetJsonStr("choose_attack_type.hint.no_return");
    ButtonRmc[NO_RETURN] = GetJsonStr("choose_attack_type.rmc.no_return");

    AutoBattleQuestion = GetJsonStr("choose_attack_type.autobattle_question");

    return 1;
}

///////////////////////////////////////////////////////////////////

// ������� ������� ������� ��������
void arrayTypesClear()
{
    for (int i=0; i<10; i++) {
        typesArray[i] = NONE;
    }
    typesIterator = -1;
    currentType = NONE;
}

// ������� ������� �������� � ������ ��������
// � ���������� ��������� �� ������������, �������� ������������ ��������
int arrayTypesPush(int type)
{
    typesIterator = 0;

    for (int i=0; i<10; i++) {
        if(typesArray[i] == type) {
            return i;
        }

        if(typesArray[i] == NONE) {
           typesArray[i] = type;
           return i;
        }
    }
    b_MsgBox("typesArray is Full!", 5);
    return -1;
}

// ������� ��������� ���������� �������� � �������
int arrayTypesGetNextType()
{
    if (currentType == NONE)
        return NONE;

    typesIterator++;
    if (typesIterator >= 10 || typesArray[typesIterator] == NONE) 
        typesIterator = 0;

    return typesArray[typesIterator];
}

///////////////////////////////////////////////////////////////////

// ���������� ������ ������ ���� ��������
void updateAttackTypeButton(int type, bool redraw = false)
{
    if (o_BattleMgr->dlg)
    {
        _DlgButton_* bttn = (_DlgButton_*)o_BattleMgr->dlg->GetItem(BTTN_ID);
        if (bttn)
        {
            currentType = type;
            bttn->def_frame_index = type*3;
            bttn->press_def_frame_index = type*3 +1;
            bttn->short_tip_text = ButtonHints[type];
            bttn->full_tip_text = ButtonRmc[currentType];

            // ������� ������� ������� �������� ��������
            // ���� ������ ��������� ������� �� ����, � ����� �� ��������
            SendNetData_AttackType(type);

            if (type == NONE) {
                // DlgItem_Send6Cmd2Item
                CALL_3(int, __thiscall, 0x5FF490, o_BattleMgr->dlg, BTTN_ID, 4096);
            } else {
                // DlgItem_Send5Cmd2Item
                CALL_3(int, __thiscall, 0x5FF520, o_BattleMgr->dlg, BTTN_ID, 4096);
            }

            if (type == MELEE && *(_word_*)0x4B2C8E != 0xE990 ) {
                *(_word_*)0x4B2C8E = 0xE990; // JMP 004B2D42 (orig code)
                o_BattleMgr->RedrawGrid();
                o_BattleMgr->Redraw();
            }

            if (type != MELEE && *(_word_*)0x4B2C8E != 0x850F ) {
                *(_word_*)0x4B2C8E = 0x850F; // JNE 004B2D42 (orig code)
                o_BattleMgr->RedrawGrid();
                o_BattleMgr->Redraw();  
            }

            if ( redraw )
            {
                int x = bttn->x; // 658
                int y = bttn->y; // 560

                if (o_HD_X > ORIG_X) {
                    x += (o_HD_X - ORIG_X) /2;
                }

                if (o_HD_Y > ORIG_Y) {
                    y += (o_HD_Y - ORIG_Y) /2;

                    if (o_HD_Y >= 664)
                        y -= 10;
                }

                // �������� ������ ������������
                CALL_4(void, __thiscall, 0x472E30, o_BattleMgr->dlg, 0, -65535, 0xFFFF); 
                o_WndMgr->RedrawScreenRect(x, y, 44, 36);
                o_BattleMgr->lastMoveToIndex = -1;
                CALL_1(int, __thiscall, 0x4ECCD0, o_InputMgr); // Put_WM_MouseMove_Action2Buffer

                o_BattleMgr->AddHintMessage(bttn->short_tip_text);
            }  
            
        } 
    }
}

// ��� ��������� ������ ����: 
// ����������� ������ ������ �������� � ������������ � ����������� �����
void initNewStackAttackType(bool redraw = false)
{
     arrayTypesClear();
     updateAttackTypeButton(NONE, redraw);

    _BattleMgr_* bm = o_BattleMgr;

    if(bm->isTactics) { return; }

    int activeStackID = 21*bm->currentStackSide + bm->currentStackIndex;    
    _BattleStack_* stack = &bm->stack[bm->currentStackSide][bm->currentStackIndex];

    if(!stack) { return; }

    int meID = o_GameMgr->GetMeID();
    if(bm->playerID[bm->currentStackSide] != meID) {
        return; 
    }    

    if (bm->autoCombatOn || o_AutoSolo || bm->IsStackAutoControlledByAI(stack) ) {
        updateAttackTypeButton(NONE, redraw);  
        return;
    }

    if (stack->creature_id >= CID_CATAPULT && stack->creature_id <= CID_ARROW_TOWER ) {
        updateAttackTypeButton(NONE, redraw);
        return;
    }


    if (bm->CanUseAnySpell(bm->currentActiveSide, FALSE) && stack->CanCastToBattleField() ) {
        if (stack->creature_id == CID_PIT_LORD ||
            stack->creature_id == CID_ARCHANGEL ||
            stack->creature_id == CID_SUPREME_ARCHANGEL) {
            if (currentType == NONE) {
                updateAttackTypeButton(CAST, redraw);
            }
            arrayTypesPush(CAST);
            arrayTypesPush(MOOVE);
        }


        if ( stack->CanCastSpellAkaFaerieDragon() ) {
            if (currentType == NONE) {
                updateAttackTypeButton(CAST, redraw);
            }
            arrayTypesPush(CAST);

            // ���� ������ �� ����� �������� - ������� ���������
            // ����� �������� ��� ������ ������ ��������� (�������� ���� ����)
            if( !stack->CanShoot(0) ) {
                arrayTypesPush(MELEE);
            }
        }
    }


    if( stack->CanShoot(0) ) {
        if (currentType == NONE) {
            updateAttackTypeButton(SHOOT, redraw);
        }
        arrayTypesPush(SHOOT);
        arrayTypesPush(MELEE);
    }


    if ( stack->WOG_isHarpy() ) {
        if (currentType == NONE) {
            updateAttackTypeButton(RETURN, redraw);
        }
        arrayTypesPush(RETURN);
        arrayTypesPush(NO_RETURN);
    }
}

///////////////////////////////////////////////////////////////////

// �������� �� ����������� ��������� ��������
_bool8_ __stdcall Y_BattleMgr_CanCast(HiHook* hook, _BattleMgr_ *bm, _int_ side, _bool8_ isHeroSpell)
{
    if (isHeroSpell != TRUE && ( currentType == MELEE || currentType == SHOOT || currentType == MOOVE) ) {
        return FALSE;
    }

    return CALL_3(_bool8_, __thiscall, hook->GetDefaultFunc(), bm, side, isHeroSpell);
}


// �������� �� ����������� �������� ��������
_LHF_(Y_BattleMgr_CanStackShoot)
{
    if (currentType == MELEE) {
        c->return_address = 0x4426F8;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

// �������� �� ������� ������
_LHF_(Y_BattleMgr_WOG_HarpyReturn)
{   
    if (currentType == NO_RETURN) {
        arrayTypesClear();
        c->return_address = 0x75E0B4;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT;
}

// ������� ������ ������� ���� �� ������ ������������ ����� (���� � ���������)
_byte_ __stdcall Y_BattleMgr_MouseClickRMC_OnButton(HiHook* hook, _Dlg_* dlg, _EventMsg_* msg)
{
    if (msg && msg->item_id == BTTN_ID) {
        b_MsgBox(ButtonRmc[currentType], 4);
    }

    return CALL_2(_byte_, __thiscall, hook->GetDefaultFunc(), dlg, msg);
}

// ������� ����� ������� ���� �� ������ ������������ ����� (������������)
// ������� ������� S
_LHF_(Y_BattleMgr_ProcessAction_LMC)
{
    int item_id = c->eax;
    if (item_id == BTTN_ID) {
        if (currentType != NONE) {            
            int type = arrayTypesGetNextType();
            updateAttackTypeButton(type, TRUE);
        }
    }

    return EXEC_DEFAULT;
}

// ��� ��������� ��������� �����
_LHF_(Y_BattleMgr_SetActiveStack)
{
    initNewStackAttackType(FALSE);
    return EXEC_DEFAULT;
}

// ����� ���������� ���������� ������
_LHF_(Y_LoHook_BM_AfterHeroSpellCast)
{
    if (!o_BattleMgr->IsHiddenBattle()) {
        initNewStackAttackType(TRUE);
    }
    return EXEC_DEFAULT;
}

// ���������� ������ �� ����� �����
_LHF_(Y_AddChooseAttackButton)
{
    arrayTypesClear();

    // ������ ������ ������������ ���� �����
    _DlgButton_* bttn = _DlgButton_::Create(658, 4, 44, 36, BTTN_ID, ButtonName, 0, 1, 0, HK_ALT, 0 );

    // ������������� ��������
    bttn->short_tip_text = ButtonHints[currentType];
    bttn->full_tip_text = ButtonRmc[currentType];

    // ��������� ������ � ���� ������ ������
    CALL_4(void, __thiscall, 0x5FE2D0, c->esi, *(int*)(c->esi+8), 1, &bttn);

    return EXEC_DEFAULT;
}

_LHF_(Y_ClickAutoBattleButton_A_id2004) {

    // ���������: �� ����� ������ �������� �������������� ������� �����?
    if (!b_MsgBox(AutoBattleQuestion, MBX_OKCANCEL)) {
        c->return_address = 0x47480F;
        return NO_EXEC_DEFAULT;
    }

    // orig code
    c->edx = o_BattleMgr->autoCombatOn;
    c->return_address = 0x474790;
    return NO_EXEC_DEFAULT;
}


_LHF_(LoHook_InitTxtFiles)
{    
    ReadJsonConfig();

    // id = 2003 (���������)
    _PI->WriteByte(0x46B3D9 +1, 4); 
    _PI->WriteByte(0x46B3D3 +1, 44);
    _PI->WriteDword(0x46B3C9 +1, (int)"icm003q.def");

    // id = 2001 (�������)
    _PI->WriteByte(0x46B2CF +1, 51); 
    _PI->WriteByte(0x46B2CB +1, 44);
    _PI->WriteDword(0x46B2BF +1, (int)"icm001q.def");

     // id = 2002 (�������)
    _PI->WriteByte(0x46B354 +1, 98); 
    _PI->WriteByte(0x46B34E +1, 44);
    _PI->WriteDword(0x46B344 +1, (int)"icm002q.def");

    // id = 2004 (���������)
    _PI->WriteByte(0x46B45E +1, 145); 
    _PI->WriteByte(0x46B458 +1, 44);
    _PI->WriteDword(0x46B44E +1, (int)"icm004q.def");
    
    // id = 2300 (������� - ��������� ��������)
    _PI->WriteDword(0x46BC1F +1, 192);
    _PI->WriteDword(0x46B800 +1, 188);

    // id = 30722 (������� - ������ �����)
    _PI->WriteDword(0x46BC8F +1, 398);
    _PI->WriteDword(0x46BC88 +1, 188);
    _PI->WriteDword(0x46BC7C +1, (int)"icm012q.def");

    // id = 2005 (���� ����)
    _PI->WriteDword(0x46B807 +1, 194);
    _PI->WriteDword(0x46B800 +1, 390);

    // id = 2006 (������� ���� �����)
    _PI->WriteDword(0x46B87E +1, 590); 

    // id = 2007 (������� ���� ����)
    _PI->WriteDword(0x46B917 +1, 590); 

    // id = 2008 (����������)
    _PI->WriteDword(0x46B4E6 +1, 611); 
    _PI->WriteByte(0x46B4E0 +1, 44);
    _PI->WriteDword(0x46B4D6 +1, (int)"icm005q.def");

    // id = 2009 (�����)
    _PI->WriteDword(0x46B56E +1, 705); 
    _PI->WriteByte(0x46B568 +1, 44);
    _PI->WriteDword(0x46B55E +1, (int)"icm006q.def");

     // id = 2010 (� ������)
    _PI->WriteDword(0x46B5F6 +1, 752); 
    _PI->WriteByte(0x46B5F0 +1, 44);
    _PI->WriteDword(0x46B5E6 +1, (int)"icm007q.def");

    // id = 2 (���)
    _PI->WriteDword(0x471ED9 +1, 195);
    _PI->WriteDword(0x471ECF +1, 390);

    return EXEC_DEFAULT;
}

///////////////////////////////////////////////////////////////////

// ������� ������ �� ����
#define NETMSG_ID 1989

void SendNetData_AttackType(int type) 
{
    _BattleMgr_* bm = o_BattleMgr;

    if ( o_NetworkGame && type != NONE )  {
        if (bm->playerID[0] >= 0 && bm->playerID[1] >= 0 ) {
            if ( o_GameMgr->IsPlayerHuman(bm->playerID[1]) ) { 
                if ( o_GameMgr->IsPlayerHuman(bm->playerID[0]) ) { 
                    int activeID = o_GameMgr->GetMeID();
                    int meID = bm->playerID[bm->currentActiveSide];

                     if ( activeID == meID ) {
                        _dword_ targetPlayerID = *(_dword_*)(0x697790 - 4*bm->currentActiveSide);

                        _int32_ netData[6]; 
                        netData[0] = -1;        // recipient_id
                        netData[1] = 0;         // field_04
                        netData[2] = NETMSG_ID; // msg_id
                        netData[3] = 24;        // size
                        netData[4] = 0;         // field_10
                        netData[5] = type;      // shortData

                        // SendData()
                        CALL_4(_int32_, __fastcall, 0x5549E0, (_dword_)&netData, targetPlayerID, 0, 1);
                     }
                }
            }
        }
    }
}

// ��������� � ��������� ������ �� ����
_dword_ __stdcall Y_HiHook_BM_GetNetData(HiHook* hook, _dword_ a1)
{
    _dword_ netData = CALL_1(_dword_, __fastcall, hook->GetDefaultFunc(), a1);

    if (netData)
    {
        int id = *(int*)(netData + 8);

        if (id == NETMSG_ID)
        {
             int type = *(int*)(netData + 20);
             currentType = type;
        }
    }

    return netData;
}

///////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    static _bool_ plugin_On = 0;

    if ( (ul_reason_for_call==DLL_PROCESS_ATTACH) && (!plugin_On) ) {
        plugin_On = 1;

        _P = GetPatcher();
        _PI = _P->CreateInstance("igrik.choose_attack_button");
        ConnectEra();

        // ������������� ���������� (��� ������ �����)
        _PI->WriteLoHook(0x4EEAC0, LoHook_InitTxtFiles); 

        // ���������� ������ �� ����� �����
        _PI->WriteLoHook(0x46B664, Y_AddChooseAttackButton);

        // �������������� � �������� �� ��� � ���
        _PI->WriteHiHook(0x474714, CALL_, EXTENDED_, THISCALL_, Y_BattleMgr_MouseClickRMC_OnButton);
        _PI->WriteLoHook(0x474764, Y_BattleMgr_ProcessAction_LMC); 

        // ��������� ������� ��������� ���� ��������
        _PI->WriteLoHook(0x442635, Y_BattleMgr_CanStackShoot);
        _PI->WriteLoHook(0x75E0E7, Y_BattleMgr_WOG_HarpyReturn);
        _PI->WriteHiHook(0x41FA10, SPLICE_, EXTENDED_, THISCALL_, Y_BattleMgr_CanCast);

        // ������� �� ������ �������������� �����
        _PI->WriteLoHook(0x47478A, Y_ClickAutoBattleButton_A_id2004);

        // ��� ��������� ��������� ����� (�������� ������)
        _PI->WriteLoHook(0x477D98, Y_BattleMgr_SetActiveStack); 

        // ����� ����� ������ (�������� ������)
        _PI->WriteLoHook(0x478997, Y_LoHook_BM_AfterHeroSpellCast);

        // ��������������� ������������ �������� ������ "� ������"
        // �� ������� ������ ���� ��� WOG
        _PI->WriteHexPatch(0x47265C, "B8 08000000 E9 64FFFFFF");

        // ��������� ������ �� ����
        // send 0x4787BC
        // get 0x473D27
        _PI->WriteHiHook(0x473D27, CALL_, EXTENDED_, FASTCALL_, Y_HiHook_BM_GetNetData);

    }
    return TRUE;
}
