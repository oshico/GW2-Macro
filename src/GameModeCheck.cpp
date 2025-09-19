#include "GameModeCheck.h"
#include "Shared.h"
#include "Mumble.h"

// =============================================================================
// COMPETITIVE MODE DETECTION
// =============================================================================

bool IsInCompetitiveMode()
{
    if (!APIDefs)
        return false;

    Mumble::Data *mumbleData = static_cast<Mumble::Data *>(
        APIDefs->DataLink_Get(DL_MUMBLE_LINK));

    if (mumbleData)
    {
        return mumbleData->Context.IsCompetitive;
    }

    return false;
}

// =============================================================================
// PVE MODE DETECTION
// =============================================================================

bool IsInPVEMode()
{
    return !IsInCompetitiveMode();
}

// =============================================================================
// GAME MODE STRING UTILITIES
// =============================================================================

const char *GetCurrentGameModeString()
{
    if (IsInCompetitiveMode())
        return "COMPETITIVE (PVP/WVW)";
    return "PVE";
}

// =============================================================================
// MACRO PERMISSION CHECKS
// =============================================================================

bool AreMacrosAllowed() { return IsInPVEMode(); }
