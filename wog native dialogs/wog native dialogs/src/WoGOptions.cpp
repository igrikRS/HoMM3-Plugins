////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Диалог WoG опций /////////////////////////////////////////////////////////////////////

struct _DlgSetup_;
#define o_DlgSetup ((_DlgSetup_*)0x2918390)
#define o_LastChoosenPage (*(_int_*)0x7B35FC)

_DlgStaticTextPcx8ed_* statbarWoGOptions = NULL;

int Redraw_WoGDlgSetup_ElemOnPage(_CustomDlg_* dlg, _DlgSetup_* ds, int page) 
{
	int count = 0;	int id = 0; int state = 0;

	for (int j=0; j<4; j++) {
		count = ds->Pages[page]->ItemList[j]->ItemCount;
		id = (1000*(page+1)) + (j*200);
		
		for (int i=0; i<count; i++) { 
			state = ds->Pages[page]->ItemList[j]->ItemState[i];
			if (state == 0) {
				((_DlgStaticText_*)dlg->GetItem(id+i))->color = 2;	
				((_DlgButton_*)dlg->GetItem(id+i+100))->def_frame_index = 0;
				((_DlgButton_*)dlg->GetItem(id+i+100))->press_def_frame_index = 1;
			} else if (state == 1) {
				((_DlgStaticText_*)dlg->GetItem(id+i))->color = 2;	
				((_DlgButton_*)dlg->GetItem(id+i+100))->def_frame_index = 2;
				((_DlgButton_*)dlg->GetItem(id+i+100))->press_def_frame_index = 3;
				// CALL_3(void, __thiscall, 0x5FF490, dlg, id+i+100, 16392); // on
			} else if (state == 2) {
				((_DlgStaticText_*)dlg->GetItem(id+i))->color = 1;			
				((_DlgButton_*)dlg->GetItem(id+i+100))->def_frame_index = 4;
				((_DlgButton_*)dlg->GetItem(id+i+100))->press_def_frame_index = 4;
				// CALL_3(void, __thiscall, 0x5FF520, dlg, id+i+100, 16392); // off
			} else if (state == 3) {
				((_DlgStaticText_*)dlg->GetItem(id+i))->color = 1;
				((_DlgButton_*)dlg->GetItem(id+i+100))->def_frame_index = 5;
				((_DlgButton_*)dlg->GetItem(id+i+100))->press_def_frame_index = 5;
			} else {
				sprintf(o_TextBuffer, "Error (for igrik)!!!\n\n Debug\nPage: %d (%d) \nCount: %d \n id: %d \n state: %d \n\n %s", page, j, count, id+i+100, state, ds->Pages[page]->ItemList[j]->ItemName[i]); 
				b_MsgBox(o_TextBuffer, 1);
			}
		}
	}
	
	return 1;
} 


int ShowHide_WoGDlgSetup_ElemOnPage(_CustomDlg_* dlg, int page, _DlgSetup_* ds, int show) 
{	
	if (!ds->Pages[page]->Enabled) 
		return 1;
	// show: показать (1), спрятать (2)
	_DlgItem_* it;
	int id = 0;

	int countsElements = 0;
	for (int j=0; j<4; j++) {
		id = 1000*(page+1) +(j*200);
		
		if ( ds->Pages[page]->ItemList[j]->ItemCount > 0 ) {
			if (show) {
				it = dlg->GetItem(800+page*10+j); if(it) { it->Show(); }
				it = dlg->GetItem(900+page*10+j); if(it) { it->Show(); }
				for (int i=0; i<ds->Pages[page]->ItemList[j]->ItemCount; i++){
					it = dlg->GetItem(id+i); if(it) { it->Show(); }
					it = dlg->GetItem(id+i+100); if(it) { it->Show(); }
				}
			} else {
				it = dlg->GetItem(800+page*10+j); if(it) { it->Hide(); }
				it = dlg->GetItem(900+page*10+j); if(it) { it->Hide(); }
				for (int i=0; i<ds->Pages[page]->ItemList[j]->ItemCount; i++){
					it = dlg->GetItem(id+i); if(it) { it->Hide(); }
					it = dlg->GetItem(id+i+100); if(it) { it->Hide(); }
				}
			}		
		}
	}
	return 1;
} 

