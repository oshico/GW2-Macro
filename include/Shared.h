#ifndef SHARED_H
#define SHARED_H

#include "Nexus.h"
#include "Mumble.h"
#include <vector>
#include <string>

// =============================================================================
// GLOBAL API POINTER
// =============================================================================

/// Global API pointer for Nexus addon framework
extern AddonAPI_t *APIDefs;

// =============================================================================
// DATA STRUCTURES
// =============================================================================

/**
 * @struct KeyAction
 * @brief Represents a single key action within a macro sequence
 *
 * Each KeyAction defines a game bind to press or release, with an optional
 * delay before execution. This allows for complex timing-based macro sequences.
 */
struct KeyAction
{
    EGameBinds gameBind; ///< The game bind to trigger (e.g., GB_SkillWeapon1)
    bool isKeyDown;      ///< True for press action, false for release action
    int delayMs;         ///< Delay in milliseconds before executing this action

    /**
     * @brief Construct a KeyAction with the specified parameters
     * @param bind The game bind to trigger
     * @param down True for a press action, false for a release action
     * @param delay Optional delay in milliseconds before execution (default: 0)
     */
    KeyAction(EGameBinds bind, bool down, int delay = 0)
        : gameBind(bind), isKeyDown(down), delayMs(delay) {}
};

/**
 * @struct Macro
 * @brief Represents a complete macro with name, identifier, and action sequence
 *
 * Macros are stored in predefined slots (MACRO_1 through MACRO_10) and can be
 * enabled/disabled. Each macro contains a sequence of KeyActions that are
 * executed in order when the macro is triggered.
 */
struct Macro
{
    std::string name;               ///< Display name of the macro
    std::string identifier;         ///< Unique identifier for keybind registration (MACRO_X format)
    bool enabled;                   ///< Whether the macro is active and can be executed
    std::vector<KeyAction> actions; ///< Sequence of actions to execute

    /**
     * @brief Construct a Macro with the specified name and identifier
     * @param n Display name for the macro
     * @param id Unique identifier for keybind system
     */
    Macro(std::string n, std::string id)
        : name(std::move(n)), identifier(std::move(id)), enabled(true) {}
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

extern std::vector<Macro> g_macros; ///< Container for all 10 macro slots
extern bool g_showMainWindow;       ///< Controls visibility of main macro manager window
extern bool g_showMacroEditor;      ///< Controls visibility of macro editor window
extern int g_selectedMacroIndex;    ///< Index of currently selected macro for editing (-1 = none)

// =============================================================================
// ADDON LIFECYCLE FUNCTIONS
// =============================================================================

/**
 * @brief Initialize the addon when loaded by Nexus
 * @param aApi Pointer to the Nexus API structure
 *
 * Sets up GUI callbacks, registers keybinds, and initializes macro system.
 */
void AddonLoad(AddonAPI_t *aApi);

/**
 * @brief Clean up resources when the addon is unloaded
 *
 * Unregisters all keybinds and GUI callbacks before shutdown.
 */
void AddonUnload();

// =============================================================================
// GUI RENDERING FUNCTIONS
// =============================================================================

/**
 * @brief Render all addon GUI elements
 *
 * Called every frame by Nexus. Renders both main window and macro editor
 * if they are visible.
 */
void AddonRender();

/**
 * @brief Render the addon's options panel in Nexus settings
 *
 * Displays addon information, statistics, and quick access buttons.
 */
void AddonOptions();

/**
 * @brief Render the main macro manager window
 *
 * Shows the table of all 10 macro slots with their status, action counts,
 * and controls for editing or deleting macros.
 */
void RenderMainWindow();

/**
 * @brief Render the macro editor window
 *
 * Provides an interface for creating and editing macro sequences, including
 * action list management and slot selection.
 */
void RenderMacroEditor();

/**
 * @brief Open the macro editor for a specific slot
 * @param index Index of macro to edit (-1 for a new macro)
 */
void OpenMacroEditor(int index = -1);

// =============================================================================
// KEYBIND AND MACRO EXECUTION
// =============================================================================

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

// =============================================================================
// MACRO MANAGEMENT FUNCTIONS
// =============================================================================

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

// =============================================================================
// JSON PERSISTENCE FUNCTIONS
// =============================================================================

/**
 * @brief Save all macros to a JSON configuration file
 * @return True if save was successful, false otherwise
 */
bool SaveMacrosToJson();

/**
 * @brief Load macros from a JSON configuration file
 * @return True if load was successful, false otherwise
 */
bool LoadMacrosFromJson();

/**
 * @brief Get the path to the configuration file
 * @return Full path to the macro configuration JSON file
 */
std::string GetConfigFilePath();

// =============================================================================
// PVE/Competitive MODE CHECKS
// =============================================================================

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
const char* GetCurrentGameModeString();

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

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Convert a game bind enum to a human-readable string
 * @param bind The EGameBinds enum value to convert
 * @return Human-readable name of the game bind
 */
const char *GetBindName(EGameBinds bind);

#endif // SHARED_H