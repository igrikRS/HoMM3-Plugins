#include "..\..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

#include "src\RoseKavalier.cpp"
#include "src\Ben.cpp"
#include "src\Npc.cpp"
#include "src\Monsters.cpp"
#include "src\Graphics.cpp"
#include "src\Spells.cpp"
#include "src\GameLogic.cpp"

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

            RK(_P, _PI);
            Ben(_PI);
            Npc(_PI);
            Monsters(_PI);
            Graphics(_PI);
            Spells(_PI);
            GameLogic(_PI);

        }
    break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    break;
    }
    return TRUE;
}
