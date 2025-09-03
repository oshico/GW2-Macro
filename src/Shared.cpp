#include "Shared.h"
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdint>
#include <csignal>
#include <setjmp.h>

AddonAPI_t* APIDefs = nullptr;

// 10 macro slots
std::vector<Macro> g_macros = {
    Macro("Empty", "MACRO_1"), Macro("Empty", "MACRO_2"), Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"), Macro("Empty", "MACRO_5"), Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"), Macro("Empty", "MACRO_8"), Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")
};

bool g_showMainWindow = false;
bool g_showMacroEditor = false;
int g_selectedMacroIndex = -1;

// Helper: convert game bind to string
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

// Execute a macro
void ExecuteMacro(const Macro& macro)
{
    if (!macro.enabled) return;

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Executing macro: " + macro.name).c_str());

    for (const auto& action : macro.actions)
    {
        if (action.delayMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(action.delayMs));

        if (action.isKeyDown)
            APIDefs->GameBinds_PressAsync(action.gameBind);
        else
            APIDefs->GameBinds_ReleaseAsync(action.gameBind);

        APIDefs->Log(LOGL_DEBUG, "MacroManager",
            ("Key " + std::string(action.isKeyDown ? "PRESS: " : "RELEASE: ") +
             GetBindName(action.gameBind)).c_str());
    }
}

// Delete macro (reset slot)
void DeleteMacro(size_t index)
{
    if (index >= g_macros.size()) return;

    g_macros[index].actions.clear();
    g_macros[index].enabled = false;
    g_macros[index].name = "Empty";
    g_macros[index].identifier = "MACRO_" + std::to_string(index + 1);
    UnregisterKeybind(g_macros[index].identifier);

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Macro slot " + std::to_string(index + 1) + " cleared").c_str());
}

// Save or update macro
void SaveMacro(const std::string& name, int slot, const std::vector<KeyAction>& actions)
{
    if (name.empty() || actions.empty() || slot < 0 || slot >= 10)
    {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: invalid name, actions, or slot");
        return;
    }

    std::string identifier = "MACRO_" + std::to_string(slot + 1);

    g_macros[slot].name = name;
    g_macros[slot].identifier = identifier;
    g_macros[slot].actions = actions;
    g_macros[slot].enabled = true;

    RegisterKeybind(g_macros[slot]);

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Saved macro in slot " + std::to_string(slot + 1)).c_str());

    g_showMacroEditor = false;
    g_selectedMacroIndex = -1;
}

// Register/Unregister keybinds
void RegisterKeybind(const Macro& macro)
{
    if (!APIDefs) return;
    APIDefs->InputBinds_RegisterWithString(macro.identifier.c_str(), ProcessKeybind, "");
}

void UnregisterKeybind(const std::string& identifier)
{
    if (APIDefs)
        APIDefs->InputBinds_Deregister(identifier.c_str());
}

// Open Macro Editor
void OpenMacroEditor(int index)
{
    g_selectedMacroIndex = index;
    g_showMacroEditor = true;
}