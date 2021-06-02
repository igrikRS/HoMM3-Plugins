/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог IF:D/E /////////////////////////////////////////////////////////

namespace dlgSphinx
{
    const int IMAGE_TYPE_PCX   = 0x4;
    const int IMAGE_TYPE_PCX16 = 0x3;
    const int IMAGE_TYPE_ERA   = 0x2;
    const int IMAGE_TYPE_ZVS   = 0x1;
    const char* IMAGE_GOTCHA = "{Gotcha!!!}\n\nYou're trying to upload some bullshit.\n But not today! (c) %s\n\n wog native dialogs.era: %s \n\n image name: {%s}";

} // namespace dlgSphinx


// переменная, которая заставляет выполняться диалог ввода из zvslib.dll
int BanDlg_CustomReq_EnterText = false;

extern "C" __declspec(dllexport) int UseWin32InputControl(int newState);

int UseWin32InputControl(int newState)
{
    // читаем предыдущее состояние
    int prevState = BanDlg_CustomReq_EnterText;

    if ( newState ) {
        BanDlg_CustomReq_EnterText = true;
    } else BanDlg_CustomReq_EnterText = false;

    // возвращаем предыдущее состояние
    return prevState;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// указатель на структуру диалога ввода
_Sphinx1_* o_Sphinx1 = 0;

// переменная, которая не даёт запуститься диалог ввода несколько раз
// можно запустить только единожды
_bool_ Dlg_CustomReq_Ban = false;

/*
* функция проверки на расширение картинки
** поддерживаются:
** - pcx
** - pcx16
** - bmp, jpg, jpeg, png
** - gif, avi
*/
_int32_ IsSupportedFormatImage(char* image_name) 
{
    if ( ERA_VERSION < 2940 )
        return false;

    // защита от вылета при image_name = "(null)"
    if ( !image_name )
        image_name = "default.bmp";

    std::string name;
    name.append(image_name);

    // переводим строку в верхний регистр
    std::transform(name.begin(), name.end(),name.begin(), ::toupper);

    if(name.substr(name.find_last_of(".") + 1) == "PCX")
       return dlgSphinx::IMAGE_TYPE_PCX; 
    if(name.substr(name.find_last_of(".") + 1) == "PCX16")
       return dlgSphinx::IMAGE_TYPE_PCX16; 

    // проверки на расширение файла для "LoadImageAsPcx16"
    if(name.substr(name.find_last_of(".") + 1) == "BMP")
       return dlgSphinx::IMAGE_TYPE_ERA; 
    if(name.substr(name.find_last_of(".") + 1) == "JPG")
       return dlgSphinx::IMAGE_TYPE_ERA; 
    if(name.substr(name.find_last_of(".") + 1) == "JPEG")
       return dlgSphinx::IMAGE_TYPE_ERA; 
    if(name.substr(name.find_last_of(".") + 1) == "PNG")
       return dlgSphinx::IMAGE_TYPE_ERA; 

    // проверки для запуска диалога из zvslib.dll
    if(name.substr(name.find_last_of(".") + 1) == "GIF")
       return dlgSphinx::IMAGE_TYPE_ZVS; 
    if(name.substr(name.find_last_of(".") + 1) == "AVI")
       return dlgSphinx::IMAGE_TYPE_ZVS; 

    // пользователь пытается загрузить какую-то дичь 
    return 0;
}

// процедура диалога ввода
int __stdcall New_Dlg_CustomReq_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    // if (_DlgTextEdit_* edit != 0)
    if ( dlg->custom_data[0] != 0) {
        dlg->SetFocuseToItem(256);
        dlg->custom_data[0] = 0;
    }

    if (msg->type == MT_MOUSEOVER) // ведение мыши
    {
        _DlgStaticTextPcx8ed_* statBar = (_DlgStaticTextPcx8ed_*)dlg->GetItem(115);
        _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
        char* text = o_NullString;
        if (it) 
        {
            if ( it->short_tip_text ) 
                text = it->short_tip_text;

            statBar->SetText(text);
            statBar->Draw();
            statBar->RedrawScreen();

        }
    } // type == MT_MOUSEOVER 

    if (msg->type == MT_MOUSEBUTTON)
    {
        if (msg->subtype == MST_LBUTTONCLICK) // ЛКМ при отжатии
        {
            if (msg->item_id >= 15 && msg->item_id <= 18) 
            {
                _DlgItem_* item;
                _DlgStaticTextPcx8ed_* itemText;
                for (int i = 0; i < 4; i++) 
                {
                    item = dlg->GetItem(25+i);                  
                    if (item) { item->SendCommand(6, 4); }

                    itemText = (_DlgStaticTextPcx8ed_*)dlg->GetItem(19+i);
                    if (itemText)
                    {
                        itemText->font = (int)smalfont2;
                        itemText->color = 1;
                    }
                }

                dlg->GetItem(msg->item_id +10)->SendCommand(5, 4);
                ((_DlgStaticTextPcx8ed_*)dlg->GetItem(msg->item_id +4))->font = (int)medfont2;
                ((_DlgStaticTextPcx8ed_*)dlg->GetItem(msg->item_id +4))->color = 7;
                o_Sphinx1->SelItm = msg->item_id -14;

                dlg->GetItem(DIID_OK)->SetEnabled(1);
                dlg->Redraw();
            }
        } // subtype == MST_LBUTTONCLICK

        //if (msg->subtype == MST_LBUTTONDOWN)  // ЛКМ при нажатии
        //{     //} // subtype == MST_LBUTTONDOWN
    } // type == MT_MOUSEBUTTON

    // dlg->Redraw();

    return r;
}

