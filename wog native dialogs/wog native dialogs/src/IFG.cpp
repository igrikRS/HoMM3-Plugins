////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////// Диалог IF:G //////////////////////////////////////////////////////////////////////////////////////// 

int __stdcall New_Dlg_IF_G_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
		if (msg->subtype == MST_LBUTTONCLICK){ 
			if (msg->item_id >= 34 && msg->item_id < 46){
				if (dlg->field_48 == 1)	{ 
					for (int i = 0; i < dlg->field_4C; i++)	{
						((_DlgButton_*)dlg->GetItem(34+i))->def_frame_index = 0;
						((_DlgButton_*)dlg->GetItem(34+i))->press_def_frame_index = 1;
						*(int*)( dlg->field_50 +4*i) = 0;
						
					}
					((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index = 2;
					((_DlgButton_*)dlg->GetItem(msg->item_id))->press_def_frame_index = 3;
					*(int*)( dlg->field_50 +4*(msg->item_id -34) ) = 1;
				}
				else {	
					int frame = ((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index;
					if (frame == 0) 
						frame = 2;
					else frame = 0; 
					((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index = frame;
					((_DlgButton_*)dlg->GetItem(msg->item_id))->press_def_frame_index = frame+1;
					*(int*)( dlg->field_50 +4*(msg->item_id -34) ) = frame ? 1 : 0;
				}			
			dlg->Redraw(TRUE); 
			}
		}
	return r;
}

int New_Dlg_IF_G(int num_str, int htxt, int state, int txt, int chRAD)
{
	o_PauseVideo();

	if (num_str > 12) 
		num_str = 12; 

	int x = 435;
	int dy = 27;
	int y = 134 + dy *num_str; 
	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_IF_G_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	int save_WOG_DisableMouse = WOG_DisableMouse;
	WOG_DisableMouse = 0; // отключаем блок изменения кадров курсора
	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// кнопка ok и её обводка
	dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, box64x30Pcx));
	dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, 1, iOkayDef, 0, 1, 1, 28, 2));

	// Титульный текст
	sprintf(o_TextBuffer, textProcS, htxt);
	dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 56, o_TextBuffer, n_MedFont, 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	int on_bttn = 0;
	for (int i = 0; i < num_str; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 76+i*dy, dlg->width -36, 24, 22+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(22+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "{%s}", *(int*)(txt +4*i));
		dlg->AddItem(_DlgStaticText_::Create(44, 80+i*dy, dlg->width -66, 16, o_TextBuffer, n_SmallFont, 1, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
		on_bttn = *(int*)(state +4*i);
		dlg->AddItem(_DlgButton_::Create(22, 80+i*dy, dlg->width -44, 18, 34+i, chRAD ? radioBttnDef : checkboxDef, on_bttn ? 2 : 0, on_bttn ? 3 : 1, 0, 0, 0));
	} 

	dlg->field_48 = chRAD;
	dlg->field_4C = num_str;
	dlg->field_50 = state;

	dlg->Run();
	dlg->Destroy(TRUE);

	// восстанавливаем кадр мыши
	b_MouseMgr_SetCursor(cursor_f, cursor_t);
	WOG_DisableMouse = save_WOG_DisableMouse;

	o_ContinueVideo();

	return 1;
}


_int_ __stdcall New_Dlg_IF_G_Lo(LoHook* h, HookContext* c)
{
	int chRAD = *(int*)(c->ebp + 0x14);
	int txt = *(int*)(c->ebp - 0xA8);
	int state = *(int*)(c->ebp - 0xA4);
	int htxt = *(int*)(c->ebp - 0xB0);
	int num_str = *(int*)(c->ebp - 0xAC);

	New_Dlg_IF_G(num_str, htxt, state, txt, chRAD);

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
