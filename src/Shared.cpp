#include "Shared.h"
#include "Mumble.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/** @brief Pointer to the Addon API definitions, set during initialization. */
AddonAPI_t *APIDefs = nullptr;

/**
 * @brief Pre-initialized macro slots (MACRO_1 through MACRO_10).
 *
 * All slots start as "Empty" and disabled. Users can assign macros to any slot
 * through the editor interface.
 */
std::vector<Macro> g_macros = {
    Macro("Empty", "MACRO_1"), Macro("Empty", "MACRO_2"), Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"), Macro("Empty", "MACRO_5"), Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"), Macro("Empty", "MACRO_8"), Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")};

/** @brief Whether the main manager window should be displayed. */
bool g_showMainWindow = false;

/** @brief Whether the macro editor window should be displayed. */
bool g_showMacroEditor = false;

/** @brief Index of the currently selected macro for editing, or -1 if none. */
int g_selectedMacroIndex = -1;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Get the human-readable name of a game bind.
 *
 * @param bind Game bind enum value.
 * @return const char* Name of the bind (e.g. "Weapon 1", "Dodge").
 */
const char *GetBindName(EGameBinds bind)
{
    switch (bind)
    {
    case GB_SkillWeapon1:
        return "Weapon 1";
    case GB_SkillWeapon2:
        return "Weapon 2";
    case GB_SkillWeapon3:
        return "Weapon 3";
    case GB_SkillWeapon4:
        return "Weapon 4";
    case GB_SkillWeapon5:
        return "Weapon 5";
    case GB_SkillHeal:
        return "Heal Skill";
    case GB_SkillUtility1:
        return "Utility 1";
    case GB_SkillUtility2:
        return "Utility 2";
    case GB_SkillUtility3:
        return "Utility 3";
    case GB_SkillElite:
        return "Elite Skill";
    case GB_MoveDodge:
        return "Dodge";
    case GB_MoveJump_SwimUp_FlyUp:
        return "Jump";
    case GB_MiscInteract:
        return "Interact";
    case GB_SkillWeaponSwap:
        return "Weapon Swap";
    default:
        return "Unknown Bind";
    }
}

/**
 * @brief Convert a game bind enum to its string identifier.
 *
 * @param bind Game bind enum value.
 * @return std::string Identifier string (e.g. "GB_SkillWeapon1").
 */
std::string GameBindToString(EGameBinds bind)
{
    switch (bind)
    {
    case GB_SkillWeapon1:
        return "GB_SkillWeapon1";
    case GB_SkillWeapon2:
        return "GB_SkillWeapon2";
    case GB_SkillWeapon3:
        return "GB_SkillWeapon3";
    case GB_SkillWeapon4:
        return "GB_SkillWeapon4";
    case GB_SkillWeapon5:
        return "GB_SkillWeapon5";
    case GB_SkillHeal:
        return "GB_SkillHeal";
    case GB_SkillUtility1:
        return "GB_SkillUtility1";
    case GB_SkillUtility2:
        return "GB_SkillUtility2";
    case GB_SkillUtility3:
        return "GB_SkillUtility3";
    case GB_SkillElite:
        return "GB_SkillElite";
    case GB_MoveDodge:
        return "GB_MoveDodge";
    case GB_MoveJump_SwimUp_FlyUp:
        return "GB_MoveJump_SwimUp_FlyUp";
    case GB_MiscInteract:
        return "GB_MiscInteract";
    case GB_SkillWeaponSwap:
        return "GB_SkillWeaponSwap";
    case GB_MoveForward:
        return "GB_MoveForward";
    case GB_MoveBackward:
        return "GB_MoveBackward";
    case GB_MoveLeft:
        return "GB_MoveLeft";
    case GB_MoveRight:
        return "GB_MoveRight";
    case GB_SkillProfession1:
        return "GB_SkillProfession1";
    case GB_SkillProfession2:
        return "GB_SkillProfession2";
    case GB_SkillProfession3:
        return "GB_SkillProfession3";
    case GB_SkillProfession4:
        return "GB_SkillProfession4";
    case GB_SkillProfession5:
        return "GB_SkillProfession5";
    default:
        return "GB_SkillWeapon1";
    }
}

/**
 * @brief Convert a string identifier into its corresponding game bind enum.
 *
 * @param bindStr Identifier string (e.g. "GB_SkillWeapon1").
 * @return EGameBinds Enum value for the bind, defaults to GB_SkillWeapon1 if invalid.
 */
