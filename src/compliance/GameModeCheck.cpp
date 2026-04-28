#include "GameModeCheck.h"
#include "../core/Shared.h"
#include "../mumble/Mumble.h"

bool IsInCompetitiveMode() {
    if (!APIDefs)
        return false;

    const auto mumbleData = static_cast<Mumble::Data *>(
        APIDefs->DataLink_Get(DL_MUMBLE_LINK));

    if (mumbleData) {
        return mumbleData->Context.IsCompetitive;
    }

    return false;
}

bool IsInPVEMode() {
    return !IsInCompetitiveMode();
}

const char *GetCurrentGameModeString() {
    if (IsInCompetitiveMode())
        return "COMPETITIVE (PVP/WVW)";
    return "PVE";
}

bool AreMacrosAllowed() { return IsInPVEMode(); }