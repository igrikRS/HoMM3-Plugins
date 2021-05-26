////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог проклятий и благословлений ////////////////////////////////////////////////

#define curse_C(i) (*(_int_*)(0x2807628 -4 +4*i) )
#define curse_H(i) (*(_int_*)(0x27FE680 -4 +4*i) )
#define curse_T(i) (*(_int_*)(0x2807128 -4 +4*i) )

int __stdcall Y_WoGCurseDlg_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    if (msg->type == MT_MOUSEOVER)  {
        _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
        char* text_bar = o_NullString;
        if (it) 
        {
            if (it->id > 10 && it->id < 76) {
                int t = it->id -10;
                sprintf(o_TextBuffer, "{%s}\n(%s)", curse_H(t), curse_T(t) );
                text_bar = o_TextBuffer;
            }
            else if ( it->id == DIID_OK ) {
                text_bar = txtresWOG->GetString(85);
            } 
            else text_bar = o_NullString;               

            ((_DlgTextEdit_*)dlg->GetItem(2))->SetText(text_bar);
            ((_DlgTextEdit_*)dlg->GetItem(2))->RedrawScreen();
            dlg->Redraw();
        } 
    }   

    return r;
}

int __stdcall Y_WoGCurseDlg_Show(LoHook* h, HookContext* c)
{
    int countCurse = 0;
    for (int i=1; i<66; i++) {
        if ( curse_C(i) > 0 ) { 
            countCurse++;
        } else break;   
    }

    int d = 45;    int x = 620; 
    int y = 92 + d*((countCurse+12) / 13); 

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Y_WoGCurseDlg_Proc);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

    dlg->AddItem(_DlgStaticText_::Create(8, y -62, x -102, 48, o_NullString, n_SmallFont, 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, y-64, x-36, 1, 3, *(int*)(*(int*)0x6AAD18 + 118), 1024));
    dlg->GetItem(3)->SendCommand(5, 4);

    int i = 1;
    for (int j = 0; j < 6; j++) {
        for (int k = 0; k < 13; k++) {          

            dlg->AddItem(_DlgStaticDef_::Create(18 +d*k, 18 +d*j, 44, 44, 10+i, "WoGCurse.def", curse_C(i), 0, 0x12));

            i++;
            if (i > countCurse) break; 
        }
        if (i > countCurse) break; 
    }

    dlg->AddItem(_DlgStaticPcx8_::Create(x-101, y-54, 0, box64x30Pcx));
    // dlg->AddItem(_DlgButton_::Create(x-100, y-53, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));
    _DlgButton_* bttnOK;
    bttnOK = b_DlgButton_Create(x-100, y-53, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2);
    // и сразу делаем выход и на ESC и ENTER
    int hotkey = 1;     CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
        hotkey = 28;    CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
    dlg->AddItem(bttnOK);

    dlg->Run();

    dlg->Destroy(TRUE);

    c->return_address = 0x754E71;
    return NO_EXEC_DEFAULT;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_Curse(PatcherInstance* _PI)
{
    // Диалог проклятий и благословлений
    _PI->WriteByte(0x754F6A +1, 0x14);
    _PI->WriteLoHook(0x754E64, Y_WoGCurseDlg_Show); 
}
