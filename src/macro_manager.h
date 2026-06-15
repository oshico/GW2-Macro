#pragma once

#include "macro.h"
#include <string>

void DeleteMacro(size_t Index);

void SaveMacro(const std::string &Name, int Slot, const std::vector<KeybindAction> &Actions);

void OpenMacroEditor(int Index = -1);

bool ImportMacroFromJson(const std::string &JsonString, int Slot);