////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Callback диалога MsgBox //////////////////////////////////////////////////////

// по большей части тупо переписан оригинальный код функции 0x4F1650,
// но в этом коде расширено кол-во кликабельных желтых рамок.
// В оригинале было всего 2 кликабельные рамки: 30729 и 30730.
// Теперь же работают все рамки: 30729-30736
// И в довесок реализован даббл.клик выбора

#define P_Dlg_MsgBox (*(_Dlg_**)0x6995E0)

#define MSG_10_OK     30720
#define MSG_10_CANCEL 30721

#define MSG_1_OK      30722
#define MSG_2_OK      30725
#define MSG_2_CANCEL  30726

#define MSG_10_IT0    30729
#define MSG_10_IT1    30730
#define MSG_10_IT2    30731
#define MSG_10_IT3    30732
#define MSG_10_IT4    30733
#define MSG_10_IT5    30734
#define MSG_10_IT6    30735
#define MSG_10_IT7    30736

int my_TimeClick_MsgBox;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// общая функция обновления всех желтых рамок диалога
int F_MsgBox_ResetYellowFrames(_Dlg_* dlg, int itemID)
{
    _DlgItem_* it;

    for (int i = MSG_10_IT0; i <= MSG_10_IT7; i++) {
        _DlgItem_* it = dlg->GetItem(i);
        if ( it ) { 
            it->SendCommand(6, 4);
            it = NULL;
        }
    }
    it = dlg->GetItem(itemID);
    if ( it ) 
        it->SendCommand(5, 4);
    
    return 2;
}

