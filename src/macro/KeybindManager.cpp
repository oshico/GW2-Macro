#include "KeybindManager.h"
#include <cstring>

#include "MacroExecutor.h"
#include "Shared.h"

void ProcessKeybind(const char *aIdentifier, const bool aIsRelease) {
    if (aIsRelease)
        return;

    if (strcmp(aIdentifier, "MACRO_SHOW_WINDOW") == 0) {
        extern bool g_showMainWindow;
        g_showMainWindow = !g_showMainWindow;
        return;
    }

    if (strcmp(aIdentifier, "MACRO_KILL") == 0) {
        KillAllMacros();
        return;
    }

    if (g_macroMutex.try_lock()) {
        if (g_killMacros.load()) {
            g_macroMutex.unlock();
            return;
        }

        for (const auto &macro: g_macros) {
            if (macro.identifier == aIdentifier) {
                ExecuteMacro(macro);
                break;
            }
        }
        g_macroMutex.unlock();
    } else {
        if (APIDefs) {
            APIDefs->GUI_SendAlert("Another macro is currently running. Wait or use Kill All.");
        }
    }
}

void SetupKeybinds() {
    if (!APIDefs)
        return;

    APIDefs->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");
    APIDefs->InputBinds_RegisterWithString("MACRO_KILL", ProcessKeybind, "CTRL+SHIFT+X");

    for (const auto &macro: g_macros)
        RegisterKeybind(macro);
}

void RegisterKeybind(const Macro &macro) {
    if (!APIDefs)
        return;

    APIDefs->InputBinds_RegisterWithString(macro.identifier.c_str(), ProcessKeybind, "");

    APIDefs->Log(
        LOGL_DEBUG,
        "MacroManager",
        ("Keybind registered for: " + macro.identifier).c_str());
}

void UnregisterKeybind(const std::string &identifier) {
    if (APIDefs) {
        APIDefs->InputBinds_Deregister(identifier.c_str());
        APIDefs->Log(
            LOGL_DEBUG,
            "MacroManager",
            ("Keybind unregistered: " + identifier).c_str());
    }
}