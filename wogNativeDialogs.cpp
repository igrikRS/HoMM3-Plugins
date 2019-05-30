//////////////////////////////////
// Author: [igrik] 				//
// Date:   01.12.2017г. 		//
// Thanks: baratorch, ZVS, MoP	//
//////////////////////////////////

// #include "stdafx.h"
#include "..\..\include\homm3.h"
#include "..\..\include\HoMM3_Dlg.cpp"
#include "..\..\include\HoMM3_Extra.h"
#include "..\..\include\WogClasses.h"

#include "era.h"
using namespace Era;


Patcher* _P;
PatcherInstance* _PI;

// всё что касается HD мода
PatcherInstance* _HD;
_bool_ isHD = false;
#define hdv(type, name) _P->VarValue<type>((_cstr_)(name))

struct _TXT_;
_TXT_* txtresWOG;
_TXT_* WogNDlg_TXT;

_Fnt_* bigfont2;
_Fnt_* medfont2;
_Fnt_* smalfont2;

#define o_HD_X (*(_int_*)0x401448)
#define o_HD_Y (*(_int_*)0x40144F)

#define isRusLang (*(_int_*)0x80321C)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог командира /////////////// /////////////////////////////////////////////////

struct _DlgNPC_;
#define o_dlgNPC ((_DlgNPC_*)0x28604D8)

_DlgStaticDef_* CommDef;
_DlgStaticTextPcx8ed_* statbar = NULL;
bool art_npc_155 = false;
int num_skill_up;
int skill_up[6][3];
int klick[2];
int time_click;
int startDlg[3];
int redraw_Dlg_hero;
int textNCPHints[] = {29, 30, 31, 32, 33, 36, 37, 38, 39, 43, 44, 45, 50, 51, 57};


int __stdcall Y_New_CommanderDlg_Proc(_CustomDlg_* dlg_Npc, _EventMsg_* msg)
{
	int r = dlg_Npc->DefProc(msg);

	int npc_id = dlg_Npc->custom_data[0];		// получаем номер командира/героя
	_Npc_* npc = GetNpc(npc_id);				// получаем структуру командира
	_Hero_* hero = o_GameMgr->GetHero(npc_id);  // структура героя
	int temp_1, temp_2;							// временная переменная
	int choice_1, last_choice[1][1];

	// обновление подсказки при движении мыши (id = 10)
	if (msg->type == MT_MOUSEOVER)
		{
			_DlgItem_* it = dlg_Npc->FindItem(msg->x_abs, msg->y_abs);
			char* text = "";
			if (it)	
			{
				switch (it->id)
				{
					case 3:	 if (!dlg_Npc->custom_data[1]) text = txtresWOG->GetString(53); break; // подсказка Уволить командира
					case 4:	 text = Get_ITxt(215, 1); break; // подсказка для просмотра биографии
					case 6:  text = Get_ITxt(npc->type + 10, 1); break; // подсказка первого спец_умения
					case 7:  text = Get_ITxt(npc->type + 10, 2); break; // подсказка второго спец_умения
					case 8:  text = txtresWOG->GetString(73); break; // подсказка выберите навыки для повышения
					case 11: text = txtresWOG->GetString(74); break; // подсказка Имя командира

					case 12: text = Get_ITxt((npc->lvl_attack < 2 && art_npc_155 ? 2 : npc->lvl_attack) + 100, 1);      break; // подсказка дефа навыка атаки
					case 13: text = Get_ITxt((npc->lvl_defence < 2 && art_npc_155 ? 2 : npc->lvl_defence) + 106, 1);     break; // подсказка дефа навыка защиты
					case 14: text = Get_ITxt((npc->lvl_hit_points < 2 && art_npc_155 ? 2 : npc->lvl_hit_points) + 112, 1);  break; // подсказка дефа навыка здоровья
					case 15: text = Get_ITxt((npc->lvl_damage < 2 && art_npc_155 ? 2 : npc->lvl_damage) + 118, 1);	   break; // подсказка дефа навыка урона
					case 16: text = Get_ITxt((npc->lvl_spell_power < 2 && art_npc_155 ? 2 : npc->lvl_spell_power) + 124, 1); break; // подсказка дефа навыка силы магии
					case 17: text = Get_ITxt((npc->lvl_speed < 2 && art_npc_155 ? 2 : npc->lvl_speed) + 130, 1);       break; // подсказка дефа навыка скорости

					case 20: case 21: case 22: case 23: case 24: case 25: 	text = Get_ITxt(225, 1); break; // подсказка артефактов

					case 31: case 61: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(67), npc->now_level +1); text = o_TextBuffer; break;// подсказка уровня командира
					case 32: case 33: text = Get_ITxt(242, 1);       break; // подсказка класса командира
					case 34: case 35: sprintf(o_TextBuffer, "%s %s", txtresWOG->GetString(68), hero->name); text = o_TextBuffer; break;// подсказка хозяина героя 
					case 36: case 37: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(71), npc->now_expa); text = o_TextBuffer; break;// подсказка текущего опыта 
					case 38: case 39: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(72), CALL_1(int, __thiscall, 0x769260, npc)); text = o_TextBuffer; break;// подсказка следующего опыта 

					case 40 :case 41: text = txtresWOG->GetString(it->id +37); break; // подсказка надписей: навыки класса или артефакты
					case 42: case 43: text = txtresWOG->GetString(it->id +27); break; // подсказка надписей: основные_навыки или вторичные_навыки

					case 45: temp_1 = Get_NpcSkillPower(npc, 0); temp_2 = hero->attack;
							 if ( npc->type == 7 ) temp_2 += (temp_2/2);
							 sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(54), temp_1 + temp_2); text = o_TextBuffer; break;							// подсказка атака
					case 46: temp_1 = Get_NpcSkillPower(npc, 1); temp_2 = hero->defence;
							 if ( npc->type == 7 ) temp_2 += (temp_2/2);
							 sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(55), temp_1 + temp_2); text = o_TextBuffer; break;							// подсказка защита

					case 47: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(56), Get_NpcSkillPower(npc, 2)); text = o_TextBuffer; break;				// подсказка здоровье
					case 48: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(58), Get_NpcSkillPower(npc, 3)/2, Get_NpcSkillPower(npc, 3)); text = o_TextBuffer; break;	// подсказка сила урон
					case 49: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(59), Get_NpcSkillPower(npc, 4)); text = o_TextBuffer; break;				// подсказка сила магии
					case 50: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(60), Get_NpcSkillPower(npc, 6)); text = o_TextBuffer; break;				// подсказка сопротивление
					case 51: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(61), ((npc->specBon[0] & 0x10) ? 24 : 0)); text = o_TextBuffer; break;		// подсказка боезапас
					case 52: sprintf(o_TextBuffer, "%s %d", txtresWOG->GetString(57), Get_NpcSkillPower(npc, 5)); text = o_TextBuffer; break;				// подсказка скорость

					case 53: case 54: case 55: case 56: case 57: case 58: 
						temp_1 = ((_DlgStaticDef_*)dlg_Npc->GetItem(it->id))->def_frame_index;  // получаем кадр дефа
						text = Get_ITxt(textNCPHints[(temp_1-1)/2], temp_1 % 2 ? 1 : 2);
						break;	// подсказка вторичные навыки	

					case 73: case 74: case 75: case 76: case 77: case 78:
						temp_1 = it->id -73;
						if (skill_up[temp_1][2] < 10){
							temp_2 = (*(int*)(0x28620C0 +296*npc_id +56 +4*skill_up[temp_1][2]));
							text = Get_ITxt(temp_2 +skill_up[temp_1][2]*6 + 101, 1);		 
						}
						else {
							temp_2 = skill_up[temp_1][2] -10;
							text = Get_ITxt( textNCPHints[temp_2], 1);
						}	break; // подсказка навыков при повышении атаки/защиты/здоровья/урона/силы_магии/скорости


					case DIID_OK:	 text = txtresWOG->GetString(85); break; // Ok
					default: text = "";	break;
				}
			statbar->SetText(text);
			statbar->Draw();
			statbar->RedrawScreen();
			}
		}

	if (msg->type == MT_MOUSEBUTTON)
	{
		if (msg->subtype == MST_RBUTTONDOWN)
		{
			switch (msg->item_id)
			{
				case 3: if (!dlg_Npc->custom_data[1]) b_MsgBox(WogNDlg_TXT->GetString(17), MBX_RMC);	break;  // Уволить командира
				case 4: case 11: b_MsgBox(Get_ITxtBio(npc_id, 1), MBX_RMC);			break;  // ПКМ для просмотра биографи
				case 6: b_MsgBoxAndDef("Dlg_NPC1.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(6))->def_frame_index, Get_ITxt(npc->type + 19, 1));	break;  // ПКМ первого спец_умения
				case 7: b_MsgBoxAndDef("Dlg_NPC1.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(7))->def_frame_index, Get_ITxt(npc->type + 19, 2));	break;  

				// ПКМ дефа навыка атаки/защиты/здоровья/урона/силы_магии/скорости
				case 12:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(12))->def_frame_index, Get_ITxt(npc->lvl_attack  + (npc->lvl_attack < 2 && art_npc_155 ? 236 : 136), 1));			 break;	 
				case 13:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(13))->def_frame_index, Get_ITxt(npc->lvl_defence + (npc->lvl_defence < 2 && art_npc_155 ? 237 : 142), 1));		 break;		
				case 14:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(14))->def_frame_index, Get_ITxt(npc->lvl_hit_points + (npc->lvl_hit_points < 2 && art_npc_155 ? 238 : 148), 1));	 break;
				case 15:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(15))->def_frame_index, Get_ITxt(npc->lvl_damage + (npc->lvl_damage < 2 && art_npc_155 ? 239 : 154), 1));			 break;
				case 16:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(16))->def_frame_index, Get_ITxt(npc->lvl_spell_power + (npc->lvl_spell_power < 2 && art_npc_155 ? 240 : 160), 1)); break;
				case 17:  b_MsgBoxAndDef("Dlg_NPC2.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(17))->def_frame_index, Get_ITxt(npc->lvl_speed + (npc->lvl_speed < 2 && art_npc_155 ? 241 : 166), 1));			 break;

				case 20: case 21: case 22: case 23: case 24: case 25: // ПКМ артефактов
					b_MsgBoxD(CALL_2(char*, __thiscall, 0x769E0E, npc, msg->item_id -20), MBX_RMC, 8, npc->arts[(msg->item_id -20)][0]);
					break;  

				case 32: case 33: 
					b_MsgBox(Get_ITxt(npc->type + 186, 1), MBX_RMC);		
					break;  // ПКМ на описании класса

				case 53: case 54: case 55: case 56: case 57: case 58:
					temp_1 = (((_DlgStaticDef_*)dlg_Npc->GetItem(msg->item_id))->def_frame_index);
					b_MsgBoxAndDef("Dlg_NPC3.def", temp_1, Get_ITxt(textNCPHints[(temp_1-1)/2] +36, 1));
					break;		// ПКМ вторичные навыки


				case 73: case 74: case 75: case 76: case 77: case 78:
					temp_1 = msg->item_id -73;
					if (skill_up[temp_1][2] < 10){ // если повышение основных скилов 1-6
						temp_2 = (*(int*)(0x28620C0 +296*npc_id +56 +4*skill_up[temp_1][2]));
						b_MsgBoxAndDef("Dlg_NPC2.def", skill_up[temp_1][0], Get_ITxt(skill_up[temp_1][2]*6 +temp_2 +137, 1));
					}
					else { // если изучение дополнительных скиллов 1-15
						temp_2 = skill_up[temp_1][2] -10;
						b_MsgBoxAndDef("Dlg_NPC2.def", skill_up[temp_1][0], Get_ITxt(textNCPHints[temp_2] +36, 1));
					}
					break; // ПКМ дефа навыка при повышении атаки/защиты/здоровья/урона/силы_магии/скорости

				case DIID_OK: b_MsgBox(WogNDlg_TXT->GetString(dlg_Npc->custom_data[1] ? 19 :18), MBX_RMC); break; // ПКМ OK Выход
			}
		}
		if (msg->subtype == MST_LBUTTONCLICK){ // ЛКМ при отжатии
			if (msg->item_id == 3)	{
				b_MsgBox(txtresWOG->GetString(62), MBX_OKCANCEL); // увольняем командира
				if (o_WndMgr->result_dlg_item_id == DIID_OK){
					npc->on = 0;							
					return dlg_Npc->Close(msg);
				}
			}
		}

		if (msg->subtype == MST_LBUTTONDOWN) // ЛКМ при нажатии
		{
			if (!dlg_Npc->custom_data[1]) // НЕ повышение уровня
			{
				if ( o_MeID == hero->owner_id && startDlg[2] != 1 ) // если это наш командир и кнопка удалить неактивна
				{						
					switch (msg->item_id)
					{
						case 20: case 21: case 22: case 23: case 24: case 25: 
							sprintf(o_TextBuffer, "%s\n\n%s", CALL_2(char*, __thiscall, 0x769E0E, npc, msg->item_id -20), txtresWOG->GetString(79));
							b_MsgBoxD(o_TextBuffer, MBX_OKCANCEL, 8, npc->arts[(msg->item_id -20)][0]);

							if (o_WndMgr->result_dlg_item_id == DIID_OK){
								int Art[2];
								Art[0] = npc->arts[(msg->item_id -20)][0];
								Art[1] = -1;
								CALL_3(char, __thiscall, 0x4E3200, hero, &Art, -1); // hero->AddBackpackArtifact
								_DlgItem_* itm = dlg_Npc->GetItem(msg->item_id);
								npc->arts[(msg->item_id -20)][0] = 0;
								npc->arts[(msg->item_id -20)][1] = 0;
								dlg_Npc->RemoveItem(itm);	itm->Destroy(TRUE);
								redraw_Dlg_hero = 1;
								if(Art[0] == 155) {
									art_npc_155 = 0;
										for (char i = 0; i < 6; ++i ){ 
											int art_freme_skill = *(int*)(0x28620C0 +296*npc_id +56 +4*i);
											((_DlgStaticDef_*)dlg_Npc->GetItem(12+i))->def_frame_index = art_freme_skill +7*i;
											((_DlgStaticDef_*)dlg_Npc->GetItem(12+i))->RedrawScreen();

										}
								}
								dlg_Npc->Redraw(TRUE);
							}	break;
					}
				}
			}
			else // если повышение уровня
			{
				if (msg->item_id >= 73 && msg->item_id <= 78)
				{
					if (klick[0] == msg->item_id && (o_GetTime() - time_click) < 300 ){	
						// попробуем проиграть звук кнопки
						e_ClickSound();
						return dlg_Npc->Close(msg);
					}
					else {
						dlg_Npc->GetItem(DIID_OK)->SetEnabled(1);
						for (char i = 0; i < num_skill_up; ++i ){
							((_DlgStaticDef_*)dlg_Npc->GetItem(73 + i))->def_frame_index = skill_up[i][0];
							((_DlgStaticDef_*)dlg_Npc->GetItem(73 + i))->RedrawScreen();
						}

						((_DlgStaticDef_*)dlg_Npc->GetItem(msg->item_id))->def_frame_index = skill_up[msg->item_id -73][1];
						((_DlgStaticDef_*)dlg_Npc->GetItem(msg->item_id))->RedrawScreen();
						dlg_Npc->Redraw(1);
					}
					klick[0] = msg->item_id;
					klick[1] = msg->item_id -72;

					time_click = o_GetTime();
					
					if (skill_up[msg->item_id -73][2] < 10)
						 *(int*)(0x286069C +4*(msg->item_id -72)) = skill_up[msg->item_id -73][2];	
					else *(int*)(0x286069C +4*(msg->item_id -72)) = 0 -(skill_up[msg->item_id -73][2] -9);								

				}
			}
		}
	}

	// делаем подвижность дефа командира	
	if (((o_GetTime() - DwordAt(0x6989E8)) & 0x80000000) == 0) /*  && !npc->alive) */ 
		{
			CALL_1(void, __thiscall, 0x4EB140, CommDef);  
			dlg_Npc->Redraw(1);
			_dword_ t = o_GetTime() - DwordAt(0x6989E8);
			if ((signed int)t < 100) t = 100;
			DwordAt(0x6989E8) += t;
		}
	return r;
}

