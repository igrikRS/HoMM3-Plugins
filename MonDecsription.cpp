// Расширение диалога монстров в бою. 
 
#include "..\..\include\homm3.h"

// Объекты patcher_x86.
Patcher* _P;
PatcherInstance* _PI;


int __stdcall Y_MoralLuckBonus(LoHook* h, HookContext* c)
{
	// цифровое отображение морали
	if (*(int*)(c->edi+1256) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->edi+1256));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->edi+1256));

	int bonMoralNew = CALL_1 (int, __cdecl, 6386834, 80);
	int bonMoral = CALL_12 (int, __thiscall, 6014624, bonMoralNew, 48, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, 3006, 10, 0, 8);

	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx, *(int*)(c->ebx+8), 1, &bonMoral); 

	// цифровое отображение удачи
	if (*(int*)(c->edi+1260) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->edi+1260));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->edi+1260));

	int bonLuckNew = CALL_1 (int, __cdecl, 6386834, 80);
	int bonLuck = CALL_12 (int, __thiscall, 6014624, bonLuckNew, 101, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, 3007, 10, 0, 8);

	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx, *(int*)(c->ebx+8), 1, &bonLuck); 

	return EXEC_DEFAULT;
}

int __stdcall Y_MoralLuckBonus2(LoHook* h, HookContext* c)
{
	if (*(int*)(c->ebx+104) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->ebx+104));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->ebx+104));	

	int bonMoralNew = CALL_1 (int, __cdecl, 6386834, 80);
	int bonMoral = CALL_12 (int, __thiscall, 6014624, bonMoralNew, 48, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, 3006, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx+48, *(int*)(c->ebx+56), 1, &bonMoral);

	// отображение единиц удачи
	if (*(int*)(c->ebx+124) < 1)
		sprintf(o_TextBuffer, "%d", *(int*)(c->ebx+124));
	else
		sprintf(o_TextBuffer, "+%d", *(int*)(c->ebx+124));	

	int bonLuckNew = CALL_1 (int, __cdecl, 6386834, 80);
	int bonLuck = CALL_12 (int, __thiscall, 6014624, bonLuckNew, 101, 209, 20, 20, o_TextBuffer, "tiny.fnt", 4, 3007, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x5FE2D0, c->ebx+48, *(int*)(c->ebx+56), 1, &bonLuck);

	return EXEC_DEFAULT;
}


int __stdcall Y_SpellShow(LoHook* h, HookContext* c)
{
	// цифровое отображение длительности заклинаний
	int Spell = *(int*)(c->esi);
	int DlgShow = *(int*)(c->ebp-40);
	if (Spell >=0)
	{
		if (Spell == 47 || Spell == 59 || Spell == 72)
			int Spell = 47;
		else
		{
			sprintf(o_TextBuffer, "x%d", *(int*)(c->esi+12));
			int SpellNew = CALL_1 (int, __cdecl, 6386834, 80);
			int SpellShow = CALL_12 (int, __thiscall, 6014624, SpellNew, *(int*)(c->ebp-28), 202, 46, 20, o_TextBuffer, "tiny.fnt", 4, 3003 - *(int*)(c->ebp-32), 10, 0, 8);
			CALL_4 (int, __thiscall, 0x5FE2D0, DlgShow + 48, *(int*)(DlgShow +56), 1, &SpellShow); 
		}	
	}
	return EXEC_DEFAULT;
}


int __stdcall Y_SpellInfo1(LoHook* h, HookContext* c)
{
	// в этой функции я не смог поставить HiHook
	// потому что в WoGе после её выполнения вылетает
	if( c->eax >= 3000 && c->eax <= 3007 )
	{	
		switch (c->eax){
			case 3000: c->eax = 11; break;	// спелл_1
			case 3001: c->eax = 11; break;	// спелл_2
			case 3002: c->eax = 11; break;	// спелл_3	
			case 3006: c->eax = 9; break;	// мораль	
			case 3007: c->eax = 10; break;	// удача
			default: c->eax = -1; break;	// на всякий
		}		
		c->return_address = 0x5F4B34;
		return NO_EXEC_DEFAULT;	
	}
	return EXEC_DEFAULT;
}


