/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог выбора атаки монстром //////////////////////////////////////////

int __stdcall Dlg_ChooseMonAttack_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
		if (msg->subtype == MST_LBUTTONCLICK){ 
			if (msg->item_id >= 10 && msg->item_id < 12){
				*(int*)0x282E700 = msg->item_id - 10;
				return dlg->Close(msg);
			}
		}
	return r;
}

int __stdcall Dlg_ChooseMonAttack(HiHook* hook, int a1, int a2, int a3)
{
	int count = *(int*)0x282E6F8;

	int x = 300;	int dy = 27;	int y = 83 +dy*count;

	// делаем отображение в правом нижнем углу битвы
	int dlg_x = (o_HD_X >> 1) +385 -x;
	int dlg_y = (o_HD_Y >> 1) +230 -y;

	_CustomDlg_* dlg = _CustomDlg_::Create(dlg_x, dlg_y, x, y, DF_SCREENSHOT | DF_SHADOW, Dlg_ChooseMonAttack_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// Титульный текст
	sprintf(o_TextBuffer, "%s", *(int*)0x282E6F4);
	dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 46, o_TextBuffer, n_MedFont, 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	for (int i = 0; i < count; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 66+dy*i, dlg->width -36, 24, 4+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(4+i)->SendCommand(5, 4);
			
		sprintf(o_TextBuffer, "{%s}", *(int*)(*(int*)0x282E6FC +4*i));
		dlg->AddItem(_DlgStaticText_::Create(44, 70+dy*i, dlg->width -66, 16, o_TextBuffer, n_SmallFont, 1, 7+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

		dlg->AddItem(_DlgButton_::Create(22, 70+dy*i, dlg->width -44, 18, 10+i, "radiobttn.def", 0, 3, 0, 0, 0));
	}
	
	dlg->Run();
	dlg->Destroy(TRUE);
	b_MouseMgr_SetCursor(cursor_f, cursor_t);

	int ret = *(int*)0x282E700;
	return ret; 
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_ChooseAttack(PatcherInstance* _PI)
{
	// диалог выбора типа атаки монстром
	_PI->WriteHiHook(0x7737BE, SPLICE_, EXTENDED_, THISCALL_, Dlg_ChooseMonAttack); 

}
