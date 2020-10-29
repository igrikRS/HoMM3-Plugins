#define _H3API_PATCHER_X86_
#include "../../Headers/H3API.hpp"

using namespace h3;

Patcher* _P;
PatcherInstance* _PI;

/////////////////////////////////////////////////////
// Primary skill overflow © RoseKavalier
/////////////////////////////////////////////////////

/*
 *
 * This hook prevents overflow of primary skills during level up. It only triggers
 * if the PSkill was just "increased" to -128
 *
 */
_LHF_(LevelUpPrimary_OF) // for level up
{
	BYTE primary = c->AL();
	if (primary == 0x80) // only activate when going from +127 to -128
		return NO_EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from artifacts. It only triggers
 * if you currently have positive PSkill and if the artifact bonus is also positive.
 *
 */
_LHF_(ArtifactPrimary_OF)
{
	BYTE primary = c->CL();
	BYTE artifact = c->DL();
	if (primary > 0x7F && artifact <= 0x7F && (primary - artifact) <= 0x7F)
		c->ecx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills when visiting a Wandering Scholar.
 * It only triggers if the PSkill is about to increase from +127 to -128.
 *
 */
_LHF_(ScholarPrimary_OF)
{
	H3Hero *hero = (H3Hero*)c->ebx;
	if (hero->primarySkill[c->edi] == 127)
		return NO_EXEC_DEFAULT; // don't increase

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills when opening Pandora's Box or visiting an Event.
 * It will not trigger if your PSKill is precisely at 127, to allow some freedom for mapmakers.
 *
 */
_LHF_(PandoraPrimary_OF)
{
	BYTE primary = ByteAt(c->ebx + c->eax + 0x476);
	BYTE increase = c->CL();
	if (increase != 127)  // overflow allowed if increase is precisely 127
	{
		if (primary <= 0x7F && (primary + increase) > 0x7F)
			c->ecx = (0x7F - primary);
	}
	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from relics. It only triggers
 * if you currently have positive PSkill and if the artifact bonus is also positive.
 *
 */
_LHF_(RelicPrimary_OF)
{
	BYTE primary = c->AL();
	BYTE increase = c->DL();
	if (primary <= 0x7F && increase <= 0x7F && (primary + increase) > 0x7F)
		c->edx = (0x7F - primary);

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills when collecting rewards from Seer Huts.
 *
 */
_LHF_(SeerPrimary_OF)
{
	BYTE primary = c->BL();
	BYTE increase = c->DL();
	if (primary > 0x7F && increase <= 0x7F && (primary - increase) <= 0x7F)
		c->ebx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills when visiting Arenas.
 * Only triggers at 126 or 127 which would overflow.
 *
 */
_LHF_(ArenaPrimary_OF)
{
	BYTE primary = c->CL();
	if (primary == 0x80 || primary == 0x81)
		c->ecx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from School of Wars.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(SchoolOfWarPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Mercenary Camps
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(MercenaryCampPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Schools of Magic.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(SchoolOfMagicPrimary_OF)
{
	BYTE primary = c->BL();
	if (primary == 0x80)
		c->ebx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Gardens of Knowledge.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(GardenOfKnowledgePrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Star Axes.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(StarAxisPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Marletto Towers.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(MarlettoTowerPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Wall of Knowledge.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(WallOfKnowledgePrimary_OF)
{
	BYTE primary = c->BL();
	if (primary == 0x80)
		c->ebx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Order of Fire.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(OrderOfFirePrimary_OF)
{
	BYTE primary = c->BL();
	if (primary == 0x80)
		c->ebx = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Hall of Valhalla.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(HallOfValhallaPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Cage of Warlords.
 * Only triggers at -128 meaning overflow has just occured.
 *
 */
_LHF_(CageOfWarlordsPrimary_OF)
{
	BYTE primary = c->AL();
	if (primary == 0x80)
		c->eax = 0x7F;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Libraries of Enlightenment.
 * Only triggers at 126 or 127 which would overflow.
 *
 */
_LHF_(LibraryOfEnlightenmentPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->esi);

	for (int i = 0; i < 4; i++)
	{
		if (hero->primarySkill[i] == 126 || hero->primarySkill[i] == 127)
			hero->primarySkill[i] = 127;
		else
			hero->primarySkill[i] += 2;
	}
	c->return_address = 0x4A2FCC;
	return NO_EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Brimstone Clouds.
 * Only triggers at 126 or 127 which would overflow.
 *
 */
_LHF_(BrimstoneCloudsPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->edx);
	if (hero->primarySkill[2] == 127 || hero->primarySkill[2] == 126)
		return NO_EXEC_DEFAULT;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Earth Guardian.
 * Only triggers between 116 and 127 as the bonus is +12.
 *
 */
_LHF_(EarthGuardianPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->edx);
	if (hero->primarySkill[2] <= 0x7F && hero->primarySkill[2] >= 0x74)
		return NO_EXEC_DEFAULT;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Warlord's Monument.
 * Only triggers between 108 and 127 as the bonus is +20.
 *
 */
_LHF_(WarlordsMonumentPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->edx);
	if (hero->primarySkill[0] <= 0x7F && hero->primarySkill[0] >= 0x6C)
		return NO_EXEC_DEFAULT;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Blood Obelisk.
 * Only triggers between 126 and 127 as the bonus is +2.
 *
 */
_LHF_(BloodObeliskPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->eax);
	if (hero->primarySkill[0] <= 0x7F && hero->primarySkill[0] >= 0x7E)
		return NO_EXEC_DEFAULT;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Glyphs of Fear.
 * Only triggers between 126 and 127 as the bonus is +2.
 *
 */
_LHF_(GlyphsOfFear_OF)
{
	H3Hero *hero = (H3Hero*)(c->edx);
	if (hero->primarySkill[1] <= 0x7F && hero->primarySkill[1] >= 0x7E)
		return NO_EXEC_DEFAULT;

	return EXEC_DEFAULT;
}

/*
 *
 * This hook prevents overflow of primary skills from Carnivorous Plant.
 * Only triggers between 118 and 127 as the bonus is +10.
 *
 */
_LHF_(CarnivorousPlantPrimary_OF)
{
	H3Hero *hero = (H3Hero*)(c->eax);
	if ((UINT8)hero->primarySkill[1] > 0x7F || (UINT8)hero->primarySkill[1] < 0x75)
		hero->primarySkill[1] += 10;
	if ((UINT8)hero->primarySkill[0] > 0x7F || (UINT8)hero->primarySkill[0] < 0x75)
		hero->primarySkill[0] += 10;

	c->return_address = 0x463A98;
	return NO_EXEC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void HooksInit()
{
	_PI->WriteLoHook(0x4DAB67, LevelUpPrimary_OF);
	_PI->WriteLoHook(0x4E2DFE, ArtifactPrimary_OF);
	_PI->WriteLoHook(0x4A4B7F, ScholarPrimary_OF);
	_PI->WriteLoHook(0x49F8A2, PandoraPrimary_OF);
	_PI->WriteLoHook(0x4E2D73, RelicPrimary_OF);
	_PI->WriteLoHook(0x574157, SeerPrimary_OF);
	_PI->WriteLoHook(0x49E543, ArenaPrimary_OF);
	_PI->WriteLoHook(0x4A786E, SchoolOfWarPrimary_OF);
	_PI->WriteLoHook(0x4A35B0, MercenaryCampPrimary_OF);
	_PI->WriteLoHook(0x4A31D4, SchoolOfMagicPrimary_OF);
	_PI->WriteLoHook(0x4A2410, GardenOfKnowledgePrimary_OF);
	_PI->WriteLoHook(0x4A3AC0, StarAxisPrimary_OF);
	_PI->WriteLoHook(0x4A1D50, MarlettoTowerPrimary_OF);
	_PI->WriteLoHook(0x5BDEA0, WallOfKnowledgePrimary_OF);
	_PI->WriteLoHook(0x5BDF55, OrderOfFirePrimary_OF);
	_PI->WriteLoHook(0x5BE143, OrderOfFirePrimary_OF);
	_PI->WriteLoHook(0x5BE224, CageOfWarlordsPrimary_OF);
	_PI->WriteLoHook(0x4A2F92, LibraryOfEnlightenmentPrimary_OF);
	_PI->WriteLoHook(0x463976, BrimstoneCloudsPrimary_OF);
	_PI->WriteLoHook(0x4639A4, EarthGuardianPrimary_OF);
	_PI->WriteLoHook(0x4639D2, WarlordsMonumentPrimary_OF);
	_PI->WriteLoHook(0x463A2F, BloodObeliskPrimary_OF);
	_PI->WriteLoHook(0x4639FC, GlyphsOfFear_OF);
	_PI->WriteLoHook(0x463A64, CarnivorousPlantPrimary_OF);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

const char* PLUGIN_NAME = "RK Primary skill overflow";

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	static BOOL pluginOn = FALSE;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!pluginOn)
		{
			_P = GetPatcher();
			_PI = _P->CreateInstance(PLUGIN_NAME);


			HooksInit();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
