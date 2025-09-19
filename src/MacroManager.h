#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "Macro.h"
#include <vector>

extern std::vector<Macro> g_macros; ///< Container for all 10 macro slots
extern bool g_showMainWindow;       ///< Controls visibility of main macro manager window
extern bool g_showMacroEditor;      ///< Controls visibility of macro editor window
extern int g_selectedMacroIndex;    ///< Index of currently selected macro for editing (-1 = none)

/**
 * @brief Handle keybind press events from Nexus
 * @param aIdentifier String identifier of the pressed keybind
 * @param aIsRelease True if this is a key release event (ignored)
 *
 * Routes keybind events to appropriate handlers (main window toggle or macro execution).
 */
void ProcessKeybind(const char *aIdentifier, bool aIsRelease);

/**
 * @brief Set up all keybind registrations
 *
 * Registers the main window toggle keybind and all macro slot keybinds.
 */
void SetupKeybinds();

/**
 * @brief Register a keybind for the specified macro
 * @param macro The macro to register a keybind for
 */
void RegisterKeybind(const Macro &macro);

/**
 * @brief Unregister a keybind by its identifier
 * @param identifier The keybind identifier to unregister
 */
void UnregisterKeybind(const std::string &identifier);

/**
 * @brief Execute a macro's action sequence with game mode restrictions
 *
 * Processes each KeyAction in the macro's sequence, respecting delays and
 * press/release states. Only executes if the macro is enabled AND the
 * current game mode permits macro usage (PVE only).
 *
 * @param macro The macro to execute
 * @note Automatically blocks execution in competitive modes (PvP/WvW)
 * @note Shows a user alert when macros are blocked in competitive content
 * @see AreMacrosAllowed(), IsInCompetitiveMode()
 */
void ExecuteMacro(const Macro &macro);

/**
 * @brief Delete or reset a macro slot to an empty state
 * @param index Index of the macro slot to delete
 *
 * Clears all actions, disables the macro, resets name to "Empty",
 * and unregisters its keybind.
 */
void DeleteMacro(size_t index);

/**
 * @brief Save a macro to the specified slot
 * @param name Display name for the macro
 * @param slot Slot number (0–9) to save the macro to
 * @param actions Vector of KeyActions that make up the macro sequence
 *
 * Creates or updates a macro in the specified slot and registers its keybind.
 */
void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);

/**
 * @brief Open the macro editor for a specific slot
 * @param index Index of macro to edit (-1 for a new macro)
 */
void OpenMacroEditor(int index = -1);

/**
 * @brief Convert a game bind enum to a human-readable string
 * @param bind The EGameBinds enum value to convert
 * @return Human-readable name of the game bind
 */
const char *GetBindName(EGameBinds bind);

/**
 * @brief Convert a game bind enum to its string identifier.
 *
 * @param bind Game bind enum value.
 * @return std::string Identifier string (e.g. "GB_SkillWeapon1").
 */
std::string GameBindToString(EGameBinds bind);

/**
 * @brief Convert a string identifier into its corresponding game bind enum.
 *
 * @param bindStr Identifier string (e.g. "GB_SkillWeapon1").
 * @return EGameBinds Enum value for the bind, defaults to GB_SkillWeapon1 if invalid.
 */
EGameBinds StringToGameBind(const std::string &bindStr);

#endif