// исправление отображения графики тени Силового Поля
int __stdcall Fix_ForceFieldShadow(LoHook *h, HookContext *c)
{
	if (o_BattleMgr->action == SPL_FORCE_FIELD) { // если заклинание Силовое Поле
		IntAt(c->ebp -0x14) = 0x63AC6C; // активная сторона тут всегда должна быть 0 (0x63AC6C всегда равна нулю!)
		c->return_address = 0x5A3699;
		return NO_EXEC_DEFAULT;
	}
    return EXEC_DEFAULT;
}

// восстановление параметра тени курсора в бою
// при автобитве этот параметр обнуляется, но не восстанавливается
// значит восстановим вручную
int __stdcall Y_RestoreBattleShadow(LoHook *h, HookContext *c)
{
    o_BATTLE_CursorShadow = DwordAt(c->ebp -0x1C);
    return EXEC_DEFAULT;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void Graphics(PatcherInstance* _PI)
{
    // центрирование изображения по ПКМ в городе на иконке найма войск (ранее уходило сильно влево)
    _PI->WriteHexPatch(0x5D47B3, "0F BF 57 18  8B 4F 24 B8  FF FF FF FF  90");

    // отключить окно-сообщение тактики в начале боя (просто пропускаем его показ)
    _PI->WriteHexPatch(0x462D98, "EB 30 90 90 90 90"); // 0x462D98 JMP SHORT 0x462DCA

    // фикс невлезающего кол-ва существ (100-249 и т.п.) в маленьком окне героя ПКМ
    _PI->WriteByte(0x52F7CE +1, 34); // герой
    _PI->WriteByte(0x5310F1 +1, 34); // замок, гарнизон

    // исправить координаты кнопки Сказочных Драконов
    _PI->WriteDword(0x5F3D9F, 235);  // подложка поз.Y
    _PI->WriteByte(0x5F3DA4, 21);    // подложка поз.X
    _PI->WriteDword(0x5F3DF5, 235);  // кнопка   поз.Y
    _PI->WriteByte(0x5F3DFA, 21);    // кнопка   поз.X

    // исправление неправильных иконок героев Инферно (Ксерафакс и Ксерон)
    _PI->WriteDword(0x79984C, 63);
    _PI->WriteDword(0x799850, 57);

    // исправление неправильных кнопок
    // в диалоге таверны
    _PI->WriteDword(0x5D7ACA, 0x682A24); // iCN6432.def
    // в диалоге резделения отрядов
    _PI->WriteDword(0x449A41, 0x682A24); // iCN6432.def	
    // в диалоге преобразователя скелетов
    _PI->WriteDword(0x565E4A, 0x682A24); // iCN6432.def

    // смещение портрета героя в диалоге повышения уровня героя
    _PI->WriteDword(0x4F90CB +1, 0xAA);

    // исправление ошибки ERM в командре IF:N1, теперь командра работает 
    // со всеми локальными, глобальными и отрицательными переменными z, а не только с z1
    _PI->WriteByte(0x749093, 0xB0);
    _PI->WriteByte(0x74909C, 0xB0);
    _PI->WriteByte(0x7490B0, 0xB0);
    _PI->WriteByte(0x7490B6, 0xB0);
    _PI->WriteByte(0x7490CD, 0xB0);

    // Решение проблемы отображения некоторых строк (в русской локализации) в диалоге экспы монстров.
    // Суть в подмене типа копирования символов со знакового на беззнаковое ( MOVSX -> MOVZX )
    _PI->WriteByte(0x71F3FC, 0xB6);
    _PI->WriteByte(0x71F5BA, 0xB6);
    _PI->WriteByte(0x71F5D3, 0xB6);
    _PI->WriteByte(0x723657, 0xB6);
    _PI->WriteByte(0x723219, 0xB6);
    _PI->WriteByte(0x7238D8, 0xB6);
    _PI->WriteByte(0x7217BB, 0xB6);
    _PI->WriteByte(0x723CBD, 0xB6);
    _PI->WriteByte(0x721B03, 0xB6);
    _PI->WriteByte(0x722792, 0xB6); 
    _PI->WriteByte(0x723ACB, 0xB6);
    _PI->WriteByte(0x723F1C, 0xB6);

    // исправление включения тени, которое не выполняется при автобитве
    _PI->WriteLoHook(0x462C6C, Y_RestoreBattleShadow);

	// исправление отображения графики тени Силового Поля
    _PI->WriteLoHook(0x5A368C, Fix_ForceFieldShadow); 
	// включить показ тени для Силового Поля (by RoseKavalier)
    _PI->WriteJmp(0x5A365D, 0x5A3666);
    _PI->WriteJmp(0x5A37B9, 0x5A37C2);

    // восстановить описание по ПКМ на правую кнопку перелистывания артефактов в рюкзаке героя (© daemon_n)
    _PI->WriteDword(0x5641A2 + 2, 0x6A673C);

    // Фикс размера и положения текстового виджета с отображением цены для ПКМ на жилище существ 
    // (ранее влезало только 4 символа) (© daemon_n)
    _PI->WriteByte(0x55205E +1, 40);
    _PI->WriteByte(0x552060 +2, 36);

    // Исправление описания текста морали для Ангелов (© Hawaiing)
    _PI->WriteCodePatch(0x760A4F, "%n", 5); // 5 nops
}
