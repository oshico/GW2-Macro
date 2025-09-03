#ifndef SHARED_H
#define SHARED_H

#include "Nexus.h"
#include <vector>
#include <string>

/**
 * @file Shared.h
 * @brief Shared declarations and structures for the Macro Keybind Manager addon
 * @author oshico
 * @version 0.1.1
 */

// Global API pointer for Nexus addon framework
extern AddonAPI_t *APIDefs;

/**
 * @struct KeyAction
 * @brief Represents a single key action within a macro sequence
 * 
 * Each KeyAction defines a game bind to press or release, with an optional delay
 * before execution. This allows for complex timing-based macro sequences.
 */
struct KeyAction
{
    EGameBinds gameBind;    ///< The game bind to trigger (e.g., GB_SkillWeapon1)
    bool isKeyDown;         ///< True for press action, false for release action
    int delayMs;            ///< Delay in milliseconds before executing this action

    /**
     * @brief Constructs a KeyAction with the specified parameters
     * @param bind The game bind to trigger
     * @param down Whether this is a press (true) or release (false) action
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
    std::string name;                   ///< Display name of the macro
    std::string identifier;             ///< Unique identifier for keybind registration (MACRO_X format)
    bool enabled;                       ///< Whether the macro is active and can be executed
    std::vector<KeyAction> actions;     ///< Sequence of actions to execute

    /**
     * @brief Constructs a Macro with the specified name and identifier
     * @param n Display name for the macro
     * @param id Unique identifier for keybind system
     */
    Macro(std::string n, std::string id)
        : name(std::move(n)), identifier(std::move(id)), enabled(true) {}
};

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

extern std::vector<Macro> g_macros;        ///< Container for all 10 macro slots
extern bool g_showMainWindow;              ///< Controls visibility of main macro manager window
extern bool g_showMacroEditor;             ///< Controls visibility of macro editor window
extern int g_selectedMacroIndex;           ///< Index of currently selected macro for editing (-1 = none)

// =============================================================================
// ADDON LIFECYCLE FUNCTIONS
// =============================================================================

/**
 * @brief Initializes the addon when loaded by Nexus
 * @param aApi Pointer to the Nexus API structure
 * 
 * Sets up GUI callbacks, registers keybinds, and initializes macro system.
 */
void AddonLoad(AddonAPI_t *aApi);

/**
 * @brief Cleans up resources when addon is unloaded
 * 
 * Unregisters all keybinds and GUI callbacks before shutdown.
 */
void AddonUnload();

// =============================================================================
// GUI RENDERING FUNCTIONS
// =============================================================================

/**
 * @brief Main render callback for addon GUI elements
 * 
 * Called every frame by Nexus. Renders both main window and macro editor
 * if they are visible.
 */
void AddonRender();

/**
 * @brief Renders the addon's options panel in Nexus settings
 * 
 * Displays addon information, statistics, and quick access buttons.
 */
void AddonOptions();

/**
 * @brief Renders the main macro manager window
 * 
 * Shows the table of all 10 macro slots with their status, action counts,
 * and controls for editing/deleting macros.
 */
void RenderMainWindow();

/**
 * @brief Renders the macro editor window
 * 
 * Provides interface for creating and editing macro sequences, including
 * action list management and slot selection.
 */
void RenderMacroEditor();

/**
 * @brief Opens the macro editor for a specific slot
 * @param index Index of macro to edit (-1 for new macro)
 */
void OpenMacroEditor(int index = -1);

// =============================================================================
// KEYBIND AND MACRO EXECUTION
// =============================================================================

/**
 * @brief Handles keybind press events from Nexus
 * @param aIdentifier String identifier of the pressed keybind
 * @param aIsRelease True if this is a key release event (ignored)
 * 
 * Routes keybind events to appropriate handlers (main window toggle or macro execution).
 */
void ProcessKeybind(const char *aIdentifier, bool aIsRelease);

/**
 * @brief Sets up all keybind registrations
 * 
 * Registers the main window toggle keybind and all macro slot keybinds.
 */
void SetupKeybinds();

/**
 * @brief Registers a keybind for the specified macro
 * @param macro The macro to register a keybind for
 */
void RegisterKeybind(const Macro &macro);

/**
 * @brief Unregisters a keybind by its identifier
 * @param identifier The keybind identifier to unregister
 */
void UnregisterKeybind(const std::string &identifier);

// =============================================================================
// MACRO MANAGEMENT FUNCTIONS
// =============================================================================

/**
 * @brief Executes a macro's action sequence
 * @param macro The macro to execute
 * 
 * Processes each KeyAction in the macro's sequence, respecting delays and
 * press/release states. Only executes if macro is enabled.
 */
void ExecuteMacro(const Macro &macro);

/**
 * @brief Deletes/resets a macro slot to empty state
 * @param index Index of the macro slot to delete
 * 
 * Clears all actions, disables the macro, resets name to "Empty",
 * and unregisters its keybind.
 */
void DeleteMacro(size_t index);

/**
 * @brief Saves a macro to the specified slot
 * @param name Display name for the macro
 * @param slot Slot number (0-9) to save the macro to
 * @param actions Vector of KeyActions that make up the macro sequence
 * 
 * Creates or updates a macro in the specified slot and registers its keybind.
 */
void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Converts a game bind enum to human-readable string
 * @param bind The EGameBinds enum value to convert
 * @return Human-readable name of the game bind
 */
const char *GetBindName(EGameBinds bind);

#endif // SHARED_H