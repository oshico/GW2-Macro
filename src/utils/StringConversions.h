#pragma once

#include "../macro/Macro.h"

const char* GetBindName(EGameBinds bind);

std::string GameBindToString(EGameBinds bind);

EGameBinds StringToGameBind(const std::string& bindStr);

const char* GetMouseButtonName(EMouseButton button);

std::string MouseButtonToString(EMouseButton button);

EMouseButton StringToMouseButton(const std::string& buttonStr);

std::string PositionTypeToString(EPositionType type);

EPositionType StringToPositionType(const std::string& typeStr);