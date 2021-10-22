//////////////////////////////////////////////////////////////////////////////////
//////////////////////// Воговский текстовый диалог //////////////////////////////
////////////////// Воговский диалог с картинкой или дефом ////////////////////////

int __stdcall Y_Dlg_WoGMessage(HiHook* hook, char* Msg1, char* Msg2, char* Msg3, char* Msg4, char* Msg5)
{
    std::string message;

    if(Msg1) {        
        message.append(Msg1);
        message.append("\n");
    }
    if(Msg2) {        
        message.append(Msg2);
        message.append("\n");
    }
    if(Msg3) {        
        message.append(Msg3);
        message.append("\n");
    }
    if(Msg4) {        
        message.append(Msg4);
        message.append("\n");
    }
    if(Msg5)        
        message.append(Msg5);

    b_MsgBox((char*)message.c_str(), 5);

    return 0;
}


/////////////////////////////////////////////////////////////////////
// замена воговского диалога с одной картинкой и текстом (будет PCX8)  
void __stdcall Y_WoG_PicDlg_Pcx8(HiHook* hook, char* Msg1, char* Pic)
{
    if ( !Pic )
        Pic = "default.pcx";

    std::string name;
    name.append(Pic);

    // переводим строку в нижний регистр
    transform(name.begin(), name.end(), name.begin(), tolower);

    // получаем расширение файла
    std::string extension = name.substr(name.find_last_of(".") + 1);

    if( extension == "pcx" )
        b_MsgBoxAndPcx8(Msg1, Pic, 1);
    else {
        name.append("\n\nSorry! Only {*.pcx} images are supported!");
        b_MsgBox((char*)name.c_str(), 5);
    }
}

//////////////////////////////////////////////////////////////////////////////
// замена воговского диалога с одним дефом и текстом (будет анимированный деф)
void __stdcall Y_WoG_PicDlg_Def(HiHook* hook, char* Msg1, char* DefName)
{
    if ( !DefName )
        DefName = "CH00.def";

    std::string name;
    name.append(DefName);

    // переводим строку в нижний регистр
    transform(name.begin(), name.end(), name.begin(), tolower);

    // получаем расширение файла
    std::string extension = name.substr(name.find_last_of(".") + 1);

    if( extension == "def" )
        b_MsgBoxAndAnimatedDef(Msg1, DefName);
    else {
        name.append("\n\nSorry! Only {*.def} images are supported!");
        b_MsgBox((char*)name.c_str(), 5);
    }    
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Dlg_WoGMessageBox(PatcherInstance* _PI)
{
    // замена воговского диалога WoGMessageBox
    _PI->WriteHiHook(0x773C29, SPLICE_, SAFE_, CDECL_, Y_Dlg_WoGMessage);

    // замена воговского диалога с одной картинкой и текстом (будет PCX8)   
    _PI->WriteHiHook(0x773CE2, SPLICE_, EXTENDED_, CDECL_, Y_WoG_PicDlg_Pcx8);

    // замена воговского диалога с одним дефом и текстом (будет анимированный деф)  
    _PI->WriteHiHook(0x773D72, SPLICE_, EXTENDED_, CDECL_, Y_WoG_PicDlg_Def);

    // примеры запуска диалогов (можно и через ERM)
    // CALL_5(int, __cdecl, 0x773C29, текст1, текст2 или 0, текст3 или 0, текст4 или 0, текст5 или 0); // WoGMessage
    // CALL_2(void, __cdecl, 0x773CE2, текст, картинка);          // PicDlg
    // CALL_2(void, __cdecl, 0x773D72, текст, анимированный_деф); // PicDlg_Int
}
