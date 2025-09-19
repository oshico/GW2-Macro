#ifndef GAME_MODE_CHECKS_H
#define GAME_MODE_CHECKS_H

/**
 * @brief Check if the player is currently in a competitive game mode (PvP/WvW)
 *
 * This function uses the Mumble link data to detect competitive modes by checking
 * the IsCompetitive flag in the game context. This is the most reliable method
 * for detecting PvP and WvW content.
 *
 * @return true if player is in PvP or WvW mode, false otherwise
 * @note Uses Mumble link data accessed through Nexus API
 */
bool IsInCompetitiveMode();

/**
 * @brief Check if the player is currently in PVE game mode
 *
 * This is a convenience function that simply returns the inverse of
 * IsInCompetitiveMode(). Returns true when the player is NOT in competitive
 * modes (PvP/WvW).
 *
 * @return true if player is in PVE mode, false if in competitive mode
 * @see IsInCompetitiveMode()
 */
bool IsInPVEMode();

/**
 * @brief Get a human-readable string describing the current game mode
 *
 * Returns a descriptive string indicating whether the player is in
 * competitive mode (PvP/WvW) or PVE mode. Useful for UI display.
 *
 * @return "COMPETITIVE (PVP/WVW)" or "PVE" depending on current mode
 * @see IsInCompetitiveMode()
 */
const char *GetCurrentGameModeString();

/**
 * @brief Check if macro execution is currently permitted
 *
 * This function determines whether macros can be executed based on the
 * current game mode. Macros are only allowed in PVE content and are
 * automatically blocked in competitive modes (PvP/WvW).
 *
 * @return true if macros can be executed (PVE mode), false otherwise
 * @note This is the primary function used by ExecuteMacro() to enforce restrictions
 * @see ExecuteMacro(), IsInPVEMode()
 */
bool AreMacrosAllowed();

#endif