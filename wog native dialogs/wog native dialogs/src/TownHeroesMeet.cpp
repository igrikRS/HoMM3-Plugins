// окно обмена героями в городе по клавише E 

bool inTownDlg;

int __stdcall Y_DlgTown_Proc(HiHook* hook, _TownMgr_* tm, _EventMsg_* msg)
{
    int result = CALL_2(int, __thiscall, hook->GetDefaultFunc(), tm, msg);

    inTownDlg = false;

    if (result) 
    {
        if (msg->type == MT_KEYDOWN && msg->subtype == HK_E) 
        {

            _DlgTextEdit_* editText = (_DlgTextEdit_*)tm->dlg->GetItem(7001);

            if (editText) 
            {
                if (!editText->enteringText) 
                {

                    int heroU_id = tm->town->up_hero_id;
                    int heroD_id = tm->town->down_hero_id;

                    if ( heroU_id != -1 && heroD_id != -1) 
                    {
                        _Hero_* heroU = o_GameMgr->GetHero(heroU_id);
                        _Hero_* heroD = o_GameMgr->GetHero(heroD_id);

                        inTownDlg = true;

                        if ( o_ExecMgr->next != (_Manager_*)o_WndMgr )
                        {
                            o_TownMgr->mgr.SetManagers(o_AdvMgr, o_WndMgr);
                            o_AdvMgr->mgr.SetManagers(NULL, o_WndMgr);
                            o_WndMgr->mgr.SetManagers(o_TownMgr, o_MouseMgr);
                        }

                        hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 1;

                        heroU->TeachScholar(heroD);
                        o_AdvMgr->SwapHeroes(heroU, heroD);

                        hdv(_bool_, "HotA.SwapMgrCalledFromTown") = 0;

                        o_TownMgr->UnHighlightArmy();       
                        o_TownMgr->Redraw();   

                        inTownDlg = false;
                    }
                }
            }
        }
    }

    return result;
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
