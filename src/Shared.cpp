#include "Shared.h"
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// Global variable definitions
AddonAPI_t* APIDefs = nullptr;
std::vector<CustomKeybind> g_keybinds;
std::vector<KeyAction> g_keyActions;  // Moved from main.cpp
bool g_showKeybindWindow = false;

// Game Bind to String Helper (moved from main.cpp)
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

// Execute Key Actions (moved from main.cpp)
void ExecuteKeyActions()
{
    for (const auto& action : g_keyActions)
    {
        if (action.delayMs > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(action.delayMs));
        }
        
        if (action.isKeyDown)
        {
            APIDefs->GameBinds_PressAsync(action.gameBind);
        }
        else
        {
            APIDefs->GameBinds_ReleaseAsync(action.gameBind);
        }
        
        APIDefs->Log(LOGL_DEBUG, "MacroManager", 
                    ("Key " + std::string(action.isKeyDown ? "PRESS: " : "RELEASE: ") + 
                     GetBindName(action.gameBind)).c_str());
    }
}

// Delete a macro by index (moved from main.cpp)
void DeleteMacro(size_t index)
{
    if (index < g_keybinds.size())
    {
        // Unregister the keybind first
        UnregisterKeybind(g_keybinds[index].identifier);
        
        // Remove from vector
        g_keybinds.erase(g_keybinds.begin() + index);
        
        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro deleted");
    }
}

// Delete a key action from sequence (moved from main.cpp)
void DeleteKeyAction(size_t index)
{
    if (index < g_keyActions.size())
    {
        g_keyActions.erase(g_keyActions.begin() + index);
        APIDefs->Log(LOGL_INFO, "MacroManager", "Key action deleted");
    }
}

// Save macro function (extracted from GUI code)
bool SaveMacro(const std::string& name, const std::string& keyCombo)
{
    if (g_keyActions.empty() || name.empty())
    {
        return false;
    }
    
    std::string identifier = "KB_MACRO_" + std::to_string(g_keybinds.size() + 1);
    CustomKeybind newKeybind(name, identifier, keyCombo);
    
    g_keybinds.push_back(newKeybind);
    RegisterKeybind(newKeybind);
    
    APIDefs->Log(LOGL_INFO, "MacroManager", 
                ("Saved macro: " + newKeybind.name).c_str());
    
    return true;
}

// Clear key sequence function
void ClearKeySequence()
{
    g_keyActions.clear();
}

// Keybind management functions
void RegisterKeybind(const CustomKeybind& keybind)
{
    if (APIDefs)
    {
        APIDefs->InputBinds_RegisterWithString(keybind.identifier.c_str(), ProcessKeybind, keybind.keyCombo.c_str());
    }
}

void UnregisterKeybind(const std::string& identifier)
{
    if (APIDefs)
    {
        APIDefs->InputBinds_Deregister(identifier.c_str());
    }
}