EGameBinds StringToGameBind(const std::string &bindStr)
{
    if (bindStr == "GB_SkillWeapon1")
        return GB_SkillWeapon1;
    if (bindStr == "GB_SkillWeapon2")
        return GB_SkillWeapon2;
    if (bindStr == "GB_SkillWeapon3")
        return GB_SkillWeapon3;
    if (bindStr == "GB_SkillWeapon4")
        return GB_SkillWeapon4;
    if (bindStr == "GB_SkillWeapon5")
        return GB_SkillWeapon5;
    if (bindStr == "GB_SkillHeal")
        return GB_SkillHeal;
    if (bindStr == "GB_SkillUtility1")
        return GB_SkillUtility1;
    if (bindStr == "GB_SkillUtility2")
        return GB_SkillUtility2;
    if (bindStr == "GB_SkillUtility3")
        return GB_SkillUtility3;
    if (bindStr == "GB_SkillElite")
        return GB_SkillElite;
    if (bindStr == "GB_MoveDodge")
        return GB_MoveDodge;
    if (bindStr == "GB_MoveJump_SwimUp_FlyUp")
        return GB_MoveJump_SwimUp_FlyUp;
    if (bindStr == "GB_MiscInteract")
        return GB_MiscInteract;
    if (bindStr == "GB_SkillWeaponSwap")
        return GB_SkillWeaponSwap;
    if (bindStr == "GB_MoveForward")
        return GB_MoveForward;
    if (bindStr == "GB_MoveBackward")
        return GB_MoveBackward;
    if (bindStr == "GB_MoveLeft")
        return GB_MoveLeft;
    if (bindStr == "GB_MoveRight")
        return GB_MoveRight;
    if (bindStr == "GB_SkillProfession1")
        return GB_SkillProfession1;
    if (bindStr == "GB_SkillProfession2")
        return GB_SkillProfession2;
    if (bindStr == "GB_SkillProfession3")
        return GB_SkillProfession3;
    if (bindStr == "GB_SkillProfession4")
        return GB_SkillProfession4;
    if (bindStr == "GB_SkillProfession5")
        return GB_SkillProfession5;

    return GB_SkillWeapon1;
}

// =============================================================================
// MACRO EXECUTION
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
void ExecuteMacro(const Macro &macro)
{
    if (!macro.enabled)
        return;

    if (!AreMacrosAllowed())
    {
        APIDefs->GUI_SendAlert("Macros disabled in PVP/WvW modes");
        return;
    }

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Executing macro: " + macro.name).c_str());

    for (const auto &action : macro.actions)
    {
        if (action.delayMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(action.delayMs));

        if (action.isKeyDown)
            APIDefs->GameBinds_PressAsync(action.gameBind);
        else
            APIDefs->GameBinds_ReleaseAsync(action.gameBind);

        APIDefs->Log(
            LOGL_DEBUG,
            "MacroManager",
            ("Action executed: " +
             std::string(action.isKeyDown ? "PRESS " : "RELEASE ") +
             GetBindName(action.gameBind))
                .c_str());
    }
}

// =============================================================================
// MACRO MANAGEMENT
// =============================================================================

/**
 * @brief Delete (reset) a macro at a specific slot index.
 *
 * Unregisters the keybind, clears its actions, disables it, and resets the slot
 * back to the "Empty" state.
 *
 * @param index Index of the macro slot to delete.
 */
void DeleteMacro(size_t index)
{
    if (index >= g_macros.size())
        return;

    UnregisterKeybind(g_macros[index].identifier);

    g_macros[index].actions.clear();
    g_macros[index].enabled = false;
    g_macros[index].name = "Empty";
    g_macros[index].identifier = "MACRO_" + std::to_string(index + 1);

    APIDefs->Log(
        LOGL_INFO,
        "MacroManager",
        ("Macro slot " + std::to_string(index + 1) + " cleared").c_str());
}

/**
 * @brief Save a macro to a specified slot.
 *
 * Validates the macro name and actions, unregisters any previous keybind,
 * updates the slot with new data, registers the new keybind, and closes
 * the editor after saving.
 *
 * @param name     Name of the macro.
 * @param slot     Slot index (0–9).
 * @param actions  List of actions to assign to the macro.
 */
