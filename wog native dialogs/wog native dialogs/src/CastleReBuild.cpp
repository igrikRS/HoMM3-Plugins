////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог перестройки замков ////////////////////////////////////////////////////////

int __stdcall Dlg_ChooseCastleReBuild_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
	if (msg->type == MT_MOUSEBUTTON) {
		if (msg->subtype == MST_RBUTTONDOWN){ // при ПКМ
			if (msg->item_id >= 10 && msg->item_id <= 18){
				_Dlg_ dlgTwn;
				_Def_ *def = o_LoadDef(0x65F318); // "itpa.def" from 0x57C791
				CALL_2(_CustomDlg_*, __thiscall, 0x5761A0, &dlgTwn, 1);
				int town = msg->item_id -10;
				CALL_4(_bool8_, __thiscall, 0x5761C0, &dlgTwn, def, 2 * town + 2, town);
				dlgTwn.RMC_Show(); 
				CALL_1(void, __thiscall, 0x48FA10, &dlgTwn);				
				// dlgTwn.Destroy(TRUE);
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
	}
	return r;
}

int __stdcall Dlg_ChooseCastleReBuild(HiHook* hook, int a1, int a2)
{
	int dx = 64;
	int dy = 72;
    int x = 36 +dx*3;  
    int y = 356;  
    int zx = x/2 -33;
    int zy = 55; 

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Dlg_ChooseCastleReBuild_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	dlg->AddItem(_DlgStaticText_::Create(20, 20, x-40, 60, txtresWOG->GetString(22) , n_medfont2, 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	for (int i = 0; i < 9; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18 +dx*(i%3), 78 +dy*(i/3), 62, 68, 20+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(20+i)->SendCommand(6, 4);

		dlg->AddItem(_DlgStaticDef_::Create(20 +dx*(i%3), 80 +dy*(i/3), 58, 64, 10+i, "itpt.def", i*2, 0, 0x12)); 	
	}

    dlg->AddItem(_DlgStaticPcx8_::Create(zx, dlg->height -zy, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(zx +1, dlg->height -zy +1, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));
    dlg->GetItem(DIID_OK)->SetEnabled(0); 

    // dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width - zx -64, dlg->height -zy, 0, "Box64x30.pcx"));
    // dlg->AddItem(_DlgButton_::Create(dlg->width - zx -64 +1, dlg->height -zy +1, 64, 30, DIID_CANCEL, "iCancel.def", 0, 1, 1, 1, 2));

	// dlg->custom_data[0] = 9; // чтобы потом получить -1 (в случае нажатия "отмена") 
	// БАГ ВОГа: А "отмена" оказывается не работает
	dlg->Run();
	int ret = dlg->custom_data[0] /* -10*/ ;
	dlg->Destroy(TRUE);

	return ret; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_CastleReBuild(PatcherInstance* _PI)
{
	// Диалог перестройки замка
	_PI->WriteHiHook(0x70B10B, CALL_, EXTENDED_, THISCALL_, Dlg_ChooseCastleReBuild); 
}
