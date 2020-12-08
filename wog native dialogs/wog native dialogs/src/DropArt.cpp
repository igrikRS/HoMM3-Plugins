////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог сброса артефактов на землю ////////////////////////////////////////////////

int __stdcall Y_WoGDlg_ChooseArt_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);

	_Hero_* hero = (_Hero_*)dlg->custom_data[1];

	if (msg->type == MT_MOUSEOVER)	{
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
		char* text_bar = o_NullString;
		if (it)	
		{
			if (it->id >= 10 && it->id < 74) {
				if (hero->backpack_art[it->id - 10].id != 1) {
					sprintf(o_TextBuffer, textProcS, o_ArtInfo[hero->backpack_art[it->id - 10].id].name);
				} else {
					sprintf(o_TextBuffer, "%s: %s", o_ArtInfo[hero->backpack_art[it->id - 10].id].name, o_Spells[hero->backpack_art[it->id - 10].mod].name);
				}
				text_bar = o_TextBuffer;
			}
			else if ( it->id == DIID_CANCEL ) {
				sprintf(o_TextBuffer, textProcS, txtresWOG->GetString(12));
				text_bar = o_TextBuffer;
			}
			else if ( it->id == DIID_OK ) {
				sprintf(o_TextBuffer, textProcS, txtresWOG->GetString(11));
				text_bar = o_TextBuffer;
			} 
			else text_bar = o_NullString;				

			((_DlgTextEdit_*)dlg->GetItem(2))->SetText(text_bar);
			((_DlgTextEdit_*)dlg->GetItem(2))->RedrawScreen();
			dlg->Redraw();
		} 
	}

	if (msg->type == MT_MOUSEBUTTON){
		if ( (msg->item_id >= 10) && (msg->item_id < 74) ) {
			if (msg->subtype == MST_LBUTTONDOWN){
				for (int i = 0; i < hero->backpack_arts_count; i++) {
					dlg->GetItem(80+i)->SendCommand(6, 4);
				}	dlg->GetItem(msg->item_id +70)->SendCommand(5, 4);

				dlg->GetItem(DIID_OK)->SetEnabled(1);
				dlg->Redraw(TRUE);	
				dlg->custom_data[0] = msg->item_id -10;
			
			} // MST_LBUTTONDOWN
			else if (msg->subtype == MST_RBUTTONDOWN){
				if (hero->backpack_art[msg->item_id - 10].id != 1) {
					sprintf(o_TextBuffer, textProcS, o_ArtInfo[hero->backpack_art[msg->item_id - 10].id].description);
					b_MsgBoxD(o_TextBuffer, MBX_RMC, 8, hero->backpack_art[msg->item_id - 10].id);
				} else {
					_HStr_ textMes;
					b_MsgBoxD(CALL_2(_HStr_*, __thiscall, 0x4DB650, &(hero->backpack_art[msg->item_id - 10]), &textMes)->c_str, MBX_RMC, 9, hero->backpack_art[msg->item_id - 10].mod);
				}
			} // MST_RBUTTONDOWN
		}
	} // MT_MOUSEBUTTON

	return r;
}