void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions)
{
    if (name.empty())
    {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: name is empty");
        return;
    }

    if (actions.empty())
    {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: no actions defined");
        return;
    }

    if (slot < 0 || slot >= 10)
    {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: invalid slot number");
        return;
    }

    std::string identifier = "MACRO_" + std::to_string(slot + 1);

    UnregisterKeybind(g_macros[slot].identifier);

    g_macros[slot].name = name;
    g_macros[slot].identifier = identifier;
    g_macros[slot].actions = actions;
    g_macros[slot].enabled = true;

    RegisterKeybind(g_macros[slot]);

    APIDefs->Log(
        LOGL_INFO,
        "MacroManager",
        ("Macro '" + name + "' saved to slot " + std::to_string(slot + 1)).c_str());

    g_showMacroEditor = false;
    g_selectedMacroIndex = -1;
}

// =============================================================================
// KEYBIND MANAGEMENT
// =============================================================================

/**
 * @brief Register a keybind for a given macro.
 *
 * Registers the keybind with Nexus using the macro’s identifier. The bind is
 * registered with an empty string so the user can configure it later in Nexus.
 *
 * @param macro Macro for which to register the keybind.
 */
void RegisterKeybind(const Macro &macro)
{
    if (!APIDefs)
        return;

    APIDefs->InputBinds_RegisterWithString(macro.identifier.c_str(), ProcessKeybind, "");

    APIDefs->Log(
        LOGL_DEBUG,
        "MacroManager",
        ("Keybind registered for: " + macro.identifier).c_str());
}

/**
 * @brief Unregister a keybind by its identifier.
 *
 * Deregisters the keybind from Nexus and logs the result.
 *
 * @param identifier Identifier of the keybind to unregister.
 */
void UnregisterKeybind(const std::string &identifier)
{
    if (APIDefs)
    {
        APIDefs->InputBinds_Deregister(identifier.c_str());
        APIDefs->Log(
            LOGL_DEBUG,
            "MacroManager",
            ("Keybind unregistered: " + identifier).c_str());
    }
}

// =============================================================================
// GUI CONTROL
// =============================================================================

/**
 * @brief Open the macro editor for a given macro slot.
 *
 * Sets the global editor state to display the macro editor window, either for
 * an existing macro slot (0–9) or for a new macro (-1).
 *
 * @param index Index of the macro to edit, or -1 to create a new macro.
 */
void OpenMacroEditor(int index)
{
    g_selectedMacroIndex = index;
    g_showMacroEditor = true;

    if (index >= 0 && index < static_cast<int>(g_macros.size()))
    {
        APIDefs->Log(
            LOGL_DEBUG,
            "MacroManager",
            ("Opening editor for macro: " + g_macros[index].name).c_str());
    }
    else
    {
        APIDefs->Log(LOGL_DEBUG, "MacroManager", "Opening editor for new macro");
    }
}

// =============================================================================
// JSON SAVE FUNCTIONALITY
// =============================================================================

/**
 * @brief Save all macros to the JSON configuration file.
 *
 * Builds JSON manually with macro names, identifiers, enabled state, and actions,
 * then writes it to the file at `GetConfigFilePath()`. Creates directories if needed.
 *
 * @return true if save was successful, false otherwise.
 */
bool SaveMacrosToJson()
{
    std::string addonPath = APIDefs->Paths_GetAddonDirectory("MacroManager");
    std::string configPath = APIDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
    try
    {
        if (configPath.empty())
        {
            if (APIDefs)
                APIDefs->Log(LOGL_WARNING, "MacroManager", "No config file path available");
            return false;
        }

        std::filesystem::create_directories(std::filesystem::path(configPath).parent_path());

        nlohmann::json j;
        j["version"] = "0.1.4";

        nlohmann::json macrosArray = nlohmann::json::array();
        for (const auto &macro : g_macros)
        {
            nlohmann::json macroObj;
            macroObj["name"] = macro.name;
            macroObj["identifier"] = macro.identifier;
            macroObj["enabled"] = macro.enabled;
            nlohmann::json actionsArray = nlohmann::json::array();
            for (const auto &action : macro.actions)
            {
                nlohmann::json actionObj;
                actionObj["gameBind"] = GameBindToString(action.gameBind);
                actionObj["isKeyDown"] = action.isKeyDown;
                actionObj["delayMs"] = action.delayMs;
                actionsArray.push_back(actionObj);
            }
            macroObj["actions"] = actionsArray;
            macrosArray.push_back(macroObj);
        }
        j["macros"] = macrosArray;

        std::ofstream file(configPath);
        file << j.dump(2);
        return true;
    }
    catch (const std::exception &e)
    {
        if (APIDefs)
            APIDefs->Log(LOGL_WARNING, "MacroManager",
                         ("Failed to save macros: " + std::string(e.what())).c_str());
        return false;
    }
}

