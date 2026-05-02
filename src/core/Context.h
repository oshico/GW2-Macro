#pragma once

#include "macro/Macro.h"
#include "../nexus/Nexus.h"
#include <vector>
#include <mutex>
#include <atomic>

struct Context
{
    std::vector<Macro> macros;
    bool showMainWindow = false;
    bool showMacroEditor = false;
    bool showImportExport = false;
    int selectedMacroIndex = -1;
    std::mutex macroMutex;
    std::atomic<bool> killMacros{false};
    AddonAPI_t* apiDefs = nullptr;
};

extern Context g_context;