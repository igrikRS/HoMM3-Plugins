//////////////////////////////////
// Author: [igrik]              //
// Date:   31.05.2019г.         //
// Thanks: baratorch, ZVS, MoP	//
//////////////////////////////////

#include "..\..\..\include\homm3.h"
#include "..\..\..\include\HoMM3_Dlg.cpp"
#include "..\..\..\include\HoMM3_Extra.h" 
#include "..\..\..\include\WogClasses.h"

#include "..\..\..\include\era.h"
// using namespace Era;

Patcher* _P;
PatcherInstance* _PI;


// переменная, для чистого WND
// для Берса ее коментирую
#define DOP_FUNK_TO_ERA

struct _TXT_;
_TXT_* txtresWOG;
_TXT_* WogNDlg_TXT;

char* n_medfont2 = "medfont2.fnt";
char* n_smallfont2 = "smalfont2.fnt";
char* n_bigfont2 = "bigfont2.fnt";

int saveCursor[3];

char myString1[1024];
char myString2[1024];
char myString3[1024];

#define o_HD_X (*(_int_*)0x401448)
#define o_HD_Y (*(_int_*)0x40144F)

#define isRusLang (*(_int_*)0x80321C)

// подключение всех диалогов по отдельности
#include "src\ChangeBmp.cpp"
#include "src\NPC.cpp"
#include "src\CrExpo.cpp"
#include "src\Curse.cpp"
#include "src\DropArt.cpp"
#include "src\Sphinx.cpp"
#include "src\IFG.cpp"
#include "src\ChooseAttack.cpp"
#include "src\WoGOptions.cpp"
#include "src\CastleReBuild.cpp"
#include "src\HD5_Functions.cpp"

#ifdef DOP_FUNK_TO_ERA
#include "src\NPC_AIValue.cpp"
#include "src\TownHeroesMeet.cpp"
#include "src\Battle_ShowKilled.cpp"
#endif DOP_FUNK_TO_ERA


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

const char* ERA_version;

int __stdcall Y_Dlg_MainMenu_Create(HiHook* hook, _Dlg_* dlg) 
{
	int ret = CALL_1(int, __thiscall, hook->GetDefaultFunc(), dlg);

	ERA_version = Era::GetEraVersion();
	sprintf(o_TextBuffer, "HoMM3 ERA %s", ERA_version );
	dlg->AddItem(_DlgStaticText_::Create(596, 576, 200, 20, o_TextBuffer, n_medfont2, 7, 545, ALIGN_H_RIGHT | ALIGN_V_BOTTOM, 0)); 

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall Y_LoadAllTXTinGames(LoHook* h, HookContext* c)
{
	txtresWOG = _TXT_::Load( "\\zvs\\Lib1.res\\txtres.txt" );
	WogNDlg_TXT = _TXT_::Load( "WogNDlg.txt" );
	return EXEC_DEFAULT;
}
//-----------------------------------------------------------------------
int __stdcall Y_Hook_MainLoop(LoHook* h, HookContext* c)
{	
	// загружаем необходимые русскоязычные игровые шрифты
	bigfont2->Load(n_bigfont2); 	//_Fnt_* bigfont2;
	medfont2->Load(n_medfont2);		//_Fnt_* medfont2;
	smalfont2->Load(n_smallfont2);	//_Fnt_* smalfont2;

	bigfont2 = _Fnt_::Load(n_bigfont2);
	medfont2 = _Fnt_::Load(n_medfont2);
	smalfont2 = _Fnt_::Load(n_smallfont2);

	Dlg_WoGOptions(_PI); 			// диалог WoG Опций

#ifdef DOP_FUNK_TO_ERA
	Battle_ShowKilled(_PI);			// показ предполагаемого количества убитых монстров при атаке и стрельбе
#endif DOP_FUNK_TO_ERA

	return EXEC_DEFAULT;
} 


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


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
			_PI = _P->CreateInstance("WoG Native Dialogs"); 

			// подтягиваем ERA
			Era::ConnectEra();

			// // загружаем HD данные
			// _HD = _P->GetInstance("HD.WoG"); 
			// if (_HD) { isHD = true; }

			// создаем загрузку необходимых тектовиков
			_PI->WriteLoHook(0x4EDD65, Y_LoadAllTXTinGames);

			// загрузка MainLoop
			_PI->WriteLoHook(0x4EEAC0, Y_Hook_MainLoop);

			// делаем показ версии игры в главном меню
			_PI->WriteHiHook(0x4FB930, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_MainMenu_Create);

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

			Dlg_ChangeBmp(_PI);	// все замены кадров
			Dlg_ExpaMon(_PI); 	// диалог Экспы монстров
			Dlg_NPC(_PI); 		// диалог командира	
			Dlg_Curse(_PI);     // диалог проклятий и благословлений
			Dlg_DropArt(_PI);   // диалог сброса артефактов на землю	
			Dlg_Sphinx(_PI); 	// диалог сфинкса и IF:D/E
			Dlg_IFG(_PI); 		// диалог IF:G
			Dlg_ChooseAttack(_PI); 	// диалог выбора типа атаки монстром
			Dlg_CastleReBuild(_PI);	// диалог перестройки замка
			HD5_Functions(_PI); 	// реализация функций, вырезанных в HD 5 версии
#ifdef DOP_FUNK_TO_ERA
			Dlg_NPC_AIValue(_PI);  // расчет силы командира для AI
			Dlg_TownHeroesMeet(_PI);	// обмен героями в замке ко клавише E
#endif DOP_FUNK_TO_ERA	
			// char* oVersionERA = "{Game Version:}\n\nHoMM3 ERA 2.7.7 \n (with Wog Native Dialogs)";
			// _PI->WriteDword(0x7066E1 +1, *(int*)&oVersionERA); // ENG
			// _PI->WriteDword(0x7066CE +1, *(int*)&oVersionERA); // RUS

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


