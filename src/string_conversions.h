#pragma once

#include "macro.h"
#include "nexus/Nexus.h"
#include <string>

extern const char *GetKeybindName(EGameBinds Keybind);

extern std::string IngameKeybindToString(EGameBinds Keybind);

extern EGameBinds StringToIngameKeybind(const std::string &KeybindString);

extern const char *GetMouseButtonName(EMouseButton MouseButton);

extern std::string MouseButtonToString(EMouseButton MouseButton);

extern EMouseButton StringToMouseButton(const std::string &MouseButtonString);

extern std::string MousePositionTypeToString(EMousePositionType MousePositionType);

extern EMousePositionType StringToMousePositionType(const std::string &MousePositionTypeString);