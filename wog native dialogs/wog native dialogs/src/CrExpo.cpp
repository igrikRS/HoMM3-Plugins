////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// константы диалога экспы монстров /////////////////////////////////////////////////

#define o_Dlg_ExpaMon 0x845880

#define o_DlgLoop (*(_int_*)0x847F00)
#define o_DlgUpgr (*(_int_*)0x840E18)
#define o_DlgTrig (*(_int_*)0x845AE8)

#define o_Mon	 (*(_int_*)0x84730C)
#define o_Num	 (*(_int_*)0x847D84)
#define o_Expo	 (*(_int_*)0x85EB48)
#define o_CrExpo (*(_int_*)0x840F88)
#define o_ArtOutput (*(_int_*)0x8458D0)
#define o_MayChooseArtOpt (*(_int_*)0x841940)

#define o_Dlg_Case8 (*(_int_*)0x846B00)
int str_num_hook = NULL;

// #define o_Twrport (*(char**)0x46CB5C) // "twcrport.def"
char* n_DlgExpMon = "DlgExpMon.def";


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

	dlg->AddItem(_DlgStaticDef_::Create(dlg->width/2 -41, 26, 2, "PSKILL.def", 4, 0, 0x12)); 

	dlg->AddItem(_DlgStaticText_::Create(20, 126, dlg->width -40, 20, json_CrExpo[0], n_BigFont, 10, 5, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	for (int i = 0; i < 11; i++){
		dlg->AddItem(_DlgStaticText_::Create(24, 150 +i*20, dlg->width -48, 20, json_CrExpo[i+1], n_MedFont, 7, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));	
	}

	dlg->AddItem(_DlgStaticText_::Create(24, 150, dlg->width -48, 20, o_pCreatureInfo[o_Mon].name_single, n_MedFont, 1, 22, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));
	

	int runk = CALL_2(_int_, __cdecl, 0x727F20, 1000, o_Expo); 
	sprintf(o_TextBuffer, "%s (%d)", Get_ITxtExp(2+runk), runk);
	dlg->AddItem(_DlgStaticText_::Create(24, 170, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 23, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 190, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 24, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk +1, 0) - o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 210, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 25, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	int t1 = CALL_1(_int_, __cdecl, 0x7283D0, 1000); 
	int t2 = CALL_1(_int_, __cdecl, 0x727E20, 1000) * t1 / 100;
	sprintf(o_TextBuffer, "%d%% (%d)", t1, t2);
	dlg->AddItem(_DlgStaticText_::Create(24, 230, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 26, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Num); 
	dlg->AddItem(_DlgStaticText_::Create(24, 250, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 27, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	t1 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk, 0);
	t2 = 999999;
	if( t1 ){ t2 = o_Num * o_Expo / t1 - o_Num; }	
	sprintf(o_TextBuffer, "%d", t2); 
	dlg->AddItem(_DlgStaticText_::Create(24, 270, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 28, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727C00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 290, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 29, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727E00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 310, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 30, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t1 = CALL_1(_int_, __cdecl, 0x7283B0, 1000);
	sprintf(o_TextBuffer, "%d", t1); 
	dlg->AddItem(_DlgStaticText_::Create(24, 330, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 31, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t2 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, 10, 0);
	int t3 = 999999; 
	if ( t2 ){
		int t4 = o_Num * (t2 + t1);
		t3 = t4 / t2 - o_Num;
	}
	sprintf(o_TextBuffer, "%d", t3); 
	dlg->AddItem(_DlgStaticText_::Create(24, 350, dlg->width -48, 20, o_TextBuffer, n_MedFont, 1, 32, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	if (style == 1){
		dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, "Box64x30.pcx"));
		// Добавляем кнопку OK (id = 30725)
		_DlgButton_* bttnOK;
		bttnOK = b_DlgButton_Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 0, 2);
		// и сразу делаем выход и на ESC и ENTER
		int hotkey = 1;		CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
			hotkey = 28;	CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
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
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	sprintf(o_TextBuffer, "%s", Get_ITxtExp(340));
    dlg->AddItem(_DlgStaticText_::Create(18, 30, 424, 30, o_TextBuffer, n_BigFont, 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
	
	int dy = 27;	

	for (int i = 0; i < 9; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 62+i*dy, 424, 24, 10+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(10+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "%s", Get_ITxtExp(341 +i));
		dlg->AddItem(_DlgStaticText_::Create(44, 66+i*dy, 394, 16, o_TextBuffer, n_SmallFont, 1, 20+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

		dlg->AddItem(_DlgButton_::Create(22, 66+i*dy, 420, 18, 30+i, "radiobttn.def", 0, 1, 0, 0, 0));
	} 
	int s = CALL_1(_int_, __thiscall, 0x727BE0, o_CrExpo);	
	((_DlgButton_*)dlg->GetItem(30+s))->def_frame_index = 2;
	((_DlgButton_*)dlg->GetItem(30+s))->press_def_frame_index = 3;

	dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -20 -31, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -20 -30, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));

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

	if (msg->type == MT_MOUSEOVER)
		{
			_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
			char* text_bar = "";
			if (it)	
			{
				switch (it->id)
				{
					case 1: text_bar = Get_ITxtExp(107); break;
					
					case 6: text_bar = Get_ITxtExp(456); break;
					case 8: case 9: sprintf(o_TextBuffer, "%s", *(int*)(o_Dlg_ExpaMon +76)); text_bar = o_TextBuffer; break;				
					case 10: text_bar = Get_ITxtExp(123); break;

					case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
					case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +40)+4*(it->id-11))); 
						text_bar = o_TextBuffer; 
						break;

					case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
					case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: 
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +48)+4*(it->id-30))); 
						text_bar = o_TextBuffer; 
						break;
					
					case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
					case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65:
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +24)+4*((it->id-50)/2))); 
						text_bar = o_TextBuffer; 
						break;					

					case DIID_OK: 
						sprintf(o_TextBuffer, "%s", txtresWOG->GetString(85)); 
						text_bar = o_TextBuffer; 
						break;
					
					default: 
						text_bar = "";
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
				case 3: dlg->Redraw(); break;

				case 5: case 6: 
				case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
				case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
				case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
				case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45:
						New_Dlg_ExpaMon_Case8(4); break;  						

				case 9: if (New_Dlg_ExpaMon_BannerDlg() == 1){ 
							o_DlgLoop = 1;
							return dlg->Close(msg);	
						}	break; 

				case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
				case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65:
					frame = *(int*)(*(int*)(o_Dlg_ExpaMon +20)+4*((msg->item_id-50)/2));
					if (frame == 0){
						sprintf(o_TextBuffer, "%s\n", *(int*)(*(int*)(o_Dlg_ExpaMon +16)+4*((msg->item_id-50)/2)));
					}
					else {
						sprintf(o_TextBuffer, "%s\n\n %s\n", *(int*)(*(int*)(o_Dlg_ExpaMon +16)+4*((msg->item_id-50)/2)),
															Get_ITxtExp(*(int*)(0x840E1C +4*((msg->item_id-50)/2))+1));
					}	
					b_MsgBoxAndDef(n_DlgExpMon, frame, o_TextBuffer);	break; 
					
				default: break; 
			}
		} 

		if (msg->subtype == MST_LBUTTONCLICK){
			if (msg->item_id == 1)	{
				o_DlgUpgr = 1;
				return dlg->Close(msg);
			}
		}
		if (msg->subtype == MST_LBUTTONDOWN){ 
			if (msg->item_id == 5 || msg->item_id == 6 || (msg->item_id >= 10 && msg->item_id <= 45)){
				New_Dlg_ExpaMon_Case8(1);
			}

			if (msg->item_id == 9){
				int hero_id = *(short*)(o_CrExpo +12);
				if (hero_id < 0 || hero_id >=156 )
					hero_id = -1;

				if (hero_id == -1 || !o_MayChooseArtOpt || !o_CrExpo){
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

int New_Dlg_ExpaMon(signed int Mon, signed int  Num, int Expo, _ptr_ CRExpo)
{
	int dop_str = *(int*)(o_Dlg_ExpaMon +52);
	int art_num = *(int*)(o_Dlg_ExpaMon +88);
	int Changin = *(int*)(o_Dlg_ExpaMon +84);

	int str_txt = 1; // считаем кол-во строк в столбцах (общее)
	for (int i = 0; i < 16; i++){
		if (*(int*)(0x840E44 +4*i) != 0)
			str_txt++;
	}
	str_txt -= (dop_str + str_num_hook);
	
	int is_art = 0;
	if (CRExpo){
		if (CALL_1(_int_, __thiscall, 0x7279C0, CRExpo)){
			is_art = 1;
			if (art_num < 1)
			art_num = 1;
		}
	} 

	int x = 800;
	int y = 132 + 16*str_txt +16*dop_str +52*Round(((float)dop_str + is_art)/2); 

	if (y < 246) y = 246;

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_ExpaMon_Proc);

	Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());
	b_MouseMan_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// Рамка кнопки OK
    dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width -31 -52, dlg->height - 31 -40, 0, "Box64x30.pcx"));
	// Добавляем кнопку OK (id = 30725)
	_DlgButton_* bttnOK;
	bttnOK = b_DlgButton_Create(dlg->width -30 -52, dlg->height -30 -40, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 0, 2);
	// и сразу делаем выход и на ESC и ENTER
	int hotkey = 1;		CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	    hotkey = 28;	CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	dlg->AddItem(bttnOK);

	
	// Добавляем кнопку просмотра улучшенного (id = 1)
	int is_grade_bttn_on = *(int*)(o_Dlg_ExpaMon +56);
	if (is_grade_bttn_on){
		dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width -31 -122, dlg->height - 31 -40, 0, "Box64x30.pcx"));
		_DlgButton_* bttnUpg;
		bttnUpg = b_DlgButton_Create(dlg->width -30 -122, dlg->height -30 -40, 64, 30, 1, "DlgExpUp.def", 0, 1, 0, 0, 0);
		// и сразу делаем выход и на U и SPACE
		int hotkey = 22;	CALL_4(int, __thiscall, 0x404230, &bttnUpg->hotkeys_struct, bttnUpg->hotkeys_end, 1, &hotkey);
			hotkey = 57;	CALL_4(int, __thiscall, 0x404230, &bttnUpg->hotkeys_struct, bttnUpg->hotkeys_end, 1, &hotkey);
		dlg->AddItem(bttnUpg);
	}

	// Подсказка (id = 2)
	dlg->AddItem(_DlgStaticText_::Create(8, dlg->height -26, dlg->width -34, 18, "", n_SmallFont, 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	// заголовок диалога (id = 3)
	sprintf(o_TextBuffer, "%s", *(int*)o_Dlg_ExpaMon);
    dlg->AddItem(_DlgStaticText_::Create(150, 14, 500, 30, o_TextBuffer, n_BigFont, 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	// нижнее описание монстра красным (id = 4)
	sprintf(o_TextBuffer, "%s", *(int*)(o_Dlg_ExpaMon +4));
    dlg->AddItem(_DlgStaticText_::Create(18, dlg->height - 76, dlg->width -200, 44, o_TextBuffer, n_MedFont , 27, 4, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

	// картинка и рамка монстра (id = 5, 6)
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 5, o_Twrport, Mon +2, 0, 0x12)); 
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 6, o_Twrport, 1, 0, 0x12));  

	// название артефакта и рамка для него (id = 7,8)
    dlg->AddItem(_DlgStaticText_::Create(18, 100, 60, 15, o_GENRLTXT_TXT->GetString(616), n_MedFont, 1, 7, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
	dlg->AddItem(_DlgStaticDef_::Create(22, 116, 8, n_DlgExpMon, 159, 0, 0x12)); 
	
	// артефакт и цифры его кол-ва (id = 9)
	if (art_num > 0){
		dlg->AddItem(_DlgStaticDef_::Create(25, 119, 9, "Artifact.def", 156, 0, 0x12));    // рамка монстра (id = 9)
		sprintf(o_TextBuffer, "%d", art_num);
		dlg->AddItem(_DlgStaticText_::Create(47, 142, 20, 20, o_TextBuffer, "tiny.fnt", 1, 9, ALIGN_H_RIGHT | ALIGN_V_BOTTOM, 0));
	}
	
	// желтая рамка обрамления ранга
	int rank_mon = 0;
		if ((_ptr_)CRExpo)
			rank_mon = CALL_2(int, __cdecl, 0x727F20, 1000, Expo);
	dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 164+ 56*rank_mon, 50, 56, 16*(str_txt+dop_str), 100, *(int*)(*(int*)0x6AAD18 + 118), 1024));
    dlg->GetItem(100)->SendCommand(5, 4);

	// верхние названия	 
	for (int i = 0; i <= 10; i++){
	dlg->AddItem(_DlgStaticText_::Create(168 +56*i, 34, 58, 16, 
		CALL_3 (char*, __cdecl, 0x77710B, 1+i, 1, 0x847D88), 
		n_SmallFont, 7, 10, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
	}
	// элементы заполнения колонок (первый столбик) id = 11...(26)
	int y0 = 50;	int yy = 16;
	for (int i = 0; i < (str_txt+dop_str); i++){
		sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +36)+4*i));
		dlg->AddItem(_DlgStaticText_::Create(80, y0 + yy*i, 80, 16, o_TextBuffer, n_SmallFont, 7, 11+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
	}
	int x0 = 168;	int xx = 56;
	char text[16];
	for (char i = 0; i < (str_txt+dop_str); i++){
		sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +44)+4*i));
		for (char j = 0; j < 11; j++){
			for (char k = 0; k < 8; k++){
				if (*(char*)(*(int*)(*(int*)(o_Dlg_ExpaMon +44)+4*i) +j*8 +k) != ' ')
					text[k] = *(char*)(*(int*)(*(int*)(o_Dlg_ExpaMon +44)+4*i) +j*8 +k);
				else {
					for (char s = k; s < 16; s++){
						text[s] = ' ';
					}
					k = 7;
				}
			} 
			dlg->AddItem(_DlgStaticText_::Create(x0 + xx*j, y0 + yy*i, 54, 16, text, n_SmallFont, 1, 30+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
		}
	}
	// нижние картинки с дефами (id 50 ... )
	int xd = 18;  int xxd = 366;
	int yd = 56 +16*str_txt +16*dop_str; 
	int yyd = 52;
	dop_str += is_art;
	if (dop_str > 0 || art_num > 0){
		for (int i = 0; i < (dop_str); i++){
			dlg->AddItem(_DlgStaticDef_::Create(xd, yd, 50+i*2, n_DlgExpMon, *(int*)(*(int*)(o_Dlg_ExpaMon +20)+4*i), 0, 0x12)); 
			sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +16)+4*i));
			dlg->AddItem(_DlgStaticText_::Create(xd+54, yd, 330, 50, o_TextBuffer, n_SmallFont, 7, 51+i*2, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
			if (xd == 18)
				xd = 400;
			else xd = 18;
			if ( (i+10)%2 == 1 )
				yd += 52;
		}	
	} 

	dlg->Run();
	dlg->Destroy(TRUE);
	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Хуки //////////////////////////////////////////////////////////////////


_int_ __stdcall New_Dlg_ExpaMon_Lo(LoHook* h, HookContext* c)
{
	New_Dlg_ExpaMon(o_Mon, o_Num, o_Expo, o_CrExpo);
	str_num_hook = 0;

	c->return_address = 0x773C22; 
	return NO_EXEC_DEFAULT; 
} 

_int_ __stdcall New_Dlg_ExpaMon_Lo_IsBattle(LoHook* h, HookContext* c)
{
	o_CrExpo = c->eax;
	return EXEC_DEFAULT; 
} 

_int_ __stdcall New_Dlg_ExpaMon_NULL(LoHook* h, HookContext* c)
{
	str_num_hook++;
	return EXEC_DEFAULT; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_ExpaMon(PatcherInstance* _PI)
{
	// диалог Экспы монстров
	_PI->WriteByte((0x724101 + 2), 0);			
	_PI->WriteLoHook(0x773BA4, New_Dlg_ExpaMon_Lo);
	_PI->WriteLoHook(0x724B80, New_Dlg_ExpaMon_Lo_IsBattle);
	_PI->WriteLoHook(0x723DFF, New_Dlg_ExpaMon_NULL);	
}
