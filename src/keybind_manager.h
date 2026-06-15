#pragma once

#include "macro.h"
#include <string>

void ProcessKeybind(const char *ActionIdentifier, bool ActionIsRelease);

void SetupKeybinds();

void RegisterKeybind(const Macro &Macro);

void UnregisterKeybind(const std::string &Identifier);