////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// диалог загрузки/сохранения Вог опций /////////////////////////////////////////////////////

// #include "dirent.h"
// #include <string>
// #include <iostream>

#include "..\..\..\include\FsMan\fsman.cpp"
#include "..\..\..\include\FsMan\fsman.hpp"

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#define DIID_HINTITEM 115
#define BUFFER_SIZE 512

#define SF_FIRST_ITEM_ID 160
#define SF_MAX_SHOW_ITEMS 13

// типы папок и файлов
#define DS_TYPE_ROOT 0
#define DS_TYPE_FOLDER 1
#define DS_TYPE_FILE 2

#define DS_STRING_ROOTFOLDER ".."
#define DS_STRING_EMPTY ""

// необходимые текстовые буфферы
char DlgWogSave_EditText[1024];
char PathDirectory[1024];

// указатель на структуру диалога выбора
_CustomDlg_* ptr_NewDlg_ChooseFile;

// Для удобства расположения кода это просто объявления функций
int __stdcall NewDlg_ChooseFile_Rebuild(_CustomDlg_* dlg);
int NewDlg_ChooseFile_SetTextOnEditTextItem(_CustomDlg_* dlg, int itemID);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

struct _ScrollFiles_
{
    // id текущего элемента
    _int_ current; 
    // предыдущий элемент
    _int_ previous; 
    // предыдущий шаг
    _int_ laststep;
    // кол-во элементов
    _int_ size; 
    // id первого показываемого элемента
    _int_ firstItem; 
    // id последнего показываемого элемента
    _int_ lastItem; 
    // время даббл.клика
    _int_ timeClick;

    // конструктор
    _ScrollFiles_(_int_ setSize)
    {
        current = 0;
        previous = -1;
        size = setSize;
        laststep = 0;
        firstItem = 0;
        lastItem = setSize - 1;
        timeClick = 0;
    }
    // переинициализация стуктуры
    void Reset(_int_ setSize)
    {
        current = 0;
        previous = -1;
        size = setSize;
        laststep = 0;
        firstItem = 0;
        lastItem = setSize - 1;
        timeClick = 0;
    }
    // установка нового текущего элемента
    void SetCurrent(_int_ newCurrent)
    {       
        if (newCurrent != current) {
            previous = current;
            current = newCurrent;
        }
    }
    // установка размера шага
    void SetStep(_int_ newStep)
    {       
        laststep = newStep;
    }
    // установка времени клика
    void SetTimeClick(_int_ newTimeClick)
    {       
        timeClick = newTimeClick;
    }
    // уменьшение текущего счетчика на единицу
    int Decrease()
    {
        if (firstItem + current < 1)
            return 1;

        SetCurrent(current - 1);
        return 0;
    }
    // увеличение текущего счетчика на единицу
    int Increase()
    {
        if (firstItem + current >= size -1)
            return 1;

        SetCurrent(current + 1);
        return 0;
    }
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// функция смены папки + обновление диалога
int NewDlg_ChooseFile_ChangeDirectory(_CustomDlg_* dlg, char* folderName)
{
    // получаем необходмые структуры
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

    // сохраняем выбранный путь
    sprintf(PathDirectory, "%s\\%s\\", (char*)fs->currentDirectory().c_str(), folderName);
    cf->Directory = PathDirectory;
    // делаем переход в менеджере папок
    fs->changeDirectory(PathDirectory);
    // опустошаем файл по умолчанию
    cf->fileName = DS_STRING_EMPTY;
    // перестраиваем диалог
    NewDlg_ChooseFile_Rebuild(dlg);
    // обновляем содержимое текста в поле ввода
    NewDlg_ChooseFile_SetTextOnEditTextItem(dlg, 130);
    // и перерисовываем диалог
    dlg->Redraw();

    return 0;
}


// переход из текущей папки 
int NewDlg_ChooseFile_FolderOut(_CustomDlg_* dlg)
{
    // получаем необходмые структуры
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

    // инициализация переменных
    string nameFolder;
    _bool_ isFindRootFolder = false;

    // сначала ищем, если ли папка на уровень выше
    for(int i = 0; i < fs->listDirectories().size(); i++)
    {
        // записываем в строку названия папок
        nameFolder.assign((char*)fs->listDirectories()[i].c_str());
        // если нашли корневой каталог
        if (nameFolder == DS_STRING_ROOTFOLDER) {
            isFindRootFolder = true;
            break;
        }
    }

    // если корневой каталог не нашли: выходим
    if (!isFindRootFolder)
        return 1;

    // переходим в выбранную папку
    NewDlg_ChooseFile_ChangeDirectory(dlg, DS_STRING_ROOTFOLDER);

    return 0;
}


// переход в выбранную папку
int NewDlg_ChooseFile_FolderIn(_CustomDlg_* dlg)
{
    // получаем необходмые структуры
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];  

