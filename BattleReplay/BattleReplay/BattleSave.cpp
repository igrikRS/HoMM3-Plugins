#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

char* PLUGIN_NAME = "igrik.BattleSave";

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

int __stdcall Y_SetBattleSave(LoHook* h, HookContext* c)
{	
	_Hero_* atacker = *(_Hero_**)0x2860248;

    if (atacker) 
    {

	    int meID = o_GameMgr->GetMe()->id;
	    int attackerID = atacker->owner_id;

	    if ( meID == attackerID ) 
		    CALL_6(char, __thiscall, 0x4BEB60, o_GameMgr, "BATTLE!", 1, 0, 1, 0);

    }

	return EXEC_DEFAULT; 
}


void StartPlugin()
{	
    // ןונוה בטעגמי סמהא¸ל סויג "BATTLE!"
    _PI->WriteLoHook(0x75AE24, Y_SetBattleSave);  
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	static _bool_ initialized = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!initialized)
		{
			initialized = 1;

			_P = GetPatcher();
			_PI = _P->CreateInstance(PLUGIN_NAME);

			StartPlugin();
			
		}
		break;

	case DLL_PROCESS_DETACH:
		if (initialized)
			initialized = 0;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
