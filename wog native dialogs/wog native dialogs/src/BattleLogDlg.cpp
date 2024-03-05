////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// Расширенный лог битвы //////////////////////////////////////////////////////

int __stdcall NewBattleLogDlg_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    if ( dlg->custom_data[0] == 1 )
    {
        // получаем структуру скролл_бара текста
        _DlgScroll_* scroll = (_DlgScroll_*)dlg->custom_data[1];        
        // перерисовываем скролл_текст через скролл_бар
        CALL_1(void, __thiscall , 0x5BA350, scroll);
        // обнуляем переменную (обновление единожды)
        dlg->custom_data[0] = 0;
    }

    return r;
}

void CreateNewBattleLogDlg(_BattleMgr_* bm)
{
    // звку клика
    e_ClickSound();
    // запоминаем параметры курсора мыши
    Y_Mouse_SetCursor(0);

    // размеры диалога
    int x = 640;
    int y = 464; 

    // создаем диалог и задник диалога
    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NewBattleLogDlg_Proc);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

    // создаём подкладку
    dlg->AddItem(_DlgStaticPcx8_::Create(32, 32, 1, "BattleLog.pcx" ));

    // создаём рамочки
    b_YellowFrame_Create(dlg, 33, 33, x-65, y-111, 2, ON, o_Pal_Black);
    b_YellowFrame_Create(dlg, 32, 32, x-65, y-111, 3, ON, o_Pal_Grey);

    // получаем кол-во строк в векторе текстов лога битвы
    int field_54 = (int)&bm->dlg->field_54;
    int logStart = *(int*)(field_54 +4);
    int logEnd = *(int*)(field_54 +8);
    int lines = (logEnd - logStart) >> 2; 

    if (lines)
    {
        std::string text;

        // получаем начало вектора текстов лога битвы
        int logData = (int)bm->dlg->field_58;

        for(int i=0; i<lines; i++)
        {
            // собираем все раздельные записи в строку string
            sprintf(MyString, "%s \n", (*(_HStr_**)(logData +(4*i)))->c_str );
            text.append(MyString);
        }   

        // создаём скролл_текст
        _DlgScrollableText_* scrollText;
        dlg->AddItem(scrollText = _DlgScrollableText_::Create((char*)text.c_str(), 40, 38, x-78, y-120, n_MedFont, 1, 0));

        // подготавливаем данные к прокрутке скролл_текста
        scrollText->scroll_bar->SetValue(scrollText->scroll_bar->ticks_count -1);
        scrollText->scroll_bar->SetEnabled(scrollText->scroll_bar->ticks_count > 1);
        dlg->custom_data[0] = 1;
        dlg->custom_data[1] = (int)scrollText->scroll_bar;
    }

    // создаём кнопку ОК
    dlg->AddItem(_DlgStaticPcx8_::Create((x >> 1) -33, y -62, 2, box64x30Pcx)); 
    _DlgButton_* bttnExit = _DlgButton_::Create((x >> 1) -32, y -61, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2); 
    bttnExit->SetHotKey(1);   
    bttnExit->SetHotKey(28);
    dlg->AddItem(bttnExit);

    // запускаем диалог
    dlg->Run();
    // уничтожаем диалог
    dlg->Destroy(TRUE);
    // восстанавливаем  параметры курсора мыши
    Y_Mouse_SetCursor(1);
}


int __stdcall BattleLog_Proc(HiHook* hook, _BattleMgr_* bm, _EventMsg_*msg)
{
    if (!bm->isTactics)
    {
      if (msg->type == MT_MOUSEBUTTON)
          if (msg->subtype == MST_LBUTTONCLICK)
              if ( msg->item_id == 2005 ) 
                  CreateNewBattleLogDlg(bm);

      if (msg->type == MT_KEYDOWN)
          if ( msg->subtype == HK_H )
              CreateNewBattleLogDlg(bm);
    }

    return CALL_2(int, __thiscall, hook->GetDefaultFunc(), bm, msg);
}


void __stdcall BattleLog_ReportNewRound(HiHook* hook, _dword_ battleDlg, const char* textRound, int a3, char a4)
{
    // o_BattleMgr->round
    _int_ round = Era::v[997];
    if (round > 0)
    {
        sprintf(myString3, "%s {(%d)}", textRound, round);
        CALL_4(void, __thiscall, hook->GetDefaultFunc(), battleDlg, myString3, a3, a4);
    }    
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DlgBattleLog(PatcherInstance* _PI)
{
    // диалог статуса действий и событий в битве
    _PI->WriteHiHook(0x473A00, SPLICE_, EXTENDED_, THISCALL_, BattleLog_Proc);

    // отображение номера раунда в битве
    _PI->WriteHiHook(0x475B19, CALL_, EXTENDED_, THISCALL_, BattleLog_ReportNewRound);
}