    // получаем id подсвеченного элемента
    int itemID = ScrollFiles->firstItem + ScrollFiles->current;

    // если выход за границы - выход. В том числе, если это файл, а не папка
    if (itemID < 0 || itemID >= fs->listDirectories().size())
        return 1;

    // переходим в выбранную папку
    NewDlg_ChooseFile_ChangeDirectory(dlg, (char*)fs->listDirectories()[itemID].c_str());

    return 0;
}

// переход в корневую папку игры
int NewDlg_ChooseFile_JumpRootFolder(_CustomDlg_* dlg)
{
    // получаем необходмые структуры
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

    // получаем базовый каталог игры
    char rootFolder[BUFFER_SIZE];
    GetCurrentDirectoryA(sizeof(rootFolder), rootFolder);

    // сохраняем выбранный путь
    sprintf(PathDirectory, "%s\\", rootFolder);
    cf->Directory = PathDirectory;
    // делаем переход в менеджере папок
    fs->changeDirectory(PathDirectory);
    // опустошаем файл по умолчанию
    cf->fileName = DS_STRING_EMPTY;
    // перестраиваем диалог
    NewDlg_ChooseFile_Rebuild(dlg);
    // обновляем содержимое текста в поле ввода
    NewDlg_ChooseFile_SetTextOnEditTextItem(dlg, 130);
    // и перерисовываем диалог
    dlg->Redraw();

    return 0;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// обновление предыдущей и текущей жёлтой рамки в диалоге
int NewDlg_ChooseFile_RedrawFrames(_CustomDlg_* dlg, _ScrollFiles_* ScrollFiles)
{
    if (ScrollFiles->current >= 0 && ScrollFiles->current < SF_MAX_SHOW_ITEMS)
        dlg->GetItem(ScrollFiles->current+160)->Show_ButStayEnable();

    if (ScrollFiles->previous >= 0 && ScrollFiles->previous < SF_MAX_SHOW_ITEMS)
        dlg->GetItem(ScrollFiles->previous+160)->Hide_ButStayEnable();

    return 0;
}


// перемещение жёлтой рамки скроллом
int NewDlg_ChooseFile_MooveFrames(_CustomDlg_* dlg, int step)
{
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];

    ScrollFiles->previous = ScrollFiles->current;
    ScrollFiles->current -= (step-ScrollFiles->laststep);
    ScrollFiles->SetStep(step);

    NewDlg_ChooseFile_RedrawFrames(dlg, ScrollFiles);

    return 0;
}

// перемещение жёлтой рамки вверх
int NewDlg_ChooseFile_MooveFrameUp(_CustomDlg_* dlg)
{
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    _DlgScroll_* scrollItem = (_DlgScroll_*)dlg->GetItem(9);

    // если есть куда двигаться верх
    if (!ScrollFiles->Decrease() && scrollItem)
    {
        // если желтая рамка в крайнем верхнем положении на экране или выше видимого экрана
        if (ScrollFiles->current + ScrollFiles->laststep < ScrollFiles->firstItem )
        {
            // если желтая рамка всё же выше видимого экрана
            if (ScrollFiles->current + ScrollFiles->laststep < ScrollFiles->firstItem -1 )
            {
                // вычисляем шаг прыжка к рамке
                int needStep = ScrollFiles->firstItem + ScrollFiles->current;
                // и прыгаем так, чтобы рамка оказалась в ScrollFiles->firstItem элементе (первый видимый элемент)
                scrollItem->SetValue(needStep +1);
            }
            // переход скроллом на 1 вверх
            scrollItem->MooveUP();
        }

        // если желтая рамка выше видимого экрана
        if (ScrollFiles->current >= SF_MAX_SHOW_ITEMS)
        {
            int needStep = ScrollFiles->current -SF_MAX_SHOW_ITEMS + ScrollFiles->laststep;
            scrollItem->SetValue(needStep);
            scrollItem->MooveDown();
        }

        // дополнительно обновляем положение желтой рамки
        NewDlg_ChooseFile_RedrawFrames(dlg, ScrollFiles);

        // обновляем содержимое текста в поле ввода
        NewDlg_ChooseFile_SetTextOnEditTextItem(dlg, 130+ScrollFiles->current);

        dlg->Redraw();

        return 0;
    }

    return 1;
}

