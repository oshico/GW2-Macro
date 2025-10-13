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
 *
 * @var KeyAction::gameBind
 * The game bind enumeration representing the in-game action
 *
 * @var KeyAction::isKeyDown
 * True for key press, false for key release
 *
 * @var KeyAction::delayMs
 * Delay in milliseconds before executing this action
 */
struct KeyAction
{
    EGameBinds gameBind; ///< Game bind enumeration for the action
    bool isKeyDown;      ///< True for press action, false for release
    int delayMs;         ///< Delay in milliseconds before this action

    /**
     * @brief Construct a new KeyAction object
     *
     * @param bind The game bind to execute
     * @param down True for press, false for release
     * @param delay Delay in milliseconds (default: 0)
     */
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
 *
 * @var Macro::name
 * User-friendly display name for the macro
 *
 * @var Macro::identifier
 * Unique string identifier used for keybind registration
 *
 * @var Macro::enabled
 * Controls whether the macro can be executed
 *
 * @var Macro::actions
 * Sequence of KeyActions to execute when macro is triggered
 */
struct Macro
{
    std::string name;               ///< User-friendly display name
    std::string identifier;         ///< Unique keybind identifier
    bool enabled;                   ///< Execution enabled state
    std::vector<KeyAction> actions; ///< Sequence of key actions

    /**
     * @brief Construct a new Macro object
     *
     * @param n Display name for the macro
     * @param id Unique identifier for keybind registration
     */
    Macro(std::string n, std::string id)
        : name(std::move(n)), identifier(std::move(id)), enabled(true) {}
};

#endif