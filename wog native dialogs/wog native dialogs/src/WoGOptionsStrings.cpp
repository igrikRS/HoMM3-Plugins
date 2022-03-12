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

_bool_ testUseJsonString = false;

// функция проверяет есть ли отладочный текст в wnd.json
// для удобства заполнения строк через json
void IsNeedUseJsonString()
{
    // узнаём есть ли строка ("test_json_strings": "test") в json файле 
    string testJson = GetEraJSON("wnd.dlg_wog_options.test_json_strings");
    string test = "test"; 

    // переводим строку в нижний регистр
    transform(testJson.begin(), testJson.end(), testJson.begin(), tolower); 

    if (testJson == test)
        testUseJsonString = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void SetupJsonText( char** target, const char* jsonName)
{
    // проверяем существует ли в JSON файле "заполненная строка"
    // если будет пустышка - не будем перезаписывать
    string s1 = jsonName;
    string s2 = GetEraJSON(jsonName);

    // перезаписываем строку текста
    if (testUseJsonString || s1 != s2)
        *target = GetEraJSON(jsonName);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

// функия заполнения строк: 
// имеет всего один вызов из 0x77941A
void __stdcall WOG_ProcessAll(HiHook* h)
{
    // не вырезаем оригинальную функцию (обратная совместимость)
    CALL_0(void, __cdecl, h->GetDefaultFunc() ); 

    // проверка на необходимость подмены строк из JSON файла
    // должна быть ("use_json_strings": "on")
    string useJson = GetEraJSON("wnd.dlg_wog_options.use_json_strings");
    string campareString = "on";

    // переводим строку useJson в нижний регистр
    transform(useJson.begin(), useJson.end(), useJson.begin(), tolower);    

    // выходим, если не нужно загружать строки из JSON файла
    if (useJson != campareString)
        return;

    // для удобства заполнения строк через json (отладочный функционал)
    // при необходимости внести строку ("test_json_strings": "test")
    IsNeedUseJsonString();

    // получаем структуру диалога ВОГ Опций
    _DlgSetup_* ds = o_WogOptions;

    SetupJsonText(&ds->Name, optionsName);
    SetupJsonText(&ds->Hint, optionsHint);
    SetupJsonText(&ds->PopUp, optionsPopUp);
    SetupJsonText(&ds->Intro, optionsIntro);

    for (int i = 0; i < 8; i++) {
        sprintf(strPage, textPage, i, name);
        SetupJsonText(&ds->Pages[i]->Name, strPage);

        sprintf(strPage, textPage, i, hint);
        SetupJsonText(&ds->Pages[i]->Hint, strPage);

        sprintf(strPage, textPage, i, popUp);
        SetupJsonText(&ds->Pages[i]->PopUp, strPage);

        sprintf(strPage, textPage, i, list);
        for(int j = 0; j < 4; j++){
            sprintf(strList, strPage, j, name);
            SetupJsonText(&ds->Pages[i]->ItemList[j]->Name, strList);

            sprintf(strList, strPage, j, hint);
            SetupJsonText(&ds->Pages[i]->ItemList[j]->Hint, strList);

            sprintf(strList, strPage, j, popUp);
            SetupJsonText(&ds->Pages[i]->ItemList[j]->PopUp, strList);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Dlg_SaveDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

    SetupJsonText(&cf->Caption, "dlg_datfile.captionsave");
    SetupJsonText(&cf->Description, "dlg_datfile.descrsave");
    SetupJsonText(&cf->Mask, "dlg_datfile.filemask");

    return EXEC_DEFAULT;
} 

//int __stdcall Dlg_SaveDatFile2(LoHook* h, HookContext* c)
//{
//  c->eax = (int)GetEraJSON("dlg_datfile.cannotsave");
//
//  return EXEC_DEFAULT;
//} 

int __stdcall Dlg_LoadDatFile1(LoHook* h, HookContext* c)
{
    _ChooseFile_* cf = o_ChooseFile;

    SetupJsonText(&cf->Caption, "dlg_datfile.captionload");
    SetupJsonText(&cf->Description, "dlg_datfile.descrload");
    SetupJsonText(&cf->Mask, "dlg_datfile.filemask");

    return EXEC_DEFAULT;
} 

//int __stdcall Dlg_LoadDatFile2(LoHook* h, HookContext* c)
//{
//  // c->eax = (int)GetEraJSON("dlg_datfile.cannotload");
//  SetupJsonText((char*)c->eax, "dlg_datfile.cannotload");
//
//  return EXEC_DEFAULT;
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


