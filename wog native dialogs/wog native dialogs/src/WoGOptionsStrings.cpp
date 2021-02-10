/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// заполнение строк диалога WoG Опций из JSON файла ///////////////////////////

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

void SetupJsonText( char* target, const char* jsonName)
{
	char* jsonText = GetEraJSON(jsonName);

	if ( strcmp(jsonName, jsonText) ) {
		strcpy(target, jsonText);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

// функия заполнения строк: 
// имеет всего один вызов из 0x77941A
void __stdcall WOG_ProcessAll(HiHook* h)
{
	// не вырезаем оригинальную функцию (обратная совместимость)
	CALL_0(void, __cdecl, h->GetDefaultFunc() ); 

	// получаем структуру диалога ВОГ Опций
	_DlgSetup_* ds = o_WogOptions;

	SetupJsonText(ds->Name, optionsName);
	SetupJsonText(ds->Hint, optionsHint);
	SetupJsonText(ds->PopUp, optionsPopUp);
	SetupJsonText(ds->Intro, optionsIntro);

	for (int i = 0; i < 8; i++) {
		sprintf(strPage, textPage, i, name);
		SetupJsonText(ds->Pages[i]->Name, strPage);

		sprintf(strPage, textPage, i, hint);
		SetupJsonText(ds->Pages[i]->Hint, strPage);

		sprintf(strPage, textPage, i, popUp);
		SetupJsonText(ds->Pages[i]->PopUp, strPage);

		sprintf(strPage, textPage, i, list);
		for(int j = 0; j < 4; j++){
			sprintf(strList, strPage, j, name);
			SetupJsonText(ds->Pages[i]->ItemList[j]->Name, strList);

			sprintf(strList, strPage, j, hint);
			SetupJsonText(ds->Pages[i]->ItemList[j]->Hint, strList);

			sprintf(strList, strPage, j, popUp);
			SetupJsonText(ds->Pages[i]->ItemList[j]->PopUp, strList);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Dlg_SaveDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

	SetupJsonText(cf->Caption, "dlg_datfile.captionsave");
	SetupJsonText(cf->Description, "dlg_datfile.descrsave");
	SetupJsonText(cf->Mask, "dlg_datfile.filemask");

	return EXEC_DEFAULT;
} 

//int __stdcall Dlg_SaveDatFile2(LoHook* h, HookContext* c)
//{
//	c->eax = (int)GetEraJSON("dlg_datfile.cannotsave");
//
//	return EXEC_DEFAULT;
//} 

int __stdcall Dlg_LoadDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

	SetupJsonText(cf->Caption, "dlg_datfile.captionload");
	SetupJsonText(cf->Description, "dlg_datfile.descrload");
	SetupJsonText(cf->Mask, "dlg_datfile.filemask");

	return EXEC_DEFAULT;
} 

//int __stdcall Dlg_LoadDatFile2(LoHook* h, HookContext* c)
//{
//	// c->eax = (int)GetEraJSON("dlg_datfile.cannotload");
//	SetupJsonText((char*)c->eax, "dlg_datfile.cannotload");
//
//	return EXEC_DEFAULT;
//} 

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


void WoGOptionsStrings(PatcherInstance* _PI)
{
	if ( ERA_VERSION >= 3000 ) { // только ERA III
		_PI->WriteHiHook(0x778A9D, SPLICE_, EXTENDED_, CDECL_, WOG_ProcessAll);

		// save
		_PI->WriteLoHook(0x7779A9, Dlg_SaveDatFile1);

		// load
		_PI->WriteLoHook(0x777AF3, Dlg_LoadDatFile1);

	}

	return;
}


