#include "stdafx.h"
#include "..\..\include\homm3.h"
#include "..\..\include\HoMM3_Dlg.cpp"

#include "era.h"
using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

char* SAVEGAME_SECTION = "igrik.BattleSpellsPanel";

char myString[1024];
#define MyString (char*)myString

#define DiBoxBck_PCX (*(char**)0x48FBF3)
#define SpellInt_DEF (*(char**)0x5F6A3E)
#define SmallFont_FNT (*(char**)0x4EEAF8)

#define SPBID 3430

int playerSpellsMenu[8][10];
_bool_  SpellMenu;
_int32_ OwnerMenu;
_int32_ SideMenu;
_int32_ ShadowWidth_BattleQueue;
_int32_ ShadowWidth_SpellsPanel;


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void __fastcall Y_MsgBox_ShowSpellDescription(int spellID, int land_modifier, _Hero_* hero, int x, int y)
{   
    _Dlg_* spellBook = (_Dlg_*)o_New(0xD0);
    spellBook->Field<int>(0x6C) = land_modifier;
    
    _HStr_ H3String;
    _cstr_ spellDescr = CALL_5(_HStr_*, __thiscall, 0x59BDA0, spellBook, &H3String, spellID, hero, 0)->c_str;
    CALL_12(void, __fastcall, 0x4F6C00, spellDescr, MBX_RMC, x, y, 9, spellID, -1, 0, -1, 0, -1, 0);
    H3String.Destruct(1);

    o_Delete(spellBook);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

_DlgStaticTextPcx8ed_* spellsStatBar = NULL;

int __stdcall Y_Dlg_ShowHeroSpells_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    if (msg->type == MT_MOUSEOVER) // ведение мыши
    {
        _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
        char* text = o_NullString;
        if (it) 
        {
            if (it->id >= 10 && it->id < 70) {
                text = o_Spell[it->id].name; 
            } 
            spellsStatBar->SetText(text);
            spellsStatBar->Draw();
            spellsStatBar->RedrawScreen();
        }
    } // MT_MOUSEOVER

    if (msg->type == MT_MOUSEBUTTON) {
        
        if (msg->subtype == MST_RBUTTONDOWN) { // ПКМ
            if (msg->item_id >= 10 && msg->item_id < 70) {
                Y_MsgBox_ShowSpellDescription(msg->item_id, o_BattleMgr->spec_terr_type, o_BattleMgr->hero[SideMenu], -1, -1);
            }
        }

        if (msg->subtype == MST_LBUTTONCLICK) { // ЛКМ 
            if (msg->item_id >= 10 && msg->item_id < 70) {
                dlg->custom_data[0] = msg->item_id;
                r = dlg->Close(msg);
            }
        }
    } // MT_MOUSEBUTTON

    if ( msg->type == MT_KEYDOWN ) {
        //sprintf(MyString, "item: %d", msg->item_id);
        //  b_MsgBox(MyString, 1);
        if ( msg->subtype == HK_ESC ) {
            r = dlg->Close(msg);
        }
    }

    // клик за пределами окна
    if ( msg->type == MT_LBUTTONOUTSIDE ) {
        r = dlg->Close(msg);
    }

    return r;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int Y_Dlg_ShowHeroSpells(_Hero_* hero, int x, int y)
{
    int spellsCount = 0;

    for(int i = 10; i < 70; i++)
        if ( hero->spell_level[i] )
            spellsCount++;

    if (!spellsCount)
        return 0;

    int pos = 20;
    int dx = 50;
    int dy = 38;
    int width = 38 + dx*( spellsCount > 8 ? 8 : spellsCount );
    int hight = 96 + dy*( spellsCount / 8 );

    if ( o_HD_Y < (y+16+hight) )
        y = (o_HD_Y-16-hight);

    _CustomDlg_* dlg = _CustomDlg_::Create(x, y, width, hight, DF_SCREENSHOT | DF_SHADOW, Y_Dlg_ShowHeroSpells_Proc);
    Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());

    dlg->AddItem(spellsStatBar = _DlgStaticTextPcx8ed_::Create(8, dlg->height -26, dlg->width - 16, 18, o_NullString, SmallFont_FNT, "HD_TStat.pcx", 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER) ); 

    int k = 0;
    for(int i = 10; i < 70; i++) {
        if (hero->spell_level[i]) {
            dlg->AddItem(_DlgStaticDef_::Create(pos+dx*(k%8), pos+dy*(k/8), 48, 36, i, SpellInt_DEF, i+1, 0, 18));  

            k++;
        }
    }

    dlg->custom_data[0] = 0;
    dlg->Run();
    int setSpellID = dlg->custom_data[0];
    dlg->Destroy(TRUE);

    return setSpellID;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_Battle_Proc_UserActions(HiHook* h, _BattleMgr_* bm, _EventMsg_* msg)
{
    if ( SpellMenu ) {
        if (msg->type == MT_MOUSEOVER)
        {
            _DlgItem_* it = bm->dlg->FindItem(msg->x_abs, msg->y_abs);
            if (it) 
            {
                if (it->id >= SPBID && it->id < (SPBID+10)) {
                    int spellUsed = playerSpellsMenu[OwnerMenu][it->id -SPBID];
                    sprintf(MyString, "{%d:} %s", (it->id -SPBID +1) %10, 
                        spellUsed >= 10 ? o_Spell[spellUsed].name : o_GENRLTXT_TXT->GetString(509) );
                    bm->AddHintMessage(MyString);
                }
            }
        } // MT_MOUSEOVER

        if (msg->type == MT_MOUSEBUTTON)
        {
            int item_id = msg->item_id;
            if (item_id >= SPBID && item_id < (SPBID+10) ) 
            {
                int bttnID = item_id-SPBID;
                if (msg->subtype == MST_RBUTTONDOWN)  // ПКМ
                {
                    if ( playerSpellsMenu[OwnerMenu][bttnID] >= 10 )
                        Y_MsgBox_ShowSpellDescription(playerSpellsMenu[OwnerMenu][bttnID], bm->spec_terr_type, bm->hero[SideMenu], msg->x_abs, msg->y_abs);

                }
                if (msg->subtype == MST_LBUTTONCLICK)  // ЛКМ
                {           
                    if ( GetKeyState(17)<0 ) { // CTRL = Reset Button
                            playerSpellsMenu[OwnerMenu][bttnID] = 0;
                            
                            _DlgStaticDef_* item = (_DlgStaticDef_*)bm->dlg->GetItem(item_id-10);
                            item->def_frame_index = 0;
                            item->Draw();   

                            _DlgStaticText_* numberKey = (_DlgStaticText_*)bm->dlg->GetItem(item_id);
                            numberKey->Draw();  numberKey->RedrawScreen();
                    } 
                    else 
                    {
                        int spell = Y_Dlg_ShowHeroSpells(bm->hero[SideMenu], msg->x_abs, msg->y_abs);

                        if (spell >= 10 && spell < 70) {
                            playerSpellsMenu[OwnerMenu][bttnID] = spell;
                            spell++; // set next frame
                            _DlgStaticDef_* item = (_DlgStaticDef_*)bm->dlg->GetItem(item_id-10);
                            item->def_frame_index = spell;
                            item->Draw();   

                            _DlgStaticText_* numberKey = (_DlgStaticText_*)bm->dlg->GetItem(item_id);
                            numberKey->Draw();  numberKey->RedrawScreen();
                        }
                    }
                }
            }
        }
        if ( msg->type == MT_KEYDOWN ) {
            if ( msg->subtype >= HK_1 && msg->subtype <= HK_0 ) {
                int bttnID = msg->subtype - HK_1;
                int spellUsed = playerSpellsMenu[OwnerMenu][bttnID];

                if ( !bm->isTactics && spellUsed >= 10) { // если заклинание настроено, и не фаза тактики
                    if ( !bm->sideIsCasted[SideMenu] || bm->cheat_NoCastLimit ) {
                        if ( !bm->hero[SideMenu]->spell_level[spellUsed] ) { // у героя нет такого заклинания
                            b_MsgBox(o_GENRLTXT_TXT->GetString(676), 1);
                        } else { // заклинание есть
                            int manaCost = bm->hero[SideMenu]->GetSpellCost(spellUsed, bm->army[SideMenu], bm->spec_terr_type);
                            int heroMana = bm->hero[SideMenu]->spell_points;
                            if ( heroMana >= manaCost ) {
                                bm->UserChooseSpellTarget(spellUsed, 0);
                            } else { // не хватает маны на заклинание
                                sprintf(MyString, o_GENRLTXT_TXT->GetString(208), manaCost, heroMana);
                                sprintf(o_TextBuffer, "{%s} \n\n %s", o_Spell[spellUsed].name, MyString);
                                b_MsgBoxD(o_TextBuffer, MBX_OK, 9, spellUsed);
                            }
                        }
                    } // bm->sideIsCasted
                } // bm->isTactics
            }
        }
    } // if ( SpellMenu ) 

    return CALL_2(int, __thiscall, h->GetDefaultFunc(), bm, msg);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_BattleMgr_CreateSpellsMenu(LoHook* h, HookContext* c)
{
    _BattleMgr_* bm = o_BattleMgr;

    SpellMenu = false;

    // игра сама проверяет видимая это битва, или нет
    // поэтому нет смысла проверять на "видимую битву"

    // если оба компы: спелл-меню нет
    if ( !bm->isNotAI[0] && !bm->isNotAI[1] ) 
        return EXEC_DEFAULT;    

    // если оба люди: спелл-меню нет
    if ( bm->isHuman[0] && bm->isHuman[1] ) 
        return EXEC_DEFAULT;

    int meID = o_GameMgr->GetMeID();

    // проверяем левую сторону
    if ( bm->isHuman[0] && bm->owner_id[0] == meID ) {
        if ( bm->hero[0] && bm->CanUseAnySpell(0, 1) ) {
            SpellMenu = true;
            OwnerMenu = meID;
            SideMenu = 0;
        }
    }
    // проверяем правую сторону
    if ( bm->isHuman[1] && bm->owner_id[1] == meID ) {
        if ( bm->hero[1] && bm->CanUseAnySpell(1, 1)  ) {
            SpellMenu = true;
            OwnerMenu = meID;
            SideMenu = 1;
        }
    }

    // создаём элементы спелл-меню
    if ( SpellMenu ) {
        _Dlg_* dlg = (_Dlg_*)c->esi;

        int dy = 60;

        Dlg_SetBackgroundWithHDFrame(dlg, -63, -4, 60, 630, SPBID -30, DiBoxBck_PCX); 

        for (int i = 0; i < 10; i++) {
            b_YellowFrame_Create(dlg, -58, 22+i*dy, 50, 38, SPBID-20+i, ON, o_Pal_Black); // замена цвета item->Field<int>(0x30) 
                 
            int frame = 0;
            if ( playerSpellsMenu[OwnerMenu][i] >= 10 )
                frame = playerSpellsMenu[OwnerMenu][i] +1;

            dlg->AddItem(_DlgStaticDef_::Create(-57, 23+i*dy, 48, 36, SPBID-10+i, SpellInt_DEF, frame, 0, 18));

            sprintf(MyString, "%d", (i+1)%10);  
            dlg->AddItem(_DlgStaticText_::Create(-56, 23+i*dy, 47, 36, MyString, "verd10b.fnt", 1, SPBID+i, ALIGN_H_RIGHT | ALIGN_V_BOTTOM, 0));        
        }
    }

    return EXEC_DEFAULT;
} 

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int __stdcall BattleMgr_Dlg_HeroPreviewDlg_OffSet(LoHook* h, HookContext* c)
{
    if ( SpellMenu ) {
        // смещаем левую статистику героя и монстра
        // левую определяем, если x < 200
        if (IntAt(c->ebp +8))
            if ( IntAt(c->ebp +8) < 200)
                IntAt(c->ebp +8) -= ShadowWidth_SpellsPanel;
    }

    return EXEC_DEFAULT;
} 

// увеличения ширины тени, при первом показе диалога битвы
signed int __stdcall Y_Battle_SpellsMenu_SetShadow(HiHook* h, _dword_ buffer, int x, int y, int width, int height)
{
    // если меню заклинаний было создано (и это первые 2 вызова)
    if ( SpellMenu == 1 || SpellMenu == 2 ) 
    {
        // если диалог битвы был создан
        _Dlg_* dlg = o_BattleMgr->dlg;
        if ( dlg && ( /* только нижняя полоса тени */ height == 8 || height == 7 ) ) 
        {   
            // некрасивое решение: 
            // глобальный счетчик вызовов
            // делаю только для 2х первых вызовов
            SpellMenu++;
            x -= ShadowWidth_SpellsPanel;
            width += (ShadowWidth_SpellsPanel + ShadowWidth_BattleQueue);
        }
    }

    return CALL_5(signed int, __thiscall, h->GetDefaultFunc(), buffer, x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void __stdcall OnAfterErmInstructions (TEvent* Event) {
    // при старте карты очищаем список панели заклинаний
    for (int playerID = 0; playerID < 8; playerID++) {
        for (int spellID = 0; spellID < 10; spellID++) {
            playerSpellsMenu[playerID][spellID] = 0;
        }
    }
}

void __stdcall OnSavegameRead (TEvent* Event) {
    int dataSize = 0;
    ReadSavegameSection(sizeof(int), &dataSize, SAVEGAME_SECTION);
    
    if (dataSize > 0) {
        ReadSavegameSection(sizeof(playerSpellsMenu), &playerSpellsMenu, SAVEGAME_SECTION);
    }
}

void __stdcall OnSavegameWrite (TEvent* Event) {
    int dataSize = sizeof(playerSpellsMenu);
    WriteSavegameSection(sizeof(dataSize), &dataSize, SAVEGAME_SECTION);
    WriteSavegameSection(sizeof(playerSpellsMenu), &playerSpellsMenu, SAVEGAME_SECTION);
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int __stdcall HooksInit(LoHook* h, HookContext* c)
{
    if (EXE_VERSION == TE) {
        if ( o_HD_X >= 1076 && o_HD_Y >= 630) {

            // поддтягиваем ERA
            ConnectEra();
            RegisterHandler(OnAfterErmInstructions, "OnAfterErmInstructions");
            RegisterHandler(OnSavegameRead, "OnSavegameRead");
            RegisterHandler(OnSavegameWrite, "OnSavegameWrite");

            // создаём элементы диалога
            _PI->WriteLoHook(0x471FF9, Y_BattleMgr_CreateSpellsMenu);

            // сдвигаем характеристики героя и монстра
            _PI->WriteLoHook(0x46BEC5, BattleMgr_Dlg_HeroPreviewDlg_OffSet);
            _PI->WriteLoHook(0x46CA05, BattleMgr_Dlg_HeroPreviewDlg_OffSet);

            // действия игрока в битве
            _PI->WriteHiHook(0x473F55, CALL_, EXTENDED_, THISCALL_, Y_Battle_Proc_UserActions);

            // реализуем тень
            _PI->WriteHiHook(0x44E2C0, SPLICE_, EXTENDED_, THISCALL_, Y_Battle_SpellsMenu_SetShadow);

            // делаем совместимость тени с плагином BattleQueue
            ShadowWidth_SpellsPanel = 59;
            ShadowWidth_BattleQueue = 0;
            PatcherInstance* _RK_BQ = _P->GetInstance("ERA.BattleQueue");   

            if (_RK_BQ)
                ShadowWidth_BattleQueue = 53;
        }
    }

    return EXEC_DEFAULT;
} 

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

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
            _PI = _P->CreateInstance(SAVEGAME_SECTION);
        
            _PI->WriteLoHook(0x4EEAF2, HooksInit);
            
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


