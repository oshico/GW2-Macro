#include "game_mode_check.h"
#include "mumble/Mumble.h"
#include "shared.h"

bool IsPlayerInCompetitiveGameMode() {
    if (!ApiDefinition)
        return false;

    const auto MumbleLinkData = static_cast<Mumble::Data *>(
        ApiDefinition->DataLink_Get(DL_MUMBLE_LINK));

    if (MumbleLinkData) {
        return MumbleLinkData->Context.IsCompetitive;
    }

    return false;
}

bool IsPlayerInPlayerVersusEnvironmentGameMode() {
    return !IsPlayerInCompetitiveGameMode();
}

const char *GetCurrentGameModeString() {
    if (IsPlayerInCompetitiveGameMode())
        return "COMPETITIVE (PVP/WVW)";
    return "PVE";
}

bool AreMacrosAllowed() { return IsPlayerInPlayerVersusEnvironmentGameMode(); }