// функция для очистки памяти от загруженых изоражений
int New_Dlg_CustomReq_PicDestroy(_Pcx16_* pic[4], int countPictures )
{
    // удаляем загруженные картинки
    for (int i = 0; i < countPictures; i++) {
        if ( pic[i] )
            pic[i]->DerefOrDestruct();
    }

    return 1;
};


// создание и заполнение элементами диалога ввода
int New_Dlg_CustomReq(_Sphinx1_* Sphinx)
{
    // высчитываем размеры диалога
    int x = 540;
    int y = 0;
    int yy = 120;
    int lines = 0;
    
    int lines1 = 0;
    int h_text1 = 0;
    int h_text2 = 0;
    int h_text3 = 0;
    int h_pic = 0;
    
    if (Sphinx->Text1) {
        lines1 = medfont2->GetLinesCountInText(Sphinx->Text1, x -40);
        h_text1 = lines1 *16 +15;
        yy += h_text1 ;
    }
    int cansel_show = Sphinx->ShowCancel;
    if (Sphinx->Text2) {
        h_text2 = 16;
        yy += h_text2 +26; // 26 выделяем высоту на поле ввода текста
    } 

    if (Sphinx->Text3 ) {
        h_text3 = 16;
        yy += h_text3;
    }

    if (Sphinx->Text2 && Sphinx->Text3 ) {
        yy -= h_text3;
    }

    if (!Sphinx->Text2 && !Sphinx->Text3 ) {
        cansel_show = 0;
    }

    if (Sphinx->Pic1Path || Sphinx->Pic2Path) {
        h_pic = 100;
        yy += h_pic;
    }

    int count_bttns = 0;
    if (Sphinx->Text3) {
        if (Sphinx->Chk1Text) { yy += 27; count_bttns++; }
        if (Sphinx->Chk2Text) { yy += 27; count_bttns++; }
        if (Sphinx->Chk3Text) { yy += 27; count_bttns++; }
        if (Sphinx->Chk4Text) { yy += 27; count_bttns++; }
    }

    if (count_bttns > 1) {
        if (Sphinx->Text2) { 
            yy -= 32;
        }
    }

    // проверка на выход за границы экрана 600px по вертикали
    if (yy > 580) {
        h_text1 -= yy - 580;
        yy = 580;
    }

    // перзаписываем утверджённую высоту диалога
    y = yy;

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_CustomReq_Proc);
    Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());

    _DlgStaticText_* Text1 = 0;
    if (Sphinx->Text1) {
        dlg->AddItem(Text1 = _DlgStaticText_::Create(24, 20, dlg->width -48, h_text1, Sphinx->Text1, n_MedFont, 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));
    }


    int ptr_pic1 = (int)&Sphinx->Pic1Path;
    int count_pics = 0; // считаем кол-во изображений
    for (int i = 0; i < 4; i++) {
        if ( *(char**)(ptr_pic1 +4*i) )
            count_pics++;
    }

    _Pcx16_* o_Pic[4];

    char* pic_name = o_NullString;
    // строим изображения id 10-14
    if (count_pics) {
        int max_width_pic = 400 / count_pics;
        for (int i = 0; i < count_pics; i++) {
            int delta = x/(count_pics+1);
            int start_x = (delta - (max_width_pic/2) ) + (delta*i);
            int start_y = 24+h_text1;
            
            char* pPath = *(char**)(ptr_pic1 +4*i);
            char* pHint = *(char**)(ptr_pic1 +4*i +16);
        
            char* short_name = GetShortFileName_Y(pPath);

            // получаем "тип" картинки, и по этому "типу" обрабатываем способ загрузки
            _int32_ imageType = IsSupportedFormatImage(short_name);

            if ( imageType >= dlgSphinx::IMAGE_TYPE_ERA )
            {           
                // грузим картинку через era.dll->LoadImageAsPcx16()
                if ( imageType == dlgSphinx::IMAGE_TYPE_ERA )
                    o_Pic[i] = (_Pcx16_*)Era::LoadImageAsPcx16(pPath, short_name, 0, 0, max_width_pic, 100, 3); 
                // *.pcx16
                if ( imageType == dlgSphinx::IMAGE_TYPE_PCX16 )
                {
                    std::string pcxName;
                    pcxName.append(short_name);
                    int pcxNameLength = pcxName.length() -2;
                    pcxName.erase(pcxNameLength);
                    o_Pic[i] = o_LoadPcx16(pcxName.c_str());
                }
                // *.pcx (8 bit)
                if ( imageType == dlgSphinx::IMAGE_TYPE_PCX )
                    o_Pic[i] = (_Pcx16_*)o_LoadPcx8(short_name); 
                
                int pic_x = 0;
                int pic_y = 0;
                if (o_Pic[i]) 
                {
                    // проверяем размеры загруженной картинки
                    // и при выходе за границы 100х100
                    pic_x = o_Pic[i]->width;
                    pic_y = o_Pic[i]->height;

                    // вычисляем привязку изображений (координаты верхнего левого угла картинки)
                    start_x += ( (max_width_pic/2) - pic_x/2);
                    start_y += (50 - pic_y/2);

                    if ( imageType == dlgSphinx::IMAGE_TYPE_PCX ) // *.pcx (8 bit)
                        dlg->AddItem(_DlgStaticPcx8_::Create(start_x, start_y, pic_x, pic_y, 10+i, o_Pic[i]->name));
                    else // *.pcx16 or era.dll->LoadImageAsPcx16()
                        dlg->AddItem(_DlgStaticPcx16_::Create(start_x, start_y, pic_x, pic_y, 10+i, o_Pic[i]->name, 2048));

                    // dlg->GetItem(10+i)->full_tip_text = pPath; // ПКМ на картинке

                    if (pHint)
                    {
                        dlg->GetItem(10+i)->full_tip_text = o_NullString;
                        dlg->GetItem(10+i)->short_tip_text = pHint;
                    }
                }
            } 
            else if ( imageType == dlgSphinx::IMAGE_TYPE_ZVS ) // avi or gif file
            {
                // в диалоге неподдерживаемая картинка gif/avi, поэтому выходим из нашего диалога
                // и передадим управление воговскому из zvslib.dll который их может обработать
                
                // уничтожаем диалог
                dlg->Destroy(TRUE);
                // уничтожаем картинки
                New_Dlg_CustomReq_PicDestroy(&o_Pic[0], count_pics);

                // 10 = значит нужно грузить стандартный диалог
                Sphinx->SelItm = 10; 
                return Sphinx->SelItm;  
            } 
            else
            {
                // преждупреждающее сообщение, что пользователь грузит дичь
                char* ermTypeTrigger = "IF:D/E";
                sprintf(MyString, dlgSphinx::IMAGE_GOTCHA, wndText::PLUGIN_AUTHOR, ermTypeTrigger, short_name);
                b_MsgBox(MyString, 5);

                // уничтожаем диалог
                dlg->Destroy(TRUE);
                // уничтожаем картинки
                New_Dlg_CustomReq_PicDestroy(&o_Pic[0], count_pics);

                // Отмена: -1 = cancel 
                Sphinx->SelItm = -1; 
                return Sphinx->SelItm;

            } // end else IsSupportedFormatImage()
        } // end for()
    } // end if (count_pics)

    if (Sphinx->Text2) {
        dlg->AddItem(_DlgStaticText_::Create(24, 30+h_text1+h_pic, dlg->width -48, h_text2, Sphinx->Text2, n_MedFont, 7, 5, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));
    }

    if (Sphinx->Text3) {
        dlg->AddItem(_DlgStaticText_::Create(24, 30+h_text1+h_pic, dlg->width -48, h_text3, Sphinx->Text3, n_MedFont, 7, 6, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));
    }

    if (Sphinx->Text2 && Sphinx->Text3)
    {
        dlg->GetItem(5)->x = 36;
        dlg->GetItem(5)->width = x/2 -48;

        // смещаем вправо Text3
        dlg->GetItem(6)->x = x/2 +12;
        dlg->GetItem(6)->width = x/2 -48;
    }

    _DlgTextEdit_* edit_text = 0;
    dlg->custom_data[0] = 0; // всегда (но для единичного раза установка будет 1)
    if (Sphinx->Text2) {
        // создаём кнопки id 15-18
        dlg->custom_data[0] = 1;  // один раз установить фокус на ввод текста
        int editText_y = (dlg->GetItem(5)->y) + h_text2 +11;
        int editText_x = dlg->GetItem(5)->x;
        int editText_width = dlg->GetItem(5)->width;

        // поле ввода текста
        if (editText_width > 250) {
            editText_width = 220;
            editText_x = x/2 -110;
        }
        b_YellowFrame_Create(dlg, editText_x, editText_y -1, editText_width +1, 20, 8, ON, o_Pal_Grey);
        // b_YellowFrame_Create(dlg, editText_x -2, editText_y -2, editText_width +4, 22, 8, ON, o_Pal_Y);

        dlg->AddItem(edit_text = _DlgTextEdit_::Create(editText_x+2, editText_y, editText_width-2, 18, 64, o_NullString, n_MedFont, 1, ALIGN_H_LEFT | ALIGN_V_CENTER, "WoGTextEdit.pcx", 256, 4, 0, 0));
        dlg->GetItem(256)->full_tip_text = o_NullString;
        dlg->GetItem(256)->short_tip_text = Sphinx->Text2;
    }

    if (Sphinx->Text3) {
        // создаём кнопки id 15-18 (19-22)
        int bttn_y = (dlg->GetItem(6)->y) + h_text3 +10;
        int bttn_x = dlg->GetItem(6)->x;
        int bttn_width = dlg->GetItem(6)->width;

        if (Sphinx->Chk1Text) {
            dlg->AddItem(_DlgStaticTextPcx8ed_::Create(bttn_x+1, bttn_y+1, bttn_width-2, 20, Sphinx->Chk1Text, n_SmallFont, adRollvrPcx, 1, 19, ALIGN_H_CENTER | ALIGN_V_CENTER) );
            b_YellowFrame_Create(dlg, bttn_x-1, bttn_y-1, bttn_width+2, 23, 25, OFF, o_Pal_Y);
            b_YellowFrame_Create(dlg, bttn_x, bttn_y, bttn_width, 21, 15, ON, o_Pal_Grey);
            dlg->GetItem(15)->full_tip_text = o_NullString;
            dlg->GetItem(15)->short_tip_text = Sphinx->Chk1Hint;
            bttn_y += 25;           
        }

        if (Sphinx->Chk2Text) {
            dlg->AddItem(_DlgStaticTextPcx8ed_::Create(bttn_x+1, bttn_y+1, bttn_width-2, 20, Sphinx->Chk2Text, n_SmallFont, adRollvrPcx, 1, 20, ALIGN_H_CENTER | ALIGN_V_CENTER) );
            b_YellowFrame_Create(dlg, bttn_x-1, bttn_y-1, bttn_width+2, 23, 26, OFF, o_Pal_Y);
            b_YellowFrame_Create(dlg, bttn_x, bttn_y, bttn_width, 21, 16, ON, o_Pal_Grey);
            dlg->GetItem(16)->full_tip_text = o_NullString;
            dlg->GetItem(16)->short_tip_text = Sphinx->Chk2Hint;
            bttn_y += 25;           
        }

        if (Sphinx->Chk3Text) {
            dlg->AddItem(_DlgStaticTextPcx8ed_::Create(bttn_x+1, bttn_y+1, bttn_width-2, 20, Sphinx->Chk3Text, n_SmallFont, adRollvrPcx, 1, 21, ALIGN_H_CENTER | ALIGN_V_CENTER) );
            b_YellowFrame_Create(dlg, bttn_x-1, bttn_y-1, bttn_width+2, 23, 27, OFF, o_Pal_Y);
            b_YellowFrame_Create(dlg, bttn_x, bttn_y, bttn_width, 21, 17, ON, o_Pal_Grey);
            dlg->GetItem(17)->full_tip_text = o_NullString;
            dlg->GetItem(17)->short_tip_text = Sphinx->Chk3Hint;
            bttn_y += 25;           
        }

        if (Sphinx->Chk4Text) {
            dlg->AddItem(_DlgStaticTextPcx8ed_::Create(bttn_x+1, bttn_y+1, bttn_width-2, 20, Sphinx->Chk4Text, n_SmallFont, adRollvrPcx, 1, 22, ALIGN_H_CENTER | ALIGN_V_CENTER) );
            b_YellowFrame_Create(dlg, bttn_x-1, bttn_y-1, bttn_width+2, 23, 28, OFF, o_Pal_Y);
            b_YellowFrame_Create(dlg, bttn_x, bttn_y, bttn_width, 21, 18, ON, o_Pal_Grey);
            dlg->GetItem(18)->full_tip_text = o_NullString;
            dlg->GetItem(18)->short_tip_text = Sphinx->Chk4Hint;
            bttn_y += 25;
        }
    }

    int x_center = x / 2; // вычисляем середину ширины диалога

    if ( cansel_show )
    {
        dlg->AddItem(_DlgStaticPcx8_::Create(x_center +16, dlg->height -74, 0, box64x30Pcx));
        dlg->AddItem(_DlgButton_::Create(x_center +17, dlg->height -73, 64, 30, DIID_CANCEL, iCancelDef, 0, 1, 1, 1, 2)); 
        dlg->GetItem(DIID_CANCEL)->full_tip_text = o_NullString;
        dlg->GetItem(DIID_CANCEL)->short_tip_text = txtresWOG->GetString(12);
        x_center -= 47; // для правильного смещения кнопки ОК
    } 
    dlg->AddItem(_DlgStaticPcx8_::Create(x_center -33, dlg->height -74, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(x_center -32, dlg->height -73, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 28, 2));
    dlg->GetItem(DIID_OK)->full_tip_text = o_NullString;
    dlg->GetItem(DIID_OK)->short_tip_text = txtresWOG->GetString(11);

    if ( count_bttns > 0) { 
        dlg->GetItem(DIID_OK)->SetEnabled(0); 
    }

    // (id = 115) подсказка в статус баре   
    dlg->AddItem(_DlgStaticTextPcx8ed_::Create(8, dlg->height -18 -8, dlg->width - 16, 18, o_NullString, n_SmallFont, adRollvrPcx, 1, 115, ALIGN_H_CENTER | ALIGN_V_CENTER) ); // HD_TStat.pcx

    // обнуляем переменную выбранного пункта
    Sphinx->SelItm = 0;
    dlg->Run();

    // если был создан диалог с вводом текста: возвращаем введенный текст
    if ( edit_text ) {      
        Sphinx->Text4 = edit_text->text;
    }

    // уничтожаем диалог
    dlg->Destroy(TRUE);

    if (o_WndMgr->result_dlg_item_id == DIID_CANCEL){ 
        Sphinx->SelItm = -1; 
    }
    if (o_WndMgr->result_dlg_item_id == DIID_OK) { 
        if (!Sphinx->SelItm) {
            Sphinx->SelItm = 5; 
        }
    }

    // уничтожаем картинки
    New_Dlg_CustomReq_PicDestroy(&o_Pic[0], count_pics);

    return Sphinx->SelItm;  
}

