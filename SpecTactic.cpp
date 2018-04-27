#include "..\..\include\homm3.h"

// ќбъекты patcher_x86.
Patcher* _P;
PatcherInstance* _PI;



// расчет силы бонуса у специалистов по тактике
int __stdcall GetSpecTacticPower(_Hero_* heroA, _Hero_* heroD)
{
	short powerA = 0;
	short powerD = 0;
	if (heroA)
	{
		int SpecA0 = *(int*)(*(int*)0x679C80 + heroA->id *40);
		int SpecA1 = *(int*)(*(int*)0x679C80 + heroA->id *40 + 4);
		if(SpecA0 == 0 && SpecA1 == 19)
			powerA = heroA->level / 5;
	}
	if (heroD)
	{
		int SpecD0 = *(int*)(*(int*)0x679C80 + heroD->id *40);
		int SpecD1 = *(int*)(*(int*)0x679C80 + heroD->id *40 + 4);
		if(SpecD0 == 0 && SpecD1 == 19)
			powerD = heroD->level / 5;
	}
	return (powerA - powerD);
}

// дл€ курсора
int __stdcall TacticSpecHeroCur(LoHook* h, HookContext* c)
{
	c->eax += GetSpecTacticPower(o_BattleMgr->hero[0], o_BattleMgr->hero[1]);
	return EXEC_DEFAULT;
} 
// дл€ боевой сетки
int __stdcall TacticSpecHeroHex(LoHook* h, HookContext* c)
{
	c->ecx += GetSpecTacticPower(o_BattleMgr->hero[0], o_BattleMgr->hero[1]);
	return EXEC_DEFAULT;
} 

// в настройке битвы (ставим тактику)
int __stdcall SetTactic(LoHook* h, HookContext* c)
{
	// разрешаем тактическую фазу даже без втор.навыка
	if ( GetSpecTacticPower(o_BattleMgr->hero[0], o_BattleMgr->hero[1]) != 0 && o_BattleMgr->Field<_byte_>(+0x53C5) != 1 )
	{	
		o_BattleMgr->Field<_byte_>(+0x13D68) = 1;
	}


	// запрет тактической фазы, если бонус и уровень втор.навыка равны (всмысле конечный бонус)
	if( GetSpecTacticPower(o_BattleMgr->hero[0], o_BattleMgr->hero[1]) + ( o_BattleMgr->Field<_int_>(+0x13D70) * 2) == 0 )
	{
		o_BattleMgr->Field<_byte_>(+0x13D68) = 0;
		o_BattleMgr->Field<_int_>(+0x13D70) = 0;
	} 

	return EXEC_DEFAULT;
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
			_PI = _P->CreateInstance("TSW_TacticSpec"); 

			_PI->WriteLoHook(0x47625D, TacticSpecHeroCur);	// јтакующий дл€ курсора
			_PI->WriteLoHook(0x475D3A, TacticSpecHeroHex);	// јтакующий дл€ боевой сетки

			_PI->WriteLoHook(0x476268, TacticSpecHeroCur);	// «ащитник дл€ курсора
			_PI->WriteLoHook(0x475D5E, TacticSpecHeroHex);	// «ащитник дл€ боевой сетки

			_PI->WriteLoHook(0x46285C, SetTactic);			// в начале битвы при настройке тактики
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}