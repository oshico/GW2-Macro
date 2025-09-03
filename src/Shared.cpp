/**
 * @file Shared.cpp
 * @brief Implementation of shared functionality for Macro Keybind Manager
 * @author oshico
 * @version 0.1.1
 */

#include "Shared.h"
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

AddonAPI_t* APIDefs = nullptr;

/**
 * @brief Pre-initialized macro slots (MACRO_1 through MACRO_10)
 * 
 * All slots start as "Empty" and disabled. Users can assign macros to any slot
 * through the editor interface.
 */
std::vector<Macro> g_macros = {
    Macro("Empty", "MACRO_1"), Macro("Empty", "MACRO_2"), Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"), Macro("Empty", "MACRO_5"), Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"), Macro("Empty", "MACRO_8"), Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")
};

bool g_showMainWindow = false;
bool g_showMacroEditor = false;
int g_selectedMacroIndex = -1;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

const char* GetBindName(EGameBinds bind)
{
    switch (bind)
    {
    case GB_SkillWeapon1: return "Weapon 1";
    case GB_SkillWeapon2: return "Weapon 2";
    case GB_SkillWeapon3: return "Weapon 3";
    case GB_SkillWeapon4: return "Weapon 4";
    case GB_SkillWeapon5: return "Weapon 5";
    case GB_SkillHeal: return "Heal Skill";
    case GB_SkillUtility1: return "Utility 1";
    case GB_SkillUtility2: return "Utility 2";
    case GB_SkillUtility3: return "Utility 3";
    case GB_SkillElite: return "Elite Skill";
    case GB_MoveDodge: return "Dodge";
    case GB_MoveJump_SwimUp_FlyUp: return "Jump";
    case GB_MiscInteract: return "Interact";
    case GB_SkillWeaponSwap: return "Weapon Swap";
    default: return "Unknown Bind";
    }
}

// =============================================================================
// MACRO EXECUTION
// =============================================================================

void ExecuteMacro(const Macro& macro)
{
    // Skip execution if macro is disabled
    if (!macro.enabled) 
        return;

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Executing macro: " + macro.name).c_str());

    // Execute each action in sequence
    for (const auto& action : macro.actions)
    {
        // Apply delay before action if specified
        if (action.delayMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(action.delayMs));

        // Execute the key press or release
        if (action.isKeyDown)
            APIDefs->GameBinds_PressAsync(action.gameBind);
        else
            APIDefs->GameBinds_ReleaseAsync(action.gameBind);

        // Log the action for debugging
        APIDefs->Log(LOGL_DEBUG, "MacroManager",
            ("Action executed: " + std::string(action.isKeyDown ? "PRESS " : "RELEASE ") +
             GetBindName(action.gameBind)).c_str());
    }
}

// =============================================================================
// MACRO MANAGEMENT
// =============================================================================

void DeleteMacro(size_t index)
{
    if (index >= g_macros.size()) 
        return;

    // Unregister the keybind before clearing
    UnregisterKeybind(g_macros[index].identifier);

    // Reset macro to empty state
    g_macros[index].actions.clear();
    g_macros[index].enabled = false;
    g_macros[index].name = "Empty";
    g_macros[index].identifier = "MACRO_" + std::to_string(index + 1);

    APIDefs->Log(LOGL_INFO, "MacroManager", 
        ("Macro slot " + std::to_string(index + 1) + " cleared").c_str());
}

void SaveMacro(const std::string& name, int slot, const std::vector<KeyAction>& actions)
{
    // Validate input parameters
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

    // Generate identifier for the slot
    std::string identifier = "MACRO_" + std::to_string(slot + 1);

    // Unregister old keybind if one exists
    UnregisterKeybind(g_macros[slot].identifier);

    // Update macro data
    g_macros[slot].name = name;
    g_macros[slot].identifier = identifier;
    g_macros[slot].actions = actions;
    g_macros[slot].enabled = true;

    // Register new keybind
    RegisterKeybind(g_macros[slot]);

    APIDefs->Log(LOGL_INFO, "MacroManager", 
        ("Macro '" + name + "' saved to slot " + std::to_string(slot + 1)).c_str());

    // Close editor after successful save
    g_showMacroEditor = false;
    g_selectedMacroIndex = -1;
}

// =============================================================================
// KEYBIND MANAGEMENT
// =============================================================================

void RegisterKeybind(const Macro& macro)
{
    if (!APIDefs) 
        return;
        
    // Register keybind with empty string (user will set through Nexus)
    APIDefs->InputBinds_RegisterWithString(macro.identifier.c_str(), ProcessKeybind, "");
    
    APIDefs->Log(LOGL_DEBUG, "MacroManager", 
        ("Keybind registered for: " + macro.identifier).c_str());
}

void UnregisterKeybind(const std::string& identifier)
{
    if (APIDefs)
    {
        APIDefs->InputBinds_Deregister(identifier.c_str());
        APIDefs->Log(LOGL_DEBUG, "MacroManager", 
            ("Keybind unregistered: " + identifier).c_str());
    }
}

// =============================================================================
// GUI CONTROL
// =============================================================================

void OpenMacroEditor(int index)
{
    g_selectedMacroIndex = index;
    g_showMacroEditor = true;
    
    if (index >= 0 && index < static_cast<int>(g_macros.size()))
    {
        APIDefs->Log(LOGL_DEBUG, "MacroManager", 
            ("Opening editor for macro: " + g_macros[index].name).c_str());
    }
    else
    {
        APIDefs->Log(LOGL_DEBUG, "MacroManager", "Opening editor for new macro");
    }
}