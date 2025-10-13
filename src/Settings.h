#ifndef SETTINGS_H
#define SETTINGS_H

#include "Macro.h"
#include <vector>

/**
 * @brief Save all macros to the JSON configuration file
 * 
 * Builds a JSON object containing all macro data (names, identifiers, enabled states,
 * and action sequences) and writes it to the configuration file. Creates directories
 * if they don't exist.
 * 
 * @return true if save was successful
 * @return false if save failed (logged to console)
 * 
 * @note File path: "MacroManager/macros.json" in addon directory
 * @note Creates backup of existing file before overwriting
 */
bool SaveMacrosToJson();

/**
 * @brief Load macros from the JSON configuration file
 * 
 * Parses the JSON configuration file and updates the global macro list with saved
 * names, identifiers, enabled states, and action sequences. Uses default values
 * if the file is missing, malformed, or contains invalid data.
 * 
 * @return true if load was successful
 * @return false if load failed (uses defaults, logged to console)
 * 
 * @note If loading fails, all macros are reset to default empty state
 */
bool LoadMacrosFromJson();

#endif