// иницаиализации диалога ввода
int __stdcall Y_Dlg_CustomReq(LoHook* h, HookContext* c)
{   
    Dlg_CustomReq_Ban++;
    if (Dlg_CustomReq_Ban == 1) 
    {
        o_PauseVideo();

        _Sphinx1_* Sphinx = (_Sphinx1_*)(c->ebp -0x260);
        o_Sphinx1 = Sphinx; // делаем глобальное сохранение адреса класса, чтобы потом при необходимости иметь прямой доступ

        int result = 0;

        if (Sphinx->Text2 && BanDlg_CustomReq_EnterText ) {         
            result = 10; // гребанные выкрутасы. Мне такие костыли очень, очень не нравятся!
        }

        if (result != 10) 
        {
            // устанавливаем стандартный курсор мыши
            Y_Mouse_SetCursor(0);

            result = New_Dlg_CustomReq(Sphinx); 

            // возвращаем запомненный курсор мыши
            Y_Mouse_SetCursor(1);
        }

        if (result == 10) 
        { // в диалоге gif/avi = грузим стандартный воговский диалог            
            CALL_0(void, __stdcall, 0x771978); //  WoG_BeforeDialog()  
            int soundEffect = CALL_0(int, __cdecl, 0x71666C) > 4;
            int v5 = CALL_0(int, __cdecl, 0x771116); // int __cdecl Service_GetForegroundWindow()
            result = CALL_4(int, __cdecl, *(int*)0x2878F1C, v5, (int)Sphinx, /* ".\\Data\\ZVS\\LIB1.RES"*/ (char*)0x7A4DC8, soundEffect);
            int v6 = CALL_0(int, __cdecl, 0x771116); // int __cdecl Service_GetForegroundWindow()
            // CALL_1(int, __cdecl, *(int*)0x7850F0, v6); // WOG_SetFocus()
            CALL_0(void, __stdcall, 0x77186A);  // WOG_AfterDialog()
        }

        o_ContinueVideo();
        Dlg_CustomReq_Ban = 0;
    } 

    c->return_address = 0x772CF2;
    return NO_EXEC_DEFAULT;
} 


