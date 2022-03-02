////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог командира /////////////// /////////////////////////////////////////////////

_DlgStaticDef_* NPC_Def;
_DlgStaticTextPcx8ed_* NPC_StatBar = NULL;
int time_click, timeClickAvtoSolo;

char* npc1Def = "Dlg_NPC1.def";
char* npc2Def = "Dlg_NPC2.def";
char* npc3Def = "Dlg_NPC3.def";

struct _DlgNPC_;
#define o_dlgNPC ((_DlgNPC_*)0x28604D8)
#define WoG_NoNPC (*(_int_*)0x277192C)

int __stdcall Y_New_CommanderDlg_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);
    _DlgNPC_* dlgNPC = o_dlgNPC; // воговская структура диалога

    if (msg->type == MT_MOUSEOVER) // ведение мыши
    {
        _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
        char* text = o_NullString;
        if (it) 
        {
            int IDM = it->id;
            switch (IDM)
            {
                case 3: 
                    text = txtresWOG->GetString(53); 
                    break; // подсказка Уволить командира
                
                case 6: 
                    text = dlgNPC->PortraitHint; 
                    break; // портрет командира

                case 11: 
                    text = txtresWOG->GetString(74); 
                    break; // подсказка имя командира

                case 13: 
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(67), dlgNPC->Level);
                    text = o_TextBuffer;    
                    break; // подсказка уровень командира

                case 15: 
                    text = dlgNPC->TypeHint; 
                    break; // подсказка класса командира

                case 17: 
                    sprintf(o_TextBuffer, "%s %s", txtresWOG->GetString(68), dlgNPC->HeroName);
                    text = o_TextBuffer;    
                    break; // подсказка имя героя хозяина

                case 19: 
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(71), dlgNPC->CurExp);
                    text = o_TextBuffer;    
                    break; // подсказка текущий опыт

                case 21: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(72), dlgNPC->NextExp);
                    text = o_TextBuffer;    break; // подсказка следующий опыт

                case 22:    text = txtresWOG->GetString(77); break; // подсказка навыки класса
                case 23:    text = txtresWOG->GetString(78); break; // подсказка артефакты
                case 24:    text = txtresWOG->GetString(69); break; // подсказка основные навыки
                case 25:    text = txtresWOG->GetString(70); break; // подсказка вторичные навыки

                case 31:    
                    sprintf(o_TextBuffer, "%s %d (%d)", txtresWOG->GetString(54), dlgNPC->pAT, dlgNPC->hAT);
                    text = o_TextBuffer;    
                    break; // подсказка атака

                case 33:    
                    sprintf(o_TextBuffer, "%s %d (%d)", txtresWOG->GetString(55), dlgNPC->pDF, dlgNPC->hDF);
                    text = o_TextBuffer;    
                    break; // подсказка защита

                case 35:    
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(56), dlgNPC->pHP);
                    text = o_TextBuffer;    
                    break; // подсказка здоровье

                case 37:    
                    sprintf(o_TextBuffer, "%s %d - %d", txtresWOG->GetString(58), dlgNPC->pDML, dlgNPC->pDMH);
                    text = o_TextBuffer;    
                    break; // подсказка урон

                case 39:    
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(59), dlgNPC->pMP);
                    text = o_TextBuffer;    
                    break; // подсказка сила магии

                case 41:    
                    sprintf(o_TextBuffer, "%s %d%%", txtresWOG->GetString(60), dlgNPC->pMR);
                    text = o_TextBuffer;    
                    break; // подсказка сопротивление

                case 43:    
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(61), dlgNPC->pShots);
                    text = o_TextBuffer;    
                    break; // подсказка боезапас

                case 45:    
                    sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(57), dlgNPC->pSP);
                    text = o_TextBuffer;    
                    break; // подсказка скорость

                case 46: 
                    text = dlgNPC->SpecHint1; 
                    break;  // подсказка спец навык класса 1

                case 47: 
                    text = dlgNPC->SpecHint2; 
                    break;  // подсказка спец навык класса 2

                case 48: case 49: case 50: case 51: case 52: case 53:  
                    text = dlgNPC->ArtHints[IDM -48]; 
                    break; // подсказка артефакты[48-53]

                case 54: case 55: case 56: case 57: case 58: case 59:  
                    text = dlgNPC->BonusHints[IDM -54]; 
                    break; // подсказка основные навыки[54-59]
                
                case 60: case 61: case 62: case 63: case 64: case 65:  
                    text = dlgNPC->SpecBonusHints[IDM -60]; 
                    break; // подсказка доп. навыки[60-65]

                case 67: case 68: case 69: case 70: case 71: case 72:  
                    text = dlgNPC->NextHints[IDM -67]; 
                    break; // подсказка изучаемые. навыки[66-72] 

                case DIID_OK: 
                    text = dlgNPC->Request ? json_Npc[2] : json_Npc[1]; 
                    break; // подсказка Ok

                default: 
                    text = o_NullString;    break;
            }
        NPC_StatBar->SetText(text);
        NPC_StatBar->Draw();
        NPC_StatBar->RedrawScreen();
        }
    } // type == MT_MOUSEOVER

    if (msg->type == MT_MOUSEBUTTON)
    {
        if (msg->subtype == MST_RBUTTONDOWN)  // ПКМ
        {
            int IDR = msg->item_id;
            switch (IDR)
            {
                case 3: 
                    b_MsgBox(json_Npc[3], MBX_RMC); 
                    break;  // уволить командира

                case 6: case 11: 
                    b_MsgBox(dlgNPC->Description, MBX_RMC); 
                    break; // биография

                case 15:  
                    b_MsgBox(dlgNPC->TypePopUpText, MBX_RMC);   
                    break; // описание класса командира

                case 46: 
                    b_MsgBoxAndDef(npc1Def, (int)dlgNPC->SpecIcon1, dlgNPC->SpecPopUpText1);  
                    break;      // спец навык класса 1

                case 47: 
                    b_MsgBoxAndDef(npc1Def, (int)dlgNPC->SpecIcon2, dlgNPC->SpecPopUpText2);  
                    break;      // спец навык класса 2
                
                case 48: case 49: case 50: case 51: case 52: case 53:  // артефакты[48-53]
                    b_MsgBoxD(dlgNPC->ArtPopUpTexts[IDR -48], MBX_RMC, 8, (int)dlgNPC->ArtIcons[IDR -48]); 
                    break; 

                case 54: case 55: case 56: case 57: case 58: case 59:  // основные навыки[54-59]
                    b_MsgBoxAndDef(npc2Def, (int)dlgNPC->Bonus[IDR -54], dlgNPC->BonusPopUpText[IDR -54]); 
                    break; 

                case 60: case 61: case 62: case 63: case 64: case 65:  // доп. навыки[60-65]
                    b_MsgBoxAndDef(npc3Def, (int)dlgNPC->SpecBonus[IDR -60], dlgNPC->SpecBonusPopUpText[IDR -60]); 
                    break;

                case 67: case 68: case 69: case 70: case 71: case 72:  // изучаемые. навыки[67-72]
                    b_MsgBoxAndDef(npc2Def, (int)dlgNPC->Next[IDR -67], dlgNPC->NextPopUpTexts[IDR -67]); 
                    break;
    
                default: 
                    break;
            }
        } // subtype == MST_RBUTTONDOWN
        if (msg->subtype == MST_LBUTTONCLICK) // ЛКМ при отжатии
        {
            if (msg->item_id == 3)  {
                b_MsgBox(txtresWOG->GetString(62), MBX_OKCANCEL); // увольняем командира
                if (o_WndMgr->result_dlg_item_id == DIID_OK){
                    dlgNPC->DlgLeft = -1;                           
                    return dlg->Close(msg);
                }
            }

        } // subtype == MST_LBUTTONCLICK

        if (msg->subtype == MST_LBUTTONDOWN)  // ЛКМ при нажатии
        {
            if (dlgNPC->Request == 2) // повышение уровня с выбором навыков
            { 
                if ( msg->item_id >= 67 && msg->item_id <= 72 )
                {
                    int last_id = dlgNPC->DlgLeft; // всегда +1
                    int this_id = msg->item_id - 66; // всегда +1

                    if (last_id == this_id && ((o_GetTime() - time_click) < 300 ) ) { // реализация дабл_клика по выбранному навыку
                        e_ClickSound();
                        return dlg->Close(msg);
                    } else {
                        ((_DlgStaticDef_*)dlg->GetItem(66 + last_id))->def_frame_index = (int)dlgNPC->Next[last_id -1];
                        ((_DlgStaticDef_*)dlg->GetItem(msg->item_id))->def_frame_index = (int)dlgNPC->NextActive[this_id -1];
                        ((_DlgStaticDef_*)dlg->GetItem(msg->item_id))->RedrawScreen();
                        dlgNPC->DlgLeft = this_id;
                        e_ClickSound();
                    }
                    time_click = o_GetTime();
                }
            } else {// !dlgNPC->Request
                if ( msg->item_id >= 48 && msg->item_id <= 53 ) {
                    int itid = msg->item_id -48;
                    if ( (dlgNPC->Flags & 4) && (dlgNPC->Request == 0) ) { // если можно передавать артефакты

                        sprintf(o_TextBuffer, "%s\n\n%s", dlgNPC->ArtPopUpTexts[itid], txtresWOG->GetString(79));
                        b_MsgBoxD(o_TextBuffer, MBX_OKCANCEL, 8, (int)dlgNPC->ArtIcons[itid]);

                        if (o_WndMgr->result_dlg_item_id == DIID_OK){
                            dlgNPC->ArtOutput[itid] = 1; // отдаём артефакт
                            dlgNPC->DlgTop = 1; // говорим о том, что нужно будет заново вызвать диалог в функции Y_Dlg_NPC_ShowAgain_IfGetArtHero
                            return dlg->Close(msg);
                        }
                    }
                }
            }
        } // subtype == MST_LBUTTONDOWN
    } // type == MT_MOUSEBUTTON

    // Если прописан чит-код "gosolo"
    if (o_AutoSolo == 1) {  
        if ( o_GetTime() - timeClickAvtoSolo >= 2000 ) {
            e_ClickSound();
            return dlg->Close(msg);
        }       
    }

    //  делаем подвижность дефа командира   
    if (( (o_GetTime() - DrawingWaitTime) & 0x80000000) == 0) /*  && !npc->alive) */ 
    {
        CALL_1(_int_, __thiscall, 0x4EB140, NPC_Def); 
        dlg->Redraw(1);
        _dword_ t = o_GetTime() - DrawingWaitTime;
        if ((signed int)t < 100) t = 100;
        DrawingWaitTime += t;
    }

    return r;
}

