#ifndef GAME_MODE_CHECKS_H
#define GAME_MODE_CHECKS_H

bool IsInCompetitiveMode();

bool IsInPVEMode();

const char *GetCurrentGameModeString();

bool AreMacrosAllowed();

#endif