// диалог вопросов Сфинкса
int __stdcall Y_WoGDlg_SphinxReq(HiHook* hook, int Num) 
{
    // if ( если окно ввода должно быть отключено (и необходима работа стандартного воговского (опция 911)) )
    if ( BanDlg_CustomReq_EnterText )
    {
        // вызов оригинальной функции
        return CALL_1(int, __cdecl, hook->GetDefaultFunc(), Num);
    } 
    else 
    {
        _Sphinx1_ Sphinx;
        Sphinx.SelItm = -1;
        Sphinx.Text1 = CALL_3(char*, __cdecl, 0x77710B, Num, 0, 0x289BFF0); 
        Sphinx.Text2 = CALL_3(char*, __cdecl, 0x77710B, 122, 0, 0x7C8E3C); 
        Sphinx.Text3 = 0;
        *(_byte_*)0x28AAB88 = 57; // Answer[0]='9';
        *(_byte_*)0x28AAB89 = 57; // Answer[1]='9';
        *(_byte_*)0x28AAB8A = 57; // Answer[2]='9';
        *(_byte_*)0x28AAB8B = 57; // Answer[3]='9';
        *(_byte_*)0x28AAB8C = 0;  // Answer[4]=0;

        // иницаиализация параметров: так делает WOG, детка.
        // тут я не стал ничего выдумывать и просто скопировал код
        Sphinx.Text4 = (char*)0x28AAB88;
        Sphinx.Pic1Path = 0;
        Sphinx.Pic2Path = 0;
        Sphinx.Pic3Path = 0;
        Sphinx.Pic4Path = 0;
        Sphinx.Pic1Hint = 0;
        Sphinx.Pic2Hint = 0;
        Sphinx.Pic3Hint = 0;
        Sphinx.Pic4Hint = 0;
        Sphinx.Chk1Text = 0;
        Sphinx.Chk2Text = 0;
        Sphinx.Chk3Text = 0;
        Sphinx.Chk4Text = 0;
        Sphinx.Chk1Hint = 0;
        Sphinx.Chk2Hint = 0;
        Sphinx.Chk3Hint = 0;
        Sphinx.Chk4Hint = 0;
        Sphinx.ShowCancel = 0;

        // пишем в глобальный указатель на структуру диалога
        o_Sphinx1 = (_Sphinx1_*)&Sphinx;

        // устанавливаем стандартный курсор мыши
        Y_Mouse_SetCursor(0);

        // запускаем создание и выполнение диалога
        New_Dlg_CustomReq(o_Sphinx1); 

        // возвращаем курсор мыши
        Y_Mouse_SetCursor(1);

        // выполняем WOG'овскиие функции перед передачей управления WOG'у
        CALL_3(void, __cdecl, 0x710B9B, 0x28AAB88, 512, Sphinx.Text4); // WoG_StrCopy(Answer, int 512, Sphinx.Text4)
        return CALL_2(int, __cdecl, 0x772DFD, 0x28AAB88, CALL_3(char*, __cdecl, 0x77710B, Num, 1, 0x289BFF0));
    }
}


