////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог перестройки замков ////////////////////////////////////////////////////////

int townID;
#define itpaDef (*(char**)0x57C792) // "itpa.def" (small)
#define itptDef (*(char**)0x45204E) // "itpt.def" (large)

int __stdcall Dlg_ChooseCastleReBuild_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
	if (msg->type == MT_MOUSEBUTTON) {
		if (msg->subtype == MST_RBUTTONDOWN){ // при ПКМ
			if (msg->item_id >= 10 && msg->item_id <= 18){
				_Dlg_ dlgTwn;
				_Def_ *def = o_LoadDef(0x65F318); 
				CALL_2(_CustomDlg_*, __thiscall, 0x5761A0, &dlgTwn, 1);
				int town = msg->item_id -10;
				CALL_4(_bool8_, __thiscall, 0x5761C0, &dlgTwn, def, 2 * town + 2, town);
				dlgTwn.RMC_Show(); 
				CALL_1(void, __thiscall, 0x48FA10, &dlgTwn);				
			}
		}

		if (msg->subtype == MST_LBUTTONDOWN){ // при нажатии ЛКМ
			if (msg->item_id >= 10 && msg->item_id <= 18){

				for (int i = 0; i < 9; i++) {
					dlg->GetItem(20+i)->SendCommand(6, 4);
				}				
				dlg->GetItem(msg->item_id +10)->SendCommand(5, 4);
				dlg->GetItem(DIID_OK)->SetEnabled(1);
				dlg->custom_data[0] = msg->item_id -10;
				dlg->Redraw(TRUE);
			}
		}

		if (msg->subtype == MST_LBUTTONCLICK) // ЛКМ при отжатии
		{
			if (msg->item_id == DIID_CANCEL)	{
				dlg->custom_data[0] = townID;
				return dlg->Close(msg);
			}
		}
	}
	return r;
}

int __stdcall Dlg_ChooseCastleReBuild(HiHook* hook)
{
	int dx = 64;
	int dy = 75;
    int x = 36 +dx*5;  
    int y = 312;  
    int zx = x/2;
    int zy = 55; 

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Dlg_ChooseCastleReBuild_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	dlg->AddItem(_DlgStaticText_::Create(20, 20, x-40, 60, txtresWOG->GetString(22) , n_BigFont, 7, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	for (int i = 0; i < 5; i++){
		          b_YellowFrame_Create(dlg, 18 +dx*i, 88, 62, 68, 30+i, ON, o_Pal_Black);
				  b_YellowFrame_Create(dlg, 17 +dx*i, 87, 64, 70, 20+i, OFF, o_Pal_Y);
		dlg->AddItem(_DlgStaticDef_::Create(20 +dx*i, 90, 58, 64, 10+i, itptDef, i*2, 0, 0x12)); 	
	}

	for (int i = 5; i < 9; i++){
		          b_YellowFrame_Create(dlg, 50 +dx*(i%5), 88+dy, 62, 68, 30+i, ON, o_Pal_Black);
				  b_YellowFrame_Create(dlg, 49 +dx*(i%5), 87+dy, 64, 70, 20+i, OFF, o_Pal_Y);
		dlg->AddItem(_DlgStaticDef_::Create(52 +dx*(i%5), 90+dy, 58, 64, 10+i, itptDef, i*2, 0, 0x12)); 	
	}

    dlg->AddItem(_DlgStaticPcx8_::Create(zx -71, dlg->height -zy, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(zx -70, dlg->height -zy +1, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 28, 2));
    dlg->GetItem(DIID_OK)->SetEnabled(0); 

    dlg->AddItem(_DlgStaticPcx8_::Create(zx+5, dlg->height -zy, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(zx+6, dlg->height -zy +1, 64, 30, DIID_CANCEL, iCancelDef, 0, 1, 0, 1, 2));

	dlg->Run();
	int ret = dlg->custom_data[0];
	dlg->Destroy(TRUE);

	return ret; 
}

int __stdcall Dlg_ChooseCastleReBuild_SetCansellBttn(LoHook* h, HookContext* c)
{
	townID = DwordAt(c->ebp -0x68);

	if (townID < 0 || townID > 10)
		townID = 0;

	return EXEC_DEFAULT;
} 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_CastleReBuild(PatcherInstance* _PI)
{
	// Диалог перестройки замка
	// _PI->WriteHiHook(0x70B10B, CALL_, EXTENDED_, THISCALL_, Dlg_ChooseCastleReBuild); 
	_PI->WriteLoHook(0x70B10B, Dlg_ChooseCastleReBuild_SetCansellBttn);
	_PI->WriteHiHook(0x771914, SPLICE_, EXTENDED_, THISCALL_, Dlg_ChooseCastleReBuild); 


	// TEST
	// _PI->WriteHexPatch(0x70B0F7, "EB 12"); //всегда диалог перестройки
}