// перемещение жёлтой рамки вниз
int NewDlg_ChooseFile_MooveFrameDown(_CustomDlg_* dlg)
{
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    _DlgScroll_* scrollItem = (_DlgScroll_*)dlg->GetItem(9);

    // если есть куда двигаться вниз
    if (!ScrollFiles->Increase() && scrollItem)
    {
        // если желтая рамка в крайнем нижнем положении на экране или ниже видимого экрана
        if (ScrollFiles->current + ScrollFiles->laststep > ScrollFiles->lastItem )
        {
            // если желтая рамка всё же ниже видимого экрана
            if (ScrollFiles->current + ScrollFiles->laststep > ScrollFiles->lastItem +1 )
            {
                // вычисляем шаг прыжка к рамке
                int needStep = ScrollFiles->current + ScrollFiles->laststep - SF_MAX_SHOW_ITEMS;
                // и прыгаем так, чтобы рамка оказалась в ScrollFiles->lastItem элементе (последний видимый элемент)
                scrollItem->SetValue(needStep);
            } 
            // переход скроллом на 1 вниз
            scrollItem->MooveDown();
        }

        // если желтая рамка выше видимого экрана
        if (ScrollFiles->current < 0)
        {
            int needStep = ScrollFiles->firstItem + ScrollFiles->current;
            scrollItem->SetValue(needStep+1);
            scrollItem->MooveUP();
        }

        // дополнительно обновляем положение желтой рамки
        NewDlg_ChooseFile_RedrawFrames(dlg, ScrollFiles);

        // обновляем содержимое текста в поле ввода
        NewDlg_ChooseFile_SetTextOnEditTextItem(dlg, 130+ScrollFiles->current);                

        dlg->Redraw();

        return 0;
    }

    return 1;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

void __fastcall NewDlg_ChooseFile_ScrollProc(int step, _CustomDlg_* dlg)
{
    // получаем необходимые структуры
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

    // получаем струкутры скроллов, и элемент скролла
    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    _DlgScroll_* scroll = (_DlgScroll_*)dlg->GetItem(9);
    
    // получаем кол-во папок и файлов
    int numFolders = fs->listDirectories().size();
    int numFiles = fs->listFiles(cf->Mask).size();

    for (int i = 0; i < SF_MAX_SHOW_ITEMS; i++)
    {
        // получаем текущие элементы картинки и текста
        _DlgStaticDef_*  itPic  = (_DlgStaticDef_*)dlg->GetItem(130 + i);
        _DlgStaticText_* itText = (_DlgStaticText_*)dlg->GetItem(145 + i);

        // вычисляем текущий элемент
        int current = i+step;

        // записываем текущие параметры скролла: id видимых первого и последнего элементов
        ScrollFiles->firstItem = step;
        ScrollFiles->lastItem = current;

        if (itPic && itText) 
        {
            if ( current < numFolders ) {
                // записываем текущий текст строки
                itText->SetText( (char*)fs->listDirectories()[current].c_str() );

                if (fs->listDirectories()[current] == DS_STRING_ROOTFOLDER) 
                    itPic->def_frame_index = DS_TYPE_ROOT;    // вид открытой папки (только для "..")
                else itPic->def_frame_index = DS_TYPE_FOLDER; // стандартный вид папки
            }
            else if ( current < numFiles+numFolders ) {
                itText->SetText( (char*)fs->listFiles(cf->Mask)[current-numFolders].c_str() );
                itPic->def_frame_index = DS_TYPE_FILE;
            }
            else break;
        }
    }

    // перемещение жёлтой рамки скроллом
    NewDlg_ChooseFile_MooveFrames(dlg, step);

    dlg->Redraw(TRUE);
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

int __stdcall NewDlg_ChooseFile_Rebuild(_CustomDlg_* dlg)
{
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];
    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];

    _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
    _DlgScroll_* scroll = (_DlgScroll_*)dlg->GetItem(9);
 
    // отключаем кнопку ОК, если нет названия по умолчанию
    string defaultName;
    defaultName.append(cf->fileName);
    if (defaultName == DS_STRING_EMPTY)
        dlg->GetItem(DIID_OK)->SetEnabled(false);

    // обнуляем текущий выбранный элемент
    ScrollFiles->SetCurrent(0);
    // и перерисовываем рамки
    NewDlg_ChooseFile_RedrawFrames(dlg, ScrollFiles);

    // получаем общее количество элементов
    int countFoldersAndFiles = fs->listDirectories().size() + fs->listFiles(cf->Mask).size();

    // и после этого полностью обнулем ScrollFiles
    ScrollFiles->Reset(countFoldersAndFiles);

    // подготовливаем кол-во для скролла
    countFoldersAndFiles -= SF_MAX_SHOW_ITEMS;

    // устанавливаем основные параметры скролла
    if (scroll) {
        scroll->SetEnabled(countFoldersAndFiles >= 1);
        scroll->SetTicksCount(countFoldersAndFiles + 1);
        scroll->SetValue(0);
    }
 
    // путь текущего каталога
    ((_DlgStaticText_*)dlg->GetItem(5))->SetText( (char*)fs->currentDirectory().c_str() );

    // и сразу правим подсказку EditText
    _DlgTextEdit_* editText = (_DlgTextEdit_*)dlg->GetItem(256);
    if ( editText ){
        editText->full_tip_text = o_NullString;
        sprintf(DlgWogSave_EditText, "%s\\%s", (char*)fs->currentDirectory().c_str(), cf->fileName);
        editText->short_tip_text = DlgWogSave_EditText;
    }

    // получаем кол-во папок и файлов
    int numFolders = fs->listDirectories().size();
    int numFiles = fs->listFiles(cf->Mask).size();

    for (int i = 0; i < SF_MAX_SHOW_ITEMS; i++)
    {
        _DlgStaticDef_*  itPic  = (_DlgStaticDef_*)dlg->GetItem(130 + i);
        _DlgStaticText_* itText = (_DlgStaticText_*)dlg->GetItem(145 + i);  
        _DlgItem_* frame = dlg->GetItem(160 + i);

        // если элементы существует
        if (itPic && itText && frame) 
        {
            itPic->Show(); 
            itText->Show();
            frame->Show();
            frame->Hide_ButStayEnable();

            if ( i < numFolders ) {
                // записываем номер последнего показанного элемента
                ScrollFiles->lastItem = i;
                // устанавливаем текст
                itText->SetText( (char*)fs->listDirectories()[i].c_str() );

                if (fs->listDirectories()[i] == DS_STRING_ROOTFOLDER) // меняем тип картинки на "папка"
                    itPic->def_frame_index = DS_TYPE_ROOT;    // вид открытой папки (только для "..")
                else itPic->def_frame_index = DS_TYPE_FOLDER; // стандартный вид папки
            }
            else if ( i < numFiles+numFolders ) {
                // записываем номер последнего показанного элемента
                ScrollFiles->lastItem = i;
                // устанавливаем текст
                itText->SetText( (char*)fs->listFiles(cf->Mask)[i-numFolders].c_str() );
                // меняем тип картинки на "файл"
                itPic->def_frame_index = DS_TYPE_FILE;
            }
            else 
            {
                // скрываем все неиспользуемые файлы, папки и рамки
                itPic->Hide(); 
                itText->Hide();
                frame->Hide();
            }
        }
    }

    // включаем первую желтую рамку
    dlg->GetItem(160)->Show_ButStayEnable();

    return 0;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// функция обновления текста в поле ввода текста
int NewDlg_ChooseFile_UdateEditText(_CustomDlg_* dlg, char* newText)
{
    _DlgTextEdit_* editText = (_DlgTextEdit_*)dlg->GetItem(256);
    _DlgItem_* btn_OK = dlg->GetItem(DIID_OK);

    if (editText)
    {
        editText->SetEditText(newText);

        if (editText->text)
            btn_OK->SetEnabled(1);
        else {
            btn_OK->SetEnabled(0);
            newText = " ";
        }

        sprintf(DlgWogSave_EditText, "%s\\%s", ((_DlgStaticText_*)dlg->GetItem(5))->text, newText);
        editText->short_tip_text = DlgWogSave_EditText;     
    }

    return 0;
}

// функция установки текста в поле ввода
int NewDlg_ChooseFile_SetTextOnEditTextItem(_CustomDlg_* dlg, int itemID)
{    
    _DlgStaticDef_* defFile   = (_DlgStaticDef_*)dlg->GetItem(itemID);
    _DlgStaticText_* textFile = (_DlgStaticText_*)dlg->GetItem(itemID + 15);

    if (defFile && textFile)
    {
        int fileType = defFile->def_frame_index;
        char* text = textFile->text;

        if (fileType != DS_TYPE_FILE)
            NewDlg_ChooseFile_UdateEditText(dlg, DS_STRING_EMPTY);
        else NewDlg_ChooseFile_UdateEditText(dlg, text);
    }

    return 0;
}

// функция обновления подсказки основного диалга
void NewDlg_ChooseFile_UpdateHintText(_CustomDlg_* dlg, _EventMsg_* msg)
{
    _DlgStaticTextPcx8ed_* statBar = (_DlgStaticTextPcx8ed_*)dlg->GetItem(DIID_HINTITEM);
    _DlgItem_* it = dlg->FindItem(msg->x_abs, msg->y_abs);

    fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
    _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

    char* hint = DS_STRING_EMPTY;

    if (it) 
    {
        // разрешаем или запрещаем управлять кнопками: влево, вправо, вверх, вниз,  home
        if ( it && (it->id == 97 || (it->id >= 160 && it->id <= 172) ))
            dlg->custom_data[3] = true;
        else dlg->custom_data[3] = false;

        // разрешаем редактировать текст ввода, только если на него наведён курсор
        if (cf->FileEditEnabled)
        {
            _DlgTextEdit_* editText = (_DlgTextEdit_*)dlg->GetItem(256);
            if (editText)
            {
                if (!dlg->custom_data[3]) 
                {  
                    if (editText->text)
                        // собираем правильный текст подсказки: полный путь + введённый текст           
                        sprintf(DlgWogSave_EditText, "%s\\%s", ((_DlgStaticText_*)dlg->GetItem(5))->text, ((_DlgTextEdit_*)dlg->GetItem(256))->text); 
                    else
                        sprintf(DlgWogSave_EditText, "%s\\%s", ((_DlgStaticText_*)dlg->GetItem(5))->text, " ");
                    // разрешаем ввод текста                
                    editText->SetFocused(1);
                } else {
                    // запрещаем ввод текста    
                    editText->SetFocused(0);
                }
            }
        }

        if ( it->short_tip_text ) 
            hint = it->short_tip_text;

        statBar->SetText(hint);
        statBar->Draw();
        statBar->RedrawScreen();
    }
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// главная процедура диалога
int __stdcall NewDlg_ChooseFile_Proc(_CustomDlg_* dlg, _EventMsg_* msg)
{
    int r = dlg->DefProc(msg);

    if (msg->type == MT_MOUSEOVER) // ведение мыши
        NewDlg_ChooseFile_UpdateHintText(dlg, msg);

    if (msg->type == MT_MOUSEBUTTON)
    {
        _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];
        fsman::FsMan* fs = (fsman::FsMan*)dlg->custom_data[1];
        _ChooseFile_* cf = (_ChooseFile_*)dlg->custom_data[2];

        if (msg->subtype == MST_LBUTTONCLICK) // ЛКМ при отжатии
        {
            if (msg->item_id == DIID_OK)
            {
                // копируем название файла в буфер (для удобного вытаскивания названия для ERM)
                sprintf(o_TextBuffer, "%s", ((_DlgTextEdit_*)dlg->GetItem(256))->text );
                // создаём строку, для проверки на пустой элемент и для более удобной с ним работой
                string EditText;
                EditText.append(o_TextBuffer);

                if (!EditText.empty())
                {
                    // сохраняем путь к файлу
                    sprintf(PathDirectory, "%s", (char*)fs->currentDirectory().c_str());
                    // указываем, что файл был выбран
                    dlg->custom_data[5] = 0;
                    // и закрываем диалог
                    return dlg->Close(msg);
                }
            }
        } // subtype == MST_LBUTTONCLICK

        if (msg->subtype == MST_LBUTTONDOWN)  // ЛКМ при нажатии
        { 
            // подсветка кликнутой жёлтой рамки и дабблклик
            if (msg->item_id >= 160 && msg->item_id <= 173)
            {
                // получаем параметры: текст и тип картинки, по которой мы определеим это папка или файл
                int fileType = ((_DlgStaticDef_*)dlg->GetItem(msg->item_id -30))->def_frame_index;
                char* text = ((_DlgStaticText_*)dlg->GetItem(msg->item_id -15))->text;

                // непосредственно дабблклик по тому же элементу
                if (ScrollFiles->current == msg->item_id -160 && o_GetTime() - ScrollFiles->timeClick < 300 )
                {
                    if (fileType != DS_TYPE_FILE) {
                        // переходим в выбранную папку
                        NewDlg_ChooseFile_ChangeDirectory(dlg, text);
                    } else {
                        // копируем название файла в буфер (для удобного вытаскивания названия для ERM)
                        sprintf(o_TextBuffer, "%s", text);
                        // сохраняем путь к файлу
                        sprintf(PathDirectory, "%s", (char*)fs->currentDirectory().c_str());
                        // указываем, что файл был выбран
                        dlg->custom_data[5] = 0;
                        // и закрываем диалог
                        return dlg->Close(msg);
                    }
                } 
                else // либо просто подсвечиваем элемент
                {
                    if (fileType != DS_TYPE_FILE)
                         NewDlg_ChooseFile_UdateEditText(dlg, DS_STRING_EMPTY);
                    else NewDlg_ChooseFile_UdateEditText(dlg, text);

                    ScrollFiles->SetCurrent(msg->item_id -160);
                    NewDlg_ChooseFile_RedrawFrames(dlg, ScrollFiles);
                    dlg->Redraw();
                    ScrollFiles->SetTimeClick(o_GetTime());
                }
            }
        
        } // subtype == MST_LBUTTONDOWN
    } // type == MT_MOUSEBUTTON

    if (msg->type == MT_KEYDOWN)
    {
        // кнопки управления доступны, если курсор только на квадрате с файлами/папками
        if ( dlg->custom_data[3] )
        {
            _ScrollFiles_* ScrollFiles = (_ScrollFiles_*)dlg->custom_data[0];

            if (msg->subtype == HK_BACKSPACE) 
                NewDlg_ChooseFile_FolderOut(dlg);

            if (msg->subtype == HK_ENTER) 
                NewDlg_ChooseFile_FolderIn(dlg);

            if (msg->subtype == HK_ARROW_LEFT) 
                NewDlg_ChooseFile_FolderOut(dlg);

            if (msg->subtype == HK_ARROW_RIGHT) 
                NewDlg_ChooseFile_FolderIn(dlg);

            // переход желтой рамкой выбора на 1 позицию верх
            if (msg->subtype == HK_ARROW_UP)
                NewDlg_ChooseFile_MooveFrameUp(dlg);

            // переход желтой рамкой выбора на 1 позицию вниз
            if (msg->subtype == HK_ARROW_DOWN) 
                NewDlg_ChooseFile_MooveFrameDown(dlg);

            // прыгаем в корневой каталог игры
            if (msg->subtype == HK_HOME)
                NewDlg_ChooseFile_JumpRootFolder(dlg);
        }
    }

    return r;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

int __stdcall NewDlg_ChooseFile(_ChooseFile_* cf)
{    
    // создаём менеджер перемещения по папкам/файлам
    fsman::FsMan fs(cf->Directory); 

    int x = 512;  int x_center = x >> 1; int dx = 24; int dxx = dx * 2;
    int y = 520;

    // создаём диалог
    _CustomDlg_* dlg = _CustomDlg_::Create(-1, -1, x, y, DF_SCREENSHOT | DF_SHADOW, NewDlg_ChooseFile_Proc);
    Set_DlgBackground_RK(dlg, 1, o_GameMgr->GetMeID());

    // сохраняем указатель на диалог
    ptr_NewDlg_ChooseFile = dlg;

    // подсказка в статус баре ( DIID_HINTITEM == 115 )  
    dlg->AddItem(_DlgStaticTextPcx8ed_::Create(8, y-26, x-16, 18, o_NullString, n_SmallFont, adRollvrPcx, 1, DIID_HINTITEM, ALIGN_H_CENTER | ALIGN_V_CENTER) ); 

    // кнопка CANCEL
    if ( cf->ShowCancel )
    {
        dlg->AddItem(_DlgStaticPcx8_::Create(x_center +16, y-74, DIID_CANCEL_FRAME, box64x30Pcx));
        dlg->AddItem(_DlgButton_::Create(x_center +17, y-73, 64, 30, DIID_CANCEL, iCancelDef, 0, 1, 1, 1, 2)); 
        dlg->GetItem(DIID_CANCEL)->full_tip_text = o_NullString;
        dlg->GetItem(DIID_CANCEL)->short_tip_text = txtresWOG->GetString(12);
        x_center -= 47; // для правильного смещения кнопки ОК
    } 
    // кнопка ОК
    dlg->AddItem(_DlgStaticPcx8_::Create(x_center -33, y-74, DIID_OK_FRAME, box64x30Pcx));
    dlg->AddItem(_DlgButton_::Create(x_center -32, y-73, 64, 30, DIID_OK, iOkayDef, 0, 1, 0, 28, 2));
    dlg->GetItem(DIID_OK)->full_tip_text = o_NullString;
    dlg->GetItem(DIID_OK)->short_tip_text = txtresWOG->GetString(11);

    // Титульный текст
    dlg->AddItem(_DlgStaticText_::Create(dx, 20, x-dxx, 26, cf->Caption, n_MedFont, 7, 3, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // желтая обводка пояснения
    b_YellowFrame_Create(dlg, dx+1, 51, x-dxx, 66, 94, ON, o_Pal_Black);
    b_YellowFrame_Create(dlg, dx, 50, x-dxx, 66, 95, ON, o_Pal_Grey); 
    // Текст пояснения
    dlg->AddItem(_DlgStaticText_::Create(dx+4, 50, x-dxx-8, 66, cf->Description, n_SmallFont, 1, 4, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // Текст: текущий адрес
    dlg->AddItem(_DlgStaticText_::Create(dx+8, 116, x-dxx-16, 24, o_NullString, n_SmallFont, 7, 5, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

    // Текст выбранный файл
    dlg->AddItem(_DlgStaticText_::Create(dx, y-130, x-dxx, 30, txtresWOG->GetString(97), n_MedFont, 7, 6, ALIGN_H_CENTER | ALIGN_V_CENTER, 0));

    // обводка поля ввода
    b_YellowFrame_Create(dlg, dx+1, y-101, x-dxx, 19, 96, ON, o_Pal_Black); // id = 96
    b_YellowFrame_Create(dlg, dx, y-102, x-dxx, 19, 97, ON, o_Pal_Grey); // id = 96

    // поле ввода, если разрешено вводить
    if (cf->FileEditEnabled) 
        dlg->AddItem(_DlgTextEdit_::Create(dx+2, y-100, x-dxx-4, 16, 64, cf->fileName, n_SmallFont, 1, ALIGN_H_LEFT | ALIGN_V_CENTER, "WoGTextEdit.pcx", 256, 4, 0, 0));
    else  // если нельзя, то простой нередактируемый текст
        dlg->AddItem(_DlgStaticTextPcx8ed_::Create(dx+2, y-100, x-dxx-4, 16, cf->fileName, n_SmallFont, "WoGTextEdit.pcx", 1, 256, 0));

    // начальные координаты элементов прокрутки: картинка папки, текст, желтая рамка (по 13 шт.)
    int startY = 144;
    int dy = 19;

    // рамка со списком файлов и папок
    b_YellowFrame_Create(dlg, dx+1, startY-3, x-dxx, y-266, 96, ON, o_Pal_Black); // id = 98
    b_YellowFrame_Create(dlg, dx, startY-4, x-dxx, y-266, 97, ON, o_Pal_Grey);    // id = 99

    _DlgItem_* tempItem;
    //  создаём элементы списка (пока что пустышки)
    for (int i = 0; i < SF_MAX_SHOW_ITEMS; i++)
    {  
        dlg->AddItem(tempItem = _DlgStaticDef_::Create(dx+5, startY+1 +i*dy, 16, 16, i+130, "WNDFOLDER.def", 1, 0, 0x12)); 

        dlg->AddItem(tempItem =_DlgStaticText_::Create(dx+25, startY +i*dy, x-dxx-44, dy-1, o_NullString, n_SmallFont, 1, i+145, ALIGN_H_LEFT | ALIGN_V_CENTER, 0));

        b_YellowFrame_Create(dlg, dx+3, startY +i*dy, x-dxx-24, dy-1, i+160, OFF, o_Pal_Y); 
    }

    // пишем указатели, для лёгкого к ним доступа
    _ScrollFiles_ ScrollFiles(1); // временно создаём с 1 элементов
    dlg->custom_data[0] = (int)&ScrollFiles;
    dlg->custom_data[1] = (int)&fs;
    dlg->custom_data[2] = (int)cf;

    // переменная разрешающая/запрещающая перемещасться в папках по кнопкам
    dlg->custom_data[3] = false;

    // создаём пустой скролл (id 9)
    _DlgScroll_* scroll = _DlgScroll_::Create( x-42, 142, 16, y-270, 9, 0, (_ptr_)NewDlg_ChooseFile_ScrollProc, 0, 0, 0); 
    dlg->AddItem(scroll);

    // перестроение/наполнение диалога
    NewDlg_ChooseFile_Rebuild(dlg);

    // результат диалога:
    // 0 - выбран файл
    // 1 - нажата отмена
    dlg->custom_data[5] = 1; 

    // выполняем диалог
    dlg->Run();

    // уничтожаем диалог
    dlg->Destroy(TRUE);

    // удаляем указатель на диалог
    ptr_NewDlg_ChooseFile = NULL;

    return dlg->custom_data[5];
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// подготовительная функция, нужна, чтобы "обновлять" диалог: перемещене по папкам
// по другому не получается у меня правильно управлять скроллом
int NewDlg_ChooseFile_Prepare(_ChooseFile_* cf)
{   
    // для начала, сделаем константную ссылку на путь
    // т.е. скопируем текст, а потом подменим ссылку
    // размер буффера 1024 байта
    sprintf(PathDirectory, "%s", cf->Directory);
    cf->Directory = PathDirectory; 

    // результат: 
    // 0 - выбран файл 
    // 1 - нажата отмена 

    // cf->buf: 
    // если 0, то полный путь к файлу, 
    // если 1 - то буффер пустой

    int result = 1; // базовый результат: отмена

    // непосредственный вызов диалога
    result = NewDlg_ChooseFile(cf);

    // проверка на совпадение расширений (маска)
    if (!result && PathDirectory && o_TextBuffer) {

        // временная строка названия сохраняемого файла
        string fileName;
        fileName.assign(o_TextBuffer);

        // временная строка маски
        string fileMask;
        fileMask.assign(cf->Mask);

        // переводим строки в нижний регистр для последующего сравнения
        transform(fileName.begin(), fileName.end(), fileName.begin(), tolower);
        transform(fileMask.begin(), fileMask.end(), fileMask.begin(), tolower);

        // получаем маску сохряняемого файла и маску файлов диалога
        string extensionFile = fileName.substr(fileName.find_last_of(".") + 1);
        string extensionMask = fileMask.substr(fileMask.find_last_of(".") + 1);

        // сравниваем маски, если разные: дописываем маску к названию файла
        if (extensionFile != extensionMask && extensionMask != "*") {
           sprintf(o_TextBuffer, "%s.%s", fileName.c_str(), extensionMask.c_str() ); 
        }

        // к тому же ВОГ хочет, чтобы мы сохранили полный путь к файлу, поэтому сделаем это:
        sprintf(MyString, "%s\\%s", PathDirectory, o_TextBuffer);
        cf->Buf = MyString;
    }    

    return result;
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

int __stdcall Dlg_WoG_Options_SaveOrLoad(HiHook* hook, _ChooseFile_* cf, char* defaultName)
{
    // 1) если окно ввода должно быть отключено (например для ввода китайских символов) 
    // 2) на всякий случай делаем работоспособным вызов старой версии диалога
    if ( BanDlg_CustomReq_EnterText || GetKeyState(VK_CTRL) < 0 )        
        return CALL_2(int, __cdecl, hook->GetDefaultFunc(), cf, defaultName);

    // путь файла wog.ini
    char iniPath[BUFFER_SIZE]; 
    // путь файла *.dat
    char fileName[BUFFER_SIZE];

    // получаем базовый каталог игры
    GetCurrentDirectoryA(sizeof(iniPath), iniPath);

    // генерируем полный путь к файлу wog.ini
    strcat(iniPath, "\\wog.ini");    

    // получаем название файла *.dat из сохранённого в wog.ini
    GetPrivateProfileStringA("WoGification", "Options_File_Name", defaultName, fileName, BUFFER_SIZE, iniPath);

    // получаем начальный путь файла *.dat из сохранённого в wog.ini
    GetPrivateProfileStringA("WoGification", "Options_File_Path", ".\\", PathDirectory, BUFFER_SIZE, iniPath);

    // return CALL_2(int, __cdecl, hook->GetDefaultFunc(), cf, defaultName);

    cf->fileName = fileName;
    cf->Mask = "*.dat";

    // результат: 0 - выбран файл, 1 - нажата отмена
    // cf->buf: если 0, то полный путь к файлу, если 1 - то буффер пустой
    return NewDlg_ChooseFile_Prepare(cf);
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// хук для отключения кнопки ОК при пустом тексте в элементе dlgEditText
_int_ __stdcall DlgEdit_IsCorrectInpulSymbol(HiHook* hook, _DlgTextEdit_* textEdit, _EventMsg_* msg)
{
    _int_ result = CALL_2(_int_, __thiscall, hook->GetDefaultFunc(), textEdit, msg);

    // только если диалог с таким текстовым полем создан
    if (ptr_NewDlg_ChooseFile && textEdit->text && textEdit->focused)
    {
        // записываем в строку текст
        string text;
        text.append(textEdit->text);

        // получаем структуру кнопки ОК
        _DlgItem_* it = ptr_NewDlg_ChooseFile->GetItem(DIID_OK);

        if (it) { // если кнопка ОК существует
            if (text.empty())
                it->SetEnabled(false);
            else
                it->SetEnabled(true);

             // обновляем диалог (для быстроты только кнопки ОК и CANCEL)
             ptr_NewDlg_ChooseFile->Redraw(TRUE, DIID_OK, DIID_CANCEL);
         }
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

void DlgWoGOptions_SaveOrLoad(PatcherInstance* _PI)
{
    _PI->WriteHiHook(0x7738A8, SPLICE_, EXTENDED_, CDECL_, Dlg_WoG_Options_SaveOrLoad);
    _PI->WriteHiHook(0x5BB000, SPLICE_, EXTENDED_, THISCALL_, DlgEdit_IsCorrectInpulSymbol);
}