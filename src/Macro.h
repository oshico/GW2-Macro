#ifndef MACRO_H
#define MACRO_H

#include "Nexus.h"
#include <vector>
#include <string>

/**
 * @struct KeyAction
 * @brief Represents a single key action within a macro sequence
 *
 * Each KeyAction defines a game bind to press or release, with an optional
 * delay before execution. This allows for complex timing-based macro sequences.
 */
struct KeyAction
{
    EGameBinds gameBind;
    bool isKeyDown;
    int delayMs;

    KeyAction(EGameBinds bind, bool down, int delay = 0)
        : gameBind(bind), isKeyDown(down), delayMs(delay) {}
};

/**
 * @struct Macro
 * @brief Represents a complete macro with name, identifier, and action sequence
 *
 * Macros are stored in predefined slots (MACRO_1 through MACRO_10) and can be
 * enabled/disabled. Each macro contains a sequence of KeyActions that are
 * executed in order when the macro is triggered.
 */
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