int Create_WoGDlgSetup_ElemOnPage(_CustomDlg_* dlg, int page, _DlgSetup_* ds) 
{	
	int x = 225; int wx = 0; int y = 52; int dy = 19; int id = 0; int ry = 0;
	int itState = 0; int defState = 0;

	for (int j=0; j<4; j++) {
		if ( ds->Pages[page]->ItemList[j]->ItemCount == 0 ) continue; 
		
		// смещение по x (для 3 и 4 групп)
		if (j == 0 && ds->Pages[page]->ItemList[2]->ItemCount == 0 ) { wx = 275; }
		if (j == 2) { x = 500; y = 52; }
		if (j == 3 && x != 500 ) { x = 500; y = ry; }

		id = 1000*(page+1) +(j*200);

		// титульный текст группы
		dlg->AddItem(_DlgStaticText_::Create(x, y, 267 +wx, 19, ds->Pages[page]->ItemList[j]->Name, n_BigFont, 2, 800+page*10+j, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

		// рамка
		int hy = ds->Pages[page]->ItemList[j]->ItemCount * dy +3;  
		b_YellowFrame_Create(dlg, x, y+19, 267 +wx, hy, 900+page*10+j, ON, o_Pal_Grey);

		// кнопки группы
		for (int i=0; i<ds->Pages[page]->ItemList[j]->ItemCount; i++) {
			sprintf(o_TextBuffer, "%s", ds->Pages[page]->ItemList[j]->ItemName[i]);
			dlg->AddItem(_DlgStaticText_::Create(x+25, y+22 +dy*i, 238 +wx, 16, o_TextBuffer, n_SmallFont, 
				(ds->Pages[page]->ItemList[j]->ItemState[i] >= 2) ? 1 : 2, id +i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

			itState = ds->Pages[page]->ItemList[j]->ItemState[i];
			if (itState < 0 || itState > 3) { itState = 1; }
			if (itState < 2) { defState = itState*2; }
			if (itState == 2) { defState = 4; }
			if (itState == 3) { defState = 5; }

			dlg->AddItem(_DlgButton_::Create(x+3, y+22 +dy*i, 262 +wx, 16, id+i+100, 
				(ds->Pages[page]->ItemList[j]->Type == 2) ? "radiobttn.def" : "checkbox.def", 
				defState, (itState < 2) ? (defState+1) : defState, 0, 0, 0)); 
			 /* dlg->AddItem(_DlgTextButton_::Create(x+3, y+22 +dy*i, 262 +wx, 16, id+i+100, 
				 (ds->Pages[page]->ItemList[j]->Type == 2) ? "radiobttn.def" : "checkbox.def", 
				 ds->Pages[page]->ItemList[j]->ItemName[i], n_SmallFont, 
				 ds->Pages[page]->ItemList[j]->ItemState[i]*2, 
				 (ds->Pages[page]->ItemList[j]->ItemState[i] == 2) ? 4 : ds->Pages[page]->ItemList[j]->ItemState[i]*2+1, 0, 0, 0x4, 2)); */ 

		}
		wx = 0;
		if (j == 1) { ry = y;}
		y += hy +dy;
	}

	return 1;
}

void setYellowFrames(_CustomDlg_* dlg, int page)
{
	if (page < 0 || page > 7) {
		b_YellowFrame_Create(dlg, 215, 46, 564, 3, 50, ON, o_Pal_Y);
		b_YellowFrame_Create(dlg, 216, 47, 562, 1, 51, ON, o_Pal_Y);
		b_YellowFrame_Create(dlg, 215, 518, 564, 3, 50, ON, o_Pal_Y);
		b_YellowFrame_Create(dlg, 216, 519, 562, 1, 51, ON, o_Pal_Y);
		b_YellowFrame_Create(dlg, 776, 49, 3, 469, 51, ON, o_Pal_Y); // h = 518
		b_YellowFrame_Create(dlg, 777, 50, 1, 467, 50, ON, o_Pal_Y);

		int dx=60; 
		for (int i=0; i<7; i++) {
			b_YellowFrame_Create(dlg, 215, 101+dx*i, 1, 5, 50, ON, o_Pal_Y);
			b_YellowFrame_Create(dlg, 217, 100+dx*i, 1, 7, 50, ON, o_Pal_Y);
			b_YellowFrame_Create(dlg, 216, 99+dx*i, 1, 9, 51, ON, o_Pal_Y);
		}

		// id = 60...140
		for (int i=0; i<8; i++) {
			b_YellowFrame_Create(dlg, 217, 46+dx*i, 1, 54, 60+10*i, ON, o_Pal_Y); // в.п.
			b_YellowFrame_Create(dlg, 215, 46+dx*i, 1, 55, 61+10*i, ON, o_Pal_Y);
			b_YellowFrame_Create(dlg, 216, 48+dx*i, 1, 52, 62+10*i, ON, o_Pal_Y);
			if (o_DlgSetup->Pages[i]->Enabled) {
				b_YellowFrame_Create(dlg, 20, 46+dx*i, 1, 54, 66+10*i, ON, o_Pal_Y); // в.л.
				b_YellowFrame_Create(dlg, 22, 48+dx*i, 1, 50, 66+10*i, ON, o_Pal_Y);
											b_YellowFrame_Create(dlg, 21, 47+dx*i, 1, 52, 63+10*i, OFF, o_Pal_Y);
				b_YellowFrame_Create(dlg, 20, 46+dx*i, 196, 1, 66+10*i, ON, o_Pal_Y); // г.в.
				b_YellowFrame_Create(dlg, 22, 48+dx*i, 194, 1, 66+10*i, ON, o_Pal_Y);
											b_YellowFrame_Create(dlg, 21, 47+dx*i, 196, 1, 64+10*i, OFF, o_Pal_Y);
				b_YellowFrame_Create(dlg, 20, 100+dx*i, 196, 1, 66+10*i, ON, o_Pal_Y); // г.н.
				b_YellowFrame_Create(dlg, 22, 98+dx*i, 194, 1, 66+10*i, ON, o_Pal_Y);
											b_YellowFrame_Create(dlg, 21, 99+dx*i, 196, 1, 65+10*i, OFF, o_Pal_Y);
			}
		}
	} else {
		int id = page*10 +60;
		int last_id = o_LastChoosenPage*10 +60; // элементы прошлой страницы

		if (o_LastChoosenPage != -1) {
			dlg->GetItem(last_id)->SendCommand(5, 4);
			dlg->GetItem(last_id+1)->SendCommand(5, 4);
			dlg->GetItem(last_id+2)->SendCommand(5, 4);
			dlg->GetItem(last_id+3)->SendCommand(6, 4);
			dlg->GetItem(last_id+4)->SendCommand(6, 4);
			dlg->GetItem(last_id+5)->SendCommand(6, 4);
		}

		dlg->GetItem(id)->SendCommand(6, 4);
		dlg->GetItem(id+1)->SendCommand(6, 4);
		dlg->GetItem(id+2)->SendCommand(6, 4);
		dlg->GetItem(id+3)->SendCommand(5, 4);
		dlg->GetItem(id+4)->SendCommand(5, 4);
		dlg->GetItem(id+5)->SendCommand(5, 4);
	}
}

int apdFont = 0;

int __stdcall Dlg_WoG_Options_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
	_DlgSetup_* ds = o_DlgSetup;	 

	if (msg->type == MT_MOUSEOVER)	{
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
		char* text_bar = "";
		if (it)	{
			if (it->id == 3 || it->id == 4) { text_bar = o_DlgSetup->Hint; }
			if (it->id == 5) { text_bar = txtresWOG->GetString(86); }
			if (it->id == 6) { text_bar = txtresWOG->GetString(87); }
			if (it->id == 7) { text_bar = txtresWOG->GetString(83); }
			if (it->id == 8) { text_bar = txtresWOG->GetString(88); }
			if (it->id == 9) { text_bar = txtresWOG->GetString(82); }
			if (it->id == 10) { text_bar = txtresWOG->GetString(84); }
			if (it->id == DIID_OK) { text_bar = txtresWOG->GetString(85); } 

			if (it->id != apdFont && apdFont != 0) {
				((_DlgStaticText_*)dlg->GetItem(apdFont))->font = (int)smalfont2; 
				dlg->Redraw();
			} 

			if (it->id >=41 && it->id <=48) {
				int id = it->id -41;
				((_DlgStaticText_*)dlg->GetItem(it->id))->font = (int)medfont2; 
				text_bar = o_DlgSetup->Pages[id]->Hint;
				apdFont = it->id;	
				dlg->Redraw();
			} else { apdFont = 0; }

			if (it->id >= 800 && it->id < 900 ) {
				text_bar = ds->Pages[(it->id / 10) % 10]->ItemList[it->id % 10]->Hint;
			}

			if (it->id >= 1000 && it->id < 9000 ) { 
				text_bar = ds->Pages[(it->id / 1000) -1]->ItemList[( ( (it->id -100) / 100) % 10 ) / 2]->ItemHint[it->id % 100];
			}
			statbarWoGOptions->SetText(text_bar);
			statbarWoGOptions->Draw();
			statbarWoGOptions->RedrawScreen();
		}
	} // MT_MOUSEOVER

		if (msg->type == MT_MOUSEBUTTON) {
			if (msg->subtype == MST_LBUTTONDOWN){
				if (msg->item_id == 3 || msg->item_id == 4) 
					b_MsgBoxBig( ds->PopUp, MBX_OK); 
				
				if (msg->item_id >= 41 && msg->item_id <= 48) { 
					((_DlgStaticText_*)dlg->GetItem(4))->Hide();
					for (int i=0; i<8; i++) {
						ShowHide_WoGDlgSetup_ElemOnPage(dlg, i, ds, 0);
					} 					
					int id = msg->item_id -41;					
					ShowHide_WoGDlgSetup_ElemOnPage(dlg, id, ds, 1);
					setYellowFrames(dlg, id);
					ds->GetListener(2, id+1, -1, -1); // o_LastChoosenPage = id;
					dlg->Redraw();					
				}
				
			} // MST_LBUTTONDOWN

			if (msg->subtype == MST_LBUTTONCLICK) { // ЛКМ при отжатии
				int callBack = 0;
				if (msg->item_id == 5 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 4); }  // Выбрать все
				if (msg->item_id == 6 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 5); }	 // Сбросить все
				if (msg->item_id == 7 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 2); }	 // По умолчанию 
				if (msg->item_id == 10 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 3); } // Мультиплеер
				if (msg->item_id == 8 || msg->item_id == 9 ) {
					((_DlgStaticText_*)dlg->GetItem(4))->Hide();
					if (o_LastChoosenPage == -1) {
						ShowHide_WoGDlgSetup_ElemOnPage(dlg, 0, ds, 1);
						setYellowFrames(dlg, 0);
						o_LastChoosenPage = 0;
					}
					callBack = ds->GetListener(0, 1, 0, msg->item_id == 8 ? 8 : 1);
				}

				if (msg->item_id >= 1000 && msg->item_id <= 8999 ) { 
					if (ds->ButtonsStates[8] == 1) {
						int page = (msg->item_id / 1000) -1;
						int itList = ( ( (msg->item_id -100) / 100) % 10 ) / 2;
						int item = msg->item_id % 100;

						int state = ds->Pages[page]->ItemList[itList]->ItemState[item];

						if (state < 2) {
							state *= -1; state += 1;
							if (ds->Pages[page]->ItemList[itList]->Type == 1 ) { // чекбоксы
								ds->Pages[page]->ItemList[itList]->ItemState[item] = state;						
								((_DlgButton_*)dlg->GetItem(msg->item_id))->def_frame_index = state*2;
								((_DlgButton_*)dlg->GetItem(msg->item_id))->press_def_frame_index = (state*2)+1;
							}
							if (ds->Pages[page]->ItemList[itList]->Type == 2 ) { // радиобаттоны
								int idoff = (1000*(page+1)) +(200*itList) +100;
								for (int k=0; k<ds->Pages[page]->ItemList[itList]->ItemCount; k++) {
									if (k != item) {
										((_DlgButton_*)dlg->GetItem(idoff+k))->def_frame_index = 0;
										((_DlgButton_*)dlg->GetItem(idoff+k))->press_def_frame_index = 1;
										ds->Pages[page]->ItemList[itList]->ItemState[k] = 0;
									} else {
										((_DlgButton_*)dlg->GetItem(idoff+k))->def_frame_index = 2;
										((_DlgButton_*)dlg->GetItem(idoff+k))->press_def_frame_index = 3;
										ds->Pages[page]->ItemList[itList]->ItemState[item] = 1;
									}
								}
							}
						callBack = ds->GetListener(1, page+1, itList, item);
						}	
					}
				}
				if ( callBack == 1) { 
					for (int i=0; i<8; i++) {
						if (ds->Pages[i]->Enabled) {
							Redraw_WoGDlgSetup_ElemOnPage(dlg, ds, i);
						}
					}
					callBack = 0;
				}
				dlg->Redraw();
			} // MST_LBUTTONCLICK

			if (msg->subtype == MST_RBUTTONDOWN){
				if (msg->item_id == 3 || msg->item_id == 4) 
					b_MsgBoxBig( ds->PopUp, MBX_RMC ); 
				
				if (msg->item_id >= 41 && msg->item_id <= 48) { 
					b_MsgBoxBig( ds->Pages[msg->item_id -41]->PopUp, MBX_RMC);
				}
				if (msg->item_id >= 800 && msg->item_id < 900) { 
					b_MsgBoxBig( ds->Pages[(msg->item_id / 10) % 10]->ItemList[msg->item_id % 10]->PopUp, MBX_RMC);
				}
				if (msg->item_id >= 1000 && msg->item_id < 9000 ) { 
					int page = (msg->item_id / 1000) -1;
					int itList = ( ( (msg->item_id -100) / 100) % 10 ) / 2;
					int item = msg->item_id % 100;
					b_MsgBoxBig( ds->Pages[page]->ItemList[itList]->ItemPopUp[item], MBX_RMC);
				}
			} // MST_RBUTTONDOWN
		} // MT_MOUSEBUTTON

	return r;
}

