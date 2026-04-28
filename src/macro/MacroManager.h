#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "Macro.h"
#include "../core/Shared.h"
#include <mutex>
#include <atomic>

extern std::vector<Macro> g_macros;
extern bool g_showMainWindow;
extern bool g_showMacroEditor;
extern int g_selectedMacroIndex;
extern std::mutex g_macroMutex;
extern std::atomic<bool> g_killMacros;

void DeleteMacro(size_t index);

void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);

void OpenMacroEditor(int index = -1);

#endif