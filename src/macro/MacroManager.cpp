#include "MacroManager.h"
#include "KeybindManager.h"

void DeleteMacro(const size_t index) {
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

void SaveMacro(const std::string &name, const int slot, const std::vector<KeyAction> &actions) {
    if (name.empty()) {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: name is empty");
        return;
    }

    if (actions.empty()) {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: no actions defined");
        return;
    }

    if (slot < 0 || slot >= 10) {
        APIDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: invalid slot number");
        return;
    }

    const std::string identifier = "MACRO_" + std::to_string(slot + 1);

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

void OpenMacroEditor(const int index) {
    g_selectedMacroIndex = index;
    g_showMacroEditor = true;

    if (index >= 0 && index < static_cast<int>(g_macros.size())) {
        APIDefs->Log(
            LOGL_DEBUG,
            "MacroManager",
            ("Opening editor for macro: " + g_macros[index].name).c_str());
    } else {
        APIDefs->Log(LOGL_DEBUG, "MacroManager", "Opening editor for new macro");
    }
}