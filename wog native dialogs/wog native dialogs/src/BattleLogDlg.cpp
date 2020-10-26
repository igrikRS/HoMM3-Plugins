////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог сброса артефактов на землю ////////////////////////////////////////////////

_ScrollItems_* o_ScrollItems;

// реализовываем функционал скролла
void __fastcall SimpleTextScrolled(int step, _CustomDlg_* dlg)
{
  _ScrollItems_* scroll = o_ScrollItems;

  int way = step - scroll->step;
  scroll->step = step;

  if (way) 
  {
	for (int j = 0; j < Abs(way); j++ )
	{
	  if (way < 0) 
	  {
		scroll->item_id_show_start--;
		scroll->item_id_show_end--;
	  }

	  _DlgStaticText_* itText;

	  for (int i = scroll->item_id_start; i < scroll->item_id_end; i++)
	  { 
		itText = (_DlgStaticText_*)dlg->GetItem(i);

		if (way > 0) // прокрутка вверх по номерам (вниз колесом) 
		{       
		  if (i == scroll->item_id_show_start) {
			itText->Hide();
		  } else if (i > scroll->item_id_show_start && i < scroll->item_id_show_end) {
			itText->y -= scroll->text_hight;
		  } else if (i == scroll->item_id_show_end) {
			itText->Show();
		  }
		} // end if (way == 1)
       
		if (way < 0) // прокрутка вниз по номерам (вверх колесом) 
		{       
		  if (i == scroll->item_id_show_start) {
			itText->Show();
		  } else if (i > scroll->item_id_show_start && i < scroll->item_id_show_end) {
			itText->y += scroll->text_hight;
		  } else if (i == scroll->item_id_show_end) {
			itText->Hide();
		  }
		} // end if (way == -1)
	  } // end for(i)
	  if (way > 0) 
	  {
		scroll->item_id_show_start++;
		scroll->item_id_show_end++;
	  }
	} // end for(j)
  } // end (way)

  if (!scroll->not_redraw)	  
	  dlg->Redraw(TRUE);
  scroll->not_redraw = 0;
}

#define FIRST_ITEM 10
#define HIGHT_ITEM 16
#define MAX_LINES_COUNT 21

void CreateNewBattleLogDlg(_BattleMgr_* bm)
{
	e_ClickSound();

	Y_Mouse_SetCursor(0);

	int x = 640;
	int y = 464; 

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	dlg->AddItem(_DlgStaticPcx8_::Create(32, 32, 1, "BattleLog.pcx" ));

	b_YellowFrame_Create(dlg, 33, 33, x-65, y-111, 2, ON, o_Pal_Black);
	b_YellowFrame_Create(dlg, 32, 32, x-65, y-111, 3, ON, o_Pal_Grey);


	int field_54 = (int)&bm->dlg->field_54;
	int logStart = *(int*)(field_54 +4);
	int logEnd = *(int*)(field_54 +8);
	int lines = (logEnd - logStart) >> 2; 

	int steps = lines - MAX_LINES_COUNT;
	if (steps < 1)
		steps = 0;
	
	_DlgScroll_* itemScroll = _DlgScroll_::Create( x-50, 34, 16, y-114, 4, steps+1, (_ptr_)SimpleTextScrolled, 0, 0, 0); // создать скролл
	dlg->AddItem(itemScroll);
	if ( steps < 1 )
		itemScroll->SetEnabled(0);

	//// DEBUG
	//b_YellowFrame_Create(dlg, 32, y-72, 240, 50, 5, ON, o_Pal_Grey);	
	//sprintf(MyString2, "lines: %d \nsteps: %d", lines, steps);
	//dlg->AddItem(_DlgStaticText_::Create(36, y-68, 232, 44, MyString2, n_SmallFont, 1, 7, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

	//b_YellowFrame_Create(dlg, (x >> 1) +48, y-72, 240, 50, 6, ON, o_Pal_Grey);
	//sprintf(MyString2, "x/y: %d x %d", x, y);
	//dlg->AddItem(_DlgStaticText_::Create((x >> 1) +52, y-68, 232, 44, MyString2, n_SmallFont, 1, 8, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
	//// end DEBUG

	int logData = (int)bm->dlg->field_58;

	int counter = 0; // counter строк
	for (int i = 0; i < MAX_LINES_COUNT; i++){
		if (counter < lines) {

			dlg->AddItem(_DlgStaticText_::Create(40, 40 +i*HIGHT_ITEM, x-96, HIGHT_ITEM, 
				(*(_HStr_**)(logData +(4*i)))->c_str, n_MedFont, 1, FIRST_ITEM +i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

			counter++;
		}
	}
	if (counter < lines) {
		for (int i = MAX_LINES_COUNT; i < lines; i++){

			dlg->AddItem(_DlgStaticText_::Create(40, y-104, x-96, HIGHT_ITEM, 
				(*(_HStr_**)(logData +(4*i)))->c_str, n_MedFont, 1, FIRST_ITEM +i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));	

			dlg->GetItem(FIRST_ITEM +i)->Hide();
		}
	}

	// запоминаем параметры для организации прокрутки
	_ScrollItems_ scroll_items;
	scroll_items.step = 0;
	scroll_items.text_hight = HIGHT_ITEM;
	scroll_items.item_id_start = FIRST_ITEM;
	scroll_items.item_id_end = FIRST_ITEM + lines;
	scroll_items.item_id_show_start = FIRST_ITEM;
	scroll_items.item_id_show_end = FIRST_ITEM + MAX_LINES_COUNT;

	o_ScrollItems = (_ScrollItems_*)&scroll_items;


	dlg->AddItem(_DlgStaticPcx8_::Create((x >> 1) -33, y -62, 2, box64x30Pcx)); 
	_DlgButton_* bttnExit = _DlgButton_::Create((x >> 1) -32, y -61, 64, 30, DIID_OK, iOkayDef, 0, 1, 1, 0, 2); 
	bttnExit->SetHotKey(1);   bttnExit->SetHotKey(28);
	dlg->AddItem(bttnExit);


	
	if ( steps > 0 ) {
		scroll_items.not_redraw = 1;
		itemScroll->btn_position = 318;
		itemScroll->tick = steps;
		itemScroll->tick_value = steps;
		SimpleTextScrolled(steps, dlg);
	}


	dlg->Run();
	dlg->Destroy(TRUE);

	Y_Mouse_SetCursor(1);
}


int __stdcall BattleLog_Proc(HiHook* hook, _BattleMgr_* bm, _EventMsg_*msg)
{

	if (msg->type == MT_MOUSEBUTTON)
		if (msg->subtype == MST_LBUTTONCLICK)
			if ( msg->item_id == 2005 )	
				CreateNewBattleLogDlg(bm);

	if (msg->type == MT_KEYDOWN)
		if ( msg->subtype == HK_H )
			CreateNewBattleLogDlg(bm);

	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), bm, msg);
}


void __stdcall BattleLog_ReportNewRound(HiHook* hook, _dword_ battleDlg, const char* textRound, int a3, char a4)
{
	sprintf(myString3, "%s {(%d)}", textRound, o_BattleMgr->round);
	CALL_4(void, __thiscall, hook->GetDefaultFunc(), battleDlg, myString3, a3, a4);
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
