/**
 * @file Shared.cpp
 * @brief Implementation of shared functionality for Macro Keybind Manager.
 *
 * Provides utilities for macro management, keybind registration, execution,
 * and JSON-based persistence. Includes helper functions for working with
 * game binds and basic JSON parsing/escaping.
 *
 * @author oshico
 * @version 0.1.2
 */

#include "Shared.h"
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
 * @brief Execute a macro by performing its configured key actions in order.
 *
 * Applies any configured delays between actions and logs execution for debugging.
 * Skips execution if the macro is disabled.
 *
 * @param macro The macro to execute.
 */
void ExecuteMacro(const Macro &macro)
{
    if (!macro.enabled)
        return;

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
// HELPER FUNCTIONS FOR MANUAL JSON HANDLING
// =============================================================================

/**
 * @brief Escape special characters in a string for JSON output.
 *
 * Replaces quotes, backslashes, and control characters with their JSON escape
 * sequences.
 *
 * @param str Input string to escape.
 * @return std::string Escaped JSON-compatible string.
 */
std::string EscapeJsonString(const std::string &str)
{
    std::string escaped;
    for (char c : str)
    {
        switch (c)
        {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\b':
            escaped += "\\b";
            break;
        case '\f':
            escaped += "\\f";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            if (c < 0x20)
            {
                escaped += "\\u0000";
            }
            else
            {
                escaped += c;
            }
            break;
        }
    }
    return escaped;
}

/**
 * @brief Unescape JSON-encoded string back into normal text.
 *
 * Converts escaped sequences like `\n`, `\t`, `\\`, and `\"` into their actual
 * character representations.
 *
 * @param str JSON-escaped string.
 * @return std::string Unescaped string.
 */
std::string UnescapeJsonString(const std::string &str)
{
    std::string unescaped;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            switch (str[i + 1])
            {
            case '"':
                unescaped += '"';
                ++i;
                break;
            case '\\':
                unescaped += '\\';
                ++i;
                break;
            case 'b':
                unescaped += '\b';
                ++i;
                break;
            case 'f':
                unescaped += '\f';
                ++i;
                break;
            case 'n':
                unescaped += '\n';
                ++i;
                break;
            case 'r':
                unescaped += '\r';
                ++i;
                break;
            case 't':
                unescaped += '\t';
                ++i;
                break;
            default:
                unescaped += str[i];
                break;
            }
        }
        else
        {
            unescaped += str[i];
        }
    }
    return unescaped;
}

// =============================================================================
// CONFIGURATION FILE PATH
// =============================================================================

/**
 * @brief Get the full path to the macros configuration file.
 *
 * Uses the Nexus API to retrieve the addon directory for "Macro" and appends
 * `macros.json`.
 *
 * @return std::string Full file path to the configuration file.
 */