// диалог посещения камней силы (повышение перв.навыков командира)
int __stdcall Y_Dlg_QuickDialog(HiHook* hook, _Sphinx1_* Sphinx)
{       
    Y_Mouse_SetCursor(0);   
    int ret = New_Dlg_CustomReq(Sphinx);
    Y_Mouse_SetCursor(1);

    // -1: Esc, 1: Ok
    return ret;
}

// описываем воговскую функцию получения CData 
int WoG_FindCData(int num) {
    return CALL_1(int, __cdecl, 0x771A13, num); 
}

// Описываем воговскую функцию разделения текста на путь и название файла
void WoG_SplitPath(char *all, char *path, char *name) {
    return CALL_3(void, __cdecl, 0x716701, all, path, name);
}


// диалог IF:B/P
int __stdcall Y_Dlg_CustomPic(HiHook* hook, int num, int startup)
{   
    int needCallOrigFunc = 0;
    int ind = WoG_FindCData(num);

    if ( ind == -1 ) 
        needCallOrigFunc = 1;
    else 
    {
        _CustomData_* CD = (_CustomData_*)(0x28809B8 +112*ind);

        WoG_SplitPath( CD->Pic[0], MyString1, MyString2 );

        _int32_ imageType = IsSupportedFormatImage(MyString2);

        if ( imageType < dlgSphinx::IMAGE_TYPE_ZVS || imageType > dlgSphinx::IMAGE_TYPE_PCX )
        {
            // преждупреждающее сообщение, что пользователь грузит дичь
            char* ermTypeTrigger = "IF:B/P";
            sprintf(MyString, dlgSphinx::IMAGE_GOTCHA, wndText::PLUGIN_AUTHOR, ermTypeTrigger, MyString2);
            b_MsgBox(MyString, 5);
        }
        else if ( imageType == dlgSphinx::IMAGE_TYPE_ZVS )
        {
            needCallOrigFunc = 1;
        } 
        else
        {
            Y_Mouse_SetCursor(0); 

            sprintf(MyString1, ".\\Maps\\%s", CD->Pic[0]);
            // MyString1 = it's path to file
            // MyString2 = it's file name 

            _Pcx16_* o_Pic; 

            if ( imageType == dlgSphinx::IMAGE_TYPE_ERA )
                o_Pic = (_Pcx16_*)Era::LoadImageAsPcx16(MyString1, MyString2, 0, 0, 750, 510, /* RESIZE_ALG_DOWNSCALE */ 3); 
            // *.pcx16
            if ( imageType == dlgSphinx::IMAGE_TYPE_PCX16 )
            {
                std::string pcxName;
                pcxName.append(MyString2);
                int pcxNameLength = pcxName.length() -2;
                pcxName.erase(pcxNameLength);
                o_Pic = o_LoadPcx16(pcxName.c_str());
            }
            // *.pcx (8 bit)
            if ( imageType == dlgSphinx::IMAGE_TYPE_PCX )
                o_Pic = (_Pcx16_*)o_LoadPcx8(MyString2); 


            int picWi = o_Pic->width;
            int picHi = o_Pic->height;

            if (picWi < 64) { picWi = 64; }
            if (picHi < 32) { picHi = 32; }

            int x = picWi +30; // 750+30 = 780 max
            int y = picHi +70; // 510+70 = 580 max

            _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
            Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

            if ( imageType == dlgSphinx::IMAGE_TYPE_PCX ) 
                dlg->AddItem(_DlgStaticPcx8_::Create(15, 16, picWi, picHi, 1, o_Pic->name));
            else 
                dlg->AddItem(_DlgStaticPcx16_::Create(15, 16, picWi, picHi, 1, o_Pic->name, 2048));

            dlg->AddItem(_DlgStaticPcx8_::Create((x >> 1) -33, y -50, 2, box64x30Pcx)); 
            dlg->AddItem(_DlgButton_::Create((x >> 1) -32, y -49, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 28, 2)); 

            dlg->Run();
            dlg->Destroy(TRUE);

            // удаляем загруженную картинку
            if ( o_Pic )
                o_Pic->DerefOrDestruct();

            Y_Mouse_SetCursor(1);
        } 
    }

    if ( needCallOrigFunc )
        return CALL_2(int, __cdecl, hook->GetDefaultFunc(), num, startup);
    else return 1;
}

 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// установка перехватов в код игры
