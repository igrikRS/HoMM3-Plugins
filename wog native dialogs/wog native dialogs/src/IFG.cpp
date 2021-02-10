////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////// Диалог IF:G //////////////////////////////////////////////////////////////////////////////////////// 

void checkBoxes_ChangeButtonFrame(_CustomDlg_* dlg, int itemId, int frame, int pressedFrame)
{
   _DlgButton_* button = (_DlgButton_*)dlg->GetItem(itemId); 

   if (button) {
        button->def_frame_index = frame;
        button->press_def_frame_index = pressedFrame;
   }
}


int __stdcall New_Dlg_IF_G_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    if (msg->subtype == MST_LBUTTONCLICK)
    { 
        if (msg->item_id >= 34 && msg->item_id < 46)
        {
            _CheckBoxes_* checkBoxes = (_CheckBoxes_*)dlg->custom_data[0];

            if ( checkBoxes->Type )	// RadioButtons
            { 
                for (int i = 0; i < checkBoxes->Count; i++)	
                {
                    checkBoxes_ChangeButtonFrame(dlg, 34+i, 0, 1);
                    checkBoxes->State[i] = false;  				
                }
                // изменяем деф выбранного элемента
                checkBoxes_ChangeButtonFrame(dlg, msg->item_id, 2, 3);
                checkBoxes->State[msg->item_id -34] = true;  

            }
            else // CheckBoxes
            {	
                // получаем индекс кадра дефа
                int frame = ((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index;
                // "инвертируем" кадр
                frame = frame ? 0 : 2;
                // устанавливаем "инвертированный" кадр дефа
                checkBoxes_ChangeButtonFrame(dlg, msg->item_id, frame, frame + 1);
                // устанавливаем состояние кнопки
                checkBoxes->State[msg->item_id -34] = frame ? 1 : 0;  
            }	

            dlg->Redraw(TRUE); 
        }
    }

    return r;
}

int New_Dlg_IF_G(_CheckBoxes_* checkBoxes)
{
    // останавливаем видео (если оно есть)
    o_PauseVideo();

    // запоминаем параметры курсора мыши
    // и устанавливаем стандартные параметры
    Y_Mouse_SetCursor(0);

    // размеры диалога
    int x = 435;
    int dy = 27;
    int y = 134 + dy * checkBoxes->Count; 

    // создаём диалог
    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_IF_G_Proc);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

    // кнопка ok и её обводка
    dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, 1, iOkayDef, 0, 1, 1, 28, 2));

    // Титульный текст
    dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 56, checkBoxes->Caption, n_MedFont, 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // Создаём и заполняем строки[12]
    for (int i = 0; i < checkBoxes->Count; i++)
    {
        // желтая обводка
        b_YellowFrame_Create(dlg, 18, 76+i*dy, dlg->width -36, 24, 22+i, ON, o_Pal_Y);		

        // текст
        dlg->AddItem(_DlgStaticText_::Create(44, 80+i*dy, dlg->width -66, 16, checkBoxes->Text[i], n_SmallFont, 7, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

        // деф кнопки (чекбоксы или радиобаттоны)
        dlg->AddItem(_DlgButton_::Create(22, 80+i*dy, dlg->width -44, 18, 34+i, 
        checkBoxes->Type ? radioBttnDef : checkboxDef, // название дефа
        checkBoxes->State[i] ? 2 : 0, // кадр дефа
        checkBoxes->State[i] ? 3 : 1, // кадр нажатого дефа
        0, 0, 0));
    } 

    // передаём структуру checkBoxes
    dlg->custom_data[0] = (_dword_)checkBoxes;

    // выполняем диалог
    dlg->Run();

    // уничтожаем диалог
    dlg->Destroy(TRUE);

    // возвращаем запомненные параметры курсора мыши
    Y_Mouse_SetCursor(1);

    // продолжаем видео (если было)
    o_ContinueVideo();

    return 1;
}


_int_ __stdcall New_Dlg_IF_G_Lo(LoHook* h, HookContext* c)
{	    
    _CheckBoxes_ checkBoxes = *(_CheckBoxes_*)(c->ebp -0xB0);

    if( checkBoxes.Count > 0 && checkBoxes.Count <= 12 )
        New_Dlg_IF_G(&checkBoxes);

    c->return_address = 0x7731C2; 
    return NO_EXEC_DEFAULT; 
}  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_IFG(PatcherInstance* _PI)
{
    // диалог IF:G
    _PI->WriteLoHook(0x77318D, New_Dlg_IF_G_Lo);		
    _PI->WriteCodePatch(0x77300C, "%n", 5);
    _PI->WriteCodePatch(0x77324B, "%n", 5);
}
