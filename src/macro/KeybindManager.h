#ifndef KEYBIND_MANAGER_H
#define KEYBIND_MANAGER_H

#include "Macro.h"

extern std::vector<Macro> g_macros;

void ProcessKeybind(const char *aIdentifier, bool aIsRelease);

void SetupKeybinds();

void RegisterKeybind(const Macro &macro);

void UnregisterKeybind(const std::string &identifier);

#endif