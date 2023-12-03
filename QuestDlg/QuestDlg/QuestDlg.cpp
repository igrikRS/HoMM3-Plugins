// #include "stdafx.h"
#include "..\..\..\include\homm3.h"
#include "..\..\..\include\HoMM3_Dlg.cpp"

//#include "..\..\include\era.h"
//using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

// 0x69CD70 Dlg_QuestLog_Ptr
#define o_QuestLogDlg (*(_Dlg_**)0x69CD70)

int quest_show_coords[4];

// высота текста описания квестов 
#define oHT 26

// показ сообщения по ПКМ c одним дефом
void RMCBox_Def(char* text, char* def_name, int def_frame, int style, int is_show_kill)
{
	// вычисляем размеры текста
	int text_hi = o_Medfont_Fnt->height;
	int str_len = strlen(text);
	int tx = str_len * 6;
	int ty = text_hi;

	while ( tx > (ty * 4) ) {
		tx = tx >> 1;	// делим на 2
		ty = ty * 2;	// умножаем на 2
	} 

	if (tx < 280) tx = 280;
	int lines = o_Medfont_Fnt->GetLinesCountInText(text, tx);
	ty = (lines +1) *text_hi;

	if (ty > 220 || lines > 5) {
		tx = 440;
		lines = o_Medfont_Fnt->GetLinesCountInText(text, tx);
		ty = (lines +1) *text_hi;
	}

	if (ty < 20) ty = 20;

	int x = tx +40;
	int y = ty +60;
	if (style != 4 ) 
		y += 40;

	// вычисляем размеры картинки
	int def_x = 0;
	int def_y = 0;
	if (def_name) {
		_Def_* def = _Def_::Load(def_name);
		def_x = def->width;
		def_y = def->height;		
		def->DerefOrDestruct();
		if (x < def_x)
			x = (def_x +40);

		y += (def_y +20);
	}

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

    dlg->AddItem(_DlgStaticText_::Create(18, 26, x-40, ty, text, "medfont.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
	//b_YellowFrame_Create(dlg, 18, 26, x-40, ty, 100, ON, o_Pal_Y);	

	//b_YellowFrame_Create(dlg, (x >> 1) - (pcx8_x >> 1) -1, 43+ty, pcx8_x +2, pcx8_y +2, 4, ON, o_Pal_Y);
	// dlg->AddItem(_DlgStaticPcx8_::Create((x >> 1) - (pcx8_x >> 1), 44+ty, pcx8_x, pcx8_y, 5, pcx_name ));
	dlg->AddItem(_DlgStaticDef_::Create((x >> 1) - (def_x >> 1), 44+ty, 3, def_name, def_frame, 0, 0x12)); 

	if (is_show_kill) {
		int df_x = (x >> 1) + (def_x >> 1) -18;
		int df_y = 44+ty+def_y -18;
		dlg->AddItem(_DlgStaticDef_::Create(df_x, df_y, 6, /* "tpthchk.def"*/ (char*)0x68C320, /* def_frame*/ 1, 0, 0x12)); 
	}


	if ( style == 4 ) {
		dlg->RMC_Show(); 
	} else {
		int x_center = x >> 1; // вычисляем середину ширины диалога (x / 2)
		dlg->AddItem(_DlgStaticPcx8_::Create(x_center -33, y -51, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(x_center -32, y -50, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));

		dlg->Run(); 
	}

    dlg->Destroy(TRUE);
} 

// показ сообщения по ПКМ c одним pcx
void RMCBox_Pcx8(char* text, char* pcx_name, int temp_param, int style, int is_show_kill)
{
	// вычисляем размеры текста
	int text_hi = o_Medfont_Fnt->height;
	int str_len = strlen(text);
	int tx = str_len * 6;
	int ty = text_hi;

	while ( tx > (ty * 4) ) {
		tx = tx >> 1;	// делим на 2
		ty = ty * 2;	// умножаем на 2
	} 

	if (tx < 280) tx = 280;
	int lines = o_Medfont_Fnt->GetLinesCountInText(text, tx);
	ty = (lines +1) *text_hi;

	if (ty > 220 || lines > 5) {
		tx = 440;
		lines = o_Medfont_Fnt->GetLinesCountInText(text, tx);
		ty = (lines +1) *text_hi;
	}

	if (ty < 20) ty = 20;

	int x = tx +40;
	int y = ty +60;
	if (style != 4 ) 
		y += 40;

	// вычисляем размеры картинки
	int pcx8_x = 0;
	int pcx8_y = 0;
	if (pcx_name) {
		_Pcx8_* pcx8 = o_LoadPcx8(pcx_name);
		pcx8_x = pcx8->width;
		pcx8_y = pcx8->height;		
		pcx8->DerefOrDestruct();
		if (x < pcx8_x)
			x = (pcx8_x +40);

		y += (pcx8_y +20);
	}

    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
    Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

    dlg->AddItem(_DlgStaticText_::Create(18, 26, x-40, ty, text, "medfont.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 
	//b_YellowFrame_Create(dlg, 18, 26, x-40, ty, 100, ON, o_Pal_Y);	

	//b_YellowFrame_Create(dlg, (x >> 1) - (pcx8_x >> 1) -1, 43+ty, pcx8_x +2, pcx8_y +2, 4, ON, o_Pal_Y);
	dlg->AddItem(_DlgStaticPcx8_::Create((x >> 1) - (pcx8_x >> 1), 44+ty, pcx8_x, pcx8_y, 5, pcx_name ));

	if (is_show_kill) {
		int df_x = (x >> 1) + (pcx8_x >> 1) -18;
		int df_y = 44+ty+pcx8_y -18;
		dlg->AddItem(_DlgStaticDef_::Create(df_x, df_y, 6, /* "tpthchk.def"*/ (char*)0x68C320, /* def_frame*/ 1, 0, 0x12)); 
	}


	if ( style == 4 ) {
		dlg->RMC_Show(); 
	} else {
		int x_center = x >> 1; // вычисляем середину ширины диалога (x / 2)
		dlg->AddItem(_DlgStaticPcx8_::Create(x_center -33, y -51, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(x_center -32, y -50, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));

		dlg->Run(); 
	}

    dlg->Destroy(TRUE);
} 

// struct _ScrollItemsShow_;
struct _ScrollItemsShow_
{
	int item_id_start;
	int item_id_end;
	int item_id_show_start;
	int item_id_show_end;
	int last_step;
};

_ScrollItemsShow_* o_ScrollItemsShow;

// реализовываем функционал скролла
void __fastcall Dlg_QuestLog_Scroll(int step, _CustomDlg_* dlg)
{
	_ScrollItemsShow_* scit = o_ScrollItemsShow;

	int way = step - scit->last_step;
	scit->last_step = step;

	if (way) 
	{
		for (int j = 0; j < Abs(way); j++ )
		{
			if (way < 0) 
			{
				scit->item_id_show_start--;
				scit->item_id_show_end--;
			}

			_DlgStaticText_* itText;
			for (int i = scit->item_id_start; i < scit->item_id_end; i++)
			{	
				itText = (_DlgStaticText_*)dlg->GetItem(i);
				if (way > 0) // прокрутка вверх по номерам (вниз колесом) 
				{				
					if (i == scit->item_id_show_start) {
						itText->Hide();
					} else if (i > scit->item_id_show_start && i < scit->item_id_show_end) {
						itText->y -= oHT;
					} else if (i == scit->item_id_show_end) {
						itText->Show();
					}
				} // end if (way == 1)
				
				if (way < 0) // прокрутка вниз по номерам (вверх колесом) 
				{				
					if (i == scit->item_id_show_start) {
						itText->Show();
					} else if (i > scit->item_id_show_start && i < scit->item_id_show_end) {
						itText->y += oHT;
					} else if (i == scit->item_id_show_end) {
						itText->Hide();
					}
				} // end if (way == -1)
			} // end for(i)
			if (way > 0) 
			{
				scit->item_id_show_start++;
				scit->item_id_show_end++;
			}
		} // end for(j)
	} // end (way)

	dlg->Redraw(TRUE);
}


int ShowComplexDialog(char* text, int pixArr)
{
	return CALL_5(int, __fastcall, 0x4F7D20, text, pixArr, -1, -1, MBX_RMC << 16);
}


int PreShowRMC_ComplexQuestDialog(_H3Quest_* quest, char* quest_text)
{
	// добавляем текст последнего дня
	if ( quest->last_day > 0 ) { 
		_HStr_ lastDay;
		CALL_2(_HStr_*, __thiscall, 0x56D310, quest, &lastDay);
		sprintf(o_TextBuffer, "%s \n\n %s", quest_text, lastDay.c_str);
	} else sprintf(o_TextBuffer, "%s", quest_text);

	if (quest->GetQuestType() == _H3Quest_::QT_ExperienceLevel ) {
		b_MsgBoxD(o_TextBuffer, 4, 17, quest->data.achieveLevel);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_PrimarySkill ) { 						
		_List_<_int_> pixArr;
		for (int i = 0; i < 4; i++) {
			int count = quest->data.achievePrimarySkill[i];
			if (count > 0) {
				int pic[2];
				pic[0] = 31+i;  // тип картинки
				pic[1] = count; // первичный навык
				CALL_4(_dword_, __thiscall, 0x41B440, (int)&pixArr, pixArr.EndData, 1, (int)&pic);
			}
		}
		ShowComplexDialog(o_TextBuffer, (int)&pixArr);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_DefeatHero ) {
		int hero_id = quest->data.killHero.targetHero;
		if (hero_id > -1 ) {
			_Hero_* hero = o_GameMgr->GetHero(hero_id);
			RMCBox_Pcx8(o_TextBuffer, (char*)o_HeroInfo[hero->pic].hpl_name, 0, 4, 1);
		}
	}

	if (quest->GetQuestType() == _H3Quest_::QT_BeHero ) {
		int hero_id = quest->data.beHero;
		if (hero_id > -1 ) {
			_Hero_* hero = o_GameMgr->GetHero(hero_id);
			RMCBox_Pcx8(o_TextBuffer, (char*)o_HeroInfo[hero->pic].hpl_name, 0, 4, 0);
		}
	}

	if (quest->GetQuestType() == _H3Quest_::QT_BePlayer ) {
		b_MsgBoxD(o_TextBuffer, 4, 10, quest->data.bePlayer);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_DefeatMonster ) {
		char str_coords[1000];
		int packCoords = quest->data.killMonster.packedCoords;
		int qx = b_unpack_x(packCoords);
		int qy = b_unpack_y(packCoords);
		int qz = b_unpack_z(packCoords);
		sprintf(str_coords, ".\n\n %s %d/%d/%d", o_GENRLTXT_TXT->GetString(400), qx, qy, qz );

		strcat(o_TextBuffer, str_coords);
		// b_MsgBoxD(o_TextBuffer, 4, 21, quest->data.killMonster.displayCreatureType);
		RMCBox_Def(o_TextBuffer, (char*)(*(int*)0x4DF1D5), quest->data.killMonster.displayCreatureType +2, 4, 1);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_BringArtifacts ) { 						
		_List_<_int_> pixArr;
		for (int* i = (int*)quest->data.getArtifacts.first; i < quest->data.getArtifacts.last; i++) {
			int pic[2];
			pic[0] = 8; // тип картинки
			pic[1] = *i; // id артефакта
			CALL_4(_dword_, __thiscall, 0x41B440, (int)&pixArr, pixArr.EndData, 1, (int)&pic);
		}
		ShowComplexDialog(o_TextBuffer, (int)&pixArr);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_BringCreatures ) { 						
		_List_<_int_> pixArr;
		int* j = (int*)quest->data.getCreatures.number.first;
		for (int* i = (int*)quest->data.getCreatures.type.first; i < quest->data.getCreatures.type.last; i++) {							
			int pic[2];
			pic[0] = 21; // тип картинки
			int mon_type = *i;
			int mon_count = *j;
			pic[1] = mon_type | (mon_count << 16); // пакуем всё в одну переменную
			CALL_4(_dword_, __thiscall, 0x41B440, (int)&pixArr, pixArr.EndData, 1, (int)&pic);
			j++;
		}
		ShowComplexDialog(o_TextBuffer, (int)&pixArr);
	}

	if (quest->GetQuestType() == _H3Quest_::QT_BringResources ) { 						
		_List_<_int_> pixArr;
		for (int i = 0; i < 7; i++) {
			int count = quest->data.getResources[i];
			if (count > 0) {
				int pic[2];
				pic[0] = i; // тип картинки
				pic[1] = count; // первичный навык
				CALL_4(_dword_, __thiscall, 0x41B440, (int)&pixArr, pixArr.EndData, 1, (int)&pic);
			}
		}
		ShowComplexDialog(o_TextBuffer, (int)&pixArr);
	}

	return 1;
}


int __stdcall Dlg_QuestLog_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);
	if (msg->type == MT_MOUSEBUTTON) 
	{
		if (msg->item_id >= 50 && msg->item_id < 200) {
			int player_id = dlg->custom_data[0];
			_DlgStaticText_* it = (_DlgStaticText_*)dlg->GetItem(msg->item_id);
			if (msg->subtype == MST_LBUTTONDOWN ) 
			{ // при нажатии ЛКМ
				// поиск по всей карте определенного квеста 
				//(мне такой способ нихера не нравится)
				_GameMgr_* gm = o_GameMgr;		
				int need_find = it->field_3C;
				int start_dataSH = (int)gm->Map.QuestSeerHuts.Data;
				int start_dataGG = (int)gm->Map.QuestGuards.Data;

				for (int z = 0; z < gm->Map.has_underground +1; z++) {
					for (int x = 0; x < gm->Map.size; x++) {
						for (int y = 0; y < gm->Map.size; y++) {
							_MapItem_* mapIt = gm->Map.GetItem(x, y, z);

							if (mapIt) {
								int obj_id_real = mapIt->object_type;

								if (obj_id_real == 34) { 
									obj_id_real = mapIt->GetReal__object_type();
								}

								if ( obj_id_real == 83) {
									if (need_find == (19*mapIt->setup + start_dataSH) ) { // вот тут надо думать: mapIt->setup когда стоит герой неправильно считывается
										// вычисляем правильную центровку с учетом HD
										int hex_x = (o_HD_X -192) >> 6; // разделить на 64
										int hex_y = (o_HD_Y -56) >> 6;
										quest_show_coords[0] = 1;
										quest_show_coords[1] = b_pack_xyz(x -hex_x, y-hex_y, z);
											break;
									}
								}

								if ( obj_id_real == 215) {
									if (need_find == (5*mapIt->setup + start_dataGG) ) {
										// вычисляем правильную центровку с учетом HD
										int hex_x = (o_HD_X -192) >> 6; // разделить на 64
										int hex_y = (o_HD_Y -56) >> 6;
										quest_show_coords[0] = 1;
										quest_show_coords[1] = b_pack_xyz(x -hex_x, y-hex_y, z);

											break;
									}
								}
							} // end if mapIt
						}
					}
				}
				// закрываем диалог перед показом объекта на карте
				if (quest_show_coords[0] == 1) {
					return dlg->Close(msg);
				}
			} // ЛКМ

			if (msg->subtype == MST_RBUTTONDOWN ) { // при ПКМ 
				int quest_type = it->field_28;
				_HStr_ textQuest;
				if ( quest_type == 1 ) { // SeerHut
					_H3QuestSeerHut_* questSH =(_H3QuestSeerHut_*)it->field_3C;
					CALL_3(_HStr_*, __thiscall, 0x5747B0, questSH, &textQuest, player_id);	
					PreShowRMC_ComplexQuestDialog(questSH->quest, textQuest.c_str);
				} 

				if ( quest_type == 2 ) { // QuestGuard
					_H3QuestGuard_* questGG =(_H3QuestGuard_*)it->field_3C;
					CALL_3(_HStr_*, __thiscall, 0x573210, questGG, &textQuest, player_id);
					PreShowRMC_ComplexQuestDialog(questGG->quest, textQuest.c_str);	
				}
			}
		}
	}
	return r;
}

//int __stdcall Dlg_BuildQuestLog(HiHook* hook, int player_id) 
int __stdcall Dlg_BuildQuestLog(LoHook* h, HookContext* c)
{
	// 0x573A40 посещение хижины провидца
	// 0x572DF0 посещение страж задания
	// 0x573610 инициализация квестов 
	// 0x570320 тут настрока квестов при загрузке карты
	// int player_id = o_MeID;

	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	int player_id = IntAt(c->ebp -0x18);
	_GameMgr_* gm = o_GameMgr;

	// int x = 640;
	int x = 440;
	int y = 536;

	int xt = x -100;
	char* font_name = o_Smalfont_Fnt->name;

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Dlg_QuestLog_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	o_QuestLogDlg = dlg;
	dlg->custom_data[0] = player_id;

	// создаем полоски оформления
	for (int i = 0; i < 17; i++)
	{
		b_YellowFrame_Create(dlg, 35, (oHT*2 -2) + oHT*i +10, xt+5, 1, i+10, ON, o_Pal_Black); // o_Pal_Grey
		b_YellowFrame_Create(dlg, 36, (oHT*2 -1) + oHT*i +10, xt+5, 1, i+27, ON, o_Pal_Grey); // o_Pal_Black
	}

	int pQuestSH_start = 0;
	int pQuestSH_end = 0;
	int questSH_counts = 0;
	if ( gm->Map.QuestSeerHuts.Data ) {
		pQuestSH_start = (int)gm->Map.QuestSeerHuts.Data;
		pQuestSH_end = (int)gm->Map.QuestSeerHuts.EndData;
		questSH_counts = (pQuestSH_end - pQuestSH_start) / 19;
	}

	int pQuestGG_start = 0;
	int pQuestGG_end = 0;
	int questGG_counts = 0;
	if ( gm->Map.QuestGuards.Data ) {
		pQuestGG_start = (int)gm->Map.QuestGuards.Data;
		pQuestGG_end = (int)gm->Map.QuestGuards.EndData;
		questGG_counts = (pQuestGG_end - pQuestGG_start) / 5;
	}

	// id = 3
	// sprintf(o_TextBuffer, "Журнал: %d хижин и %d стражей", questSH_counts, questGG_counts );
	dlg->AddItem(_DlgStaticText_::Create(24, 20, x-60, 40, (char*)(*(int*)0x68C79C), o_Bigfont_Fnt->name, 7, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	int i_id = 0;
	int save_y_coords;
	int text_y = oHT*2;
	_DlgStaticText_* text_item = 0;
	
	// проходим по заданиям хижины провидца
	if ( questSH_counts ) 
	{
		for (int i = 0; i < questSH_counts; i++) 
		{
			_H3QuestSeerHut_* questSH = (_H3QuestSeerHut_*)(19*i + pQuestSH_start);
			
			if ( questSH->quest ) 
			{
				if ( questSH && (1 << player_id) & questSH->players_visited ) 
				{
					_HStr_ string;
					CALL_2(_HStr_*, __thiscall, 0x574440, questSH, &string);
					//CALL_3(_HStr_*, __thiscall, 0x574580, questSH, &string, player_id);

					if (!string.c_str)
						continue;

					if (i_id < 16) {
						text_y = oHT*2 + oHT*i_id +10;
						save_y_coords = text_y;
					} else {
						text_y = save_y_coords;
					}
					
					dlg->AddItem(text_item = _DlgStaticText_::Create(40, text_y, xt, oHT, string.c_str, o_Smalfont_Fnt->name, 1, i_id+50, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

					text_item->field_28 = 1; // значит, что передаем _H3QuestSeerHut_
					text_item->field_3C = (int)questSH; // передаем структуру _H3QuestSeerHut_

					if (i_id >= 16) {
						dlg->GetItem(i_id+50)->Hide();
					}

					i_id++;
				}
			}
		}
	}

	dlg->custom_data[1] = i_id +1; // id первого айтема стражей в списке
	// проходим по стражам задания
	if ( questGG_counts ) 
	{
		for (int i = 0; i < questGG_counts; i++) 
		{
			_H3QuestGuard_* questGG = (_H3QuestGuard_*)(5*i + pQuestGG_start);

			if ( questGG->quest ) 
			{
				if ( (1 << player_id) & questGG->players_visited ) 
				{				
					_HStr_ string;
					CALL_2(_HStr_*, __thiscall, 0x573130, questGG, &string);
					//CALL_3(_HStr_*, __thiscall, 0x573410, questGG, &string, player_id);

					if (!string.c_str)
						continue;					

					if (i_id < 16) {
						text_y = oHT*2 + oHT*i_id +10;
						save_y_coords = text_y;
					} else {
						text_y = save_y_coords;
					}

					dlg->AddItem(text_item = _DlgStaticText_::Create(41, text_y, xt, oHT, string.c_str, o_Smalfont_Fnt->name, 1, i_id+50, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

					text_item->field_28 = 2; // значит, что передаем _H3QuestGuard_
					text_item->field_3C = (int)questGG; // передаем структуру _H3QuestGuard_

					if (i_id >= 16) {
						dlg->GetItem(i_id+50)->Hide();
					}

					i_id++;
				}
			}
		}
	}
	
	int steps = i_id - 15;
	if (steps < 1)
		steps = 0;

	// id = 4
	_DlgScroll_* questDlgScroll = _DlgScroll_::Create( x-38, 30, 16, y-60, 4, steps, (_ptr_)Dlg_QuestLog_Scroll, 0, 0, 0); // создать скролл
	dlg->AddItem(questDlgScroll);
	if ( steps < 1 )
		dlg->GetItem(4)->SetEnabled(0);

	// запоминаем параметры для организации прокрутки
	_ScrollItemsShow_ scroll_items;
	scroll_items.item_id_start = 50;
	scroll_items.item_id_end = 50 + i_id;
	scroll_items.item_id_show_start = 50;
	scroll_items.item_id_show_end = 66;
	scroll_items.last_step = 0;
	o_ScrollItemsShow = (_ScrollItemsShow_*)&scroll_items;

	int x_center = x / 2; // вычисляем середину ширины диалога
	dlg->AddItem(_DlgStaticPcx8_::Create(x_center -41, dlg->height -51, 0, "Box64x30.pcx"));
	dlg->AddItem(_DlgButton_::Create(x_center -40, dlg->height -50, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));

	// (id = 115) подсказка в статус баре	
	// dlg->AddItem(_DlgStaticTextPcx8ed_::Create(8, dlg->height -18 -8, dlg->width - 16, 18, o_NullString, n_smallfont2, "AdRollvr.pcx", 1, 115, ALIGN_H_CENTER | ALIGN_V_CENTER) ); // HD_TStat.pcx

	dlg->Run();
	dlg->Destroy(TRUE);

	o_QuestLogDlg = 0;

	// возвращаем курсор
	b_MouseMgr_SetCursor(cursor_f, cursor_t);

	// LoHook
	c->return_address = 0x52EB6D;
	return NO_EXEC_DEFAULT;

	// HiHook
	// return CALL_1(int, __fastcall, hook->GetDefaultFunc(), player_id);
	// return 0;
}

// лечим создание строки с флагом игрока
_HStr_* __stdcall Dlg_QuestLog_Fix_BePlayerText(HiHook* hook, _H3Quest_* quest, _HStr_* H3string) 
{
	// 0x6A7E60 названия цветов игроков
	int player_id = quest->data.bePlayer;
	H3string->c_str = 0;
	H3string->length = 0;
	H3string->size = 0;
	H3string->Set(o_GameMgr->GetPlayerColorName(player_id));
	
	return H3string;
}

int __stdcall Dlg_ProcessAction_SetExit(HiHook* hook, _Dlg_* dlg, _EventMsg_* msg) 
{
	int ret = CALL_2(int, __thiscall, hook->GetDefaultFunc(), dlg, msg);

	if (quest_show_coords[0] == 1) {
		quest_show_coords[0] = 2; // один раз уже закрыли. Хватит :)
		msg->subtype = 10;
		msg->item_id = 10;
		dlg->Close(msg);
		return 2;
	}
	return ret;
}

int __stdcall AdvMgr_Proc_ShowQuest(LoHook* h, HookContext* c)
{
	if (quest_show_coords[0] > 0) {
		quest_show_coords[0] = 0; 
		_AdvMgr_* am = o_AdvMgr;
		am->HeroActive_DeMobilize();
		am->pack_xyz_look = quest_show_coords[1];
		CALL_2(char, __thiscall, 0x40F7D0, am, 0);
		CALL_6(void, __thiscall, 0x4136F0, am, 1, 1, 0, 0, 0);
		CALL_3(void, __thiscall, 0x40F1D0, am, 0, 0);
		// am->HeroActive_Mobilize();
	} 
	return EXEC_DEFAULT;
} 



void StartPlugin()
{
	// создаем новый диалог квестов
	_PI->WriteLoHook(0x52E953, Dlg_BuildQuestLog );

	// лечим создание строки с флагом игрока	
	_PI->WriteHiHook(0x5728B0, SPLICE_, EXTENDED_, THISCALL_, Dlg_QuestLog_Fix_BePlayerText);

	// реализация перевода взгляда
	_PI->WriteHiHook(0x4DD56A, CALL_, EXTENDED_, THISCALL_, Dlg_ProcessAction_SetExit);
	_PI->WriteLoHook(0x40883A, AdvMgr_Proc_ShowQuest); 

	return;
}

BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	static _bool_ initialized = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!initialized)
		{
			initialized = 1;

			_P = GetPatcher();
			_PI = _P->CreateInstance("QuestDlg");

			StartPlugin();
			
		}
		break;

	case DLL_PROCESS_DETACH:
		if (initialized)
			initialized = 0;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

//sprintf(o_TextBuffer, "id = %d", msg->item_id );
//b_MsgBox(o_TextBuffer, 1);


// #define o_WTF  *(_dword_*)0x69FE55

// _PI->WriteDword(0x724A9F, -2);


//int __stdcall LoHook_Name(LoHook* h, HookContext* c)
//{
//	return EXEC_DEFAULT;
//} 
//
//_PI->WriteLoHook(0x5825A5, LoHook_Name);




//int __stdcall HiHook_Name(HiHook* hook, _Dlg_* dlg) 
//{
//	int ret = CALL_1(int, __thiscall, hook->GetDefaultFunc(), dlg);
//	return ret;
//}
// 
// _PI->WriteHiHook(0x4FB930, SPLICE_, EXTENDED_, THISCALL_, HiHook_Name);







