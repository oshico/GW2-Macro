#include "KeybindManager.h"
#include "MacroExecutor.h"
#include "../core/Context.h"
#include <cstring>

void ProcessKeybind(const char* aIdentifier, const bool aIsRelease)
{
  if (aIsRelease)
    return;

  if (strcmp(aIdentifier, "MACRO_SHOW_WINDOW") == 0)
  {
    g_context.showMainWindow = !g_context.showMainWindow;
    return;
  }

  if (strcmp(aIdentifier, "MACRO_KILL") == 0)
  {
    KillAllMacros();
    return;
  }

  if (g_context.macroMutex.try_lock())
  {
    if (g_context.killMacros.load())
    {
      g_context.macroMutex.unlock();
      return;
    }

    for (const auto& macro : g_context.macros)
    {
      if (macro.identifier == aIdentifier)
      {
        ExecuteMacro(macro);
        break;
      }
    }
    g_context.macroMutex.unlock();
  }
  else
  {
    if (g_context.apiDefs)
    {
      g_context.apiDefs->GUI_SendAlert("Another macro is currently running. Wait or use Kill All.");
    }
  }
}

void SetupKeybinds()
{
  if (!g_context.apiDefs)
    return;

  g_context.apiDefs->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");
  g_context.apiDefs->InputBinds_RegisterWithString("MACRO_KILL", ProcessKeybind, "CTRL+SHIFT+X");

  for (const auto& macro : g_context.macros)
    RegisterKeybind(macro);
}

void RegisterKeybind(const Macro& macro)
{
  if (!g_context.apiDefs)
    return;

  g_context.apiDefs->InputBinds_RegisterWithString(macro.identifier.c_str(), ProcessKeybind, "");

  g_context.apiDefs->Log(
    LOGL_DEBUG,
    "MacroManager",
    ("Keybind registered for: " + macro.identifier).c_str());
}

void UnregisterKeybind(const std::string& identifier)
{
  if (g_context.apiDefs)
  {
    g_context.apiDefs->InputBinds_Deregister(identifier.c_str());
    g_context.apiDefs->Log(
      LOGL_DEBUG,
      "MacroManager",
      ("Keybind unregistered: " + identifier).c_str());
  }
}