void Dlg_Sphinx(PatcherInstance* _PI)
{
    // Диалог IF:D/E
    _PI->WriteCodePatch(0x772A6C, "%n", 5); // call    WoG_BeforeDialog()
    _PI->WriteCodePatch(0x772D39, "%n", 5); // call    WOG_AfterDialog()        
    _PI->WriteLoHook(0x772CBD, Y_Dlg_CustomReq);
    // проверяем параметр (будем ли показывать диалог ввода текста в диалоге IF:D/E)

    // диалог посещения камней силы командиров (убираем текст "Ваш командир")
    _PI->WriteDword(0x770916 +1, 256);
    // диалог посещения камней силы командиров (убираем "\\Data\\ZVS\\LIB1.RES\\NPC#.GIF")
    _PI->WriteByte(0x770990 +2, 0x2C);
    _PI->WriteByte(0x7709C7 +2, 0x2C);
    _PI->WriteByte(0x7709FB +2, 0x2C);
    _PI->WriteByte(0x770A2F +2, 0x2C);

    // диалог посещения камней силы командиров
    _PI->WriteHiHook(0x772D50, SPLICE_, EXTENDED_, CDECL_, Y_Dlg_QuickDialog);

    // диалог IF:B/P
    _PI->WriteHiHook(0x7732B4, SPLICE_, EXTENDED_, CDECL_, Y_Dlg_CustomPic);

    // диалог сфинкса
    _PI->WriteHiHook(0x772E48, SPLICE_, EXTENDED_, CDECL_, Y_WoGDlg_SphinxReq);
}

// ** end
