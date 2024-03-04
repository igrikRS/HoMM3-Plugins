////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// константы диалога экспы монстров /////////////////////////////////////////////////

#define o_DlgLoop (*(_int_*)0x847F00)
#define o_DlgUpgr (*(_int_*)0x840E18)

#define o_Mon    (*(_int_*)0x84730C)
#define o_Num    (*(_int_*)0x847D84)
#define o_Expo   (*(_int_*)0x85EB48)
#define o_CrExpo (*(_int_*)0x840F88)
#define o_ArtOutput (*(_int_*)0x8458D0)
#define o_MayChooseArtOpt (*(_int_*)0x841940)

#define o_Dlg_Case8 (*(_int_*)0x846B00)
char* n_DlgExpMon = "DlgCrExp.def";

#define CURMON (*(_int_*)0x7278B6)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// дополнительный диалог с флагом опыта /// /////////////////////////////////////////

int New_Dlg_ExpaMon_Case8(int style)
{
    int x = 440;
    int y = 390;
    if (style == 1)
        y += 40;

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());
    CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

    dlg->AddItem(_DlgStaticDef_::Create(dlg->width/2 -41, 26, 2, pskillDef, 4, 0, 0x12)); 

    dlg->AddItem(_DlgStaticText_::Create(20, 126, dlg->width -40, 20, json_CrExpo[0], n_BigFont, 10, 5, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

    for (int i = 0; i < 11; i++){
        dlg->AddItem(_DlgStaticText_::Create(24, 150 +i*20, dlg->width -48, 20, json_CrExpo[i+1], n_MedFont, 7, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));   
    }

    dlg->AddItem(_DlgStaticText_::Create(24, 150, dlg->width -48, 20, o_pCreatureInfo[o_Mon].name_single, n_MedFont, 1, 22, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));
    

    int rank = CALL_2(_int_, __cdecl, 0x727F20, CURMON, o_Expo); 
    sprintf(o_TextBuffer, "%s (%d)", Get_ITxtExp(2+rank), rank);
    dlg->AddItem(_DlgStaticText_::Create(24, 170, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 23, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    sprintf(o_TextBuffer, textProcD, o_Expo); 
    dlg->AddItem(_DlgStaticText_::Create(24, 190, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 24, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    sprintf(o_TextBuffer, textProcD, CALL_3(_int_, __cdecl, 0x727FB0, CURMON, rank +1, 0) - o_Expo); 
    dlg->AddItem(_DlgStaticText_::Create(24, 210, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 25, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    int t1 = CALL_1(_int_, __cdecl, 0x7283D0, CURMON); 
    int t2 = CALL_1(_int_, __cdecl, 0x727E20, CURMON) * t1 / 100;
    sprintf(o_TextBuffer, "%d%% (%d)", t1, t2);
    dlg->AddItem(_DlgStaticText_::Create(24, 230, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 26, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    sprintf(o_TextBuffer, textProcD, o_Num); 
    dlg->AddItem(_DlgStaticText_::Create(24, 250, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 27, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    t1 = CALL_3(_int_, __cdecl, 0x727FB0, CURMON, rank, 0);
    t2 = 999999;
    if( t1 ){ t2 = o_Num * o_Expo / t1 - o_Num; }   
    sprintf(o_TextBuffer, textProcD, t2); 
    dlg->AddItem(_DlgStaticText_::Create(24, 270, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 28, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

    sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727C00, CURMON)); 
    dlg->AddItem(_DlgStaticText_::Create(24, 290, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 29, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

    sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727E00, CURMON)); 
    dlg->AddItem(_DlgStaticText_::Create(24, 310, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 30, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

    t1 = CALL_1(_int_, __cdecl, 0x7283B0, CURMON);
    sprintf(o_TextBuffer, textProcD, t1); 
    dlg->AddItem(_DlgStaticText_::Create(24, 330, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 31, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

    t2 = CALL_3(_int_, __cdecl, 0x727FB0, CURMON, 10, 0);
    int t3 = 999999; 
    if ( t2 ){
        int t4 = o_Num * (t2 + t1);
        t3 = t4 / t2 - o_Num;
    }
    sprintf(o_TextBuffer, textProcD, t3); 
    dlg->AddItem(_DlgStaticText_::Create(24, 350, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 32, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


    if (style == 1){
        dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, box64x30Pcx));
        // Добавляем кнопку OK (id = 30725)
        _DlgButton_* bttnOK;
        bttnOK = b_DlgButton_Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2);
        bttnOK->SetHotKey(HK_ESC);
        bttnOK->SetHotKey(HK_ENTER);
        dlg->AddItem(bttnOK);
        dlg->Run();
    }
    if (style == 4)
        dlg->RMC_Show(); 
        
    dlg->Destroy(TRUE);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// диалог по выбору типа действия артефакта /////////////////////////////////////////

int __stdcall New_Dlg_ExpaMon_BannerDlg_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);
        if (msg->subtype == MST_LBUTTONCLICK){ 
            if (msg->item_id >= 30 && msg->item_id <40){
                for (int i = 0; i < 9; i++){
                    ((_DlgButton_*)dlg->GetItem(30+i))->def_frame_index = 0;
                    ((_DlgButton_*)dlg->GetItem(30+i))->press_def_frame_index = 1;
                }
                ((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index = 2;
                ((_DlgButton_*)dlg->GetItem(msg->item_id))->press_def_frame_index = 3;
                CALL_2(_int_, __thiscall, 0x728380, o_CrExpo, msg->item_id -30); 
                dlg->Redraw(TRUE);
            }
        }
    return r;
}

int New_Dlg_ExpaMon_BannerDlg()
{
    if (!o_MayChooseArtOpt || !o_CrExpo){
        b_MsgBox(Get_ITxtExp(455), 4);  
        return 0;
    }

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, 460, 360, DF_SCREENSHOT | DF_SHADOW, New_Dlg_ExpaMon_BannerDlg_Proc);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());
    _MouseMgr_* mouse = o_MouseMgr;
    mouse->SetMouseCursor(0, 0);

    dlg->AddItem(_DlgStaticText_::Create(18, 30, 424, 30, Get_ITxtExp(340), n_BigFont, 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
    
    int dy = 27;    

    for (int i = 0; i < 9; i++){
        b_YellowFrame_Create(dlg, 18, 62+i*dy, 424, 24, 10+i, ON, o_Pal_Grey);
        
        dlg->AddItem(_DlgStaticText_::Create(44, 66+i*dy, 394, 16, Get_ITxtExp(341 +i), n_SmallFont, 1, 20+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

        dlg->AddItem(_DlgButton_::Create(22, 66+i*dy, 420, 18, 30+i, radioBttnDef, 0, 1, 0, 0, 0));
    } 
    int s = CALL_1(_int_, __thiscall, 0x727BE0, o_CrExpo);  
    ((_DlgButton_*)dlg->GetItem(30+s))->def_frame_index = 2;
    ((_DlgButton_*)dlg->GetItem(30+s))->press_def_frame_index = 3;

    dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -20 -31, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -20 -30, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 28, 2));

    dlg->Run();
    int t = CALL_1(_int_, __thiscall, 0x727BE0, o_CrExpo);
    dlg->Destroy(TRUE);
    if ( s == t)
         return 0;
    else return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// основной диалог ////////////////////////////////////////////////////////////////////

int __stdcall New_Dlg_ExpaMon_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    // вытаскиваем структуру _CreatureExpo_
    _CreatureExpo_* crexpo = (_CreatureExpo_*)dlg->custom_data[0];

    if (msg->type == MT_MOUSEOVER)
    {
        _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
        char* text_bar = o_NullString;
        if (it) 
        {
            switch (it->id)
            {
                case 1: // кнопка улучшенного
                    text_bar = Get_ITxtExp(107); 
                    break;
                
                case 6: // монстр
                    text_bar = Get_ITxtExp(456); 
                    break;

                case 8: case 9: // артефакт
                    text_bar = crexpo->ArtHint; 
                    break;          

                case 10: // отряд может получить ранги ....
                    text_bar = Get_ITxtExp(123); 
                    break;

                // первая колонка (желтая)
                case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
                case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
                    text_bar = crexpo->RowCaptionHints[it->id-11]; 
                    break;

                // колонки 2-12 с параметрами (белые)
                case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
                case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: 
                    text_bar = crexpo->RowHints[it->id-30]; 
                    break;
                
                // иконки способностей и их тексты
                case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
                case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65:
                    text_bar = crexpo->HintProperties[(it->id-50) /2]; 
                    break;                  

                // кнопка ОК
                case DIID_OK: 
                    sprintf(o_TextBuffer, textProcS, txtresWOG->GetString(85)); 
                    text_bar = o_TextBuffer; 
                    break;
                // нет элемента = пустой хинт
                default: 
                    text_bar = o_NullString;
                    break;
            }
            ((_DlgTextEdit_*)dlg->GetItem(2))->SetText(text_bar);
            ((_DlgTextEdit_*)dlg->GetItem(2))->RedrawScreen();
            dlg->Redraw();
        }  
    } 

    if (msg->type == MT_MOUSEBUTTON)
    {
        if (msg->subtype == MST_RBUTTONDOWN)
        {
            int frame = 0;
            switch (msg->item_id)
            {
                // картинка и рамка монстра
                case 5: case 6: 
                // первая колонка (желтая)
                case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
                case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
                // колонки 2-12 с параметрами (белые)
                case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
                case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45:
                        New_Dlg_ExpaMon_Case8(4); 
                        break;                          

                // диалог выбора параметров Знамени Полководца
                case 9: 
                    if (New_Dlg_ExpaMon_BannerDlg() == 1){ 
                            o_DlgLoop = 1;
                            return dlg->Close(msg); 
                        }   
                    break; 

                // иконки способностей и их тексты
                case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
                case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65:
                    frame = (int)crexpo->IcoProperties[(msg->item_id-50)/2];
                    if (frame == 0){
                        sprintf(myString1, "%s\n", crexpo->TxtProperties[(msg->item_id-50)/2]);
                    }
                    else {
                        sprintf(myString1, "%s\n\n %s\n", crexpo->TxtProperties[(msg->item_id-50)/2],
                                                            Get_ITxtExp(*(int*)(0x840E1C +4*((msg->item_id-50)/2))+1));
                    }   
                    b_MsgBoxAndDef(n_DlgExpMon, frame, myString1);  break; 
                    
                default: break; 
            }
        } 

        if (msg->subtype == MST_LBUTTONCLICK){
            if (msg->item_id == 1)  {
                o_DlgUpgr = 1;
                return dlg->Close(msg);
            }
        }
        if (msg->subtype == MST_LBUTTONDOWN){ 
            if (msg->item_id == 5 || msg->item_id == 6 || (msg->item_id >= 10 && msg->item_id <= 45)){
                New_Dlg_ExpaMon_Case8(1);
            }

            if (msg->item_id == 9){

                int hero_id = -1;
                if ( o_CrExpo )
                    hero_id = *(short*)(o_CrExpo +12);

                if ( !o_MayChooseArtOpt || hero_id < 0 || hero_id >= o_HEROES_COUNT ){
                    b_MsgBox(Get_ITxtExp(455), 1);
                }
                else {
                    b_MsgBoxD(txtresWOG->GetString(103), MBX_OKCANCEL, 8, 156);
                    if (o_WndMgr->result_dlg_item_id == DIID_OK) {
                        o_DlgLoop = CALL_1(_int_, __thiscall, 0x728280, o_CrExpo);

                        _Hero_* hero = o_GameMgr->GetHero(hero_id);

                        int Art[] = {156, -1};
                        CALL_3(char, __thiscall, 0x4E3200, hero, &Art, -1); 

                        return dlg->Close(msg); 
                    }
                }
            }
        }
    } 

    return r;
}

int __stdcall Y_NewDlg_CreatureExpo(HiHook* hook, _CreatureExpo_* crexpo) 
{
    // количество существующих и видимых строк
    int linesCount = 0;
    int linesCountShowed = 0;

    // переменная существующих строк
    _bool_ trueLine = false;

    // полное количество текстовых строк
    for (int i = 15; i >= 0; i--) {
        // считаем существующие строки
        if ( crexpo->RowCaptions[i] || trueLine ) {
            linesCount++;
            trueLine = true;
        }
        // теперь считаем видимые строки
        if ( crexpo->RowCaptions[i] )
            linesCountShowed++;
    }

    // недопускаем слишком маленькую высоту диалога
    if ( linesCountShowed < 7)
        linesCountShowed = 7;

    // вручную указываем высоту текста smallfont2->height
    int smallfontHight = 16;

    int x = 800;
    int y = 132 + smallfontHight*linesCountShowed +52*Round(((float)crexpo->IcoPropertiesCount)/2); 

    // создаём диалог
    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_ExpaMon_Proc);
    Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());

    // делаем доступ к структуре _CreatureExpo_
    dlg->custom_data[0] = (int)crexpo;

    // установить курсор(0,0)
    _MouseMgr_* mouse = o_MouseMgr;
    mouse->SetMouseCursor(0, 0);
    mouse->MouseON(0);

    // Рамка кнопки OK
    dlg->AddItem(_DlgStaticPcx8_::Create(x-83, y-71, 0, box64x30Pcx));

    // Добавляем кнопку OK (id = 30725)
    _DlgButton_* bttnOK;
    bttnOK = b_DlgButton_Create(x-82, y-70, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2);
    bttnOK->SetHotKey(HK_ESC);
    bttnOK->SetHotKey(HK_ENTER);
    dlg->AddItem(bttnOK);

    // Добавляем кнопку просмотра улучшенного (id = 1)
    if (crexpo->ShowSpecButton){
        dlg->AddItem(_DlgStaticPcx8_::Create(x-153, y-71, 0, box64x30Pcx));
        _DlgButton_* bttnUpg;
        bttnUpg = b_DlgButton_Create(x-152, y-70, 64, 30, 1, "DlgExpUp.def", 0, 1, 0, 0, 0);            
        bttnUpg->SetHotKey(HK_U);
        bttnUpg->SetHotKey(HK_SPACE);
        dlg->AddItem(bttnUpg);
    }

    // Подсказка (id = 2)
    dlg->AddItem(_DlgStaticText_::Create(8, y-26, x-34, smallfontHight+2, o_NullString, n_SmallFont, 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // заголовок диалога (id = 3)
    dlg->AddItem(_DlgStaticText_::Create(150, 14, 500, 30, crexpo->Caption, n_BigFont, 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

    // нижнее описание монстра красным (id = 4)
    dlg->AddItem(_DlgStaticText_::Create(18, y-76, x-200, 44, crexpo->Info, n_MedFont, 27, 4, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

    // картинка и рамка монстра (id = 5, 6) 
    dlg->AddItem(_DlgStaticDef_::Create(18, 35, 5, o_Twrport, o_Mon +2, 0, 0x12)); 
    dlg->AddItem(_DlgStaticDef_::Create(18, 35, 6, o_Twrport, 1, 0, 0x12));  

    // название артефакта и рамка для него (id = 7, 8)
    dlg->AddItem(_DlgStaticText_::Create(18, 100, 60, 15, o_GENRLTXT_TXT->GetString(616), n_MedFont, 1, 7, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
    dlg->AddItem(_DlgStaticDef_::Create(22, 116, 8, n_DlgExpMon, 159, 0, 0x12)); 
    
    // артефакт и его количество (id = 9)
    if (crexpo->ArtCopy > 0){
        dlg->AddItem(_DlgStaticDef_::Create(25, 119, 9, artifactDef, 156, 0, 0x12));    // картинка артефакта (id = 9)
        sprintf(myString1, textProcD, crexpo->ArtCopy);
        dlg->AddItem(_DlgStaticText_::Create(47, 142, 20, 20, myString1, "tiny.fnt", 1, 9, ALIGN_H_RIGHT | ALIGN_V_BOTTOM, 0));
    }

    // желтая рамка обрамления ранга
    int rank = CALL_2(int, __cdecl, 0x727F20, CURMON, o_Expo);
    b_YellowFrame_Create(dlg, 164+ 56*rank, 50, 56, smallfontHight*linesCountShowed, 100, ON, o_Pal_Y);

    // сохраняем ранг в переменную диалога
    dlg->custom_data[1] = rank;

    // верхние названия рангов 
    for (int i = 0; i <= 10; i++){
    dlg->AddItem(_DlgStaticText_::Create(168 +56*i, 34, 58, 16, 
        CALL_3 (char*, __cdecl, 0x77710B, 1+i, 1, 0x847D88), 
        n_SmallFont, 7, 10, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
    }

    // элементы заполнения колонок (первый столбик) id = 11...26
    int lineID = 0; // счётчик видимых строк
    for (int i = 0; i < linesCount; i++) {
        if ( crexpo->RowCaptions[i] ) {
            dlg->AddItem(_DlgStaticText_::Create(80, 50 + smallfontHight*lineID, 80, 16, crexpo->RowCaptions[i], n_SmallFont, 7, 11+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
            lineID++;
        }
    }

    // элементы заполнения колонок (2-12 столбик) id = 30...45 (один id на всю линию)
    lineID = 0; // обнуляем счётчик видимых строк
    char text[16];    
    for (char i = 0; i < linesCount; i++)
    {
        if ( crexpo->RowCaptions[i] )
        {
            for (char j = 0; j < 11; j++){
                for (char k = 0; k < 8; k++){
                    if (*(char*)(crexpo->Rows[i] +j*8 +k) != ' ')
                        text[k] = *(char*)(crexpo->Rows[i] +j*8 +k);
                    else {
                        for (char s = k; s < 16; s++)
                            text[s] = ' ';

                        k = 7;
                    }
                } 
                dlg->AddItem(_DlgStaticText_::Create(168 + 56*j, 50 + smallfontHight*lineID, 54, 16, text, n_SmallFont, 1, 30+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));                
            }
            // переход на следующую строку
            lineID++;
        }
    }


    // нижние картинки с дефами (id 50 ... )
    x = 18;  
    y = 60 +16*linesCountShowed; 

    if (crexpo->IcoPropertiesCount)
    {
        for (int i = 0; i < crexpo->IcoPropertiesCount; i++)
        {
            // изображения навыков
            dlg->AddItem(_DlgStaticDef_::Create(x, y, 50+i*2, n_DlgExpMon, (int)crexpo->IcoProperties[i], 0, 0x12)); 
            
            // текстовые описания к изображениям
            dlg->AddItem(_DlgStaticText_::Create(54 + x, y, 330, 50, crexpo->TxtProperties[i], n_SmallFont, 7, 51+i*2, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
            if (x == 18)
                x = 400;
            else x = 18;
            if ( (i+10)%2 == 1 )
                y += 52;
        }   
    }   

    dlg->Run();
    dlg->Destroy(TRUE);

    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_ExpaMon(PatcherInstance* _PI)
{
    // диалог Экспы монстров
    _PI->WriteHiHook(0x773B9D, SPLICE_, EXTENDED_, CDECL_, Y_NewDlg_CreatureExpo);  
}
