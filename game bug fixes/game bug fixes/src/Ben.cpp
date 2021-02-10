/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
// by Ben80 ////// //////////////////////////////////////////////////////////////////////////////////

// The Castle's Lighthouse building bonus
int __stdcall CastleOwnerCheck(LoHook *h, HookContext *c)
{
    _Town_ *town = (_Town_*)(c->ecx);
    _Hero_ *hero = *(_Hero_**)(c->ebp - 4); // _Hero_ is stored in temp variable [LOCAL.1]

    if (hero->owner_id == town->owner_id) // normal
        return EXEC_DEFAULT;

    c->return_address = 0x4E4D6C; // skip procedure
    return NO_EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall EarthquakeBug(LoHook *h, HookContext *c)
{
    _BattleStack_ *cre = (_BattleStack_ *)(c->eax - 0x84); // offset is to creature flags
    if (cre->index_on_side == 0) // if creature is in position 0 (1st on the left), then we don't want it to be flagged 0x200000 aka CANNOT_MOVE aka WAR_MACHINE
        return NO_EXEC_DEFAULT;
    return EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void Ben(PatcherInstance* _PI)
{
    // Arrow towers damage bug fix
    // for cases:
    // Armorer sec.skill, Stoning of creature, Air shield spell
    _PI->WriteByte(0x41E3A4, 0x4D);
    _PI->WriteByte(0x41E4DF, 0x4D);
    _PI->WriteByte(0x465944, 0x4D);

    // fix Fortress attack and defense bonuses for defended Hero
    _PI->WriteByte(0x4639FE, 0x77);
    _PI->WriteByte(0x463A31, 0x76);

    // The Castle's Lighthouse building bonus
    // is now only applied to the Castle's owner
    _PI->WriteLoHook(0x4E4D40, CastleOwnerCheck);

    // Earthquake Bug will no longer kill creatures and end battle
    _PI->WriteLoHook(0x465656, EarthquakeBug);
}
