//////////////////////////////////
// Author: [igrik] 				//
// Date:   01.12.2017г. 		//
// Thanks: baratorch, ZVS, MoP	//
//////////////////////////////////

// #include "stdafx.h"
#include "..\..\include\homm3.h"
#include "..\..\include\HoMM3_Dlg.cpp"
#include "..\..\include\HoMM3_Extra.h"

Patcher* _P;
PatcherInstance* _PI;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Диалог командира /////////////// /////////////////////////////////////////////////

_DlgStaticDef_* CommDef;
_DlgStaticTextPcx8ed_* statbar = NULL;
bool art_npc_155 = false;
int num_skill_up;
int skill_up[6][3];
int klick[2];
int time_click;
int startDlg[3];
int redraw_Dlg_hero;


int __stdcall Y_New_CommanderDlg_Proc(_Dlg_* dlg_Npc, _EventMsg_* msg)
{
	int r = dlg_Npc->DefProc(msg);

	int npc_id = dlg_Npc->field_64;				// получаем номер командира/героя
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
					case 3:	 if (!dlg_Npc->field_68) text = Get_ITxt(263, 1); break; // подсказка Уволить командира
					case 4:	 text = Get_ITxt(273, 1); break; // подсказка для просмотра биографии
					case 6:  text = Get_ITxt(npc->type + 10, 1); break; // подсказка первого спец_умения
					case 7:  text = Get_ITxt(npc->type + 10, 2); break; // подсказка второго спец_умения
					case 8:  text = Get_ITxt(258, 2); break; // подсказка выберите навыки для повышения
					case 11: text = Get_ITxt(259, 1); break; // подсказка Имя командира

					case 12: text = Get_ITxt((npc->lvl_attack < 2 && art_npc_155 ? 2 : npc->lvl_attack) + 100, 1);      break; // подсказка дефа навыка атаки
					case 13: text = Get_ITxt((npc->lvl_defence < 2 && art_npc_155 ? 2 : npc->lvl_defence) + 106, 1);     break; // подсказка дефа навыка защиты
					case 14: text = Get_ITxt((npc->lvl_hit_points < 2 && art_npc_155 ? 2 : npc->lvl_hit_points) + 112, 1);  break; // подсказка дефа навыка здоровья
					case 15: text = Get_ITxt((npc->lvl_damage < 2 && art_npc_155 ? 2 : npc->lvl_damage) + 118, 1);	   break; // подсказка дефа навыка урона
					case 16: text = Get_ITxt((npc->lvl_spell_power < 2 && art_npc_155 ? 2 : npc->lvl_spell_power) + 124, 1); break; // подсказка дефа навыка силы магии
					case 17: text = Get_ITxt((npc->lvl_speed < 2 && art_npc_155 ? 2 : npc->lvl_speed) + 130, 1);       break; // подсказка дефа навыка скорости

					case 20: case 21: case 22: case 23: case 24: case 25: 	text = Get_ITxt(225, 1); break; // подсказка артефактов

					case 31: case 61: sprintf(o_TextBuffer, "%s%d", Get_ITxt(264, 1), npc->now_level +1); text = o_TextBuffer; break;// подсказка уровня командира
					case 32: case 33: text = Get_ITxt(242, 1);       break; // подсказка класса командира
					case 34: case 35: sprintf(o_TextBuffer, "%s%s", Get_ITxt(266, 1), hero->name); text = o_TextBuffer; break;// подсказка хозяина героя 
					case 36: case 37: sprintf(o_TextBuffer, "%s%d", Get_ITxt(267, 1), npc->now_expa); text = o_TextBuffer; break;// подсказка текущего опыта 
					case 38: case 39: sprintf(o_TextBuffer, "%s%d", Get_ITxt(268, 1), CALL_1(int, __thiscall, 0x769260, npc)); text = o_TextBuffer; break;// подсказка следующего опыта 

					case 40 :case 41: case 42: case 43:  text = Get_ITxt(it->id + 229, 1); break; // подсказка надписей: навыки класса/артефакты/основные_навыки/вторичные навыки

					case 45: temp_1 = Get_NpcSkillPower(npc, 0); temp_2 = hero->attack;
							 if ( npc->type == 7 ) temp_2 += (temp_2/2);
							 sprintf(o_TextBuffer, Get_ITxt(275, 2), temp_1 + temp_2); text = o_TextBuffer; break;											// подсказка атака
					case 46: temp_1 = Get_NpcSkillPower(npc, 1); temp_2 = hero->defence;
							 if ( npc->type == 7 ) temp_2 += (temp_2/2);
							 sprintf(o_TextBuffer, Get_ITxt(276, 2), temp_1 + temp_2); text = o_TextBuffer; break;											// подсказка защита

					case 47: sprintf(o_TextBuffer, Get_ITxt(277, 2), Get_NpcSkillPower(npc, 2)); text = o_TextBuffer; break;								// подсказка здоровье
					case 48: sprintf(o_TextBuffer, Get_ITxt(278, 2), Get_NpcSkillPower(npc, 3)/2, Get_NpcSkillPower(npc, 3)); text = o_TextBuffer; break;	// подсказка сила урон
					case 49: sprintf(o_TextBuffer, Get_ITxt(279, 2), Get_NpcSkillPower(npc, 4)); text = o_TextBuffer; break;								// подсказка сила магии
					case 50: sprintf(o_TextBuffer, Get_ITxt(280, 2), Get_NpcSkillPower(npc, 6)); text = o_TextBuffer; break;								// подсказка сопротивление
					case 51: sprintf(o_TextBuffer, Get_ITxt(281, 2), ((npc->specBon[0] & 0x10) ? 24 : 0)); text = o_TextBuffer; break;						// подсказка боезапас
					case 52: sprintf(o_TextBuffer, Get_ITxt(282, 2), Get_NpcSkillPower(npc, 5)); text = o_TextBuffer; break;								// подсказка скорость

					case 53: case 54: case 55: case 56: case 57: case 58: 
						temp_1 = ((_DlgStaticDef_*)dlg_Npc->GetItem(it->id))->def_frame_index;  // получаем кадр дефа
						text = Get_ITxt((temp_1+1)/2+282, temp_1 % 2 ? 1 : 2); break;		// подсказка вторичные навыки

					case 73: case 74: case 75: case 76: case 77: case 78:
						temp_1 = it->id -73;
						if (skill_up[temp_1][2] < 10){
							temp_2 = (*(int*)(0x28620C0 +296*npc_id +56 +4*skill_up[temp_1][2]));
							text = Get_ITxt(temp_2 +skill_up[temp_1][2]*6 + 101, 1);		 
						}
						else {
							text = Get_ITxt(skill_up[temp_1][2] -10 + 283, 1);
						}
						break; // подсказка навыков при повышении атаки/защиты/здоровья/урона/силы_магии/скорости


					case 30722:	 text = Get_ITxt(dlg_Npc->field_68 ? 262 :261, 1); break; // Ok Выход
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
						case 3: if (!dlg_Npc->field_68) b_MsgBox(Get_ITxt(264, 2), MBX_RMC);	break;  // Уволить командира
						case 4: case 11: b_MsgBox(Get_ITxtBio(npc_id, 1), MBX_RMC);			break;  // ПКМ для просмотра биографи
						case 6: b_MsgBoxAndDef("Dlg_NPC1.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(6))->def_frame_index, Get_ITxt(npc->type + 19, 1));	break;  // ПКМ первого спец_умения
						case 7: b_MsgBoxAndDef("Dlg_NPC1.def", ((_DlgStaticDef_*)dlg_Npc->GetItem(7))->def_frame_index, Get_ITxt(npc->type + 19, 2));	break;  

						// b_MsgBox("ПКМ на первичных навыках командира", MBX_RMC);
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
							 b_MsgBoxAndDef("Dlg_NPC3.def", temp_1, Get_ITxt((temp_1 +1)/2 +297, 1));
							break;		// ПКМ вторичные навыки


						case 73: case 74: case 75: case 76: case 77: case 78:
							// b_MsgBox("ПКМ на картинках повышения", MBX_RMC);
							temp_1 = msg->item_id -73;
							if (skill_up[temp_1][2] < 10){
								temp_2 = (*(int*)(0x28620C0 +296*npc_id +56 +4*skill_up[temp_1][2]));
								b_MsgBoxAndDef("Dlg_NPC2.def", skill_up[temp_1][0], Get_ITxt(temp_1*6 +temp_2 +137, 1));			 
							}
							else {
								b_MsgBoxAndDef("Dlg_NPC2.def", skill_up[temp_1][0], Get_ITxt(298 +skill_up[temp_1][2] -10, 1));
							}
							break; // ПКМ дефа навыка при повышении атаки/защиты/здоровья/урона/силы_магии/скорости

						case 30722: b_MsgBox(Get_ITxt(dlg_Npc->field_68 ? 262 :261, 2), MBX_RMC); break; // ПКМ Ok Выход
					}
				}
				if (msg->subtype == MST_LBUTTONCLICK){ // ЛКМ при отжатии
					if (msg->item_id == 3)	{
						b_MsgBox(Get_ITxt(264, 2), MBX_OKCANCEL); // увольняем командира
						if (o_WndMgr->result_dlg_item_id == DIID_OK){
							npc->on = 0;							
							return dlg_Npc->Close(msg);
						}
					}
				}

				if (msg->subtype == MST_LBUTTONDOWN) // ЛКМ при нажатии
				{
					if (!dlg_Npc->field_68) // НЕ повышение уровня
					{
						if ( o_MeID == hero->owner_id && startDlg[2] != 1 ) // если это наш командир и кнопка удалить неактивна
						{						
							switch (msg->item_id)
							{
								case 20: case 21: case 22: case 23: case 24: case 25: 
									sprintf(o_TextBuffer, "%s\n\n%s", CALL_2(char*, __thiscall, 0x769E0E, npc, msg->item_id -20), Get_ITxt(225, 2));
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
								dlg_Npc->GetItem(30722)->SetEnabled(1);
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
	if (((o_GetTime() - DwordAt(0x6989E8)) & 0x80000000) == 0 && !npc->alive)
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

	_Dlg_* dlg = _CustomDlg_::Create(-1, -1, temp_1, temp_2, DF_SCREENSHOT | DF_SHADOW, Y_New_CommanderDlg_Proc);

	int med_hi = /* o_medfont_Fnt->string_height*/ 20;

	// деф командира
	dlg->field_64 = NHero_id; // передаем номер героя/командира в Dlg_Proc
	dlg->field_68 = lvl_up;   // передаем повышалка ли в Dlg_Proc
	sprintf(o_TextBuffer, "%s", *(int*)(0x68295C+4+4*(o_CreatureInfo[npc->type + 174].town)) );
	dlg->AddItem(_DlgStaticPcx8_::Create(20, 70, 100, 130, 4, /*"CRBKGNEU.pcx"*/ o_TextBuffer )); // фон замка под дефом командира нейтральный (id = 4)
	dlg->AddItem(CommDef = _DlgStaticDef_::Create(-130, -80, 100, 130, 5, o_CreatureInfo[npc->type + 174].def_name, 0, 0, 0x12)); // шагающий деф командира (id = 5)

	// задник диалога под цвет игрока (id = 1)
	_DlgStaticPcx8_* fonPcx;
	dlg->AddItem(fonPcx = b_DlgStaticPcx8_Create(0, 0, dlg->width, dlg->height, 1, name_pcx, 2048));
	if (hero->owner_id != -1)
		 fonPcx->Colorize(hero->owner_id);
	else fonPcx->Colorize(o_GameMgr->GetMeID());
	

	// кнопки ок и уволить
	dlg->AddItem(_DlgButton_::Create(285, dlg->height -76, 64, 30, 30722, "iOkay.def", 0, 1, 1, 28, 2)); // кнопка ok (id = 30722)
	dlg->AddItem(_DlgButton_::Create(564, dlg->height -76, 51, 35, 3, "hsbtns2.def", 0, 1, 0, 83, 2));   // кнопка удалить командира (id = 3)
	if (lvl_up || is_del_bttn || o_MeID != hero->owner_id) dlg->GetItem(3)->SetEnabled(0);

	CALL_3 (void, __thiscall, 0x50CEA0, o_MouseMgr, 0, 0);  // установить курсор(0,0)
	CALL_2 (void, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	dlg->AddItem(_DlgStaticText_::Create(20, 70, 100, 130, "", "medfont.fnt", 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));  // подсказка на дефе (пустышка, но по ней ПКМ определяет id и выдает окно биографии)
	
	// дефы спец_умений командира
	dlg->AddItem(_DlgStaticDef_::Create(395, 113, 43, 43, 6, "Dlg_NPC1.def", npc->type *2, 0, 0x12));    // первый деф спец_умений (id = 6)
	dlg->AddItem(_DlgStaticDef_::Create(504, 113, 43, 43, 7, "Dlg_NPC1.def", npc->type *2 +1, 0, 0x12)); // первый деф спец_умений (id = 7)

	// подсказка в статус баре	
	statbar = _DlgStaticTextPcx8ed_::Create(8, dlg->height -18 -8, dlg->width - 16, 18, "", "smalfont.fnt", "AdRollvr.pcx", 1, 10, ALIGN_H_CENTER | ALIGN_V_CENTER); // HD_TStat.pcx
	dlg->AddItem(statbar);

	if (lvl_up)
		 sprintf(o_TextBuffer, Get_ITxt(258, 1), npc->name);
	else sprintf(o_TextBuffer, "%s", npc->name);
	dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, med_hi, o_TextBuffer, "medfont.fnt", 7, 11, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // имя командира (id = 11)
	if (npc->alive){
		dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, med_hi, Get_ITxt(259, 2), "medfont.fnt", 27, 11, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // описание мертв
		dlg->AddItem(_DlgStaticText_::Create(131, 18, 370, med_hi, Get_ITxt(259, 2), "medfont.fnt", 27, 11, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));	
	}

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

	dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, med_hi, Get_ITxt(264, 1), "medfont.fnt", 7, 31, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // уровень_текст (id = 31)
	sprintf(o_TextBuffer, "%d", npc->now_level +1);
	dlg->AddItem(_DlgStaticText_::Create(22, 46, 96, med_hi, o_TextBuffer, "medfont.fnt", 1, 61, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // уровень_число (id = 61)

	dlg->AddItem(_DlgStaticText_::Create(16, 205, 104, med_hi, Get_ITxt(265, 1), "medfont.fnt", 7, 32, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // класс_текст1 (id = 32)
	dlg->AddItem(_DlgStaticText_::Create(16, 220, 104, med_hi, Get_ITxt(npc->type +216, 1), "medfont.fnt", 1, 33, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // класс_текст2 (id = 33)

	dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, med_hi, Get_ITxt(266, 1), "medfont.fnt", 7, 34, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Хозяин_текст (id = 34)
	dlg->AddItem(_DlgStaticText_::Create(128, 46, 192, med_hi, hero->name, "medfont.fnt", 1, 35, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // Хозяин_имя (id = 35)
	
	dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, med_hi, Get_ITxt(267, 1), "medfont.fnt", 7, 36, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Текущий_опыт_текст (id = 36)
	sprintf(o_TextBuffer, "%d", o_Npc[NHero_id].now_expa);
	dlg->AddItem(_DlgStaticText_::Create(330, 46, 284, med_hi, o_TextBuffer, "medfont.fnt", 1, 37, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0)); // Текущий_опыт_число (id = 37)	

	dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, med_hi, Get_ITxt(268, 1), "medfont.fnt", 7, 38, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Следующий_уровень_текст (id = 38)

	sprintf(o_TextBuffer, "%d", CALL_1(int, __thiscall, 0x769260, npc));
	dlg->AddItem(_DlgStaticText_::Create(330, 68, 284, med_hi, o_TextBuffer, "medfont.fnt", 1, 39, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));   // Следующий_уровень_число (id = 39)	

	dlg->AddItem(_DlgStaticText_::Create(377, 91, 192, med_hi, Get_ITxt(269, 1), "medfont.fnt", 7, 40, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Навыки_класса_текст (id = 40)
	dlg->AddItem(_DlgStaticText_::Create(377, 160, 192, med_hi, Get_ITxt(270, 1), "medfont.fnt", 7, 41, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Артефакты_текст (id = 41)

	dlg->AddItem(_DlgStaticText_::Create(133, 247, 366, med_hi, Get_ITxt(271, 1), "medfont.fnt", 7, 42, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Основные_навыки_текст (id = 42)
	dlg->AddItem(_DlgStaticText_::Create(133, 344, 366, med_hi, Get_ITxt(272, 1), "medfont.fnt", 7, 43, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // Вторичные_навыки_текст (id = 43)
	
	temp_1 = Get_NpcSkillPower(npc, 0); 	temp_2 = hero->attack;
	if ( npc->type == 7 ) temp_2 += (temp_2/2);
	sprintf(o_TextBuffer, "%d (%d)", temp_1, temp_1 + temp_2);
	dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 65, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Атака_число (id = 65)
	dlg->AddItem(_DlgStaticText_::Create(128, 69, 192, med_hi, Get_ITxt(275, 1), "medfont.fnt", 7, 45, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Атака_текст (id = 45)

	temp_1 = Get_NpcSkillPower(npc, 1); 	temp_2 = hero->defence;
	if ( npc->type == 7 ) temp_2 += (temp_2/2);
	sprintf(o_TextBuffer, "%d (%d)", temp_1, temp_1 + temp_2);
	dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 66, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Защита_число (id = 66)
	dlg->AddItem(_DlgStaticText_::Create(128, 91, 192, med_hi, Get_ITxt(276, 1), "medfont.fnt", 7, 46, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Защита_текст (id = 46)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 2));
	dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 67, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Здоровье_число (id = 67)
	dlg->AddItem(_DlgStaticText_::Create(128, 113, 192, med_hi, Get_ITxt(277, 1), "medfont.fnt", 7, 47, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Здоровье_текст (id = 47)

	temp_1 = Get_NpcSkillPower(npc, 3);
	sprintf(o_TextBuffer, "%d - %d", temp_1/2, temp_1);
	dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 68, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Урон_число (id = 68)
	dlg->AddItem(_DlgStaticText_::Create(128, 135, 192, med_hi, Get_ITxt(278, 1), "medfont.fnt", 7, 48, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Урон_текст (id = 48)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 4));
	dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 69, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Сила_магии_число (id = 69)
	dlg->AddItem(_DlgStaticText_::Create(128, 157, 192, med_hi, Get_ITxt(279, 1), "medfont.fnt", 7, 49, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Сила_магии_текст (id = 49)

	sprintf(o_TextBuffer, "%d%%", Get_NpcSkillPower(npc, 6));
	dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 70, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Сопротивление_число (id = 70)
	dlg->AddItem(_DlgStaticText_::Create(128, 179, 192, med_hi, Get_ITxt(280, 1), "medfont.fnt", 7, 50, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Сопротивление_текст (id = 50)

	sprintf(o_TextBuffer, "%d", (npc->specBon[0] & 0x10 ? 24 : 0));
	dlg->AddItem(_DlgStaticText_::Create(128, 201, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 71, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Боезапас_число (id = 71)
	dlg->AddItem(_DlgStaticText_::Create(129, 201, 192, med_hi, Get_ITxt(281, 1), "medfont.fnt", 7, 51, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Боезапас_текст (id = 51)

	sprintf(o_TextBuffer, "%d", Get_NpcSkillPower(npc, 5));
	dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, med_hi, o_TextBuffer, "medfont.fnt", 1, 72, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));    // Скорость_число (id = 72)
	dlg->AddItem(_DlgStaticText_::Create(128, 223, 192, med_hi, Get_ITxt(282, 1), "medfont.fnt", 7, 52, ALIGN_H_LEFT | ALIGN_V_CENTER, 0)); // Скорость_текст (id = 52)


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
			dlg->GetItem(30722)->SetEnabled(0);
			
			dlg->AddItem(_DlgStaticText_::Create(133, 422, 366, med_hi, Get_ITxt(258, 2), "medfont.fnt", 7, 8, ALIGN_H_CENTER | ALIGN_V_CENTER, 0)); // подсказка выберите навыки для повышения (id = 8)

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

	_Dlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NULL);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	dlg->AddItem(_DlgStaticDef_::Create(dlg->width/2 -41, 26, 2, "PSKILL.def", 4, 0, 0x12)); 

	dlg->AddItem(_DlgStaticText_::Create(20, 126, dlg->width -40, 20, Get_ITxtExp(460), "bigfont.fnt", 10, 5, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	for (int i = 0; i < 11; i++){
		dlg->AddItem(_DlgStaticText_::Create(24, 150 +i*20, dlg->width -48, 20, Get_ITxtExp(461 +i), "medfont.fnt", 7, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));	
	}

	dlg->AddItem(_DlgStaticText_::Create(24, 150, dlg->width -48, 20, o_pCreatureInfo[o_Mon].name_single, "medfont.fnt", 1, 22, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));
	

	int runk = CALL_2(_int_, __cdecl, 0x727F20, 1000, o_Expo); 
	sprintf(o_TextBuffer, "%s (%d)", Get_ITxtExp(2+runk), runk);
	dlg->AddItem(_DlgStaticText_::Create(24, 170, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 23, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 190, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 24, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk +1, 0) - o_Expo); 
	dlg->AddItem(_DlgStaticText_::Create(24, 210, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 25, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	int t1 = CALL_1(_int_, __cdecl, 0x7283D0, 1000); 
	int t2 = CALL_1(_int_, __cdecl, 0x727E20, 1000) * t1 / 100;
	sprintf(o_TextBuffer, "%d%% (%d)", t1, t2);
	dlg->AddItem(_DlgStaticText_::Create(24, 230, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 26, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	sprintf(o_TextBuffer, "%d", o_Num); 
	dlg->AddItem(_DlgStaticText_::Create(24, 250, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 27, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	t1 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, runk, 0);
	t2 = 999999;
	if( t1 ){ t2 = o_Num * o_Expo / t1 - o_Num; }	
	sprintf(o_TextBuffer, "%d", t2); 
	dlg->AddItem(_DlgStaticText_::Create(24, 270, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 28, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727C00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 290, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 29, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	sprintf(o_TextBuffer, "%hf", CALL_1(double, __cdecl, 0x727E00, 1000)); 
	dlg->AddItem(_DlgStaticText_::Create(24, 310, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 30, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t1 = CALL_1(_int_, __cdecl, 0x7283B0, 1000);
	sprintf(o_TextBuffer, "%d", t1); 
	dlg->AddItem(_DlgStaticText_::Create(24, 330, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 31, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));

	t2 = CALL_3(_int_, __cdecl, 0x727FB0, 1000, 10, 0);
	int t3 = 999999; 
	if ( t2 ){
		int t4 = o_Num * (t2 + t1);
		t3 = t4 / t2 - o_Num;
	}
	sprintf(o_TextBuffer, "%d", t3); 
	dlg->AddItem(_DlgStaticText_::Create(24, 350, dlg->width -48, 20, o_TextBuffer, "medfont.fnt", 1, 32, ALIGN_H_RIGHT | ALIGN_V_CENTER, 0));


	if (style == 1){
		dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width/2 -33, dlg->height -22 -31, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(dlg->width/2 -32, dlg->height -22 -30, 64, 30, 1, "iOkay.def", 0, 1, 1, 28, 2));
		dlg->Run();
	}
	if (style == 4)
		dlg->RMC_Show(); 
		
	dlg->Destroy(TRUE);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// диалог по выбору типа действия артефакта /////////////////////////////////////////

int __stdcall New_Dlg_ExpaMon_BannerDlg_Proc(_Dlg_* dlg, _EventMsg_* msg)
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

	_Dlg_* dlg = _CustomDlg_::Create(-1, -1, 460, 360, DF_SCREENSHOT | DF_SHADOW, New_Dlg_ExpaMon_BannerDlg_Proc);
	Set_DlgBackground_RK(dlg, 0, o_GameMgr->GetMeID());
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	sprintf(o_TextBuffer, "%s", Get_ITxtExp(340));
    dlg->AddItem(_DlgStaticText_::Create(18, 30, 424, 30, o_TextBuffer, "bigfont.fnt", 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
	
	int dy = 27;	

	for (int i = 0; i < 9; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 62+i*dy, 424, 24, 10+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(10+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "%s", Get_ITxtExp(341 +i));
		dlg->AddItem(_DlgStaticText_::Create(44, 66+i*dy, 394, 16, o_TextBuffer, "smalfont.fnt", 1, 20+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

		// dlg->AddItem(_DlgStaticDef_::Create(20, 65+i*dy, 30+i, "SYSOPCHK.DEF", 0, 0, 0));
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

int __stdcall New_Dlg_ExpaMon_Proc(_Dlg_* dlg, _EventMsg_* msg)
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

					case 95: text_bar = "Ok"; break; 					
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
					b_MsgBoxD(Get_ITxtExp(459), MBX_OKCANCEL, 8, 156);
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


	_Dlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_ExpaMon_Proc);

	Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());
	b_MouseMan_SetCursor(0,0);
	CALL_2(void*, __thiscall, 0x50D7B0, o_MouseMgr, 0);

	// Добавляем кнопку OK (id = 30722)
    dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width -31 -52, dlg->height - 31 -40, 0, "Box64x30.pcx"));
    dlg->AddItem(_DlgButton_::Create(dlg->width -30 -52, dlg->height -30 -40, 64, 30, 95, "iOkay.def", 0, 1, 1, 28, 2));

	// Добавляем кнопку просмотра улучшенного (id = 1)
	int is_grade_bttn_on = *(int*)(o_Dlg_ExpaMon +56);
	if (is_grade_bttn_on){
		dlg->AddItem(_DlgStaticPcx8_::Create(dlg->width -31 -122, dlg->height - 31 -40, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(dlg->width -30 -122, dlg->height -30 -40, 64, 30, 1, "DlgExpUp.def", 0, 1, 0, 88, 0));
	}

	// Подсказка (id = 2)
	dlg->AddItem(_DlgStaticText_::Create(8, dlg->height -26, dlg->width -34, 18, "", "smalfont.fnt", 1, 2, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	// заголовок диалога (id = 3)
	sprintf(o_TextBuffer, "%s", *(int*)o_Dlg_ExpaMon);
    dlg->AddItem(_DlgStaticText_::Create(150, 14, 500, 30, o_TextBuffer, "bigfont.fnt", 7, 3, ALIGN_H_CENTER | ALIGN_V_TOP, 0));

	// нижнее описание монстра красным (id = 4)
	sprintf(o_TextBuffer, "%s", *(int*)(o_Dlg_ExpaMon +4));
    dlg->AddItem(_DlgStaticText_::Create(18, dlg->height - 76, dlg->width -200, 44, o_TextBuffer, "verd10b.fnt", 27, 4, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

	// картинка и рамка монстра (id = 5, 6)
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 5, "twcrport.def", Mon +2, 0, 0x12)); 
	dlg->AddItem(_DlgStaticDef_::Create(18, 35, 6, "twcrport.def", 1, 0, 0x12));  

	// название артефакта и рамка для него (id = 7,8)
    dlg->AddItem(_DlgStaticText_::Create(18, 100, 60, 15, o_GENRLTXT_TXT->GetString(615), "medfont.fnt", 1, 7, ALIGN_H_CENTER | ALIGN_V_TOP, 0));
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
		"smalfont.fnt", 7, 10, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
	}
	// элементы заполнения колонок (первый столбик) id = 11...(26)
	int y0 = 50;	int yy = 16;
	for (int i = 0; i < (str_txt+dop_str); i++){
		sprintf(o_TextBuffer, "%s", *(int*)(*(int*)(o_Dlg_ExpaMon +36)+4*i));
		dlg->AddItem(_DlgStaticText_::Create(80, y0 + yy*i, 80, 16, o_TextBuffer, "smalfont.fnt", 7, 11+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
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
			dlg->AddItem(_DlgStaticText_::Create(x0 + xx*j, y0 + yy*i, 54, 16, text, "smalfont.fnt", 1, 30+i, ALIGN_H_LEFT | ALIGN_V_TOP, 0));
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
			dlg->AddItem(_DlgStaticText_::Create(xd+54, yd, 330, 50, o_TextBuffer, "smalfont.fnt", 7, 51+i*2, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
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

int __stdcall New_Dlg_IF_G_Proc(_Dlg_* dlg, _EventMsg_* msg)
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
	_Dlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, New_Dlg_IF_G_Proc);
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
	dlg->AddItem(_DlgStaticText_::Create(20, 20, dlg->width -40, 46, o_TextBuffer, "medfont.fnt", 1, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	int on_bttn = 0;
	for (int i = 0; i < num_str; i++){
		dlg->AddItem(CALL_8 (_DlgItem_*, __thiscall, 0x44FE00, o_New(56), 18, 66+i*dy, dlg->width -36, 24, 22+i, *(int*)(*(int*)0x6AAD18 + 118), 1024));
		dlg->GetItem(22+i)->SendCommand(5, 4);
		
		sprintf(o_TextBuffer, "{%s}", *(int*)(txt +4*i));
		dlg->AddItem(_DlgStaticText_::Create(44, 70+i*dy, dlg->width -66, 16, o_TextBuffer, "smalfont.fnt", 1, 10+i, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
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

int __stdcall Y_Dlg_CommanderWoG(HiHook* hook, _Npc_* npc, int is_lvlup, int is_del_bttn, int a4)
{
	CALL_4(void*, __thiscall, hook->GetDefaultFunc(), npc, is_lvlup, is_del_bttn, a4); 

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
			char* version_dll = "WoG_Native_Dialogs ver 2.66";
		
			// диалог Экспы монстров
			Y_ChangeBmp_To_DefFrame(); 			
			_PI->WriteByte((0x724101 + 2), 0);			
			_PI->WriteLoHook(0x773BA4, New_Dlg_ExpaMon_Lo);
			_PI->WriteLoHook(0x724B80, New_Dlg_ExpaMon_Lo_IsBattle);
			_PI->WriteLoHook(0x723DFF, New_Dlg_ExpaMon_NULL);	

			// диалог командира
			_PI->WriteHiHook(0x76A46E, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_CommanderWoG);	
			_PI->WriteLoHook(0x7736EF, Y_DlgNPC_SetResult);

			// диалог IF:G
			_PI->WriteLoHook(0x77318D, New_Dlg_IF_G_Lo);		
			_PI->WriteCodePatch(0x77300C, "%n", 5);
			_PI->WriteCodePatch(0x77324B, "%n", 5);

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
