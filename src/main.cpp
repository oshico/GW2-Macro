#include "macro/Macro.h"
#include <vector>
#include <mutex>
#include <atomic>

std::vector g_macros = {
    Macro("Empty", "MACRO_1"), Macro("Empty", "MACRO_2"), Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"), Macro("Empty", "MACRO_5"), Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"), Macro("Empty", "MACRO_8"), Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")
};

bool g_showMainWindow = false;
bool g_showMacroEditor = false;
int g_selectedMacroIndex = -1;
std::mutex g_macroMutex;
std::atomic<bool> g_killMacros(false);