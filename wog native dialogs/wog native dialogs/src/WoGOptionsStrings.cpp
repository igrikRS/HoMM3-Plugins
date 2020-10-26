/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// заполнение строк диалога WoG Опций из JSON файла ///////////////////////////

// Особая благодарность Берсеркеру!
// Он помог найти и исправить сложный вылет при отсутвствии файла ZSETUP00.TXT :-)


#define o_WogOptions ((_DlgSetup_*)0x2918390)
#define o_ChooseFile ((_ChooseFile_*)0x7B3614)

char strPage[1024];
char strList[1024];

char* textPage = "wog_options.page%d.%s";
char* list = "group%d.%s";

char* name = "name";
char* hint = "hint";
char* popUp = "popup";	

char* optionsName = "wog_options.main.name";
char* optionsHint = "wog_options.main.hint";
char* optionsPopUp = "wog_options.main.popup";
char* optionsIntro = "wog_options.main.intro";

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

// функия заполнения строк: 
// имеет всего один вызов из 0x77941A
void __stdcall WOG_ProcessAll(HiHook* h)
{
	// НЕ вызываем орининальную функцию
	// CALL_0(void, __cdecl, h->GetDefaultFunc() );  // потом обязательно закоментировать

	// получаем структуру диалога ВОГ Опций
	_DlgSetup_* ds = o_WogOptions;

	// вызываем функцию BuildAll();
	CALL_0(void, __cdecl, 0x7787CE); 

	// А теперь сами реализуем заполнение строк
	ds->Name = GetEraJSON(optionsName);
	ds->Hint = GetEraJSON(optionsHint);
	ds->PopUp = GetEraJSON(optionsPopUp);
	ds->Intro = GetEraJSON(optionsIntro);

	for (int i = 0; i < 8; i++) {
		sprintf(strPage, textPage, i, name);
		ds->Pages[i]->Name = GetEraJSON(strPage);

		sprintf(strPage, textPage, i, hint);
		ds->Pages[i]->Hint = GetEraJSON(strPage);

		sprintf(strPage, textPage, i, popUp);
		ds->Pages[i]->PopUp = GetEraJSON(strPage);

		sprintf(strPage, textPage, i, list);
		for(int j = 0; j < 4; j++){
			sprintf(strList, strPage, j, name);
			ds->Pages[i]->ItemList[j]->Name = GetEraJSON(strList);

			sprintf(strList, strPage, j, hint);
			ds->Pages[i]->ItemList[j]->Hint = GetEraJSON(strList);

			sprintf(strList, strPage, j, popUp);
			ds->Pages[i]->ItemList[j]->PopUp = GetEraJSON(strList);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Dlg_SaveDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

	cf->Caption = GetEraJSON("dlg_datfile.captionsave");
	cf->Description = GetEraJSON("dlg_datfile.descrsave");
	cf->Mask = GetEraJSON("dlg_datfile.filemask");

	return EXEC_DEFAULT;
} 

int __stdcall Dlg_SaveDatFile2(LoHook* h, HookContext* c)
{
	c->eax = (int)GetEraJSON("dlg_datfile.cannotsave");

	return EXEC_DEFAULT;
} 

int __stdcall Dlg_LoadDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

	cf->Caption = GetEraJSON("dlg_datfile.captionload");
	cf->Description = GetEraJSON("dlg_datfile.descrload");
	cf->Mask = GetEraJSON("dlg_datfile.filemask");

	return EXEC_DEFAULT;
} 

int __stdcall Dlg_LoadDatFile2(LoHook* h, HookContext* c)
{
	c->eax = (int)GetEraJSON("dlg_datfile.cannotload");

	return EXEC_DEFAULT;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


void WoGOptionsStrings(PatcherInstance* _PI)
{
	if ( ERA_VERSION >= 3000 ) { // только ERA III
		_PI->WriteHiHook(0x778A9D, SPLICE_, EXTENDED_, CDECL_, WOG_ProcessAll);

		// save
		_PI->WriteCodePatch(0x77793D, "%n", 75); // 75 nops 
		_PI->WriteCodePatch(0x7779ED, "%n", 17); // 17 nops 
		_PI->WriteLoHook(0x777983, Dlg_SaveDatFile1);
		_PI->WriteLoHook(0x7779F9, Dlg_SaveDatFile2);

		// load
		_PI->WriteCodePatch(0x777A87, "%n", 75); // 75 nops 
		_PI->WriteCodePatch(0x777B6E, "%n", 17); // 17 nops 
		_PI->WriteLoHook(0x777ACD, Dlg_LoadDatFile1);
		_PI->WriteLoHook(0x777B7A, Dlg_SaveDatFile2);

		// вырезаем загрузку файла "ZSETUP00.TXT"
		_PI->WriteCodePatch(0x7792ED, "%n", 37); 
	}

	return;
}