// =============================================================================
// JSON LOAD FUNCTIONALITY
// =============================================================================

/**
 * @brief Load macros from the JSON configuration file.
 *
 * Parses the JSON manually and updates `g_macros` with names, identifiers,
 * enabled states, and action sequences. Uses default values if file is missing
 * or malformed.
 *
 * @return true if load was successful, false otherwise.
 */
bool LoadMacrosFromJson()
{
    std::string addonPath = APIDefs->Paths_GetAddonDirectory("MacroManager");
    std::string configPath = APIDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
    try
    {
        if (configPath.empty())
        {
            APIDefs->Log(LOGL_INFO, "MacroManager", "No config file path available, using defaults");
            return false;
        }

        std::ifstream file(configPath);
        if (!file.is_open())
        {
            APIDefs->Log(LOGL_INFO, "MacroManager", "Config file not found, using defaults");
            return false;
        }

        nlohmann::json j;
        file >> j;

        if (j.contains("macros") && j["macros"].is_array())
        {
            size_t index = 0;
            for (const auto &macroObj : j["macros"])
            {
                if (index >= g_macros.size())
                    break;

                g_macros[index].name = macroObj.value("name", "Empty");
                g_macros[index].enabled = macroObj.value("enabled", false);

                if (macroObj.contains("actions") && macroObj["actions"].is_array())
                {
                    for (const auto &actionObj : macroObj["actions"])
                    {
                        if (actionObj.is_object() &&
                            actionObj.contains("gameBind") && actionObj["gameBind"].is_string() &&
                            actionObj.contains("isKeyDown") && actionObj["isKeyDown"].is_boolean() &&
                            actionObj.contains("delayMs") && actionObj["delayMs"].is_number())
                        {
                            std::string bindStr = actionObj["gameBind"];
                            EGameBinds gameBind = StringToGameBind(bindStr);
                            bool isKeyDown = actionObj["isKeyDown"];
                            int delayMs = actionObj["delayMs"];

                            g_macros[index].actions.emplace_back(gameBind, isKeyDown, delayMs);
                        }
                        else
                        {
                            APIDefs->Log(LOGL_WARNING, "MacroManager", "Skipping invalid action in macro");
                        }
                    }
                }
                index++;
            }
        }
        return true;
    }
    catch (const std::exception &e)
    {
        if (APIDefs)
            APIDefs->Log(LOGL_WARNING, "MacroManager",
                         ("Failed to load macros: " + std::string(e.what())).c_str());

        for (size_t i = 0; i < g_macros.size(); ++i)
        {
            g_macros[i].name = "Empty";
            g_macros[i].identifier = "MACRO_" + std::to_string(i + 1);
            g_macros[i].enabled = false;
            g_macros[i].actions.clear();
        }

        return false;
    }
}

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
bool IsInCompetitiveMode()
{
    if (!APIDefs)
        return false;

    Mumble::Data *mumbleData = static_cast<Mumble::Data *>(
        APIDefs->DataLink_Get(DL_MUMBLE_LINK));

    if (mumbleData)
    {
        // Check the isCompetitive flag from Mumble link
        return mumbleData->Context.IsCompetitive;
    }

    return false;
}

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
bool IsInPVEMode()
{
    // Simply check if we're NOT in competitive mode
    return !IsInCompetitiveMode();
}

/**
 * @brief Get a human-readable string describing the current game mode
 *
 * Returns a descriptive string indicating whether the player is in
 * competitive mode (PvP/WvW) or PVE mode. Useful for UI display.
 *
 * @return "COMPETITIVE (PVP/WVW)" or "PVE" depending on current mode
 * @see IsInCompetitiveMode()
 */
const char *GetCurrentGameModeString()
{
    if (IsInCompetitiveMode())
        return "COMPETITIVE (PVP/WVW)";
    return "PVE";
}

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
bool AreMacrosAllowed()
{
    return IsInPVEMode();
}
