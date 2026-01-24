#ifndef MACRO_H
#define MACRO_H

#include "Nexus.h"
#include <vector>
#include <string>

enum class EInputType
{
    GameBind,
    MouseButton,
    MouseMove
};

enum class EMouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    X1 = 3,
    X2 = 4
};

enum class EPositionType
{
    Absolute,
    Relative
};

struct MousePosition
{
    int x;
    int y;
    EPositionType positionType;

    MousePosition() : x(0), y(0), positionType(EPositionType::Absolute) {}
    MousePosition(int _x, int _y, EPositionType type = EPositionType::Absolute)
        : x(_x), y(_y), positionType(type) {}
};

struct KeyAction
{
    EInputType inputType;
    EGameBinds gameBind;
    EMouseButton mouseButton;
    MousePosition mousePosition;
    bool isKeyDown;
    bool moveBeforeClick;
    int delayMs;

    KeyAction(EGameBinds bind, bool down, int delay = 0)
        : inputType(EInputType::GameBind), gameBind(bind), 
          mouseButton(EMouseButton::Left), mousePosition(),
          isKeyDown(down), moveBeforeClick(false), delayMs(delay) {}

    KeyAction(EMouseButton button, bool down, int delay = 0)
        : inputType(EInputType::MouseButton), gameBind(GB_SkillWeapon1),
          mouseButton(button), mousePosition(),
          isKeyDown(down), moveBeforeClick(false), delayMs(delay) {}

    KeyAction(EMouseButton button, bool down, MousePosition pos, int delay = 0)
        : inputType(EInputType::MouseButton), gameBind(GB_SkillWeapon1),
          mouseButton(button), mousePosition(pos),
          isKeyDown(down), moveBeforeClick(true), delayMs(delay) {}

    KeyAction(MousePosition pos, int delay = 0)
        : inputType(EInputType::MouseMove), gameBind(GB_SkillWeapon1),
          mouseButton(EMouseButton::Left), mousePosition(pos),
          isKeyDown(false), moveBeforeClick(false), delayMs(delay) {}
};

struct Macro
{
    std::string name;
    std::string identifier;
    bool enabled;
    std::vector<KeyAction> actions;

    Macro(std::string n, std::string id)
        : name(std::move(n)), identifier(std::move(id)), enabled(true) {}
};

#endif