#pragma once

#include "Macro.h"
#include "../core/Context.h"
#include <string>

void DeleteMacro(size_t index);

void SaveMacro(const std::string &name, int slot, const std::vector<KeyAction> &actions);

void OpenMacroEditor(int index = -1);

bool ImportMacroFromJson(const std::string& jsonStr, int slot);
