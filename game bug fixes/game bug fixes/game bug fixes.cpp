#include "..\..\..\include\homm3.h"
#include "..\..\..\include\era.h"
using namespace Era;

Patcher* _P;
PatcherInstance* _PI;

char myString[1024];
#define MyString  (char*)myString

static _bool_ TIPHON = false;

#include "src\RoseKavalier.cpp"
#include "src\Ben.cpp"
#include "src\Npc.cpp"
#include "src\Monsters.cpp"
#include "src\Graphics.cpp"
#include "src\Spells.cpp"
#include "src\SammonCreatures.cpp"
#include "src\GameLogic.cpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HooksInit(Patcher* _P, PatcherInstance* _PI)
{
    RK(_P, _PI);
    Ben(_PI);
    Npc(_PI);
    Monsters(_PI);
    Graphics(_PI);
    Spells(_PI);
    WOG_SammonCreatures(_PI);
    GameLogic(_PI);    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static _bool_ bug_fixes_On = false;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!bug_fixes_On)
        {
            bug_fixes_On = true;    

            _P = GetPatcher();
            _PI = _P->CreateInstance("game bug fixes extended"); 

            ConnectEra();

            if (PluginExists("typhon"))
                TIPHON = true;

            if (_PI)
                HooksInit(_P, _PI);

        }
    break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
    }
    return TRUE;
}
