#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "Macro.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

extern std::vector<Macro> g_macros;
extern bool g_showMainWindow;
extern bool g_showMacroEditor;
extern int g_selectedMacroIndex;
extern std::mutex g_macroMutex;
extern std::atomic<bool> g_killMacros;

void ProcessKeybind(const char *aIdentifier, bool aIsRelease);
void SetupKeybinds();
void RegisterKeybind(const Macro &macro);
void UnregisterKeybind(const std::string &identifier);
void ExecuteMacro(const Macro &macro);
void KillAllMacros();
void ReleaseAllGameKeys();
void DeleteMacro(size_t index);
void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);
void OpenMacroEditor(int index = -1);
const char *GetBindName(EGameBinds bind);
std::string GameBindToString(EGameBinds bind);
EGameBinds StringToGameBind(const std::string &bindStr);

void MoveMouse(const MousePosition &pos);
void SendMouseInput(EMouseButton button, bool isDown);
void SendMouseClickAtPosition(EMouseButton button, bool isDown, const MousePosition &pos);
const char *GetMouseButtonName(EMouseButton button);
std::string MouseButtonToString(EMouseButton button);
EMouseButton StringToMouseButton(const std::string &buttonStr);
std::string PositionTypeToString(EPositionType type);
EPositionType StringToPositionType(const std::string &typeStr);

#endif