#pragma once

#include "Macro.h"
#include "MacroExecutor.h"
#include <string>

void ProcessKeybind(const char *aIdentifier, bool aIsRelease);

void SetupKeybinds();

void RegisterKeybind(const Macro &macro);

void UnregisterKeybind(const std::string &identifier);