// чтобы не дублировать код, пишем универсальную функцию выхода из MsgBox_Proc
int F_MsgBox_Return(_EventMsg_* msg, int itemID)
{
    msg->type = 0x200;
    msg->subtype = 10;
    msg->item_id = 10;
    o_WndMgr->result_dlg_item_id = itemID;
    o_TimeClick = 0;
    my_TimeClick_MsgBox = 0;

    return 2;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// *** по неоднократной просьбе Berserker'a ***
// функция поиска следующего элемента от текущего активного
// при выборе клавишами влево(-1) или вправо(+1).
// поддерживает циклический (круговой) выбор элементов

int Y_New_MsgBox_GetNextItem(int way, int currentItem)
{
    _Dlg_* dlg = P_Dlg_MsgBox;

    // получаем список (в битах) разрешённых элементов
    int bitAccess = dlg->GetItem(1525)->field_28;
    if ( bitAccess == -1 ) bitAccess = 255;

    // получаем список (в битах) существующих элементов
    int bitExist = 0;
    for (int i = 0; i <= 7; i++)
    {
        _DlgItem_* it = dlg->GetItem(i + MSG_10_IT0);

        if ( it ) // элемент существует - создаём его бит доступности
            bitExist |= (1 << i);
    }

    // прокрутка вправо (поиск следующего элемента)
    if ( way == 1 )
    {
        int nextItem = 0;
        if (currentItem != -1)
            nextItem = currentItem + 1; 
        
        for (int i = 0; i <= 7; i++)
        {
            int temp = i + nextItem;
            if ( temp > 7) temp -= 8;

            if ( bitExist >> temp & 1 ) 
                if ( bitAccess >> temp & 1 ) 
                    return temp;
        }
    }

    // прокрутка влево (поиск предыдущего элемента)
    if ( way == -1 )
    {
        int nextItem = 0;
        if (currentItem != -1)
            nextItem = currentItem - 1 + 1; 
        
        for (int i = 7; i >= 0; i--)
        {
            int temp = i + nextItem; 
            if ( temp > 7) temp -= 8; 

            if ( bitExist >> temp & 1 ) 
                if ( bitAccess >> temp & 1 ) 
                    return temp;
        }
    } 

    // элемент не найден
    return -1;

}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Callback диалога MsgBox (ПКМ обрабатывается в другом месте)

signed int __stdcall Y_New_MsgBox_Proc(HiHook* hook, _EventMsg_* msg)
{   
    if ( o_TimeClick ) 
    {
        int time = o_GetTime() - o_TimeClick;

        if ( time >= 0)
            return F_MsgBox_Return(msg, 9999);

    } 

    if (msg->type == MT_KEYDOWN)
    {
        // если тип сообщения: с выбором элементов
        if (b_MsgBox_Style_id == 7 || b_MsgBox_Style_id == 10) 
        {    
            // *** по неоднократной просьбе Berserker'a ***
            // прокрутка элементов списка по стрелкам влево/вправо
            if (msg->subtype == 75 /* <<--- */ || msg->subtype == 77 /* --->> */ ) 
            {    
                // получаем текущий выбранный элемент в диалоге
                int currentItem = b_MsgBox_Result_id;

                // если элемент есть, нужно передать его id в виде 0-7
                if (currentItem != -1) 
                    currentItem -= MSG_10_IT0;

                // функция нахождения следующего существующего и доступного элемента
                int nextItem = Y_New_MsgBox_GetNextItem( msg->subtype -76, currentItem );

                // если элемент найден - делаем его активным
                if (nextItem != -1) { 
                    msg->type = MT_MOUSEBUTTON;
                    msg->subtype = MST_LBUTTONCLICK;
                    msg->item_id = nextItem + MSG_10_IT0;
                }
            }
        }
    }
    
    int result = 1; 

    if (msg->type == MT_MOUSEBUTTON)
    {
        if (msg->subtype == MST_LBUTTONCLICK)
        {   
            _Dlg_* dlg = P_Dlg_MsgBox;
            int temp = 0;

            switch ( msg->item_id )
            {
                case MSG_10_OK:
                case MSG_10_CANCEL:
                case MSG_2_OK:
                case MSG_2_CANCEL:
                    result = F_MsgBox_Return(msg, msg->item_id);                        
                    break;


                case MSG_1_OK:
                    if (b_MsgBox_Style_id == 7 || b_MsgBox_Style_id == 10)
                        temp = b_MsgBox_Result_id;
                    else 
                        temp = msg->item_id;

                    result = F_MsgBox_Return(msg, temp); 
                    break;

                 case MSG_10_IT0:
                 case MSG_10_IT1:
                 case MSG_10_IT2:
                 case MSG_10_IT3:
                 case MSG_10_IT4:
                 case MSG_10_IT5:
                 case MSG_10_IT6:
                 case MSG_10_IT7:
                     if (b_MsgBox_Style_id == 7 || b_MsgBox_Style_id == 10)
                     {
                         // проверяем маску ERA: разрешён ли элемент для выбора
                         int bitAccess = dlg->GetItem(1525)->field_28;
                         
                         if ( bitAccess >> ( msg->item_id - MSG_10_IT0 ) & 1 ) 
                         {
                             b_MsgBox_Result_id = msg->item_id;
                             F_MsgBox_ResetYellowFrames(dlg, msg->item_id);
                             dlg->GetItem(MSG_1_OK)->SetEnabled(1);
                             dlg->Redraw(1);
                        }
                     }
                     break;

                default:
                    break;
            }                           
        }
        if (msg->subtype == MST_LBUTTONDOWN) // реализация даббл.клика
        {
            if (msg->item_id >= MSG_10_IT0 && msg->item_id <= MSG_10_IT7)   
            {   
                if (b_MsgBox_Style_id == 7 || b_MsgBox_Style_id == 10) {
                    if ( (o_GetTime() - my_TimeClick_MsgBox) < 300 && msg->item_id == b_MsgBox_Result_id) 
                    {   
                        e_ClickSound();
                        result = F_MsgBox_Return(msg, msg->item_id);
                    } else my_TimeClick_MsgBox = o_GetTime();
                }
            }
        }
    }

    // оригинальную функцию НЕ вызываем 
    return result;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// установка жёлтой рамки (создание активного элемента по умолчанию для ERA 3)
int __stdcall Y_New_MsgBox_SetDefaultFrameEnabled(LoHook* h, HookContext* c)
{
    _Dlg_* dlg = P_Dlg_MsgBox;

    // дефолтная маска: читаем маску, которую передала нам ERA 
    int bitAccess = Era::GetMaskMsgBoxItId();
    dlg->GetItem(1525)->field_28 = bitAccess;   

    if (b_MsgBox_Style_id == 7 || b_MsgBox_Style_id == 10)
    {   
        if ( ERA_VERSION < 3009 )
            return EXEC_DEFAULT;

        // получаем дефолтный активный элемент
        int itemID = Era::GetDefaultMsgBoxItId(); 

        if ( itemID >= 0 && itemID <= 7 )
        {
            // проверяем дефолтный элемент на разрешенную маску
            if ( bitAccess >> itemID & 1)
            {
                // создаём правильный id элемента
                itemID += MSG_10_IT0;

                // получаем структуры акт.элемента и кнопки ОК
                _DlgItem_* it = P_Dlg_MsgBox->GetItem(itemID);
                _DlgItem_* ok = P_Dlg_MsgBox->GetItem(MSG_1_OK);

                if ( it ) 
                {   
                    // подсвечиваем акт.элемент
                    it->SendCommand(5, 4);

                    // заносит результат в результирующую глобальную переменную
                    b_MsgBox_Result_id = itemID;

                    if (ok) // включаем кнопку ОК
                        ok->SetEnabled(1);
                }
            }
        }
    }
    // выполнить затёртый хуком код
    return EXEC_DEFAULT;
}

// для ERA 3: создание специального элемента диалога, 
// который будет хранить битовую маски "разрешенных к выбору элементов"
signed int __stdcall Y_New_MsgBox_GetBitMask(HiHook* hook, _GameMgr_* gm)
{   
    _Dlg_* dlg = P_Dlg_MsgBox;
    dlg->AddItem(_DlgItem_::Create(0, 0, 0, 0, 1525, 1));   

    return CALL_1(signed int, __thiscall, hook->GetDefaultFunc(), gm);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Dlg_MsgBox(PatcherInstance* _PI)
{
    // новый Callback диалога MsgBox
    _PI->WriteHiHook(0x4F1650, SPLICE_, EXTENDED_, THISCALL_, Y_New_MsgBox_Proc);

    // установка дефолтной желтой рамки (как буд-то она уже выбранна)
    _PI->WriteLoHook(0x4F7B46, Y_New_MsgBox_SetDefaultFrameEnabled);

    // создание элемента для хранения "разрешенных к выбору элементов"
    _PI->WriteHiHook(0x4F71BB, CALL_, EXTENDED_, THISCALL_, Y_New_MsgBox_GetBitMask);

    // правильное смещение для жёлтых рамок
    _PI->WriteByte(0x4F7985 +2, 1); // увеличение ширины
    _PI->WriteByte(0x4F7988 +2, 1); // увеличение высоты

    // увеличение высоты скролл текста
    if ( o_HD_Y >= 664)
        _PI->WriteDword(0x4F662F +1, o_HD_Y-440);
}