// 0x76A46E процесс подготовки окна командира тут
_int_ __stdcall Y_Dlg_NPC_Show(HiHook* hook, _DlgNPC_* dlgNPC)
{
    time_click = 0; // переменная для дабл_клика
    _Npc_* npc = GetNpc(dlgNPC->DlgTop);    // структура командира

    // если структуры командира нет
    if (npc == NULL) {
        // жестко устанавливаем:
        dlgNPC->Request = 0; // НЕ повышение уровня
        dlgNPC->DlgLeft = 0; // возврат: ничего не делать
        return dlgNPC->DlgLeft;
    }

    dlgNPC->DlgTop = 0; // теперь её обнуляем и будем хранить данные о том, что при передаче арта герою нужно будет заново вызвать диалог

    // если повышение уровня, то считаем сколько навыков можно изучить
    // и если навыки все изучены, то неоходимо окно меньшего формата y = 505;
    if ( dlgNPC->Request ) { // при повышении уровня
        for (int i = 0; i < 6; ++i ){ 
            if ((int)o_dlgNPC->Next[i] < 102) { // 102 - последний кадр в Dlg_NPC2.def, дальше мусор (не считать)
                dlgNPC->Request = 2; // устанавливаем, что есть что повышать из навыков командира
            }
        }
    }

    // dlgNPC->Request = 0; // НЕ повышение уровня
    // dlgNPC->Request = 1; // повышение уровня без картинок
    // dlgNPC->Request = 2; // повышение уровня с картинками
    
    int medfontHI = 20; // medfont2->height;
    int tempVar;

    int x = 634; 
    int y = 505;
    char* name_pcx = "Dlg_NPCn.pcx";
    if (dlgNPC->Request == 2) { 
        y = 600;
        name_pcx = "Dlg_NPCu.pcx";
    }
    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Y_New_CommanderDlg_Proc);

    // установить курсор(0,0)
    _MouseMgr_* mouse = o_MouseMgr;
    mouse->SetMouseCursor(0, 0);
    mouse->MouseON(0);

    // (id = 4, 5) шагающий деф командира (делаем за фоновой картинкой)
    sprintf(o_TextBuffer, textProcS, *(int*)(0x68295C+4+4*(o_CreatureInfo[npc->type + 174].town)) );
    dlg->AddItem(_DlgStaticPcx8_::Create(20, 70, 100, 130, 4, /*"CRBKGNEU.pcx"*/ o_TextBuffer )); // фон замка под дефом командира нейтральный (id = 4)
    dlg->AddItem(NPC_Def = _DlgStaticDef_::Create(-130, -80, 100, 130, 5, o_CreatureInfo[npc->type + 174].def_name, 0, 0, 18)); // шагающий деф командира (id = 5)
    //NPC_Def->def->groups_count = 2; 


    // задник диалога под цвет игрока (id = 1)
    _DlgStaticPcx8_* fonPcx;
    dlg->AddItem(fonPcx = b_DlgStaticPcx8_Create(0, 0, dlg->width, dlg->height, 1, name_pcx, 2048));
    tempVar = o_GameMgr->GetHero(npc->id)->owner_id; // цвет хозяина героя
    if (tempVar == -1) {
        tempVar = o_GameMgr->GetMeID();
    } 
    fonPcx->Colorize(tempVar); // установить цвет рамки диалога 

    // кнопки ок и уволить
    _DlgButton_* bttnOK;
    bttnOK = b_DlgButton_Create(285,  dlg->height -76, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2);
    // и сразу делаем выход и на ESC и ENTER
    int hotkey = 1;     CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
        hotkey = 28;    CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
    dlg->AddItem(bttnOK);

    // (id = 3) кнопка уволить командира 
    dlg->AddItem(_DlgButton_::Create(564, dlg->height -76, 51, 35, 3, "Dlg_NPCdel.def", 0, 1, 0, 83, 2));   
    if ( !(dlgNPC->Flags & 8) || !(dlgNPC->Flags & 1) ) dlg->GetItem(3)->SetEnabled(0); 

    // (id = 6) подсказка на дефе (пустышка, но по ней ПКМ определяет id и выдает окно биографии)
    dlg->AddItem(_DlgStaticText_::Create(20, 70, 100, 130, o_NullString, n_MedFont, 1, 6, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));  

    // (id = 10) подсказка в статус баре    
    dlg->AddItem(NPC_StatBar = _DlgStaticTextPcx8ed_::Create(8, dlg->height -18 -8, dlg->width - 16, 18, o_NullString, n_SmallFont, adRollvrPcx, 1, 10, ALIGN_H_CENTER | ALIGN_V_CENTER) ); // HD_TStat.pcx

    if ( !(dlgNPC->Flags & 1) ){ // текстовые вставки " МЕРТВ!!! "  (id = 8, 9)
        dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, medfontHI, txtresWOG->GetString(66), n_MedFont, 27, 8, ALIGN_H_LEFT  | ALIGN_V_CENTER, 0)); 
        dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, medfontHI, txtresWOG->GetString(66), n_MedFont, 27, 9, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));  
    }
    // (id = 11) имя командира 
    dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, medfontHI, dlgNPC->Name, n_MedFont, 7, 11, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 

    // (id = 12, 13) уровень текст и число 
    dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, medfontHI, txtresWOG->GetString(67), n_MedFont, 7, 12, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
    sprintf(o_TextBuffer, textProcD, dlgNPC->Level);
    dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, medfontHI, o_TextBuffer, n_MedFont, 1, 13, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); 

    // (id = 14, 15) класс командира текст 
    dlg->AddItem(_DlgStaticText_::Create(16, 205, 110, medfontHI, txtresWOG->GetString(65), n_MedFont, 7, 14, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
    dlg->AddItem(_DlgStaticText_::Create(16, 220, 110, medfontHI, dlgNPC->Type, n_MedFont, 1, 15, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // (id = 16, 17) имя героя хозяина 
    dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, medfontHI, txtresWOG->GetString(68), n_MedFont, 7, 16, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
    dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, medfontHI, dlgNPC->HeroName, n_MedFont, 1, 17, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); 

    // (id = 18, 19) текущий опыт 
    dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, medfontHI, txtresWOG->GetString(71), n_MedFont, 7, 18, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
    sprintf(o_TextBuffer, textProcD, dlgNPC->CurExp);
    dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, medfontHI, o_TextBuffer, n_MedFont, 1, 19, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

    // (id = 20, 21) следующий уровень
    dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, medfontHI, txtresWOG->GetString(72), n_MedFont, 7, 20, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
    sprintf(o_TextBuffer, textProcD, dlgNPC->NextExp);
    dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, medfontHI, o_TextBuffer, n_MedFont, 1, 21, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); 

    // (id = 22) Навыки_класса_текст 
    dlg->AddItem(_DlgStaticText_::Create(377, 90, 192, medfontHI, txtresWOG->GetString(77), n_MedFont, 7, 22, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
    // (id = 23) Артефакты_текст 
    dlg->AddItem(_DlgStaticText_::Create(377, 159, 192, medfontHI, txtresWOG->GetString(78), n_MedFont, 7, 23, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
    // (id = 24) Основные_навыки_текст 
    dlg->AddItem(_DlgStaticText_::Create(133, 246, 366, medfontHI, txtresWOG->GetString(69), n_MedFont, 7, 24, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
    // (id = 25) Вторичные_навыки_текст 
    dlg->AddItem(_DlgStaticText_::Create(133, 343, 366, medfontHI, txtresWOG->GetString(70), n_MedFont, 7, 25, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 

    ///////////////////////////////////////////////////////////

    // (id = 30, 31) Атака 
    sprintf(o_TextBuffer, "%d (%d)", dlgNPC->pAT, dlgNPC->hAT);
    dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 30, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); 
    dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, medfontHI, txtresWOG->GetString(54), n_MedFont, 7, 31, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 32, 33) Защита 
    sprintf(o_TextBuffer, "%d (%d)", dlgNPC->pDF, dlgNPC->hDF);
    dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 32, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));  
    dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, medfontHI, txtresWOG->GetString(55), n_MedFont, 7, 33, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 34, 35) Здоровье 
    sprintf(o_TextBuffer, textProcD, dlgNPC->pHP);
    dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 34, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));  
    dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, medfontHI, txtresWOG->GetString(56), n_MedFont, 7, 35, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 36, 37) Урон 
    sprintf(o_TextBuffer, "%d - %d", dlgNPC->pDML, dlgNPC->pDMH);
    dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 36, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));   
    dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, medfontHI, txtresWOG->GetString(58), n_MedFont, 7, 37, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

    // (id = 38, 39) Сила магии 
    sprintf(o_TextBuffer, textProcD, dlgNPC->pMP);
    dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 38, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));   
    dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, medfontHI, txtresWOG->GetString(59), n_MedFont, 7, 39, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 40, 41) Сопротивление 
    sprintf(o_TextBuffer, "%d%%", dlgNPC->pMR);
    dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 40, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));  
    dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, medfontHI, txtresWOG->GetString(60), n_MedFont, 7, 41, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 42, 43) Боезапас 
    sprintf(o_TextBuffer, textProcD, dlgNPC->pShots);
    dlg->AddItem(_DlgStaticText_::Create(128, 201, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 42, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));   
    dlg->AddItem(_DlgStaticText_::Create(129, 201, 192, medfontHI, txtresWOG->GetString(61), n_MedFont, 7, 43, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    // (id = 44, 45) Скорость 
    sprintf(o_TextBuffer, textProcD, dlgNPC->pSP);
    dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, medfontHI, o_TextBuffer, n_MedFont, 1, 44, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    
    dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, medfontHI, txtresWOG->GetString(57), n_MedFont, 7, 45, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 

    ///////////////////////////////////////////////////////////

    //  (id = 46, 47) дефы спец умений командира в зависимости от класса (пропатчено по адресу 0x7A2228)
    dlg->AddItem(_DlgStaticDef_::Create(395, 113, 43, 43, 46, npc1Def, (int)dlgNPC->SpecIcon1, 0, 18));    
    dlg->AddItem(_DlgStaticDef_::Create(504, 113, 43, 43, 47, npc1Def, (int)dlgNPC->SpecIcon2, 0, 18)); 

    // (id = 48 - 53) артефакты (пропатчено по адресу 0x7A3B78)
    for (int i = 0; i < 6; ++i ){ 
        if ((int)dlgNPC->ArtIcons[i]) { // если кадр дефа != 0 (пропатчено в 0x76AB3B)
            dlg->AddItem(_DlgStaticDef_::Create(330 +48*i, 184, 45, 45, 48+i, artifactDef, (int)dlgNPC->ArtIcons[i], 0, 18));
        }
    }

    // (id = 54 - 59) основные навыки (пропатчено по адресу 0x7A26F0)
    for (int i = 0; i < 6; ++i ){ 
        dlg->AddItem(_DlgStaticDef_::Create(40 +96*i, 270, 70, 70, 54+i, npc2Def, (int)dlgNPC->Bonus[i], 0, 18));
    }

    // (id = 60 - 65) дополнительные навыки командира (пропатчено по адресу 0x76A92F)
    for (int i = 0; i < 6; ++i ){ 
        if ((int)dlgNPC->SpecBonus[i]) { // если кадр дефа != 0 (пропатчено в 0x76A92F)
            dlg->AddItem(_DlgStaticDef_::Create(57 +93*i, 367, 46, 46, 60+i, npc3Def, (int)dlgNPC->SpecBonus[i], 0, 18));
        }
    }
    // согласно исходников WoG функция будет возвращать
    // номер выбранной картинки, если Request = 1
    // и ничего (0), если Request = 0;
    // возврат -1 = удалит командира
    dlgNPC->DlgLeft = 0;

    if ( dlgNPC->Request == 2 ) { // при повышении уровня c выбором картинок
        // (id = 66) подсказка выберите навыки для повышения 
        dlg->AddItem(_DlgStaticText_::Create(133, 422, 366, medfontHI, txtresWOG->GetString(73), n_MedFont, 7, 66, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
        dlg->AddItem(_DlgStaticDef_::Create(40, 445, 70, 70, 67, npc2Def, (int)dlgNPC->NextActive[0], 0, 18));
        dlgNPC->DlgLeft = 1;
        for (int i = 1; i < 6; ++i ){ 
            if ((int)dlgNPC->Next[i] < 102) { // 102 - последний кадр в Dlg_NPC2.def, дальше мусор (не считать)
                // (id = 68 - 72) выбираем навыки для повышения 
                dlg->AddItem(_DlgStaticDef_::Create(40 +96*i, 445, 70, 70, 67+i, npc2Def, (int)dlgNPC->Next[i], 0, 18));
            }
        }
    } 
    // Если прописан чит-код "gosolo"
    if (o_AutoSolo == 1) {
        timeClickAvtoSolo = o_GetTime();
    }

    dlg->Run(); 
    dlg->Destroy(TRUE);

    return dlgNPC->DlgLeft; 
}

void __stdcall Y_Dlg_NPC_Prepare(HiHook* hook, _Npc_* npc, int is_lvlup, int bttn_del, int flags)
{
    CALL_4(void, __thiscall, hook->GetDefaultFunc(), npc, is_lvlup, bttn_del, flags);
    o_dlgNPC->DlgTop = npc->id;
    o_dlgNPC->Request = is_lvlup;

    return;
}

// функция, которая будет вызывать заново диалог командира
// если им ранее был снят и передан артефакт герою
_int_ __stdcall Y_Dlg_NPC_ShowAgain_IfGetArtHero(LoHook* h, HookContext* c)
{
    if (o_dlgNPC->DlgTop == 1) { // нужно открыть диалог командира заново
        c->return_address = 0x76E968;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT; 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// создание кнопки командира ////////////////////////////////////////////////////////

// воговские константы (содовские константы нужно будет копировать в нижеуказанные)
#define WoG_DlgHero_msg (*(_EventMsg_**)0x28604CC)
#define WoG_DlgHero_hero (*(_Hero_**)0x2860470)
#define WoG_DlgHero_art (*(_int_*)0x2860820)

// опция командиров (0-вкл, 1-выкл)
#define WoG_NoNPC (*(_int_*)0x277192C)
// id кнопки командира
#define NPC_BttnID 4445


_CustomDlg_* __stdcall Y_HeroDlg_Create(HiHook* hook, _CustomDlg_* dlg)
{
    CALL_1(_CustomDlg_*, __thiscall, hook->GetDefaultFunc(), dlg);

    if (!WoG_NoNPC) // если командиры включены (обратная проверка!)
        dlg->AddItem(_DlgButton_::Create(316, 17, NPC_BttnID, "bttnNPC.def", 0, 1, 0, 0));

    return dlg;
}

int __stdcall Y_HeroDlg_Proc(HiHook* hook, _CustomDlg_* dlg, _EventMsg_* msg)
{
    // реализуем клики мышью в окне героя
    if (msg->type == MT_MOUSEBUTTON) {
        if (msg->subtype == MST_LBUTTONDOWN || 
            msg->subtype == MST_LBUTTONCLICK ||
            msg->subtype == MST_RBUTTONDOWN) {

            // сохраняем координаты клика
            _int32_ x_abs = msg->x_abs;
            _int32_ y_abs = msg->y_abs;

            // теперь специально для WoGа модифицируем координаты мыши (без них он с HD не работает, скотина)
            if (o_HD_X > ORIG_X) // 1180-800
                msg->x_abs -= ((o_HD_X - ORIG_X) >> 1); 

            if (o_HD_Y > ORIG_Y) // 664-600
                msg->y_abs -= ((o_HD_Y - ORIG_Y) >> 1);

            // производим копирование указателя на текущего героя в воговский указатель
            WoG_DlgHero_hero = o_HeroDlg_Hero; // вот зачем такие махинации?!! Агрр....
            WoG_DlgHero_msg = msg;
            WoG_DlgHero_art = o_CurArt.id; 

            // выполняем воговский триггер клика в окне героя (!?CM2)
            // wog 0x76EC85: call sub_74F7DE MouseClickHero(CNPC_ms,CNPC_hp)
            if ( CALL_2(signed int, __cdecl, 0x74F7DE, WoG_DlgHero_msg, WoG_DlgHero_hero) ) {
                // if true: CM:R0 or Hero dlg is redraw
                o_WndMgr->f_3C = -1;
                return 1;
            }

            // описываем реализацию передачи артефактов в слоты существ (0-6)
            if (msg->item_id >= 68 && msg->item_id <= 74 ) {
                if ( o_CurArt.id != -1 ) {
                    // CrExpoSet::DropArt2Stack(CNPC_ms,CNPC_hp,CNPC_DropArt)
                    // перенос артефакта в слот существ (на данный момент только Знамя Полководца (id 156))
                    if ( CALL_3(signed int, __cdecl, 0x72521B, WoG_DlgHero_msg, WoG_DlgHero_hero, WoG_DlgHero_art) ) {
                        o_CurArt.id = -1;
                        o_CurArt.mod = -1;
                        b_MouseMgr_SetCursor(0, 0);
                    }
                    // HeroDlg->WogRedrawCreatureSlots(-1);
                    CALL_1(void, __cdecl, 0x716E81, -1);

                    o_WndMgr->f_3C = -1;
                    return 1;
                }
            }

            // реализуем нажатие на кнопку окна командира (и просто клик, и клик с артом в руке)
            // обработает это всё wog функция 0x76E865 RealClickNPC(CNPC_ms,CNPC_hp,CNPC_DropArt)
            if ( !WoG_NoNPC && msg->item_id == NPC_BttnID ) {
                if ( msg->subtype == MST_RBUTTONDOWN || msg->subtype == MST_LBUTTONCLICK ) {

                    if ( msg->subtype == MST_LBUTTONCLICK )
                        msg->subtype = MST_LBUTTONDOWN; 
                    // приходится делать так, ибо установлена проверка на тип клика в 0x76E8A6
                    // можно конечно и запатчить её, но вдруг кто-то захочет открыть окно командира через UN:C
                    // короче оставляем так ...

                    if ( CALL_3(signed int, __cdecl, 0x76E865, WoG_DlgHero_msg, WoG_DlgHero_hero, WoG_DlgHero_art) ) {
                        // ниже просто копируем код вога
                        o_CurArt.id = -1;
                        o_CurArt.mod = -1;
                        b_MouseMgr_SetCursor(0, 0);

                        o_WndMgr->f_3C = -1;
                        return 1;
                    }
                }
            }

            // возвращаем координаты клика
            msg->x_abs = x_abs;
            msg->y_abs = y_abs;
        }
    }

    // ведение мыши в окне героя: реализуем хинт кнопки командира
    if (!WoG_NoNPC) { // если командиры включены (обратная проверка!) 
        if (msg->type == MT_MOUSEOVER) {
            _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);

            if (it && it->id == NPC_BttnID) {
                _DlgMsg_ m; CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 115, 0, 0, 0, (int)json_Npc[0], 0)); 
                dlg->Redraw(TRUE, 114, 115);
                return 1;       
            }  
        } 
    }

    return CALL_2(int, __thiscall, hook->GetDefaultFunc(), dlg, msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_NPC(PatcherInstance* _PI)
{
    // диалог командира
    _PI->WriteHiHook(0x7736E8, SPLICE_, EXTENDED_, CDECL_, Y_Dlg_NPC_Show);
    _PI->WriteHiHook(0x76A46E, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_NPC_Prepare);
    _PI->WriteLoHook(0x76EAA5, Y_Dlg_NPC_ShowAgain_IfGetArtHero);

    // добавление кнопки командира
    _PI->WriteHiHook(0x4DE980, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Create);
    _PI->WriteHiHook(0x4DD540, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Proc);

    // вырезаем Воговских хук для создания функционала командира
    // путём восстанавления оригинального кода SOD
    _PI->WriteHexPatch(0x4DD632, "C742 3C FFFFFFFF");
}
