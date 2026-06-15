#include "macro_manager.h"
#include "keybind_manager.h"
#include "macro.h"
#include "macro_save.h"
#include "nexus/Nexus.h"
#include "nlohmann/json.hpp"
#include "shared.h"
#include <string>

void DeleteMacro(const size_t Index) {
    if (Index >= Macros.size())
        return;

    UnregisterKeybind(Macros[Index].Identifier);

    Macros[Index].Actions.clear();
    Macros[Index].Enabled = false;
    Macros[Index].Name = "Empty";
    Macros[Index].Identifier = "MACRO_" + std::to_string(Index + 1);

    ApiDefinition->Log(LOGL_INFO, "MacroManager", ("Macro slot " + std::to_string(Index + 1) + " cleared").c_str());
}

void SaveMacro(const std::string &Name, const int Slot, const std::vector<KeybindAction> &Actions) {
    if (Name.empty()) {
        ApiDefinition->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: name is empty");
        return;
    }

    if (Actions.empty()) {
        ApiDefinition->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: no actions defined");
        return;
    }

    if (Slot < 0 || Slot >= 10) {
        ApiDefinition->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: invalid slot number");
        return;
    }

    const std::string Identifier = "MACRO_" + std::to_string(Slot + 1);

    UnregisterKeybind(Macros[Slot].Identifier);

    Macros[Slot].Name = Name;
    Macros[Slot].Identifier = Identifier;
    Macros[Slot].Actions = Actions;
    Macros[Slot].Enabled = true;

    RegisterKeybind(Macros[Slot]);

    ApiDefinition->Log(LOGL_INFO, "MacroManager", ("Macro '" + Name + "' saved to slot " + std::to_string(Slot + 1)).c_str());

    ShowEditorWindow = false;
    SelectedMacroIndex = -1;
}

void OpenMacroEditor(const int Index) {
    SelectedMacroIndex = Index;
    ShowEditorWindow = true;

    if (Index >= 0 && Index < static_cast<int>(Macros.size())) {
        ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Opening editor for macro: " + Macros[Index].Name).c_str());
    } else {
        ApiDefinition->Log(LOGL_DEBUG, "MacroManager", "Opening editor for new macro");
    }
}

bool ImportMacroFromJson(const std::string &JsonString, const int Slot) {
    try {
        const nlohmann::json Json = nlohmann::json::parse(JsonString);

        if (const Macro NewMacro = JsonToMacro(Json, Slot); !NewMacro.Actions.empty()) {
            UnregisterKeybind(Macros[Slot].Identifier);
            Macros[Slot] = NewMacro;
            SaveMacrosToJson();
        }

        return true;
    } catch (...) {
        return false;
    }
}