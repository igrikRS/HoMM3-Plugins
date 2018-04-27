// обновление артефактов в замке у торговцев артефактами
// каждую неделю из выкупленных (невыкупленные не меняются)

#include "stdafx.h"
#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;
static _bool_ plugin_On = 0;

int __stdcall Y_NewWeek(LoHook* h, HookContext* c)
{
	// артефакты у торговца в замке
	if (o_GameMgr->bMarketArt[0] == -1) o_GameMgr->bMarketArt[0] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
	if (o_GameMgr->bMarketArt[1] == -1) o_GameMgr->bMarketArt[1] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
	if (o_GameMgr->bMarketArt[2] == -1) o_GameMgr->bMarketArt[2] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
	if (o_GameMgr->bMarketArt[3] == -1) o_GameMgr->bMarketArt[3] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
	if (o_GameMgr->bMarketArt[4] == -1) o_GameMgr->bMarketArt[4] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
	if (o_GameMgr->bMarketArt[5] == -1) o_GameMgr->bMarketArt[5] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
	if (o_GameMgr->bMarketArt[6] == -1) o_GameMgr->bMarketArt[6] = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 8);

	// артефакты у торговцев на карте приключений
	if (o_GameMgr->bMarketOnMap_first != 0 ){
		for (int i = o_GameMgr->bMarketOnMap_first; i <= o_GameMgr->bMarketOnMap_last; i += 28){
			if (*(int*)(i + 0)  == -1) *(int*)(i + 0)  = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
			if (*(int*)(i + 4)  == -1) *(int*)(i + 4)  = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
			if (*(int*)(i + 8)  == -1) *(int*)(i + 8)  = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 2);
			if (*(int*)(i + 12) == -1) *(int*)(i + 12) = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
			if (*(int*)(i + 16) == -1) *(int*)(i + 16) = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
			if (*(int*)(i + 20) == -1) *(int*)(i + 20) = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 4);
			if (*(int*)(i + 24) == -1) *(int*)(i + 24) = CALL_2(int, __thiscall, 0x4C9190, o_GameMgr, 8);
		}
	}
    return EXEC_DEFAULT;
}


BOOL APIENTRY DllMain ( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	static _bool_ plugin_On = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!plugin_On)
		{
			plugin_On = 1;

			_P = GetPatcher();
			_PI = _P->CreateInstance("ArtInWeek");

			// при наступлении новой недели генерируем новые арты у торговцев
			_PI->WriteLoHook(0x4C8446, Y_NewWeek); 

		}
		break;

	case DLL_PROCESS_DETACH:
		if (plugin_On)
			plugin_On = 0;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}