int Y_New_CommanderDlg_Show(int NHero_id, bool lvl_up, bool is_del_bttn)
{
	_DlgNPC_* ZVS_DLG = o_dlgNPC;

	_Npc_* npc = GetNpc(NHero_id);					// структура командира

	if (npc->on == -1 ){ b_MsgBox(Get_ITxt(182, 1), MBX_OK); return 0;}	// командиры отключены
	if (npc->on == 0 ) { b_MsgBox(Get_ITxt(181, 1), MBX_OK); return 0;}	// командир уволен

	// обнуление переменных
	art_npc_155 = false;

	_Hero_* hero = o_GameMgr->GetHero(NHero_id);	// структура героя
	int temp_1, temp_2, temp_3; // временные переменные;	// в начале размеры окна
	char* name_pcx;									// название фоновой картинки



		temp_1 = 0;
		num_skill_up = 0;
		// повышение силов скилы (id 73...78)
		for (char i = 0; i < 6; i++ ){ 
			if( CALL_2(_int_, __thiscall, 0x769317, npc, i) ){ // если есть что повышать из первичных 
				++num_skill_up;
			}			
		}	
		temp_2 = temp_1;
		while (temp_1 < 6){
			int d = CALL_2(_int_, __thiscall, 0x769008, npc, temp_1 - temp_2);
			if( d != -1 ){ // если есть что повышать из спецумений
				++num_skill_up;
			}	
		++temp_1;
		}

		if (num_skill_up > 6) 
			num_skill_up = 6;
	
	if (!lvl_up || !num_skill_up)	{
		name_pcx = "Dlg_NPCn.pcx";	
		temp_1 = 634; temp_2 = 505;
	}
	else {
		name_pcx = "Dlg_NPCu.pcx";	
		temp_1 = 634; temp_2 = 600;	
	}

	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, temp_1, temp_2, DF_SCREENSHOT | DF_SHADOW, Y_New_CommanderDlg_Proc);

	int med_hi = medfont2->height;

	// деф командира
	dlg->custom_data[0] = NHero_id; // передаем номер героя/командира в Dlg_Proc
	dlg->custom_data[1] = lvl_up;   // передаем повышалка ли в Dlg_Proc
	sprintf(o_TextBuffer, "%s", *(int*)(0x68295C+4+4*(o_CreatureInfo[npc->type + 174].town)) );
	dlg->AddItem(_DlgStaticPcx8_::Create(20, 70, 100, 130, 4, /*"CRBKGNEU.pcx"*/ o_TextBuffer )); // фон замка под дефом командира нейтральный (id = 4)
	dlg->AddItem(CommDef = _DlgStaticDef_::Create(-130, -80, 100, 130, 5, o_CreatureInfo[npc->type + 174].def_name, 0, 0, 18)); // шагающий деф командира (id = 5)

	// задник диалога под цвет игрока (id = 1)
	_DlgStaticPcx8_* fonPcx;
	dlg->AddItem(fonPcx = b_DlgStaticPcx8_Create(0, 0, dlg->width, dlg->height, 1, name_pcx, 2048));
	if (hero->owner_id != -1)
		 fonPcx->Colorize(hero->owner_id);
	else fonPcx->Colorize(o_GameMgr->GetMeID());
	

	// кнопки ок и уволить
	_DlgButton_* bttnOK;
	// bttnOK = _DlgButton_::Create(285, dlg->height -76, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 0, 2); // кнопка ok (id = DIID_OK) ENTER
	bttnOK = b_DlgButton_Create(285,  dlg->height -76, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 0, 2);
	// и сразу делаем выход и на ESC и ENTER
	int hotkey = 1;		CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	    hotkey = 28;	CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	dlg->AddItem(bttnOK);

	dlg->AddItem(_DlgButton_::Create(564, dlg->height -76, 51, 35, 3, "Dlg_NPCdel.def", 0, 1, 0, 83, 2));   // кнопка удалить командира (id = 3)
	if (lvl_up || is_del_bttn || o_ActivePlayerID != hero->owner_id) dlg->GetItem(3)->SetEnabled(0); 

	CALL_3 (void, __thiscall, 0x50CEA0, o_MouseMgr, 0, 0);  // установить курсор(0,0)
	CALL_2 (void, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	dlg->AddItem(_DlgStaticText_::Create(20, 70, 100, 130, "", "medfont2.fnt", 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));  // подсказка на дефе (пустышка, но по ней ПКМ определяет id и выдает окно биографии)
	
	// дефы спец_умений командира
	dlg->AddItem(_DlgStaticDef_::Create(395, 113, 43, 43, 6, "Dlg_NPC1.def", npc->type *2, 0, 0x12));    // первый деф спец_умений (id = 6)
	dlg->AddItem(_DlgStaticDef_::Create(504, 113, 43, 43, 7, "Dlg_NPC1.def", npc->type *2 +1, 0, 0x12)); // первый деф спец_умений (id = 7)

	// подсказка в статус баре	
	statbar = _DlgStaticTextPcx8ed_::Create(8, dlg->height -18 -8, dlg->width - 16, 18, "", "smalfont2.fnt", "AdRollvr.pcx", 1, 10, ALIGN_H_CENTER | ALIGN_V_CENTER); // HD_TStat.pcx
	dlg->AddItem(statbar);

	if ( !(ZVS_DLG->Flags & 1) ){ // текстовые вставки " МЕРТВ!!! "  (id = 8, 9)
		dlg->AddItem(_DlgStaticText_::Create(131, 20, 370, med_hi, txtresWOG->GetString(66), "medfont2.fnt", 27, 8, ALIGN_H_LEFT  | ALIGN_V_CENTER, 0)); 
		dlg->AddItem(_DlgStaticText_::Create(131, 20, 370, med_hi, txtresWOG->GetString(66), "medfont2.fnt", 27, 9, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));	
	}
	// имя командира (id = 11)
	dlg->AddItem(_DlgStaticText_::Create(131, 20, 370, med_hi, ZVS_DLG->Name, "medfont2.fnt", 7, 11, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); 

	// артефакты (id 20...25)
	// bool art_npc_155 = false;
	for (char i = 0; i < 6; ++i ){ 
		if (npc->arts[i][0] > 0 )
			dlg->AddItem(_DlgStaticDef_::Create(330 +48*i, 184, 45, 45, 20+i, "artifact.def", npc->arts[i][0], 0, 0x12)); 
		if (npc->arts[i][0] == 155 )
			art_npc_155 = true;			
	}

	// скилы (id 12...17)
	for (char i = 0; i < 6; ++i ){ 
		int art_freme_skill = *(int*)(0x28620C0 +296*NHero_id +56 +4*i);
		if (art_freme_skill < 2 && art_npc_155)
			art_freme_skill = 6;
		dlg->AddItem(_DlgStaticDef_::Create(40 +96*i, 270, 70, 70, 12+i, "Dlg_NPC2.def", art_freme_skill +7*i, 0, 0x12));
	}

	dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, med_hi, txtresWOG->GetString(67), "medfont2.fnt", 7, 31, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // уровень_текст (id = 31)
	sprintf(o_TextBuffer, "%d", npc->now_level +1);
	dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, med_hi, o_TextBuffer, "medfont2.fnt", 1, 61, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // уровень_число (id = 61)

	dlg->AddItem(_DlgStaticText_::Create(16, 205, 104, med_hi, txtresWOG->GetString(65), "medfont2.fnt", 7, 32, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // класс_текст1 (id = 32)
	dlg->AddItem(_DlgStaticText_::Create(16, 220, 104, med_hi, Get_ITxt(npc->type +216, 1), "medfont2.fnt", 1, 33, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // класс_текст2 (id = 33)

	dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, med_hi, txtresWOG->GetString(68), "medfont2.fnt", 7, 34, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Хозяин_текст (id = 34)
	dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, med_hi, hero->name, "medfont2.fnt", 1, 35, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // Хозяин_имя (id = 35)
	
	dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, med_hi, txtresWOG->GetString(71), "medfont2.fnt", 7, 36, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Текущий_опыт_текст (id = 36)
	sprintf(o_TextBuffer, "%d", o_Npc[NHero_id].now_expa);
	dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, med_hi, o_TextBuffer, "medfont2.fnt", 1, 37, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // Текущий_опыт_число (id = 37)	

	dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, med_hi, txtresWOG->GetString(72), "medfont2.fnt", 7, 38, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Следующий_уровень_текст (id = 38)

	sprintf(o_TextBuffer, "%d", CALL_1(int, __thiscall, 0x769260, npc));
	dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, med_hi, o_TextBuffer, "medfont2.fnt", 1, 39, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));   // Следующий_уровень_число (id = 39)	

	dlg->AddItem(_DlgStaticText_::Create(377, 91, 192, med_hi, txtresWOG->GetString(77), "medfont2.fnt", 7, 40, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Навыки_класса_текст (id = 40)
	dlg->AddItem(_DlgStaticText_::Create(377, 160, 192, med_hi, txtresWOG->GetString(78), "medfont2.fnt", 7, 41, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Артефакты_текст (id = 41)

	dlg->AddItem(_DlgStaticText_::Create(133, 247, 366, med_hi, txtresWOG->GetString(69), "medfont2.fnt", 7, 42, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Основные_навыки_текст (id = 42)
	dlg->AddItem(_DlgStaticText_::Create(133, 344, 366, med_hi, txtresWOG->GetString(70), "medfont2.fnt", 7, 43, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Вторичные_навыки_текст (id = 43)
	
	temp_1 = Get_NpcSkillPower(npc, 0); 	temp_2 = hero->attack;
	if ( npc->type == 7 ) temp_2 += (temp_2/2);
	sprintf(o_TextBuffer, "%d (%d)", temp_1, temp_1 + temp_2);
	dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 65, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Атака_число (id = 65)
	dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, med_hi, txtresWOG->GetString(54), "medfont2.fnt", 7, 45, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Атака_текст (id = 45)

	temp_1 = Get_NpcSkillPower(npc, 1); 	temp_2 = hero->defence;
	if ( npc->type == 7 ) temp_2 += (temp_2/2);
	sprintf(o_TextBuffer, "%d (%d)", temp_1, temp_1 + temp_2);
	dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 66, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Защита_число (id = 66)
	dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, med_hi, txtresWOG->GetString(55), "medfont2.fnt", 7, 46, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Защита_текст (id = 46)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 2));
	dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 67, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Здоровье_число (id = 67)
	dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, med_hi, txtresWOG->GetString(56), "medfont2.fnt", 7, 47, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Здоровье_текст (id = 47)

	temp_1 = Get_NpcSkillPower(npc, 3);
	sprintf(o_TextBuffer, "%d - %d", temp_1/2, temp_1);
	dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 68, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Урон_число (id = 68)
	dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, med_hi, txtresWOG->GetString(58), "medfont2.fnt", 7, 48, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Урон_текст (id = 48)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 4));
	dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 69, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Сила_магии_число (id = 69)
	dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, med_hi, txtresWOG->GetString(59), "medfont2.fnt", 7, 49, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Сила_магии_текст (id = 49)

	sprintf(o_TextBuffer, "%d%%", Get_NpcSkillPower(npc, 6));
	dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 70, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Сопротивление_число (id = 70)
	dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, med_hi, txtresWOG->GetString(60), "medfont2.fnt", 7, 50, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Сопротивление_текст (id = 50)

	sprintf(o_TextBuffer, "%d", (npc->specBon[0] & 0x10 ? 24 : 0));
	dlg->AddItem(_DlgStaticText_::Create(128, 201, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 71, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Боезапас_число (id = 71)
	dlg->AddItem(_DlgStaticText_::Create(129, 201, 192, med_hi, txtresWOG->GetString(61), "medfont2.fnt", 7, 51, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Боезапас_текст (id = 51)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 5));
	dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, med_hi, o_TextBuffer, "medfont2.fnt", 1, 72, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Скорость_число (id = 72)
	dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, med_hi, txtresWOG->GetString(57), "medfont2.fnt", 7, 52, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Скорость_текст (id = 52)


	temp_1 = 0; 
	// Вторичные_навыки дефы (id = 53 ... 58)
	for (char i = 0; i < 6; ++i )
	{ 
		for (char j = i+1; j < 6; ++j )
		{ 
			int p = CALL_3(signed int, __thiscall, 0x768EF0, npc, i, j); // 0x76A999 = FindSpecBonusBmp
			if (p != -1 && temp_1 <= 5) {		
				dlg->AddItem(_DlgStaticDef_::Create(57 +93*temp_1, 367, 46, 46, 53+temp_1, "Dlg_NPC3.def", (j*2-1)+i*(16-2*(i-1))/2 +p, 0, 0));
				++temp_1;
			}
		}
	}

	klick[0] = 0;
	klick[1] = 0;

	if (lvl_up)
	{	
		temp_1 = 0;
		time_click = 0;

		for (char i = 0; i < 6; i++ ){
			skill_up[i][0] = -1;
			skill_up[i][1] = -1;
			skill_up[i][2] = -1;
			*(int*)(0x286069C +4 +4*i) = -1;
		}

		if (num_skill_up != 0){
			dlg->GetItem(DIID_OK)->SetEnabled(0);
			
			dlg->AddItem(_DlgStaticText_::Create(133, 422, 366, med_hi, txtresWOG->GetString(73), "medfont2.fnt", 7, 8, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // подсказка выберите навыки для повышения (id = 8)

			// повышение силов скилы (id 73...78)
			for (char i = 0; i < 6; i++ ){ 
				if( CALL_2(_int_, __thiscall, 0x769317, npc, i) ){ // если есть что повышать из первичных 
					temp_2 = *(int*)(0x28620C0 +296*NHero_id +56 +4*i);
					dlg->AddItem(_DlgStaticDef_::Create(40 +96*temp_1, 445, 70, 70, 73+temp_1, "Dlg_NPC2.def", temp_2 + 7*i +1, 0, 0x12));
					skill_up[temp_1][0] = temp_2 + 7*i +1;
					skill_up[temp_1][1] = temp_2 + 7*i +1 +(41-i*2);
					skill_up[temp_1][2] = i;
					temp_1++;
				}			
			}	
			temp_2 = temp_1;
			while (temp_1 < 6){
				int d = CALL_2(_int_, __thiscall, 0x769008, npc, temp_1 - temp_2);
				if( d != -1 ){ // если есть что повышать из спецумений
					dlg->AddItem(_DlgStaticDef_::Create(40 +96*temp_1, 445, 70, 70, 73+temp_1, "Dlg_NPC2.def", 72 + d*2, 0, 0x12));
					skill_up[temp_1][0] = 72 + d*2;
					skill_up[temp_1][1] = 72 + d*2 +1;
					skill_up[temp_1][2] = d + 10;
				}	
			temp_1++;
			}
		}
	}

	dlg->Run();		
	dlg->Destroy(TRUE);
	return klick[1];
}
///////////////////////////////////////// конец диалога командира //////////////////////////////////////////////////////////
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

	dlg->AddItem(_DlgStaticText_::Create(20, 126, dlg->width -40, 20, WogNDlg_TXT->GetString(2), "bigfont2.fnt", 10, 5, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	for (int i = 0; i < 11; i++){
		dlg->AddItem(_DlgStaticText_::Create(24, 150 +i*20, dlg->width -48, 20, WogNDlg_TXT->GetString(3 +i), "medfont2.fnt", 7, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));	
	}

	dlg->AddItem(_DlgStaticText_::Create(24, 150, dlg->width -48, 20, o_pCreatureInfo[o_Mon].name_single, "medfont2.fnt", 1, 22, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));
	

	int runk = CALL_2(_int_, __cdecl, 0x727F20, 1000, o_Expo); 
	sprintf(o_TextBuffer, "%s (%d)", Get_ITxtExp(2+runk), runk);
	dlg->AddItem(_DlgStaticText_::Create(24, 170, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 23, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 190, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 24, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk +1, 0) - o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 210, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 25, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	int t1 = CALL_1(_int_, __cdecl, 0x7283D0, 1000); 
	int t2 = CALL_1(_int_, __cdecl, 0x727E20, 1000) * t1 / 100;
	sprintf(o_TextBuffer, "%d%% (%d)", t1, t2);
	dlg->AddItem(_DlgStaticText_::Create(24, 230, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 26, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Num); 
	dlg->AddItem(_DlgStaticText_::Create(24, 250, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 27, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	t1 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk, 0);
	t2 = 999999;
	if( t1 ){ t2 = o_Num * o_Expo / t1 - o_Num; }	
	sprintf(o_TextBuffer, "%d", t2); 
	dlg->AddItem(_DlgStaticText_::Create(24, 270, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 28, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727C00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 290, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 29, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727E00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 310, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 30, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t1 = CALL_1(_int_, __cdecl, 0x7283B0, 1000);
	sprintf(o_TextBuffer, "%d", t1); 
	dlg->AddItem(_DlgStaticText_::Create(24, 330, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 31, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t2 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, 10, 0);
	int t3 = 999999; 
	if ( t2 ){
		int t4 = o_Num * (t2 + t1);
		t3 = t4 / t2 - o_Num;
	}
	sprintf(o_TextBuffer, "%d", t3); 
	dlg->AddItem(_DlgStaticText_::Create(24, 350, dlg->width -48, 20, o_TextBuffer, "medfont2.fnt", 1, 32, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


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
    dlg->AddItem(_DlgStaticText_::Create(18, 30, 424, 30, o_TextBuffer, "bigfont2.fnt", 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
	
	int dy = 27;	

	for (int i = 0; i < 9; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 62+i*dy, 424, 24, 10+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(10+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "%s", Get_ITxtExp(341 +i));
		dlg->AddItem(_DlgStaticText_::Create(44, 66+i*dy, 394, 16, o_TextBuffer, "smalfont2.fnt", 1, 20+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

		dlg->AddItem(_DlgButton_::Create(22, 66+i*dy, 420, 18, 30+i, "radiobttn.def", 0, 1, 0, 0, 0));
	} 
	int s = CALL_1(_int_, __thiscall, 0x727BE0, o_CrExpo);	
	((_DlgButton_*)dlg->GetItem(30+s))->def_frame_index = 2;
	((_DlgButton_*)dlg->GetItem(30+s))->press_def_frame_index = 3;

	dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -20 -31, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -20 -30, 64, 30, 95, "iOkay.def", 0, 1, 1, 28, 2));

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
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +40)+4*(it->id-11))); text_bar = o_TextBuffer; break;

					case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
					case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: 
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +48)+4*(it->id-30))); text_bar = o_TextBuffer; break;
					
					case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57:
					case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65:
						sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +24)+4*((it->id-50)/2))); text_bar = o_TextBuffer; break;					

					case 95: sprintf(o_TextBuffer, "%s", txtresWOG->GetString(85)); text_bar = o_TextBuffer; break;
					
					default: text_bar = "";	break;
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
					b_MsgBoxAndDef("DlgExpMon.def", frame, o_TextBuffer);	break; 
					
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
		// dlg->AddItem(_DlgButton_::Create(dlg->width -30 -122, dlg->height -30 -40, 64, 30, 1, "DlgExpUp.def", 0, 1, 0, 22, 0));
		_DlgButton_* bttnUpg;
		bttnUpg = b_DlgButton_Create(dlg->width -30 -122, dlg->height -30 -40, 64, 30, 1, "DlgExpUp.def", 0, 1, 0, 0, 0);
		// и сразу делаем выход и на U и SPACE
		int hotkey = 22;	CALL_4(int, __thiscall, 0x404230, &bttnUpg->hotkeys_struct, bttnUpg->hotkeys_end, 1, &hotkey);
			hotkey = 57;	CALL_4(int, __thiscall, 0x404230, &bttnUpg->hotkeys_struct, bttnUpg->hotkeys_end, 1, &hotkey);
		dlg->AddItem(bttnUpg);
	}

	// Подсказка (id = 2)
	dlg->AddItem(_DlgStaticText_::Create(8, dlg->height -26, dlg->width -34, 18, "", "smalfont2.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	// заголовок диалога (id = 3)
	sprintf(o_TextBuffer, "%s", *(int*)o_Dlg_ExpaMon);
    dlg->AddItem(_DlgStaticText_::Create(150, 14, 500, 30, o_TextBuffer, "bigfont2.fnt", 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	// нижнее описание монстра красным (id = 4)
	sprintf(o_TextBuffer, "%s", *(int*)(o_Dlg_ExpaMon +4));
    dlg->AddItem(_DlgStaticText_::Create(18, dlg->height - 76, dlg->width -200, 44, o_TextBuffer, /* "verd10b.fnt"*/ "medfont2.fnt" , 27, 4, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

	// картинка и рамка монстра (id = 5, 6)
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 5, "twcrport.def", Mon +2, 0, 0x12)); 
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 6, "twcrport.def", 1, 0, 0x12));  

	// название артефакта и рамка для него (id = 7,8)
    dlg->AddItem(_DlgStaticText_::Create(18, 100, 60, 15, o_GENRLTXT_TXT->GetString(616), "medfont2.fnt", 1, 7, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
	dlg->AddItem(_DlgStaticDef_::Create(22, 116, 8, "DlgExpMon.def", 159, 0, 0x12)); 
	
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
		"smalfont2.fnt", 7, 10, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
	}
	// элементы заполнения колонок (первый столбик) id = 11...(26)
	int y0 = 50;	int yy = 16;
	for (int i = 0; i < (str_txt+dop_str); i++){
		sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +36)+4*i));
		dlg->AddItem(_DlgStaticText_::Create(80, y0 + yy*i, 80, 16, o_TextBuffer, "smalfont2.fnt", 7, 11+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
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
			dlg->AddItem(_DlgStaticText_::Create(x0 + xx*j, y0 + yy*i, 54, 16, text, "smalfont2.fnt", 1, 30+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
		}
	}
	// нижние картинки с дефами (id 50 ... )
	int xd = 18;  int xxd = 366;
	int yd = 56 +16*str_txt +16*dop_str; 
	int yyd = 52;
	dop_str += is_art;
	if (dop_str > 0 || art_num > 0){
		for (int i = 0; i < (dop_str); i++){
			dlg->AddItem(_DlgStaticDef_::Create(xd, yd, 50+i*2, "DlgExpMon.def", *(int*)(*(int*)(o_Dlg_ExpaMon +20)+4*i), 0, 0x12)); 
			sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +16)+4*i));
			dlg->AddItem(_DlgStaticText_::Create(xd+54, yd, 330, 50, o_TextBuffer, "smalfont2.fnt", 7, 51+i*2, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); 
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Y_ChangeBmp_To_DefFrame()
{
		
		_PI->WriteDword(0x792048, 0); // E_ART.bmp
		int sdv = 7;
		_PI->WriteDword((0x71F86F +sdv), 1);  // E_SHOOTN.bmp
		_PI->WriteDword((0x71F89F +sdv), 2);  // N_SHOOTN.bmp

		_PI->WriteDword((0x71F94C +sdv), 3);  // E_CASTER.bmp
		_PI->WriteDword((0x71F97C +sdv), 4);  // N_CASTER.bmp

		_PI->WriteDword((0x71FA2D +sdv), 5);  // E_FEAR.bmp
		_PI->WriteDword((0x71FA5E +sdv), 6);  // N_FEAR.bmp

		_PI->WriteDword((0x71FB0B +sdv), 7);  // E_RETAIL1.bmp
		_PI->WriteDword((0x71FB3B +sdv), 8);  // N_RETAIL1.bmp

		_PI->WriteDword((0x71FBE8 +sdv), 9);  // E_DEFBON.bmp
		_PI->WriteDword((0x71FC18 +sdv), 10); // N_DEFBON.bmp

		_PI->WriteDword((0x71FCC5 +sdv), 11); // E_DWARF.bmp
		_PI->WriteDword((0x71FCF5 +sdv), 12); // N_DWARF.bmp

		_PI->WriteDword((0x71FDA8 +sdv), 13); // E_GOLEM.bmp
		_PI->WriteDword((0x71FDD8 +sdv), 14); // N_GOLEM.bmp

		_PI->WriteDword((0x71FE89 +sdv), 15); // E_DIST.bmp
		_PI->WriteDword((0x71FEBA +sdv), 16); // N_DIST.bmp

		_PI->WriteDword((0x71FF6B +sdv), 17); // E_OBST.bmp
		_PI->WriteDword((0x71FF9C +sdv), 18); // N_OBST.bmp

		_PI->WriteDword((0x72007B +sdv), 19); // E_FLY.bmp
		_PI->WriteDword((0x7200AC +sdv), 20); // N_FLY.bmp

		_PI->WriteDword((0x720156 +sdv), 21); // E_SHOOT.bmp
		_PI->WriteDword((0x720187 +sdv), 22); // N_SHOOT.bmp

		_PI->WriteDword((0x720231 +sdv), 23); // E_BREATH.bmp
		_PI->WriteDword((0x720262 +sdv), 24); // N_BREATH.bmp

		_PI->WriteDword((0x72030C +sdv), 25); // E_ALIVE.bmp
		_PI->WriteDword((0x72033D +sdv), 26); // N_ALIVE.bmp

		_PI->WriteDword((0x7203E7 +sdv), 27); // E_KING1.bmp
		_PI->WriteDword((0x720418 +sdv), 28); // N_KING1.bmp

		_PI->WriteDword((0x7204C2 +sdv), 29); // E_KING2.bmp
		_PI->WriteDword((0x7204F3 +sdv), 30); // N_KING2.bmp

		_PI->WriteDword((0x72059D +sdv), 31); // E_KING3.bmp
		_PI->WriteDword((0x7205CE +sdv), 32); // N_KING3.bmp

		_PI->WriteDword((0x720678 +sdv), 33); // E_MIND.bmp
		_PI->WriteDword((0x7206A9 +sdv), 34); // N_MIND.bmp

		_PI->WriteDword((0x720753 +sdv), 35); // E_MELEE.bmp
		_PI->WriteDword((0x720784 +sdv), 36); // N_MELEE.bmp

		_PI->WriteDword((0x72082E +sdv), 37); // E_FIRE.bmp
		_PI->WriteDword((0x72085F +sdv), 38); // N_FIRE.bmp

		_PI->WriteDword((0x720909 +sdv), 39); // E_DOUBLE.bmp
		_PI->WriteDword((0x72093A +sdv), 40); // N_DOUBLE.bmp

		_PI->WriteDword((0x7209E4 +sdv), 41); // E_RETAIL.bmp
		_PI->WriteDword((0x720A15 +sdv), 42); // N_RETAIL.bmp

		_PI->WriteDword((0x720ABF +sdv), 43); // E_MORAL.bmp
		_PI->WriteDword((0x720AF0 +sdv), 44); // N_MORAL.bmp

		_PI->WriteDword((0x720B9A +sdv), 45); // E_UNDEAD.bmp
		_PI->WriteDword((0x720BCB +sdv), 46); // N_UNDEAD.bmp

		_PI->WriteDword((0x720C75 +sdv), 47); // E_ROUND.bmp
		_PI->WriteDword((0x720CA6 +sdv), 48); // N_ROUND.bmp

		_PI->WriteDword((0x720D50 +sdv), 49); // E_DRAGON.bmp
		_PI->WriteDword((0x720D81 +sdv), 50); // N_DRAGON.bmp

		_PI->WriteDword((0x720E34 +sdv), 51); // E_HARPY.bmp
		_PI->WriteDword((0x720E65 +sdv), 52); // N_HARPY.bmp

		_PI->WriteDword((0x720F18 +sdv), 53); // E_CHAMP.bmp
		_PI->WriteDword((0x720F49 +sdv), 54); // N_CHAMP.bmp

		_PI->WriteDword((0x720FFC +sdv), 55); // E_MINOT.bmp
		_PI->WriteDword((0x72102D +sdv), 56); // N_MINOT.bmp

		_PI->WriteDword((0x7210E0 +sdv), 57); // E_SHOOTA.bmp
		_PI->WriteDword((0x721111 +sdv), 58); // N_SHOOTA.bmp

		_PI->WriteDword((0x7211C4 +sdv), 59); // E_FEARL.bmp
		_PI->WriteDword((0x7211F5 +sdv), 60); // N_FEARL.bmp

		_PI->WriteDword((0x7212A8 +sdv), 61); // E_REBIRTH.bmp
		_PI->WriteDword((0x7212D9 +sdv), 62); // N_REBIRTH.bmp

		_PI->WriteDword((0x72138C +sdv), 63); // E_UNIC.bmp
		_PI->WriteDword((0x7213BD +sdv), 64); // N_UNIC.bmp

		_PI->WriteDword((0x721484 +sdv), 65); // E_BLOCK.bmp
		_PI->WriteDword((0x7214B4 +sdv), 66); // N_BLOCK.bmp

		_PI->WriteDword((0x72155E +sdv), 67); // E_MANA.bmp
		_PI->WriteDword((0x72158E +sdv), 68); // N_MANA.bmp

		_PI->WriteDword((0x721638 +sdv), 69); // E_DBLOW.bmp
		_PI->WriteDword((0x721668 +sdv), 70); // N_DBLOW.bmp

		_PI->WriteDword((0x721A01 +sdv), 71); // E_HATE.bmp
		_PI->WriteDword((0x721A31 +sdv), 72); // N_HATE.bmp

		_PI->WriteDword((0x721C59 +sdv), 73); // E_TROLL.bmp
		_PI->WriteDword((0x721C89 +sdv), 74); // N_TROLL.bmp

		_PI->WriteDword((0x721E4C +sdv), 75); // E_CAST.bmp
		_PI->WriteDword((0x721E7C +sdv), 76); // N_CAST.bmp

		_PI->WriteDword((0x722037 +sdv), 77); // E_CAST1.bmp
		_PI->WriteDword((0x722067 +sdv), 78); // N_CAST1.bmp

		_PI->WriteDword((0x72224B +sdv), 77); // E_CAST1.bmp
		_PI->WriteDword((0x72227B +sdv), 78); // N_CAST1.bmp

		_PI->WriteDword((0x722436 +sdv), 79); // E_CAST2.bmp
		_PI->WriteDword((0x722466 +sdv), 80); // N_CAST2.bmp

		_PI->WriteDword((0x722643 +sdv), 79); // E_CAST2.bmp
		_PI->WriteDword((0x722673 +sdv), 80); // N_CAST2.bmp

		_PI->WriteDword((0x722A86 +sdv), 81); // E_BLESS.bmp
		_PI->WriteDword((0x722AB6 +sdv), 82); // N_BLESS.bmp

		_PI->WriteDword((0x72303C +sdv), 83); // E_RDEF.bmp
		_PI->WriteDword((0x72306C +sdv), 84); // N_RDEF.bmp

		_PI->WriteDword((0x72311F +sdv), 85); // E_DEATH.bmp
		_PI->WriteDword((0x72314F +sdv), 86); // N_DEATH.bmp

		_PI->WriteDword((0x72341C +sdv), 87); // E_BLOCK1.bmp
		_PI->WriteDword((0x72344C +sdv), 88); // N_BLOCK1.bmp

		_PI->WriteDword((0x7234F6 +sdv), 89); // E_BLOCK2.bmp
		_PI->WriteDword((0x723526 +sdv), 90); // N_BLOCK2.bmp

		_PI->WriteDword((0x723754 +sdv), 91); // E_SGTYPE.bmp
		_PI->WriteDword((0x723784 +sdv), 92); // N_SGTYPE.bmp

		_PI->WriteDword((0x7237D5 +sdv), 93); // E_SGNUM.bmp
		_PI->WriteDword((0x723805 +sdv), 94); // N_SGNUM.bmp

		_PI->WriteDword((0x7239C8 +sdv), 95); // E_SUMMON.bmp
		_PI->WriteDword((0x7239F8 +sdv), 96); // N_SUMMON.bmp

		_PI->WriteDword((0x723BBB +sdv), 97); // E_SUMMON1.bmp
		_PI->WriteDword((0x723BEB +sdv), 98); // N_SUMMON1.bmp

		_PI->WriteDword((0x723DAD +sdv), 99); // E_SUMMON2.bmp
		_PI->WriteDword((0x723DDD +sdv), 100); // N_SUMMON2.bmp 

		int adress = 0x792848;
		for (int i = 0; i < 58; i++){
			_PI->WriteDword(adress, 101+i);
			adress += 4;
		}

	return 1; 
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////// /////////Диалог IF:G //////////////////////////////////////////////////////////////////////////////////////// 

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
	if (num_str > 12) 
		num_str = 12; 

	int x = 435;
	int dy = 27;
	int y = 124 + dy *num_str; 
	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_IF_G_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// кнопка ok и её обводка
	dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, "Box64x30.pcx"));
	dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, 1, "iOkay.def", 0, 1, 1, 28, 2));

	// Титульный текст
	sprintf(o_TextBuffer, "%s", htxt);
	dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 46, o_TextBuffer, "medfont2.fnt", 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	int on_bttn = 0;
	for (int i = 0; i < num_str; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 66+i*dy, dlg->width -36, 24, 22+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(22+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "{%s}", *(int*)(txt +4*i));
		dlg->AddItem(_DlgStaticText_::Create(44, 70+i*dy, dlg->width -66, 16, o_TextBuffer, "smalfont2.fnt", 1, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
		on_bttn = *(int*)(state +4*i);
		dlg->AddItem(_DlgButton_::Create(22, 70+i*dy, dlg->width -44, 18, 34+i, chRAD ? "radiobttn.def" : "checkbox.def", on_bttn ? 2 : 0, on_bttn ? 3 : 1, 0, 0, 0));
	} 

	dlg->field_48 = chRAD;
	dlg->field_4C = num_str;
	dlg->field_50 = state;

	dlg->Run();
	dlg->Destroy(TRUE);
	b_MouseMgr_SetCursor(cursor_f, cursor_t);
	return 1;
}

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

	int x = 360;	int dy = 27;	int y = 83 +dy*count;
	_CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, Dlg_ChooseMonAttack_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());

	// запоминаем кадр курсора мыши
	int cursor_t = o_MouseMgr->Field<_int_>(+0x4C);
	int cursor_f = o_MouseMgr->Field<_int_>(+0x50);
	b_MouseMgr_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// Титульный текст
	sprintf(o_TextBuffer, "%s", *(int*)0x282E6F4);
	dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 46, o_TextBuffer, "medfont2.fnt", 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	for (int i = 0; i < count; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 66+dy*i, dlg->width -36, 24, 4+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(4+i)->SendCommand(5, 4);
			
		sprintf(o_TextBuffer, "{%s}", *(int*)(*(int*)0x282E6FC +4*i));
		dlg->AddItem(_DlgStaticText_::Create(44, 70+dy*i, dlg->width -66, 16, o_TextBuffer, "smalfont2.fnt", 1, 7+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

		dlg->AddItem(_DlgButton_::Create(22, 70+dy*i, dlg->width -44, 18, 10+i, "radiobttn.def", 0, 3, 0, 0, 0));
	}
	
	dlg->Run();
	dlg->Destroy(TRUE);
	b_MouseMgr_SetCursor(cursor_f, cursor_t);

	int ret = *(int*)0x282E700;
	return ret; 
} 

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
		dlg->AddItem(_DlgStaticText_::Create(x, y, 267 +wx, 19, ds->Pages[page]->ItemList[j]->Name, "medfont2.fnt", 2, 800+page*10+j, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

		// рамка
		int hy = ds->Pages[page]->ItemList[j]->ItemCount * dy +3;  
		b_YellowFrame_Create(dlg, x, y+19, 267 +wx, hy, 900+page*10+j, ON, o_Pal_Grey);

		// кнопки группы
		for (int i=0; i<ds->Pages[page]->ItemList[j]->ItemCount; i++) {
			sprintf(o_TextBuffer, "%s", ds->Pages[page]->ItemList[j]->ItemName[i]);
			dlg->AddItem(_DlgStaticText_::Create(x+25, y+22 +dy*i, 238 +wx, 16, o_TextBuffer, "smalfont2.fnt", 
				(ds->Pages[page]->ItemList[j]->ItemState[i] >= 2) ? 1 : 2, id +i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

			itState = ds->Pages[page]->ItemList[j]->ItemState[i];
			if (itState < 2) { defState = itState*2; }
			if (itState == 2) { defState = 4; }
			if (itState == 3) { defState = 5; }

			dlg->AddItem(_DlgButton_::Create(x+3, y+22 +dy*i, 262 +wx, 16, id+i+100, 
				(ds->Pages[page]->ItemList[j]->Type == 2) ? "radiobttn.def" : "checkbox.def", 
				defState, (itState < 2) ? (defState+1) : defState, 0, 0, 0)); 
			 /* dlg->AddItem(_DlgTextButton_::Create(x+3, y+22 +dy*i, 262 +wx, 16, id+i+100, 
				 (ds->Pages[page]->ItemList[j]->Type == 2) ? "radiobttn.def" : "checkbox.def", 
				 ds->Pages[page]->ItemList[j]->ItemName[i], "smalfont2.fnt", 
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
					o_LastChoosenPage = id; 
					dlg->Redraw();					
				}
				
			} // MST_LBUTTONDOWN

			if (msg->subtype == MST_LBUTTONCLICK) { // ЛКМ при отжатии
				int callBack = 0;
				if (msg->item_id == 5 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 4); }  // Выбрать все
				if (msg->item_id == 6 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 5); }	// Сбросить все
				if (msg->item_id == 7 ) { if (o_LastChoosenPage != -1) callBack = ds->GetListener(0, o_LastChoosenPage +1, 0, 2); }	// По умолчанию 
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
	
	statbarWoGOptions = _DlgStaticTextPcx8ed_::Create(7, dlg->height -26, dlg->width -14, 18, "", "smalfont2.fnt", "WoGOptions.pcx", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER); 
	dlg->AddItem(statbarWoGOptions); // подсказка в статус баре	

	dlg->AddItem(_DlgStaticText_::Create(214, 20, 370, 20, ds->Name, "bigfont2.fnt", 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // id = 3
	dlg->AddItem(_DlgStaticText_::Create(230, 50, 538, 468, ds->Intro, "bigfont2.fnt", 2, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	for (int i=0; i<8; i++) {
		if (ds->Pages[i]->Enabled) {
			dlg->AddItem(_DlgStaticText_::Create(23, 49 +60*i, 192, 49, ds->Pages[i]->Name, "smalfont2.fnt", 1, 41 +i, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));	// id = 41...48
			
			Create_WoGDlgSetup_ElemOnPage(dlg, i, ds); // функция построения элементов диалога текущей в цикле страницы
			ShowHide_WoGDlgSetup_ElemOnPage(dlg, i, ds, 0); // скрываем все элементы на странице
		}
	}
	if (ds->ButtonsStates[5] == 1) {dlg->AddItem(_DlgButton_::Create(375, 528, 64, 30, 5, "WoGBttn.def", 15, 16, 0, HK_B, 0)); } // id = 5
	if (ds->ButtonsStates[4] == 1) {dlg->AddItem(_DlgButton_::Create(440, 528, 64, 30, 6, "WoGBttn.def", 18, 19, 0, HK_N, 0)); } // id = 6
	if (ds->ButtonsStates[2] == 1) {dlg->AddItem(_DlgButton_::Create(510, 528, 64, 30, 7, "WoGBttn.def", 12, 13, 0, HK_R, 0)); } // id = 7	
	if (ds->ButtonsStates[8] == 1) {dlg->AddItem(_DlgButton_::Create(580, 528, 64, 30, 8, "WoGBttn.def", 6, 7, 0, HK_L, 0)); }   // id = 8
	if (ds->ButtonsStates[1] == 1) {dlg->AddItem(_DlgButton_::Create(645, 528, 64, 30, 9, "WoGBttn.def", 9, 10, 0, HK_S, 0)); }  // id = 9
	if (ds->ButtonsStates[0] == 1) {dlg->AddItem(_DlgButton_::Create(715, 528, 64, 30, DIID_OK, "WoGBttn.def", 0, 1, 1, HK_ENTER, 2)); } // id = 30725

	//_DlgScroll_* wogOptScroll = _DlgScroll_::Create(26, 534, 334, 16, 11, 8, (_ptr_)Dlg_WoG_Options_Scroll, 0, 0, 0); // создать ползунок
	//dlg->AddItem(wogOptScroll);

	o_LastChoosenPage = -1;   // страница диалога
	setYellowFrames(dlg, o_LastChoosenPage); // создаем массово желтые рамки (dlg, номер страницы вог диалога)

	dlg->Run();
	dlg->Destroy(TRUE); 
	o_MeID = color; 
} 



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

	dlg->AddItem(_DlgStaticText_::Create(20, 20, x-40, 60, txtresWOG->GetString(22) , "medfont2.fnt", 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

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
///////////////////////////////////////// Диалог сброса артефактов на землю ////////////////////////////////////////////////

int __stdcall Y_WoGDlg_ChooseArt_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);

	_Hero_* hero = (_Hero_*)dlg->custom_data[1];

	if (msg->type == MT_MOUSEOVER)	{
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
		char* text_bar = "";
		if (it)	
		{
			if (it->id >= 10 && it->id < 74) {
				if (hero->backpack_art[it->id - 10].id != 1) {
					sprintf(o_TextBuffer, "%s", o_ArtInfo[hero->backpack_art[it->id - 10].id].name);
				} else {
					sprintf(o_TextBuffer, "%s: %s", o_ArtInfo[hero->backpack_art[it->id - 10].id].name, o_Spells[hero->backpack_art[it->id - 10].mod].name);
				}
				text_bar = o_TextBuffer;
			}
			else if ( it->id == DIID_CANCEL ) {
				sprintf(o_TextBuffer, "%s", txtresWOG->GetString(12));
				text_bar = o_TextBuffer;
			}
			else if ( it->id == DIID_OK ) {
				sprintf(o_TextBuffer, "%s", txtresWOG->GetString(11));
				text_bar = o_TextBuffer;
			} 
			else text_bar = "";				

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
					sprintf(o_TextBuffer, "%s", o_ArtInfo[hero->backpack_art[msg->item_id - 10].id].description);
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
		sprintf(o_TextBuffer, "%s", CALL_3(char*, __cdecl, 0x77710B, 223, 0, (int)0x7C8E3C) );
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
	
	sprintf(o_TextBuffer, "%s", CALL_3(char*, __cdecl, 0x77710B, 222, 0, (int)0x7C8E3C) );
	dlg->AddItem(_DlgStaticText_::Create(24, 20, dlg->width -48, 44, o_TextBuffer, "medfont2.fnt", 1, 1, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	dlg->AddItem(_DlgStaticText_::Create(8, dlg->height -26, dlg->width -16, 18, "", "smalfont2.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

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
					dlg->AddItem(_DlgStaticDef_::Create(19 +d*k, 70 +d*j, 44, 44, 10+i, "artifact.def", hero->backpack_art[i].id, 0, 0x12));
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


    dlg->AddItem(_DlgStaticPcx8_::Create(xz -80, dlg->height -74, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(xz -79, dlg->height -73, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));
    dlg->GetItem(DIID_OK)->SetEnabled(0); 

    dlg->AddItem(_DlgStaticPcx8_::Create(xz +16, dlg->height -74, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(xz +17, dlg->height -73, 64, 30, DIID_CANCEL, "iCancel.def", 0, 1, 1, 1, 2)); 


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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог проклятий и благословлений ////////////////////////////////////////////////

#define curse_C(i) (*(_int_*)(0x2807628 -4 +4*i) )
#define curse_H(i) (*(_int_*)(0x27FE680 -4 +4*i) )
#define curse_T(i) (*(_int_*)(0x2807128 -4 +4*i) )

int __stdcall Y_WoGCurseDlg_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
	int r = dlg->DefProc(msg);

	if (msg->type == MT_MOUSEOVER)	{
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);
		char* text_bar = "";
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
			else text_bar = "";				

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

	dlg->AddItem(_DlgStaticText_::Create(8, y -62, x -102, 48, "", "smalfont2.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

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

	dlg->AddItem(_DlgStaticPcx8_::Create(x-101, y-54, 0, "Box64x30.pcx"));
    // dlg->AddItem(_DlgButton_::Create(x-100, y-53, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 28, 2));
	_DlgButton_* bttnOK;
	bttnOK = b_DlgButton_Create(x-100, y-53, 64, 30, DIID_OK, "iOkay.def", 0, 1, 1, 0, 2);
	// и сразу делаем выход и на ESC и ENTER
	int hotkey = 1;		CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	    hotkey = 28;	CALL_4(int, __thiscall, 0x404230, &bttnOK->hotkeys_struct, bttnOK->hotkeys_end, 1, &hotkey);
	dlg->AddItem(bttnOK);

	dlg->Run();

	dlg->Destroy(TRUE);

	c->return_address = 0x754E71;
	return NO_EXEC_DEFAULT;
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

int __stdcall Y_Dlg_CommanderWoG(HiHook* hook, _Npc_* npc, int is_lvlup, int is_del_bttn, int flags)
{
	CALL_4(void*, __thiscall, hook->GetDefaultFunc(), npc, is_lvlup, is_del_bttn, flags); 

	startDlg[0] = npc->id;
	startDlg[1] = is_lvlup;
	startDlg[2] = is_del_bttn;

	return 1; 
}

_int_ __stdcall Y_DlgNPC_SetResult(LoHook* h, HookContext* c)
{
	redraw_Dlg_hero = 0;
	c->eax = Y_New_CommanderDlg_Show(startDlg[0], startDlg[1], startDlg[2]);

	if(o_HeroDlg_Hero && startDlg[1] == 0 && redraw_Dlg_hero == 1){
		CALL_0(void, __cdecl, 0x4DB460);
		o_CurrentDlg->Redraw(TRUE);
	} 
	c->return_address = 0x773749; 
	return NO_EXEC_DEFAULT; 
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

// ===============================================================================
//                              добавление AIValue командира
// ===============================================================================

_int64_ getAIValue_NPC(int heroID){

	if ( heroID < 0 && heroID > 155 )
		return 0; // выход, если номер героя неправильный

	_Npc_* npc = GetNpc(heroID);				// структура командира
	if ( npc->on < 1 || npc->alive ) return 0;	// выход, если командиров нет (-1), уволен или мертв

	_Hero_* hero = o_GameMgr->GetHero(heroID);

	_int64_ value = 0;

	int attNPC = Get_NpcSkillPower(npc, 0);		attNPC += hero->attack;	
	int defNPC = Get_NpcSkillPower(npc, 1);		defNPC += hero->defence;
	int hpNPC = Get_NpcSkillPower(npc, 2);	
	int damageNPC = Get_NpcSkillPower(npc, 3);	
	int speedNPC = Get_NpcSkillPower(npc, 5);								
	int isShoot = npc->specBon[0] & 0x10 ? 2 : 1;

	value = (_int64_)((attNPC * damageNPC * speedNPC * isShoot + defNPC * hpNPC) >> 2);
	return value;
}

bool check_AIValue_isNotHero = false;

_int64_ __stdcall get_AIValue_Hook(HiHook* hook, int army)
{
	_int64_ value;

	int armyNPC = army;

	value = CALL_1(_int64_, __thiscall, hook->GetDefaultFunc(), army);

	if (!check_AIValue_isNotHero) {
		int heroID = *(int*)(armyNPC -119);

		if ( heroID >= 0 && heroID < 156)
			value += getAIValue_NPC(heroID);

	} 
	check_AIValue_isNotHero = false;
	return value; 
}


int __stdcall get_AIValue_And_NPC_Error(LoHook* h, HookContext* c)
{
	check_AIValue_isNotHero = true;
	return EXEC_DEFAULT;
} 

int __stdcall get_Fight_Value_Hook(LoHook* h, HookContext* c)
{
	_Hero_* hero = o_BattleMgr->hero[c->ebx]; 

	if (!hero) return EXEC_DEFAULT; 

	c->ecx += getAIValue_NPC(hero->id);

	return EXEC_DEFAULT;
} 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// окно обмена героями в городе по клавише E 
bool inTownDlg;

char __stdcall Y_DlgTown_Proc(HiHook* hook, _TownMgr_* tm, _EventMsg_* klick)
{
   char res = CALL_2(char, __thiscall, hook->GetDefaultFunc(), tm, klick);
   inTownDlg = false;

   if (res) {
      if (klick->type == 2 && klick->subtype == HK_E) {
         int heroU_id = tm->town->up_hero_id;
         int heroD_id = tm->town->down_hero_id;

         if ( heroU_id != -1 && heroD_id != -1) {
            _Hero_* heroU = o_GameMgr->GetHero(heroU_id);
            _Hero_* heroD = o_GameMgr->GetHero(heroD_id);

            inTownDlg = true;

            if ( *(int*)((int)o_ExecMgr +4) != (int)o_WndMgr ) {
               *(int*)((int)o_TownMgr +4) = (int)o_AdvMgr;
               *(int*)((int)o_TownMgr +8) = (int)o_WndMgr;

               *(int*)((int)o_AdvMgr +4) = NULL;
               *(int*)((int)o_AdvMgr +8) = (int)o_WndMgr;            

               *(int*)((int)o_WndMgr +4) = (int)o_TownMgr;
               *(int*)((int)o_WndMgr +8) = (int)o_MouseMgr;
            }
			if ( isHD ) { // отключаем некоторые кнопки в HD версии диалога обмена
				hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 1;
			}
			
			tm->dlg->GetItem(30720)->SetEnabled(false); // отключаем кнопку OK в городе

            CALL_2(void, __fastcall, 0x4A25B0, heroU, heroD);
            CALL_3(void, __thiscall, 0x4AAA60, o_TownMgr, heroU, heroD);
			
			tm->dlg->GetItem(30720)->SetEnabled(true); // включаем кнопку OK в городе

			if ( isHD ) { // включаем некоторые кнопки в HD версии диалога обмена
				hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 0;
			}

            CALL_1(void, __thiscall, 0x5D5930, o_TownMgr);       
            CALL_1(void, __thiscall, 0x5D5810, o_TownMgr);   

            inTownDlg = false;

            return 1;   
         }
      }
   }

   return res;
}

int __stdcall Y_Dlg_HeroesMeet(LoHook* h, HookContext* c)
{   
   if ( inTownDlg ) { // пропускаем обновление экрана
      c->return_address = 0x4AAC2A;
      return NO_EXEC_DEFAULT;
   }
   return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// показ предполагаемого количества убитых монстров при атаке и стрельбе	
_int_ damLo;
_int_ damHi;

int __stdcall Y_Battle_SetHintAttackWillKilled(LoHook* h, HookContext* c)
{
	_int_ fullHealth = 0; 
	_int_ willKilledLo = 0;
	_int_ willKilledHi = 0;

	_int_ lost;

	_BattleStack_* stackD = (_BattleStack_*)c->esi;
	fullHealth = stackD->GetFullHealth(0);

	if (fullHealth > damHi){
		if (stackD->creature.hit_points > 1) {
			lost = (fullHealth - damHi) / stackD->creature.hit_points;
			if ((fullHealth - damHi) % stackD->creature.hit_points > 0)
				lost += 1;
		}
		else lost = fullHealth - damHi;
		willKilledHi = stackD->count_current - lost;
	} else willKilledHi = stackD->count_current; 

	if (damHi != damLo) {
		if (fullHealth > damLo){
			if (stackD->creature.hit_points > 1) {
				lost = (fullHealth - damLo) / stackD->creature.hit_points;
				if ((fullHealth - damLo) % stackD->creature.hit_points > 0)
					lost += 1;
			}
			else lost = fullHealth - damLo;
			willKilledLo = stackD->count_current - lost;
		} else willKilledLo = stackD->count_current;
	}
	
	if (damHi == damLo || willKilledHi == willKilledLo) {
		if (willKilledHi > 9999) {			
			sprintf((char*)c->ecx, "%s, %s %dk", c->ecx, WogNDlg_TXT->GetString(22), (willKilledHi / 1000) );
		} else {
			sprintf((char*)c->ecx, "%s, %s %d", c->ecx, WogNDlg_TXT->GetString(22), willKilledHi);
		} 
	} else {
		if (willKilledLo > 9999) {
			sprintf((char*)c->ecx, "%s, %s %dk-%dk", c->ecx, WogNDlg_TXT->GetString(22), (willKilledLo / 1000), (willKilledHi / 1000) );
		} else {
			sprintf((char*)c->ecx, "%s, %s %d-%d", c->ecx, WogNDlg_TXT->GetString(22), willKilledLo, willKilledHi);
		} 
	}

	return EXEC_DEFAULT;
}

// перехват получения величин урона
int __stdcall Y_Battle_SetHintAttackGetDamage(LoHook* h, HookContext* c)
{
	damLo = c->edi;
	damHi = c->esi;
	return EXEC_DEFAULT;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// создание кнопки командира ////////////////////////////////////////////////////////

_CustomDlg_* __stdcall Y_HeroDlg_Create(HiHook* hook, _CustomDlg_* dlg)
{
	CALL_1(_CustomDlg_*, __thiscall, hook->GetDefaultFunc(), dlg);

	if ( *(int*)0x28620C0 != -1 ) { // если НЕ командиры отключены (-1)
		dlg->AddItem(_DlgButton_::Create(316, 17, 4445, "bttnNPC.def", 0, 1, 0, 0));
	}

	return dlg;
}

int __stdcall Y_HeroDlg_Proc(HiHook* hook, _CustomDlg_* dlg, _EventMsg_* msg)
{

	if ( *(int*)0x28620C0 == -1 ) { // если командиры отключены (-1)
		return CALL_2(int, __thiscall, hook->GetDefaultFunc(), dlg, msg);
	}

	if (msg->type == MT_MOUSEOVER) {
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);

		if (it && it->id == 4445)	{
			sprintf(o_TextBuffer, WogNDlg_TXT->GetString(16)); 
			_DlgMsg_ m;	CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 115, 0, 0, 0, (int)o_TextBuffer, 0)); 
			dlg->Redraw(TRUE, 114, 115);
			return 1;		
		}  
	} 

	if (msg->type == MT_MOUSEBUTTON) {
		if (msg->item_id == 4445) {
			if (msg->subtype == MST_RBUTTONDOWN || msg->subtype == MST_LBUTTONCLICK) {
				msg->item_id = 0;
				msg->x_abs = 400 + (o_HD_X -800)/2;
				msg->y_abs = 40 + (o_HD_Y -600)/2;
				if (msg->subtype == MST_LBUTTONCLICK) 
					msg->subtype = MST_LBUTTONDOWN; 
			}
		}
	}
	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), dlg, msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// создание кнопки WoG Options //////////////////////////////////////////////////////

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
	this_->AddItem(_DlgTextButton_::Create(622, 105, 4444, "GSPBUT2.DEF", WogNDlg_TXT->GetString(26), "smalfont2.fnt", 0, 1, 0, 0, 1));
}


char* oVersionERA = "{Game Version:}\n\nHoMM3 ERA 2.8.0 \n (with Wog Native Dialogs)";
char* version_dll = "WoG Native Dialogs: 1.1.4";

// int __stdcall Y_Dlg_MainMenu_Create(HiHook* hook, _Dlg_* dlg) 
// {
// 	int ret = 0;
// 	ret = CALL_1(int, __thiscall, hook->GetDefaultFunc(), dlg);

// 	dlg->AddItem(_DlgStaticText_::Create(4, 576, 200, 20, "HoMM3 ERA 2.7.7", "medfont2.fnt", 7, 545, ALIGN_H_LEFT | ALIGN_V_BOTTOM, 0)); 

// 	return ret;
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_LoadAllTXTinGames(LoHook* h, HookContext* c)
{
	txtresWOG = _TXT_::Load( "\\zvs\\Lib1.res\\txtres.txt" );
	WogNDlg_TXT = _TXT_::Load( "WogNDlg.txt" );
	return EXEC_DEFAULT;
}
//-----------------------------------------------------------------------
int __stdcall Y_Hook_MainLoop(LoHook* h, HookContext* c)
{	
	// загружаем необходимый текстовый файл (русский или английский)
	/* if (isRusLang) 
		WogNDlg_TXT = _TXT_::Load( "WogNDlg_Rus.txt" );
	else WogNDlg_TXT = _TXT_::Load( "WogNDlg_Eng.txt" ); */ 

	// создание кнопки WoG Options
	_PI->WriteDword((0x779100 + 3), 640);
	_PI->WriteDword((0x779119 + 3), 640);
	_PI->WriteByte((0x779132 + 3), 110);
	_PI->WriteByte((0x779147 + 3), 110);
	_PI->WriteHiHook(0x579CE0, SPLICE_, EXTENDED_, THISCALL_, Y_NewScenarioDlg_Create);
	_PI->WriteHiHook(0x587FD0, SPLICE_, EXTENDED_, THISCALL_, Y_NewScenarioDlg_Proc);
	// диалог WoG Опций
	 _PI->WriteHiHook(0x779213, CALL_, EXTENDED_, THISCALL_, Dlg_WoG_Options_Show);

	// показ предполагаемого количества убитых монстров при атаке и стрельбе (подмена строк)
	_PI->WriteDword(0x4925FD +1, (int)&WogNDlg_TXT );	_PI->WriteDword(0x492605 +2, 88); // рукопашная (подмена строк)
	_PI->WriteDword(0x492825 +2, (int)&WogNDlg_TXT );	_PI->WriteDword(0x492837 +2, 92); // стрелять (подмена строк)
	_PI->WriteDword(0x49279C +1, (int)&WogNDlg_TXT );	_PI->WriteDword(0x4927A4 +2, 96); // последний выстрел (подмена строк)
	// показ предполагаемого количества убитых монстров при атаке и стрельбе 
	_PI->WriteLoHook(0x493058, Y_Battle_SetHintAttackGetDamage); // получение урона			
	_PI->WriteLoHook(0x4925FD, Y_Battle_SetHintAttackWillKilled); // рукопашная
	_PI->WriteLoHook(0x492825, Y_Battle_SetHintAttackWillKilled); // стрелять
	_PI->WriteLoHook(0x49279C, Y_Battle_SetHintAttackWillKilled); // последний выстрел

	bigfont2 = _Fnt_::Load("bigfont2.fnt");
	medfont2 = _Fnt_::Load("medfont2.fnt");
	smalfont2 = _Fnt_::Load("smalfont2.fnt");

	return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    static _bool_ plugin_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        if (!plugin_On)
        {
			plugin_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("WoG_Native_Dialogs"); 
						
			// загружаем HD данные
			_HD = _P->GetInstance("HD.WoG"); if (_HD) { isHD = true; }

			// создаем загрузку необходимых тектовиков
			_PI->WriteLoHook(0x4EDD65, Y_LoadAllTXTinGames);

			// загрузка MainLoop
			_PI->WriteLoHook(0x4EEAC0, Y_Hook_MainLoop);

			// делаем показ версии игры в главном меню
			// _PI->WriteHiHook(0x4FB930, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_MainMenu_Create);

			// диалог Экспы монстров
			Y_ChangeBmp_To_DefFrame(); 			
			_PI->WriteByte((0x724101 + 2), 0);			
			_PI->WriteLoHook(0x773BA4, New_Dlg_ExpaMon_Lo);
			_PI->WriteLoHook(0x724B80, New_Dlg_ExpaMon_Lo_IsBattle);
			_PI->WriteLoHook(0x723DFF, New_Dlg_ExpaMon_NULL);	

			// диалог командира (старый вызов)
			// новый пока пишется, но еще не доступен
			_PI->WriteHiHook(0x76A46E, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_CommanderWoG);	
			_PI->WriteLoHook(0x7736EF, Y_DlgNPC_SetResult);

			// добавление кнопки командира
			_PI->WriteDword((0x76ED72 +3), 400);  
			_PI->WriteDword((0x76ED8A +3), 400);
			_PI->WriteByte((0x76EDA0 +3), 40);	
			_PI->WriteByte((0x76EDB3 +3), 40); 
			_PI->WriteHiHook(0x4DE980, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Create);
			_PI->WriteHiHook(0x4DD540, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Proc);

			// диалог IF:G
			_PI->WriteLoHook(0x77318D, New_Dlg_IF_G_Lo);		
			_PI->WriteCodePatch(0x77300C, "%n", 5);
			_PI->WriteCodePatch(0x77324B, "%n", 5);

			// диалог выбора типа атаки монстром
			_PI->WriteHiHook(0x7737BE, SPLICE_, EXTENDED_, THISCALL_, Dlg_ChooseMonAttack); 

			// Диалог перестройки замка
			_PI->WriteHiHook(0x70B10B, CALL_, EXTENDED_, THISCALL_, Dlg_ChooseCastleReBuild); 

			// коректировка описаний монстров
			// не в бою
			_PI->WriteDword(0x5F3E3F, 0x660CB4);	// шрифт tiny.fnt
			_PI->WriteDword(0x5F3E4C, 234);			// поз.Y
			_PI->WriteByte(0x5F3E51, 22);			// поз.X 
			// в бою
			_PI->WriteDword(0x5F446A, 0x660CB4);	// шрифт tiny.fnt
			_PI->WriteDword(0x5F4477, 234);			// поз.Y
			_PI->WriteByte(0x5F447C, 22);			// поз.X 
			// при покупке
			_PI->WriteDword(0x5F4885, 0x660CB4);	// шрифт tiny.fnt
			_PI->WriteDword(0x5F4892, 234);			// поз.Y
			_PI->WriteByte(0x5F4897, 22);			// поз.X 

			// обмен героями в замке ко клавише E
			_PI->WriteHiHook(0x5D3640, SPLICE_, EXTENDED_, THISCALL_, Y_DlgTown_Proc);
			_PI->WriteLoHook(0x4AAC1B, Y_Dlg_HeroesMeet);

			// добавляем расчет командиров в проверку Fight_Value
			_PI->WriteLoHook(0x41EAD2, get_Fight_Value_Hook);

			// добавляем в расчет AI_Value и расчет командиров
			 _PI->WriteHiHook(0x44A950, SPLICE_, EXTENDED_, THISCALL_, get_AIValue_Hook);
			// ставим лоухук, чтобы понять что идет расчет AI_Value без наличия героя
			_PI->WriteLoHook(0x5C1867, get_AIValue_And_NPC_Error);
			_PI->WriteLoHook(0x42758F, get_AIValue_And_NPC_Error);
			_PI->WriteLoHook(0x42CA6B, get_AIValue_And_NPC_Error);
			_PI->WriteLoHook(0x52846A, get_AIValue_And_NPC_Error);	

			// диалог сброса артефактов на землю
			_PI->WriteHiHook(0x7548BC, SPLICE_, EXTENDED_, THISCALL_, Y_WoGDlg_ChooseArt);

			// Диалог проклятий и благословлений
			_PI->WriteByte(0x754F6A +1, 0x14);
			_PI->WriteLoHook(0x754E64, Y_WoGCurseDlg_Show);	

// =================================================================================
			// подмена сообщения по ПКМ на кнопке Создатели в главном меню	
			// char* oVersionERA = "{Game Version:}\n\nHoMM3 ERA 2.7.7 \n (with Wog Native Dialogs)";
			//_PI->WriteDword(0x7066E1 +1, *(int*)&oVersionERA); // ENG
			//_PI->WriteDword(0x7066CE +1, *(int*)&oVersionERA); // RUS

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
