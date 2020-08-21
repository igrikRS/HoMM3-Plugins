// окно обмена героями в городе по клавише E 

bool inTownDlg;

char __stdcall Y_DlgTown_Proc(HiHook* hook, _TownMgr_* tm, _EventMsg_* klick)
{
   char res = CALL_2(char, __thiscall, hook->GetDefaultFunc(), tm, klick);
   inTownDlg = false;

   if (res) {
      if (klick->type == 2 && klick->subtype == HK_E) {
         int heroU_id = tm->town->up_hero_id;
         int heroD_id = tm->town->down_hero_id;

         if ( heroU_id != -1 && heroD_id != -1) {
            _Hero_* heroU = o_GameMgr->GetHero(heroU_id);
            _Hero_* heroD = o_GameMgr->GetHero(heroD_id);

            inTownDlg = true;

            if ( *(int*)((int)o_ExecMgr +4) != (int)o_WndMgr ) {
               *(int*)((int)o_TownMgr +4) = (int)o_AdvMgr;
               *(int*)((int)o_TownMgr +8) = (int)o_WndMgr;

               *(int*)((int)o_AdvMgr +4) = NULL;
               *(int*)((int)o_AdvMgr +8) = (int)o_WndMgr;            

               *(int*)((int)o_WndMgr +4) = (int)o_TownMgr;
               *(int*)((int)o_WndMgr +8) = (int)o_MouseMgr;
            }

			hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 1;

			
			tm->dlg->GetItem(30720)->SetEnabled(false); // отключаем кнопку OK в городе

            CALL_2(void, __fastcall, 0x4A25B0, heroU, heroD);
            CALL_3(void, __thiscall, 0x4AAA60, o_TownMgr, heroU, heroD);
			
			tm->dlg->GetItem(30720)->SetEnabled(true); // включаем кнопку OK в городе


			hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 0;


            CALL_1(void, __thiscall, 0x5D5930, o_TownMgr);       
            CALL_1(void, __thiscall, 0x5D5810, o_TownMgr);   

            inTownDlg = false;

            return 1;   
         }
      }
   }

   return res;
}

int __stdcall Y_Dlg_HeroesMeet(LoHook* h, HookContext* c)
{   
   if ( inTownDlg ) { // пропускаем обновление экрана
      c->return_address = 0x4AAC2A;
      return NO_EXEC_DEFAULT;
   }
   return EXEC_DEFAULT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Dlg_TownHeroesMeet(PatcherInstance* _PI)
{
	// обмен героями в замке ко клавише E
	_PI->WriteHiHook(0x5D3640, SPLICE_, EXTENDED_, THISCALL_, Y_DlgTown_Proc);
	_PI->WriteLoHook(0x4AAC1B, Y_Dlg_HeroesMeet);
}
