#pragma once

#include "nexus/Nexus.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

enum class EMacroInputType {
    GameBind,
    MouseButton,
    MouseMove
};

enum class EMouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    X1 = 3,
    X2 = 4,
};

enum class EMousePositionType {
    Absolute,
    Relative
};

struct EMousePosition {
    int x;
    int y;
    EMousePositionType MousePositionType;

    EMousePosition() : x(0), y(0), MousePositionType(EMousePositionType::Absolute) {}

    EMousePosition(const int _x, const int _y, const EMousePositionType type = EMousePositionType::Absolute) : x(_x), y(_y), MousePositionType(type) {}
};

struct KeybindAction {
    EMacroInputType MacroInputType;
    EGameBinds GameBind;
    EMouseButton MouseButton;
    EMousePosition MousePosition;
    bool IsKeybindDown;
    bool MoveBeforeMouseClick;
    int DelayMilliseconds;

    KeybindAction(const EGameBinds bind, const bool down, const int delay = 0) : MacroInputType(EMacroInputType::GameBind), GameBind(bind), MouseButton(EMouseButton::Left), IsKeybindDown(down), MoveBeforeMouseClick(false), DelayMilliseconds(delay) {}

    KeybindAction(const EMouseButton button, const bool down, const int delay = 0) : MacroInputType(EMacroInputType::MouseButton), GameBind(GB_SkillWeapon1), MouseButton(button), IsKeybindDown(down), MoveBeforeMouseClick(false), DelayMilliseconds(delay) {}

    KeybindAction(const EMouseButton button, const bool down, const EMousePosition pos, const int delay = 0) : MacroInputType(EMacroInputType::MouseButton), GameBind(GB_SkillWeapon1), MouseButton(button), MousePosition(pos), IsKeybindDown(down), MoveBeforeMouseClick(true), DelayMilliseconds(delay) {}

    explicit KeybindAction(const EMousePosition pos, const int delay = 0) : MacroInputType(EMacroInputType::MouseMove), GameBind(GB_SkillWeapon1), MouseButton(EMouseButton::Left), MousePosition(pos), IsKeybindDown(false), MoveBeforeMouseClick(false), DelayMilliseconds(delay) {}
};

struct Macro {
    std::string Name;
    std::string Identifier;
    bool Enabled;
    std::vector<KeybindAction> Actions;

    Macro(std::string n, std::string id) : Name(std::move(n)), Identifier(std::move(id)), Enabled(true) {}
};

nlohmann::json MacroToJson(const Macro &Macro, int Slot);

Macro JsonToMacro(const nlohmann::json &Json, int Slot);