std::string GetConfigFilePath()
{
    if (!APIDefs)
        return "";

    std::string addonDir = APIDefs->Paths_GetAddonDirectory("Macro");
    return addonDir + "/macros.json";
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
    try
    {
        std::string configPath = GetConfigFilePath();
        if (configPath.empty())
        {
            APIDefs->Log(LOGL_WARNING, "MacroManager", "Failed to get config file path");
            return false;
        }

        std::filesystem::path filePath(configPath);
        std::filesystem::create_directories(filePath.parent_path());

        std::stringstream json;
        json << "{\n";
        json << "  \"version\": \"0.1.2\",\n";
        json << "  \"macros\": [\n";

        for (size_t i = 0; i < g_macros.size(); ++i)
        {
            const Macro &macro = g_macros[i];

            json << "    {\n";
            json << "      \"name\": \"" << EscapeJsonString(macro.name) << "\",\n";
            json << "      \"identifier\": \"" << EscapeJsonString(macro.identifier) << "\",\n";
            json << "      \"enabled\": " << (macro.enabled ? "true" : "false") << ",\n";
            json << "      \"actions\": [\n";

            for (size_t j = 0; j < macro.actions.size(); ++j)
            {
                const KeyAction &action = macro.actions[j];

                json << "        {\n";
                json << "          \"gameBind\": \"" << GameBindToString(action.gameBind) << "\",\n";
                json << "          \"isKeyDown\": " << (action.isKeyDown ? "true" : "false") << ",\n";
                json << "          \"delayMs\": " << action.delayMs << "\n";
                json << "        }";

                if (j < macro.actions.size() - 1)
                    json << ",";
                json << "\n";
            }

            json << "      ]\n";
            json << "    }";

            if (i < g_macros.size() - 1)
                json << ",";
            json << "\n";
        }

        json << "  ]\n";
        json << "}\n";

        std::ofstream file(configPath);
        if (!file.is_open())
        {
            APIDefs->Log(LOGL_WARNING, "MacroManager", "Failed to open config file for writing");
            return false;
        }

        file << json.str();
        file.close();

        APIDefs->Log(LOGL_INFO, "MacroManager", "Macros saved successfully");
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
// SIMPLE JSON PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Extract a JSON string value by key starting from a position.
 *
 * @param json JSON string to parse.
 * @param key Key name to extract.
 * @param pos Reference to current parse position; updated after extraction.
 * @return std::string Extracted and unescaped string value.
 */
std::string ExtractJsonString(const std::string &json, const std::string &key, size_t &pos)
{
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = json.find(searchKey, pos);
    if (keyPos == std::string::npos)
        return "";

    size_t valueStart = json.find("\"", keyPos + searchKey.length());
    if (valueStart == std::string::npos)
        return "";

    valueStart++;
    size_t valueEnd = json.find("\"", valueStart);
    if (valueEnd == std::string::npos)
        return "";

    pos = valueEnd + 1;
    return UnescapeJsonString(json.substr(valueStart, valueEnd - valueStart));
}

/**
 * @brief Extract a boolean value from JSON by key starting from a position.
 *
 * @param json JSON string to parse.
 * @param key Key name to extract.
 * @param pos Reference to current parse position; updated after extraction.
 * @return true if value is true, false if false or key not found.
 */
bool ExtractJsonBool(const std::string &json, const std::string &key, size_t &pos)
{
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = json.find(searchKey, pos);
    if (keyPos == std::string::npos)
        return false;

    size_t valueStart = keyPos + searchKey.length();
    std::string substr = json.substr(valueStart, 10);

    if (substr.find("true") != std::string::npos)
    {
        pos = valueStart + 4;
        return true;
    }
    else if (substr.find("false") != std::string::npos)
    {
        pos = valueStart + 5;
        return false;
    }

    return false;
}

/**
 * @brief Extract an integer value from JSON by key starting from a position.
 *
 * @param json JSON string to parse.
 * @param key Key name to extract.
 * @param pos Reference to current parse position; updated after extraction.
 * @return int Parsed integer value, or 0 if not found.
 */
int ExtractJsonInt(const std::string &json, const std::string &key, size_t &pos)
{
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = json.find(searchKey, pos);
    if (keyPos == std::string::npos)
        return 0;

    size_t valueStart = keyPos + searchKey.length();
    size_t valueEnd = valueStart;

    while (valueEnd < json.length() && (json[valueEnd] == ' ' || json[valueEnd] == '\t' || json[valueEnd] == '\n'))
        valueEnd++;

    valueStart = valueEnd;

    while (valueEnd < json.length() && (isdigit(json[valueEnd]) || json[valueEnd] == '-'))
        valueEnd++;

    if (valueEnd > valueStart)
    {
        pos = valueEnd;
        return std::stoi(json.substr(valueStart, valueEnd - valueStart));
    }

    return 0;
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
    try
    {
        std::string configPath = GetConfigFilePath();
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

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::string json = buffer.str();

        size_t macrosStart = json.find("\"macros\":");
        if (macrosStart == std::string::npos)
        {
            APIDefs->Log(LOGL_WARNING, "MacroManager", "Invalid config file format");
            return false;
        }

        size_t arrayStart = json.find("[", macrosStart);
        size_t arrayEnd = json.find("]", arrayStart);
        if (arrayStart == std::string::npos || arrayEnd == std::string::npos)
        {
            APIDefs->Log(LOGL_WARNING, "MacroManager", "Invalid macros array format");
            return false;
        }

        size_t pos = arrayStart + 1;
        int macroIndex = 0;

        while (pos < arrayEnd && macroIndex < static_cast<int>(g_macros.size()))
        {
            size_t objStart = json.find("{", pos);
            if (objStart == std::string::npos || objStart > arrayEnd)
                break;

            size_t objEnd = objStart + 1;
            int braceCount = 1;

            while (objEnd < json.length() && braceCount > 0)
            {
                if (json[objEnd] == '{')
                    braceCount++;
                else if (json[objEnd] == '}')
                    braceCount--;
                objEnd++;
            }

            if (braceCount > 0)
                break;

            size_t macroPos = objStart;
            std::string name = ExtractJsonString(json, "name", macroPos);
            std::string identifier = ExtractJsonString(json, "identifier", macroPos);

            macroPos = objStart;
            bool enabled = ExtractJsonBool(json, "enabled", macroPos);

            if (!name.empty() && !identifier.empty() && macroIndex < static_cast<int>(g_macros.size()))
            {
                g_macros[macroIndex].name = name;
                g_macros[macroIndex].identifier = identifier;
                g_macros[macroIndex].enabled = enabled;
                g_macros[macroIndex].actions.clear();

                size_t actionsStart = json.find("\"actions\":", objStart);
                if (actionsStart != std::string::npos && actionsStart < objEnd)
                {
                    size_t actionsArrayStart = json.find("[", actionsStart);
                    size_t actionsArrayEnd = json.find("]", actionsArrayStart);

                    if (actionsArrayStart != std::string::npos && actionsArrayEnd != std::string::npos)
                    {
                        size_t actionPos = actionsArrayStart + 1;

                        while (actionPos < actionsArrayEnd)
                        {
                            size_t actionObjStart = json.find("{", actionPos);
                            if (actionObjStart == std::string::npos || actionObjStart > actionsArrayEnd)
                                break;

                            size_t actionObjEnd = actionObjStart + 1;
                            int actionBraceCount = 1;

                            while (actionObjEnd < json.length() && actionBraceCount > 0)
                            {
                                if (json[actionObjEnd] == '{')
                                    actionBraceCount++;
                                else if (json[actionObjEnd] == '}')
                                    actionBraceCount--;
                                actionObjEnd++;
                            }

                            size_t actionDataPos = actionObjStart;
                            std::string gameBindStr = ExtractJsonString(json, "gameBind", actionDataPos);

                            actionDataPos = actionObjStart;
                            bool isKeyDown = ExtractJsonBool(json, "isKeyDown", actionDataPos);

                            actionDataPos = actionObjStart;
                            int delayMs = ExtractJsonInt(json, "delayMs", actionDataPos);

                            if (!gameBindStr.empty())
                            {
                                EGameBinds gameBind = StringToGameBind(gameBindStr);
                                g_macros[macroIndex].actions.emplace_back(gameBind, isKeyDown, delayMs);
                            }

                            actionPos = actionObjEnd;
                        }
                    }
                }
            }

            pos = objEnd;
            macroIndex++;
        }

        APIDefs->Log(LOGL_INFO, "MacroManager", "Macros loaded successfully");
