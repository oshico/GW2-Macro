#ifndef SHARED_H
#define SHARED_H

#include "Nexus.h"
#include <vector>
#include <string>

// Global API pointer - accessible from all files that include this header
extern AddonAPI_t* APIDefs;

// Key Action Structure (moved from main.cpp)
struct KeyAction
{
    EGameBinds gameBind;
    bool isKeyDown;
    int delayMs;
    
    KeyAction(EGameBinds bind, bool down, int delay = 0) : gameBind(bind), isKeyDown(down), delayMs(delay) {}
};

// Keybind structure
struct CustomKeybind
{
    std::string name;
    std::string identifier;
    std::string keyCombo;
    bool enabled;
    
    CustomKeybind(std::string n, std::string id, std::string combo) 
        : name(std::move(n)), identifier(std::move(id)), keyCombo(std::move(combo)), enabled(true) {}
};

// Global variables
extern std::vector<CustomKeybind> g_keybinds;
extern std::vector<KeyAction> g_keyActions;  // Moved from main.cpp
extern bool g_showKeybindWindow;

// Function prototypes for the addon lifecycle
void AddonLoad(AddonAPI_t* aApi);
void AddonUnload();

// Render callback prototypes
void AddonRender();
void AddonOptions();

// Event handler prototypes  
void HandleExampleEvent(void* aEventArgs);

// Keybind handler prototype
void ProcessKeybind(const char* aIdentifier, bool aIsRelease);

// Setup function prototypes
void SetupKeybinds();
void SetupEvents();

// Keybind management
void RegisterKeybind(const CustomKeybind& keybind);
void UnregisterKeybind(const std::string& identifier);

// Helper functions (moved from main.cpp)
const char* GetBindName(EGameBinds bind);
void ExecuteKeyActions();
void DeleteMacro(size_t index);
void DeleteKeyAction(size_t index);

// Macro management functions
bool SaveMacro(const std::string& name, const std::string& keyCombo);
void ClearKeySequence();

#endif // SHARED_H