int __stdcall Y_WoGDlg_ChooseArt(HiHook* hook, int a1, _Hero_* hero, int Remove)
{	
	int countArts = hero->backpack_arts_count;
	if (!countArts) { // если артефактов в рюкзаке нет
		sprintf(o_TextBuffer, textProcS, CALL_3(char*, __cdecl, 0x77710B, 223, 0, (int)0x7C8E3C) );
		b_MsgBox(o_TextBuffer, 1);
		return -1;	
	} 

	// сжимаем порядок артефактов убираю все пустые слоты
	int tmp_art = 0; 
	int tmp_mod = 0; 
	for(int i = 0; i<64; i++){ 
		for(int j = 63; j>=(i+1); j--){ 
			if(hero->backpack_art[j-1].id == -1 ){ 
				tmp_art = hero->backpack_art[j].id;  hero->backpack_art[j].id  = hero->backpack_art[j-1].id;  hero->backpack_art[j-1].id  = tmp_art; 
				tmp_mod = hero->backpack_art[j].mod; hero->backpack_art[j].mod = hero->backpack_art[j-1].mod; hero->backpack_art[j-1].mod = tmp_mod; 
			} 
		} 
	} 

	int d = 47;
    int x = 412; 
	int y = 525;
	int xz = x/2;

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Y_WoGDlg_ChooseArt_Proc);
	Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());

	dlg->custom_data[1] = (_dword_)hero;
	
	sprintf(o_TextBuffer, textProcS, CALL_3(char*, __cdecl, 0x77710B, 222, 0, (int)0x7C8E3C) );
	dlg->AddItem(_DlgStaticText_::Create(24, 20, dlg->width -48, 44, o_TextBuffer, n_MedFont, 1, 1, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	dlg->AddItem(_DlgStaticText_::Create(8, dlg->height -26, dlg->width -16, 18, o_NullString, n_SmallFont, 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	int i = 0;
	for (int j = 0; j < 8; j++) {
		for (int k = 0; k < 8; k++) {			
			dlg->AddItem(_DlgStaticPcx8_::Create(18 +d*k, 69 +d*j, 47, 47, 3, "artslot.pcx"));

			dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18 +d*k, 69 +d*j, 46, 46, 80+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
			dlg->GetItem(80+i)->SendCommand(6, 4);

			if (i < countArts) {
				if (hero->backpack_art[i].id == 1 && o_HD_X >= 808 && o_HD_Y >= 608) {
					dlg->AddItem(_DlgStaticDef_::Create(19 +d*k, 70 +d*j, 44, 44, 10+i, "def.SpScrArt", hero->backpack_art[i].mod, 0, 0x12));
				} else {				
					dlg->AddItem(_DlgStaticDef_::Create(19 +d*k, 70 +d*j, 44, 44, 10+i, artifactDef, hero->backpack_art[i].id, 0, 0x12));
				}
			}

			i++;
			// if (i >= countArts) break; 
		}
		// if (i >= countArts) break; 
	}

	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);


    dlg->AddItem(_DlgStaticPcx8_::Create(xz -80, dlg->height -74, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(xz -79, dlg->height -73, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 28, 2));
    dlg->GetItem(DIID_OK)->SetEnabled(0); 

    dlg->AddItem(_DlgStaticPcx8_::Create(xz +16, dlg->height -74, 0, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(xz +17, dlg->height -73, 64, 30, DIID_CANCEL, iCancelDef, 0, 1, 1, 1, 2)); 


	int ret = -1; 
	dlg->Run();
	ret = dlg->custom_data[0];
	dlg->Destroy(TRUE);

	b_MouseMgr_SetCursor(cursor_f, cursor_t);

	if (ret < 0) {
		return -1;
	}

	if (o_WndMgr->result_dlg_item_id == DIID_CANCEL){
		return -1;
	}
	if (o_WndMgr->result_dlg_item_id == DIID_OK){
		int indexArt = ret;
		int idArt = hero->backpack_art[indexArt].id;
		if (idArt != 1) {
			ret = hero->backpack_art[indexArt].id;
		} else {
			ret = hero->backpack_art[indexArt].mod +1000;
		}	hero->RemoveBackpackArtifact(indexArt);
	}

	return ret;
}

// делаем диалог сброса и оставление монстров по CRTL+ПКМ 
int __stdcall WoG_PlaceCreature_RMC_Crtl(LoHook* h, HookContext* c)
{
    if ( GetKeyState(VK_CTRL) < 0 )
	    return EXEC_DEFAULT;
    else
    {
        c->return_address = 0x7575A3;
        return NO_EXEC_DEFAULT;
    }
} 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_DropArt(PatcherInstance* _PI)
{
	// диалог сброса артефактов на землю
	_PI->WriteHiHook(0x7548BC, SPLICE_, EXTENDED_, THISCALL_, Y_WoGDlg_ChooseArt);

    // делаем диалог сброса и оставление монстров по CRTL+ПКМ    
    _PI->WriteLoHook(0x7570EB, WoG_PlaceCreature_RMC_Crtl);	
}