int __stdcall Y_SpellInfo2(LoHook* h, HookContext* c)
{
	// подсказка для циферных отображений спеллов, морали и удачи
	switch (c->esi){
		case 3000: c->esi = 221; break; // спелл_1
		case 3001: c->esi = 222; break; // спелл_2
		case 3002: c->esi = 223; break; // спелл_3	
		case 3006: c->esi = 219; break; // мораль	
		case 3007: c->esi = 220; break;	// удача
	}
	return EXEC_DEFAULT;
}


int __stdcall Y_SpellShow1(LoHook* h, HookContext* c)
{
	int New = CALL_1 (int, __cdecl, 0x617492, 80);
	int SpellShow = CALL_12 (int, __thiscall, 6014624, New, 15, c->ebx + 16, 46, 20, "", "tiny.fnt", 4, c->edi + 3003, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x4230D0, c->esi, *(int*)(c->esi + 8), 1, &SpellShow); 
	return EXEC_DEFAULT;
}

int __stdcall Y_SpellShow2(LoHook* h, HookContext* c)
{
	int New = CALL_1 (int, __cdecl, 0x617492, 80);
	int SpellShow = CALL_12 (int, __thiscall, 6014624, New, 15, *(int*)(c->ebp + 24) + 16, 46, 20, "", "tiny.fnt", 4, c->ebx + 3003, 10, 0, 8);
	CALL_4 (int, __thiscall, 0x4230D0, c->esi, *(int*)(c->esi + 8), 1, &SpellShow); 
	return EXEC_DEFAULT;
}


int __stdcall Y_SpellShow3(LoHook* h, HookContext* c)
{
	_Dlg_* dlg = (_Dlg_*)(c->edi + 56);
	int item = 3006 - *(int*)(c->ebp + 8);
	int spell = *(int*)c->esp - 1;

	if (spell >= 0 && spell != 47 && spell != 59 && spell != 72)
		sprintf(o_TextBuffer, "x%d", *(int*)(c->esi + 4 * spell + 408));
	else
		sprintf(o_TextBuffer, "");

	((_DlgStaticText_*)dlg->GetItem(item))->SetText(o_TextBuffer);

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
			_PI = _P->CreateInstance("MonDescription");     	  
   	  
			// Патч на tiny.fnt малые описания монстров
			*(int*)6243434 = 6687924; // в бою
			*(int*)6244485 = 6687924; // при покупке
			*(int*)6241855 = 6687924; // не в бою
				
			_PI->WriteLoHook(0x5F3C43, Y_MoralLuckBonus);	// мораль и удача в бою
			_PI->WriteLoHook(0x5F439B, Y_MoralLuckBonus2);	// мораль и удача вне в боя

			// отображение длительности заклинаний в окне монстра (из HotA)
			_PI->WriteLoHook(0x5F6BE2, Y_SpellShow);
			_PI->WriteLoHook(0x5F4B05, Y_SpellInfo1);  // тут вог не дает поставить HiHook  
			_PI->WriteLoHook(0x5F522A, Y_SpellInfo2); 
			// отображение длительности заклинаний в расширенной статистике (из HotA)
			_PI->WriteLoHook(0x46D12B, Y_SpellShow1);	
			_PI->WriteLoHook(0x46D4CB, Y_SpellShow2);	
			_PI->WriteLoHook(0x46D9F0, Y_SpellShow3);

		}
   	  
   	case DLL_THREAD_ATTACH:
   	case DLL_THREAD_DETACH:
   	case DLL_PROCESS_DETACH:
 	   	break;
 	}
 	return TRUE;
}
/* СПРАВКА:
sprintf(o_TextBuffer, "Тип %d, подтип %d", ObjectMain_Type[1], ObjectMain_Type[2]); 
b_MsgBox(o_TextBuffer, 1);
EDI (28), ESI (24), EBP (20), ESP (16), EBX (12), EDX (8), ECX (4), EAX (0)
*/