#include "keybind_manager.h"
#include "macro_executor.h"
#include "shared.h"
#include <cstring>

void ProcessKeybind(const char *ActionIdentifier, const bool ActionIsRelease) {
    if (ActionIsRelease)
        return;

    if (strcmp(ActionIdentifier, "MACRO_SHOW_WINDOW") == 0) {
        ShowMainWindow = !ShowMainWindow;
        return;
    }

    if (strcmp(ActionIdentifier, "MACRO_KILL") == 0) {
        KillAllMacros();
        return;
    }

    if (MacroMutex.try_lock()) {
        if (KillMacros.load()) {
            MacroMutex.unlock();
            return;
        }

        for (const auto &Macro : Macros) {
            if (Macro.Identifier == ActionIdentifier) {
                ExecuteMacro(Macro);
                break;
            }
        }
        MacroMutex.unlock();
    } else {
        if (ApiDefinition) {
            ApiDefinition->GUI_SendAlert("Another macro is currently running. Wait or use Kill All.");
        }
    }
}

void SetupKeybinds() {
    if (!ApiDefinition)
        return;

    ApiDefinition->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");
    ApiDefinition->InputBinds_RegisterWithString("MACRO_KILL", ProcessKeybind, "CTRL+SHIFT+X");

    for (const auto &macro : Macros)
        RegisterKeybind(macro);
}

void RegisterKeybind(const Macro &Macro) {
    if (!ApiDefinition)
        return;

    ApiDefinition->InputBinds_RegisterWithString(Macro.Identifier.c_str(), ProcessKeybind, "");

    ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Keybind registered for: " + Macro.Identifier).c_str());
}

void UnregisterKeybind(const std::string &Identifier) {
    if (ApiDefinition) {
        ApiDefinition->InputBinds_Deregister(Identifier.c_str());
        ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Keybind unregistered: " + Identifier).c_str());
    }
}