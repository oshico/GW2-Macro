#include "GameModeCheck.h"
#include "../core/Context.h"
#include "../mumble/Mumble.h"

extern Context g_context;

bool IsInCompetitiveMode() {
    if (!g_context.apiDefs)
        return false;

    const auto mumbleData = static_cast<Mumble::Data *>(
        g_context.apiDefs->DataLink_Get(DL_MUMBLE_LINK));

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