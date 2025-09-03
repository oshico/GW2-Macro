#ifndef SHARED_H
#define SHARED_H

#include "Nexus.h"
#include <vector>
#include <string>

// Global API pointer
extern AddonAPI_t *APIDefs;

// Key Action Structure
struct KeyAction
{
    EGameBinds gameBind;
    bool isKeyDown;
    int delayMs;

    KeyAction(EGameBinds bind, bool down, int delay = 0)
        : gameBind(bind), isKeyDown(down), delayMs(delay) {}
};

// Macro structure
struct Macro
{
    std::string name;
    std::string identifier;
    bool enabled;
    std::vector<KeyAction> actions;

    Macro(std::string n, std::string id)
        : name(std::move(n)), identifier(std::move(id)), enabled(true) {}
};

// Global variables
extern std::vector<Macro> g_macros;
extern bool g_showMainWindow;
extern bool g_showMacroEditor;
extern int g_selectedMacroIndex;


// Function prototypes
void AddonLoad(AddonAPI_t *aApi);
void AddonUnload();

void AddonRender();
void AddonOptions();

void ProcessKeybind(const char *aIdentifier, bool aIsRelease);

void SetupKeybinds();

void RegisterKeybind(const Macro &macro);
void UnregisterKeybind(const std::string &identifier);

const char *GetBindName(EGameBinds bind);
void ExecuteMacro(const Macro &macro);
void DeleteMacro(size_t index);
void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);

void RenderMainWindow();
void RenderMacroEditor();
void OpenMacroEditor(int index = -1);

#endif // SHARED_H