/* void __fastcall Dlg_WoG_Options_Scroll(int step, _CustomDlg_* dlg)
{
	// Устанавливаем ползунок в ближайшее к точке клика положение
	_DlgSetup_* ds = o_DlgSetup;

	if (step >= 0 && step < 7)ShowHide_WoGDlgSetup_ElemOnPage(dlg, o_LastChoosenPage, ds, 0);
	ShowHide_WoGDlgSetup_ElemOnPage(dlg, step, ds, 1);
	setYellowFrames(dlg, step);
	o_LastChoosenPage = step; 

	//sprintf(o_TextBuffer, "%d", step);
	//b_MsgBox(o_TextBuffer, 1);

    _DlgMsg_ m;
    CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 11, 0, 0, 0, 0, 0) );
    dlg->Redraw(TRUE);

}*/


void __stdcall Dlg_WoG_Options_Show(HiHook* hook, int a1)
{
	int color = o_MeID;	o_MeID = 1;
	_DlgSetup_* ds = o_DlgSetup;
	_CustomDlg_* dlg = _CustomDlg_::Create(o_HD_X/2 -400, o_HD_Y/2 -300, 800, 600, DF_SCREENSHOT , Dlg_WoG_Options_Proc);
	Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());
	
	statbarWoGOptions = _DlgStaticTextPcx8ed_::Create(7, dlg->height -26, dlg->width -14, 18, "", n_SmallFont, "WoGOptions.pcx", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER); 
	dlg->AddItem(statbarWoGOptions); // подсказка в статус баре	

	dlg->AddItem(_DlgStaticText_::Create(214, 20, 370, 20, ds->Name, json_WoGOpt[0], 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // id = 3
	dlg->AddItem(_DlgStaticText_::Create(230, 50, 538, 468, ds->Intro, json_WoGOpt[0], 2, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); //id = 4

	for (int i=0; i<8; i++) {
		if (ds->Pages[i]->Enabled) {
			dlg->AddItem(_DlgStaticText_::Create(23, 49 +60*i, 192, 49, ds->Pages[i]->Name, n_SmallFont, 1, 41 +i, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));	// id = 41...48
			
			Create_WoGDlgSetup_ElemOnPage(dlg, i, ds); // функция построения элементов диалога текущей в цикле страницы
			ShowHide_WoGDlgSetup_ElemOnPage(dlg, i, ds, 0); // скрываем все элементы на странице
		}
	}
	if (ds->ButtonsStates[5] == 1) {dlg->AddItem(_DlgButton_::Create(375, 528, 64, 30, 5, "WoGBttn.def", 15, 16, 0, HK_B, 0)); } // id = 5 // выбрать всё
	if (ds->ButtonsStates[4] == 1) {dlg->AddItem(_DlgButton_::Create(440, 528, 64, 30, 6, "WoGBttn.def", 18, 19, 0, HK_N, 0)); } // id = 6 // сбросить всё
	if (ds->ButtonsStates[2] == 1) {dlg->AddItem(_DlgButton_::Create(510, 528, 64, 30, 7, "WoGBttn.def", 12, 13, 0, HK_R, 0)); } // id = 7 // по умолчанию
	if (ds->ButtonsStates[8] == 1) {dlg->AddItem(_DlgButton_::Create(580, 528, 64, 30, 8, "WoGBttn.def", 6, 7, 0, HK_L, 0)); }   // id = 8 // загрузить
	if (ds->ButtonsStates[1] == 1) {dlg->AddItem(_DlgButton_::Create(645, 528, 64, 30, 9, "WoGBttn.def", 9, 10, 0, HK_S, 0)); }  // id = 9 // сохранить
	if (ds->ButtonsStates[3] == 1) {dlg->AddItem(_DlgButton_::Create(305, 528, 64, 30, 10, "WoGBttn.def", 21, 22, 0, HK_M, 0)); }  // id = 10 // мультиплеер
	if (ds->ButtonsStates[0] == 1) {dlg->AddItem(_DlgButton_::Create(715, 528, 64, 30, DIID_OK, "WoGBttn.def", 0, 1, 1, HK_ENTER, 2)); } // id = 30725

	//_DlgScroll_* wogOptScroll = _DlgScroll_::Create(26, 534, 334, 16, 11, 8, (_ptr_)Dlg_WoG_Options_Scroll, 0, 0, 0); // создать ползунок
	//dlg->AddItem(wogOptScroll);

	o_LastChoosenPage = -1;   // страница диалога
	setYellowFrames(dlg, o_LastChoosenPage); // создаем массово желтые рамки (dlg, номер страницы вог диалога)

	dlg->Run();
	dlg->Destroy(TRUE); 
	o_MeID = color; 
} 


// #############################################################################################
// ##################################  создание кнопки WoG Options #############################

int __stdcall Y_NewScenarioDlg_Proc(HiHook* hook, _CustomDlg_* this_, _EventMsg_* msg)
{
	if ( (msg->type == MT_MOUSEBUTTON) && (msg->subtype == MST_LBUTTONCLICK) && (msg->item_id == 4444) ){
		msg->item_id = 0;
		msg->x_abs = 640;
		msg->y_abs = 110; 
		// CALL_0(void, __cdecl, 0x7790E1);
	}
	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), this_, msg);
}

void __stdcall Y_NewScenarioDlg_Create(HiHook* hook, _NewScenarioDlg_* this_, int type)
{
	CALL_2(void, __thiscall, hook->GetDefaultFunc(), this_, type);
	this_->AddItem(_DlgTextButton_::Create(622, 105, 4444, "GSPBUT2.DEF", json_WoGOpt[1], n_SmallFont, 0, 1, 0, 0, 1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_WoGOptions(PatcherInstance* _PI)
{
	// создание кнопки WoG Options
	_PI->WriteDword((0x779100 + 3), 640);
	_PI->WriteDword((0x779119 + 3), 640);
	_PI->WriteByte((0x779132 + 3), 110);
	_PI->WriteByte((0x779147 + 3), 110);
	_PI->WriteHiHook(0x579CE0, SPLICE_, EXTENDED_, THISCALL_, Y_NewScenarioDlg_Create);
	_PI->WriteHiHook(0x587FD0, SPLICE_, EXTENDED_, THISCALL_, Y_NewScenarioDlg_Proc);
	// диалог WoG Опций
	 _PI->WriteHiHook(0x779213, CALL_, EXTENDED_, THISCALL_, Dlg_WoG_Options_Show);
}
