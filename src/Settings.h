#ifndef SETTINGS_H
#define SETTINGS_H

#include "Macro.h"
#include <vector>

/**
 * @brief Save all macros to the JSON configuration file.
 *
 * Builds JSON manually with macro names, identifiers, enabled state, and actions,
 * then writes it to the file at `GetConfigFilePath()`. Creates directories if needed.
 *
 * @return true if save was successful, false otherwise.
 */
bool SaveMacrosToJson();

/**
 * @brief Load macros from the JSON configuration file.
 *
 * Parses the JSON manually and updates `g_macros` with names, identifiers,
 * enabled states, and action sequences. Uses default values if file is missing
 * or malformed.
 *
 * @return true if load was successful, false otherwise.
 */
bool LoadMacrosFromJson();

#endif