#include "MacroManager.h"
#include "Shared.h"
#include "GameModeCheck.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

// =============================================================================
// GLOBAL MACRO VARIABLES
// =============================================================================

std::vector<Macro> g_macros = {
    Macro("Empty", "MACRO_1"), Macro("Empty", "MACRO_2"), Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"), Macro("Empty", "MACRO_5"), Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"), Macro("Empty", "MACRO_8"), Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")};

bool g_showMainWindow = false;
bool g_showMacroEditor = false;
int g_selectedMacroIndex = -1;
std::mutex g_macroMutex;
std::atomic<bool> g_killMacros(false);

// =============================================================================
// KEYBIND HANDLING
// =============================================================================

void ProcessKeybind(const char *aIdentifier, bool aIsRelease)
{
    if (aIsRelease)
        return;

    if (strcmp(aIdentifier, "MACRO_SHOW_WINDOW") == 0)
    {
        g_showMainWindow = !g_showMainWindow;
        return;
    }

     if (strcmp(aIdentifier, "MACRO_KILL") == 0)
    {
        KillAllMacros();
        return;
    }

    if (g_macroMutex.try_lock())
    {
        if (g_killMacros.load())
        {
            g_macroMutex.unlock();
            return;
        }

        for (const auto &macro : g_macros)
        {
            if (macro.identifier == aIdentifier)
            {
                ExecuteMacro(macro);
                break;
            }
        }
        g_macroMutex.unlock();
    }
    else
    {
        if (APIDefs)
        {
            APIDefs->GUI_SendAlert("Another macro is currently running. Wait or use Kill All.");
        }
    }
}

void SetupKeybinds()
{
    if (!APIDefs)
        return;

    APIDefs->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");
    APIDefs->InputBinds_RegisterWithString("MACRO_KILL", ProcessKeybind, "CTRL+SHIFT+X");
    
    for (const auto &macro : g_macros)
        RegisterKeybind(macro);
}

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
// MACRO EXECUTION
// =============================================================================

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
        if (g_killMacros.load())
        {
            APIDefs->Log(LOGL_INFO, "MacroManager", "Macro execution stopped by Kill All");
            g_killMacros.store(false);
            return;
        }

        if (action.delayMs > 0)
        {
            int remainingDelay = action.delayMs;
            const int chunkSize = 50;

            while (remainingDelay > 0 && !g_killMacros.load())
            {
                int currentChunk = (remainingDelay > chunkSize) ? chunkSize : remainingDelay;
                std::this_thread::sleep_for(std::chrono::milliseconds(currentChunk));
                remainingDelay -= currentChunk;
            }

            if (g_killMacros.load())
            {
                APIDefs->Log(LOGL_INFO, "MacroManager", "Macro execution stopped during delay");
                g_killMacros.store(false);
                return;
            }
        }

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

    APIDefs->Log(LOGL_INFO, "MacroManager", ("Macro completed: " + macro.name).c_str());
}

// =============================================================================
// KILL ALL FUNCTIONALITY
// =============================================================================

void KillAllMacros()
{
    g_killMacros.store(true);
    
    std::lock_guard<std::mutex> lock(g_macroMutex);
    
    g_killMacros.store(false);
    
    ReleaseAllGameKeys();

    APIDefs->Log(LOGL_INFO, "MacroManager", "All macros killed");
    APIDefs->GUI_SendAlert("All macros stopped");
}

void ReleaseAllGameKeys()
{
    if (!APIDefs)
        return;

    EGameBinds allBinds[] = {
        GB_SkillWeapon1, GB_SkillWeapon2, GB_SkillWeapon3, GB_SkillWeapon4, GB_SkillWeapon5,
        GB_SkillHeal, GB_SkillUtility1, GB_SkillUtility2, GB_SkillUtility3, GB_SkillElite,
        GB_MoveDodge, GB_MoveJump_SwimUp_FlyUp, GB_MiscInteract, GB_SkillWeaponSwap,
        GB_MoveForward, GB_MoveBackward, GB_MoveLeft, GB_MoveRight,
        GB_SkillProfession1, GB_SkillProfession2, GB_SkillProfession3, 
        GB_SkillProfession4, GB_SkillProfession5
    };

    APIDefs->Log(LOGL_DEBUG, "MacroManager", "Releasing all game keys...");

    for (EGameBinds bind : allBinds)
    {
        APIDefs->GameBinds_ReleaseAsync(bind);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    APIDefs->Log(LOGL_INFO, "MacroManager", "All game keys released");
}


// =============================================================================
// MACRO MANAGEMENT
// =============================================================================

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
// UTILITY FUNCTIONS
// =============================================================================

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