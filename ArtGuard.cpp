// защита артефактов от кражи рядом с монстрами
// автор: igrik
// дата: 09/11/2018

#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

//	ФУНКЦИЯ: есть ли стража рядом с объектом (возвращает координаты клетки с монстрами)
_int_ isGuardNearTheObject(_AdvMgr_* advMng, _int_ xyz)
{
	int xd = b_unpack_x(xyz);
	int yd = b_unpack_y(xyz);
	int z = b_unpack_z(xyz);

	if (!(0x0100 & CALL_3(WORD, __fastcall, 0x4F8040, xd, yd, z)) ) {
		return 0; // монстров вокруг клетки нет
	} 

	int x, y;
	int size = advMng->map->size;
	// ряд на уровне арта --------------------------------------------------------------------------------------
	x = xd -1; y = yd;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }
	// ряд ниже арта --------------------------------------------------------------------------------------------
	x = xd -1; y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd;	y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	// ряд выше арта --------------------------------------------------------------------------------------------  	
	x = xd -1; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }
	// -----------------------------------------------------------------------------------------------------
	return 0; // монстров вокруг клетки нет
}

// при движении мыши на КП
_int_ __stdcall Y_GetCursorFrameOnAdvMap(HiHook* hook, _AdvMgr_* advMng, _MapItem_* obj)
{ 
	switch (obj->object_type) {
		case 5: case 6: case 81: case 93: // типы защищаемых объектов
			if ( isGuardNearTheObject(advMng, advMng->pack_xyz) ) { 
				return 5; // возвращаем боевой курсор
			}	break;

		default: break;
	}
	return CALL_2(_int_, __thiscall, hook->GetDefaultFunc(), advMng, obj);
} 

void __stdcall Y_Hero_Enter_To_Object(HiHook* hook, _AdvMgr_* advMng, _Hero_* hero, _MapItem_* obj, _int_ xyz, _int_ isPlayer)
{
	int xyz_54 = 0; // инициализации переменной - координаты охранников 

	switch (obj->object_type) {
		// case 5: case 6: case 12: case 79: case 81: case 82: case 93: case 101:  // типы защищаемых объектов
		case 5:		// артефакт
		case 6:		// ящик пондоры
		case 81:	// ученый
		case 93:	// свиток с заклинаниями
			xyz_54 = isGuardNearTheObject(advMng, xyz); // проверка стражи объекта в соседних клетках
			if ( xyz_54 ){  // если стража есть
				_MapItem_* obj_54 = advMng->map->GetItem(b_unpack_x(xyz_54), b_unpack_y(xyz_54), b_unpack_z(xyz_54)); // получаем структуру объекта "стража"
				//advMng->monBattle_type = (_word_)obj_54->os_type; // (advMng+<536>)
				//advMng->monBattle_num = (_word_)obj_54->draw_num; // (advMng+<540>)
				//advMng->monBattle_side = hero->x < (_word_)((_word_)(xyz_54 << 6) >> 6) ? 1 : 0; // поворот монстра при атаке (advMng+<544>)

				//CALL_5(void, __thiscall, 0x4A73B0, advMng, obj_54, hero, xyz_54, isPlayer); // нападаем на монстра рядом с артом

				//advMng->monBattle_type = -1; // после битвы возвращаем параметры
				//advMng->monBattle_num = -1;  // после битвы возвращаем параметры
				// CALL_5(void, __thiscall, 0x4A8160, advMng, hero, obj_54, xyz_54, isPlayer);
				CALL_5(void, __thiscall, hook->GetDefaultFunc(), advMng, hero, obj_54, xyz_54, isPlayer);

				if (hero->owner_id < 0) {    // если герой убит или сбежал (т.е. номер хозяина героя стал == -1)
					return; // выйти из функции ПОСЕЩЕНИЕ ОБЪЕКТА
				}
			}	break;

		default: break;
	}

	CALL_5(void, __thiscall, hook->GetDefaultFunc(), advMng, hero, obj, xyz, isPlayer);
	return;
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    static _bool_ plugin_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!plugin_On)
        {
			plugin_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("ArtsGuard"); 			

			_PI->WriteHiHook(0x40E97F, CALL_, EXTENDED_, THISCALL_, Y_GetCursorFrameOnAdvMap);
			// _PI->WriteHiHook(0x4AA766, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);

			// SPLICE_ нельзя использовать, потому что сначала выполняется триггер !?OB, а потом только хук через SPLICE_
			// _PI->WriteHiHook(0x4A8160, SPLICE_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);
			 _PI->WriteHiHook(0x4ACA04, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object); // на 2 этих хука ругается антивирус
			 _PI->WriteHiHook(0x4AA766, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object); // на 2 этих хука ругается антивирус

			// долбанные антивирусы! Приходится извращаться через цикл!
			//int ptr_hooks[2] = {0x4ACA04, 0x4AA766};
			//for (int i=0; i<2; i++) {
			//	_PI->WriteHiHook(ptr_hooks[i], CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);
			//}

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
