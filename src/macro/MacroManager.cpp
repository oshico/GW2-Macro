#include "MacroManager.h"
#include "KeybindManager.h"
#include "../core/Context.h"
#include "../persistence/Settings.h"
#include <string>

void DeleteMacro(const size_t index)
{
  if (index >= g_context.macros.size())
    return;

  UnregisterKeybind(g_context.macros[index].identifier);

  g_context.macros[index].actions.clear();
  g_context.macros[index].enabled = false;
  g_context.macros[index].name = "Empty";
  g_context.macros[index].identifier = "MACRO_" + std::to_string(index + 1);

  g_context.apiDefs->Log(
    LOGL_INFO,
    "MacroManager",
    ("Macro slot " + std::to_string(index + 1) + " cleared").c_str());
}

void SaveMacro(const std::string& name, const int slot, const std::vector<KeyAction>& actions)
{
  if (name.empty())
  {
    g_context.apiDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: name is empty");
    return;
  }

  if (actions.empty())
  {
    g_context.apiDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: no actions defined");
    return;
  }

  if (slot < 0 || slot >= 10)
  {
    g_context.apiDefs->Log(LOGL_WARNING, "MacroManager", "Cannot save macro: invalid slot number");
    return;
  }

  const std::string identifier = "MACRO_" + std::to_string(slot + 1);

  UnregisterKeybind(g_context.macros[slot].identifier);

  g_context.macros[slot].name = name;
  g_context.macros[slot].identifier = identifier;
  g_context.macros[slot].actions = actions;
  g_context.macros[slot].enabled = true;

  RegisterKeybind(g_context.macros[slot]);

  g_context.apiDefs->Log(
    LOGL_INFO,
    "MacroManager",
    ("Macro '" + name + "' saved to slot " + std::to_string(slot + 1)).c_str());

  g_context.showMacroEditor = false;
  g_context.selectedMacroIndex = -1;
}

void OpenMacroEditor(const int index)
{
  g_context.selectedMacroIndex = index;
  g_context.showMacroEditor = true;

  if (index >= 0 && index < static_cast<int>(g_context.macros.size()))
  {
    g_context.apiDefs->Log(
      LOGL_DEBUG,
      "MacroManager",
      ("Opening editor for macro: " + g_context.macros[index].name).c_str());
  }
  else
  {
    g_context.apiDefs->Log(LOGL_DEBUG, "MacroManager", "Opening editor for new macro");
  }
}

bool ImportMacroFromJson(const std::string& jsonStr, const int slot)
{
  try
  {
    const nlohmann::json j = nlohmann::json::parse(jsonStr);

    if (const Macro newMacro = JsonToMacro(j, slot); !newMacro.actions.empty())
    {
      UnregisterKeybind(g_context.macros[slot].identifier);
      g_context.macros[slot] = newMacro;
      SaveMacrosToJson();
    }

    return true;
  }
  catch (...)
  {
    return false;
  }
}