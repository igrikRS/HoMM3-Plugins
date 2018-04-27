#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

#define o_SSTraits (*(_int_*)0x67DCF0)
#define o_DlgHeroInfoSStext ((const char*)(*(int*)0x6A80D0))

_Dlg_* __stdcall Y_HeroDlg_Create(HiHook* hook, _Dlg_* dlg)
{
	CALL_1(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg);
	
	dlg->AddItem(_DlgStaticDef_::Create(18, 420, 44, 44, 501, "secskill.def", 0, 0, 16)); // 9 навык (id 501, 502, 503)
	dlg->AddItem(_DlgStaticText_::Create(68, 444, 90, 18, "", "smalfont.fnt", 1, 502, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
	dlg->AddItem(_DlgStaticText_::Create(68, 424, 90, 18, "", "smalfont.fnt", 1, 503, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
	
	dlg->AddItem(_DlgStaticDef_::Create(161, 420, 44, 44, 504, "secskill.def", 0, 0, 16)); // 10 навык (id 504, 505, 506)
	dlg->AddItem(_DlgStaticText_::Create(211, 444, 90, 18, "", "smalfont.fnt", 1, 505, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));
	dlg->AddItem(_DlgStaticText_::Create(211, 424, 90, 18, "", "smalfont.fnt", 1, 506, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

	dlg->AddItem(_DlgStaticText_::Create(208, 141, 44, 18, "", "smalfont.fnt", 1, 507, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));
	dlg->AddItem(_DlgStaticText_::Create(255, 141, 44, 18, "", "smalfont.fnt", 1, 508, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

	// скрываем названия первичных навыков
	_DlgItem_* it;
	it = dlg->GetItem(103);		if (it) it->Hide();					// текст атака
	it = dlg->GetItem(104);		if (it) it->Hide();					// текст защита
	it = dlg->GetItem(105);		if (it) it->Hide();					// текст к.сила
	it = dlg->GetItem(106);		if (it) it->Hide();					// текст знание
	/* it = dlg->GetItem(108);		if (it) it->Hide();					// текст опыт
	it = dlg->GetItem(112);		if (it) it->Hide();					// значение опыт */ 
	it = dlg->GetItem(107);		if (it) it->Hide();	                // иконка проклятий
	it = dlg->GetItem(139);		if (it) it->Hide();	                // иконка бонуса богов 
	it = dlg->GetItem(118);		if (it){it->x = 250; it->y = 30;}   // иконка специальности
	it = dlg->GetItem(1);		if (it) it->width = 166;			// имя героя
	it = dlg->GetItem(140);		if (it) it->width = 166;			// класс героя

	return dlg;
}

int ss_9, ss_10;

int __stdcall Y_HeroDlg_Update(HiHook* hook, _Dlg_* dlg)
{	
	ss_9  = -1;
	ss_10 = -1;
	for (int i = 0; i < 28; i++) {
		if ( o_HeroDlg_Hero->second_skill_show[i] == 9 ) 
			ss_9 = i;

		if ( o_HeroDlg_Hero->second_skill_show[i] == 10 )
			ss_10 = i;
	}

	if (ss_9 >= 0){
		((_DlgStaticDef_*)dlg->GetItem(501))->def_frame_index = 2 + 3*ss_9 + o_HeroDlg_Hero->second_skill[ss_9];
		sprintf(o_TextBuffer, "%s", *(int*)(16*ss_9 + o_SSTraits) );
		((_DlgStaticText_*)dlg->GetItem(502))->SetText(o_TextBuffer);
		sprintf(o_TextBuffer, "%s", *(int*)(o_HeroDlg_Hero->second_skill[ss_9]*4 + 0x6A75D4) );
		((_DlgStaticText_*)dlg->GetItem(503))->SetText(o_TextBuffer);
	} else {
		((_DlgStaticDef_*)dlg->GetItem(501))->def_frame_index = 0;
		((_DlgStaticText_*)dlg->GetItem(502))->SetText("");
		((_DlgStaticText_*)dlg->GetItem(503))->SetText("");	
	}

	if (ss_10 >= 0){
		((_DlgStaticDef_*)dlg->GetItem(504))->def_frame_index = 2 + 3*ss_10 + o_HeroDlg_Hero->second_skill[ss_10];
		sprintf(o_TextBuffer, "%s", *(int*)(16*ss_10 + o_SSTraits) );
		((_DlgStaticText_*)dlg->GetItem(505))->SetText(o_TextBuffer);
		sprintf(o_TextBuffer, "%s", *(int*)(o_HeroDlg_Hero->second_skill[ss_10]*4 + 0x6A75D4) );
		((_DlgStaticText_*)dlg->GetItem(506))->SetText(o_TextBuffer);
	} else {
		((_DlgStaticDef_*)dlg->GetItem(504))->def_frame_index = 0;
		((_DlgStaticText_*)dlg->GetItem(505))->SetText("");
		((_DlgStaticText_*)dlg->GetItem(506))->SetText("");	
	}

	CALL_1(void*, __thiscall, hook->GetDefaultFunc(), dlg);

	return 1;
}

int __stdcall Y_HeroDlg_UpdateLuckMoral(HiHook* hook, _Hero_* hero)
{
	if (o_CurrentDlg){
		int moral = CALL_4(int, __thiscall, 0x4E3C20, o_HeroDlg_Hero, 0, 0, 1); 
		sprintf(o_TextBuffer, "%d", moral );
		((_DlgStaticText_*)o_CurrentDlg->GetItem(507))->SetText(o_TextBuffer);

		int luck = CALL_4(int, __thiscall, 0x4E3930 , o_HeroDlg_Hero, 0, 0.0, 1); 
		sprintf(o_TextBuffer, "%d", luck );
		((_DlgStaticText_*)o_CurrentDlg->GetItem(508))->SetText(o_TextBuffer);
	}

	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), hero);
}

int __stdcall Y_HeroDlg_Proc(HiHook* hook, _Dlg_* dlg, _EventMsg_* msg)
{
	if (msg->type == MT_MOUSEOVER) {
		_DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);

////////////////////////////////////////////////////////////////////////////////////
		// sprintf(o_TextBuffer, "Координаты: %d %d (id: %d)", msg->x_abs -254, msg->y_abs -39, it->id ); 
		// _DlgMsg_ m;	CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 115, 0, 0, 0, (int)o_TextBuffer, 0)); 
		// dlg->Redraw(TRUE, 114, 115);
////////////////////////////////////////////////////////////////////////////////////
		if (it)	{
			if ( it->id >= 501 && it->id <= 503 && ss_9 != -1 ) {
				sprintf(o_TextBuffer, o_DlgHeroInfoSStext, *(int*)(o_HeroDlg_Hero->second_skill[ss_9]*4 + 0x6A75D4), *(int*)(o_SSTraits +16*ss_9) ); 
				_DlgMsg_ m;	CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 115, 0, 0, 0, (int)o_TextBuffer, 0)); 
				dlg->Redraw(TRUE, 114, 115);
				return 1;
			}

			if ( it->id >= 504 && it->id <= 506 && ss_10 != -1 ) {
				sprintf(o_TextBuffer, o_DlgHeroInfoSStext, *(int*)(o_HeroDlg_Hero->second_skill[ss_10]*4 + 0x6A75D4), *(int*)(o_SSTraits +16*ss_10) ); 
				_DlgMsg_ m;	CALL_2(void, __thiscall, 0x5FF3A0, dlg, m.Set(512, 3, 115, 0, 0, 0, (int)o_TextBuffer, 0)); 
				dlg->Redraw(TRUE, 114, 115);
				return 1;
			}
		}
	}

	if (msg->type == MT_MOUSEBUTTON) {
		if (msg->subtype == MST_RBUTTONDOWN || msg->subtype == MST_LBUTTONDOWN)	{
			int temp = 0;
			int style = msg->subtype == MST_LBUTTONDOWN ? 1 : 4; 
			switch (msg->item_id)	{
				case 501: case 502: case 503:
					temp = ((_DlgStaticDef_*)dlg->GetItem(501))->def_frame_index;
					if ( temp > 0 ) {
						sprintf(o_TextBuffer, "%s", *(int*)(6917512 + 16*ss_9 + 4*o_HeroDlg_Hero->second_skill[ss_9]) );
						CALL_12(void, __fastcall, 0x4F6C00, o_TextBuffer, style, -1, -1, 20, temp, -1, 0, -1, 0, -1, 0); 
					}	break; 

				case 504: case 505: case 506:
					temp = ((_DlgStaticDef_*)dlg->GetItem(504))->def_frame_index;
					if ( temp > 0 ) {
						sprintf(o_TextBuffer, "%s", *(int*)(6917512 + 16*ss_10 + 4*o_HeroDlg_Hero->second_skill[ss_10]) );
						CALL_12(void, __fastcall, 0x4F6C00, o_TextBuffer, style, -1, -1, 20, temp, -1, 0, -1, 0, -1, 0); 						
					}	break;

				default: 
					break;
			}
		}
	}	
	
	return CALL_2(int, __thiscall, hook->GetDefaultFunc(), dlg, msg);
}





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
			_PI = _P->CreateInstance("10SecondSkills"); 

			// увеличиваем кол-во доступных втор.навыков для изучения
			int ss_num = 10;
			_PI->WriteByte(0x4A7DD0, ss_num); 
			_PI->WriteByte(0x4DAFCE, ss_num); 
			_PI->WriteByte(0x4E256A, ss_num); 
			_PI->WriteByte(0x57417F, ss_num); 
			_PI->WriteByte(0x5F0C87, ss_num); 
			_PI->WriteByte(0x5F0F50, ss_num); 
			_PI->WriteByte(0x744AB4, ss_num); 

			// сдвигаем навыки WoG богов (zgodbon.def) и проклятий
			/* _PI->WriteDword(0x75482A +1, 171); // богов по y (ориг =180)
			_PI->WriteByte(0x75482F +1, 66); // богов по x (ориг =63)

			_PI->WriteDword(0x7548A4 +1, 171); // проклятие по y (ориг =180)
			_PI->WriteByte(0x7548A9 +1, 113); // проклятие по x (ориг =180)*/ 

			// ОПЫТ и МАНА
			// иконка опыта (id = 119)
			_PI->WriteDword(0x4E0DDE +1, 229 -56); // posY	= 229
			_PI->WriteByte(0x4E0DE3 +1, 20);	// posX	= 19
			// опыт текст (id = 108)
			_PI->WriteDword(0x4DED67 +1, 232 -56); // ypos = 232
			_PI->WriteByte(0x4DED6C +1, 68);    // xpos = 68
			// опыт число (id = 112)
			_PI->WriteDword(0x4E123F +1, 252 -56); // ypos = 252
			_PI->WriteByte(0x4E1244 +1, 68);    // xpos = 68


			// иконка маны (id = 120)
			_PI->WriteDword(0x4E0E28 +1, 229 -56); // posY = 229	
			_PI->WriteDword(0x4E0E2D +1, 163);  // posX	= 162
			// мана текст (id = 109)
			_PI->WriteDword(0x4DEDB7 +1, 232 -56); // ypos = 232
			_PI->WriteDword(0x4DEDBC +1, 212);  // xpos = 211
			// мана число (id = 113)
			_PI->WriteDword(0x4E1289 +1, 252 -56); // ypos = 252
			_PI->WriteDword(0x4E128E +1, 212);  // xpos = 211


			// ВТОРИЧНЫЕ НАВЫКИ 
			// иконки
			_PI->WriteDword(0x4DF7EF +1, 229);     // posY = 276   id = 79
			_PI->WriteDword(0x4DF83E +1, 229);     // posY = 276   id = 80
			_PI->WriteDword(0x4DF890 +1, 276);     // posY = 324   id = 81
			_PI->WriteDword(0x4DF8DF +1, 276);     // posY = 324   id = 82
			_PI->WriteDword(0x4DF931 +1, 324);     // posY = 372   id = 83
			_PI->WriteDword(0x4DF980 +1, 324);     // posY = 372   id = 84
			_PI->WriteDword(0x4DF9D2 +1, 372);     // posY = 420   id = 85
			_PI->WriteDword(0x4DFA21 +1, 372);     // posY = 420   id = 86

			// имена навыков
			_PI->WriteDword(0x4DFA74 +1, 252);     // posY = 300   id = 87
			_PI->WriteDword(0x4DFAC4 +1, 252);     // posY = 300   id = 88			
			_PI->WriteDword(0x4DFB17 +1, 300);     // posY = 348   id = 89
			_PI->WriteDword(0x4DFB67 +1, 300);     // posY = 348   id = 90
			_PI->WriteDword(0x4DFBBA +1, 348);     // posY = 396   id = 91
			_PI->WriteDword(0x4DFC0A +1, 348);     // posY = 396   id = 92
			_PI->WriteDword(0x4DFC5D +1, 396);     // posY = 444   id = 93
			_PI->WriteDword(0x4DFCAD +1, 396);     // posY = 444   id = 94

			// уровни навыков
			_PI->WriteDword(0x4DFD00 +1, 232);     // posY = 280   id = 95
			_PI->WriteDword(0x4DFD50 +1, 232);     // posY = 280   id = 96			
			_PI->WriteDword(0x4DFDA3 +1, 280);     // posY = 328   id = 97
			_PI->WriteDword(0x4DFDF3 +1, 280);     // posY = 328   id = 98			
			_PI->WriteDword(0x4DFE46 +1, 328);     // posY = 376   id = 99
			_PI->WriteDword(0x4DFE96 +1, 328);     // posY = 376   id = 100			
			_PI->WriteDword(0x4DFEE9 +1, 376);     // posY = 424   id = 101
			_PI->WriteDword(0x4DFF39 +1, 376);     // posY = 424   id = 102

			// переносим первичные навыки
			_PI->WriteByte(0x4DF477 +1, 97);       // атака        posY = 111 
			_PI->WriteByte(0x4DF479 +1, 20);       // id = 50      posX = 32 
			_PI->WriteDword(0x4DEEB1 +1, 142);     // текст атаки  posY = 158 
			_PI->WriteByte(0x4DEEB6 +1, 20);       // id = 46      posX = 32 

			_PI->WriteByte(0x4DF4C4 +1, 97);       // защита       posY = 111 
			_PI->WriteByte(0x4DF4C6 +1, 67);       // id = 51      posX = 102 
			_PI->WriteDword(0x4DEF02 +1, 142);     // текст защиты posY = 158 
			_PI->WriteByte(0x4DEF07 +1, 67);       // id = 47      posX = 101 

			_PI->WriteByte(0x4DF511 +1, 97);       // колдов       posY = 111 
			_PI->WriteDword(0x4DF513 +1, 114);     // id = 52      posX = 172 
			_PI->WriteDword(0x4DEF53 +1, 142);     // текст колдов posY = 158 
			_PI->WriteDword(0x4DEF58 +1, 114);     // id = 48      posX = 172 

			_PI->WriteByte(0x4DF561 +1, 97);       // знания       posY = 111 
			_PI->WriteDword(0x4DF563 +1, 162);     // id = 53      posX = 242 
			_PI->WriteDword(0x4DEFA7 +1, 142);     // текст знания posY = 158 
			_PI->WriteDword(0x4DEFAC +1, 162);     // id = 49      posX = 242 

			_PI->WriteDword(0x4E0CFC +1, 98);      // мораль       posY = 184 
			_PI->WriteDword(0x4E0D01 +1, 209);     // id = 116     posX = 182 

			_PI->WriteDword(0x4E0D48 +1, 98);      // удача        posY = 184 
			_PI->WriteDword(0x4E0D4D +1, 255);     // id = 117     posX = 240 


			_PI->WriteHiHook(0x4DE980, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Create);
			_PI->WriteHiHook(0x4E1CC0, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Update);
			_PI->WriteHiHook(0x4E1940, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_UpdateLuckMoral);
			_PI->WriteHiHook(0x4DD540, SPLICE_, EXTENDED_, THISCALL_, Y_HeroDlg_Proc);

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}