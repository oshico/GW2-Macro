#include "macro.h"
#include "string_conversions.h"
#include <stdexcept>

nlohmann::json MacroToJson(const Macro &Macro, const int Slot) {
    nlohmann::json MacroObject;
    const std::string Identifier = "MACRO_" + std::to_string(Slot + 1);

    MacroObject["name"] = Macro.Name;
    MacroObject["identifier"] = Identifier;
    MacroObject["enabled"] = Macro.Enabled;

    nlohmann::json ActionsArray = nlohmann::json::array();
    for (const auto &Action : Macro.Actions) {
        nlohmann::json ActionObject;

        if (Action.MacroInputType == EMacroInputType::GameBind) {
            ActionObject["inputType"] = "GameBind";
            ActionObject["gameBind"] = IngameKeybindToString(Action.GameBind);
            ActionObject["isKeyDown"] = Action.IsKeybindDown;
        } else if (Action.MacroInputType == EMacroInputType::MouseButton) {
            ActionObject["inputType"] = "MouseButton";
            ActionObject["mouseButton"] = MouseButtonToString(Action.MouseButton);
            ActionObject["isKeyDown"] = Action.IsKeybindDown;
            ActionObject["moveBeforeClick"] = Action.MoveBeforeMouseClick;

            if (Action.MoveBeforeMouseClick) {
                ActionObject["mouseX"] = Action.MousePosition.x;
                ActionObject["mouseY"] = Action.MousePosition.y;
                ActionObject["poGameBindsitionType"] = MousePositionTypeToString(Action.MousePosition.MousePositionType);
            }
        } else if (Action.MacroInputType == EMacroInputType::MouseMove) {
            ActionObject["inputType"] = "MouseMove";
            ActionObject["mouseX"] = Action.MousePosition.x;
            ActionObject["mouseY"] = Action.MousePosition.y;
            ActionObject["positionType"] = MousePositionTypeToString(Action.MousePosition.MousePositionType);
        }

        ActionObject["delayMs"] = Action.DelayMilliseconds;
        ActionsArray.push_back(ActionObject);
    }

    MacroObject["actions"] = ActionsArray;
    return MacroObject;
}

Macro JsonToMacro(const nlohmann::json &Json, const int Slot) {
    if (!Json.is_object() || !Json.contains("name") || !Json.contains("actions"))
        throw std::invalid_argument("Invalid macro JSON");

    const auto Name = Json["name"].get<std::string>();
    if (Name.empty() || Name.length() > 128)
        throw std::invalid_argument("Invalid macro name");

    if (!Json["actions"].is_array())
        throw std::invalid_argument("Actions must be an array");

    Macro NewMacro(Name, "MACRO_" + std::to_string(Slot + 1));
    NewMacro.Enabled = Json.value("enabled", false);

    for (const auto &ActionObject : Json["actions"]) {
        if (!ActionObject.is_object() || !ActionObject.contains("inputType"))
            throw std::invalid_argument("Invalid action in macro");

        std::string InputTypeString = ActionObject["inputType"];
        int DelayMilliseconds = ActionObject.value("delayMs", 0);

        if (InputTypeString == "GameBind") {
            if (!ActionObject.contains("gameBind") || !ActionObject.contains("isKeyDown"))
                throw std::invalid_argument("GameBind action missing required fields");
            EGameBinds GameBind = StringToIngameKeybind(ActionObject["gameBind"].get<std::string>());
            bool IsKeybindDown = ActionObject["isKeyDown"].get<bool>();
            NewMacro.Actions.emplace_back(GameBind, IsKeybindDown, DelayMilliseconds);
        } else if (InputTypeString == "MouseButton") {
            if (!ActionObject.contains("mouseButton") || !ActionObject.contains("isKeyDown"))
                throw std::invalid_argument("MouseButton action missing required fields");
            EMouseButton MouseButton = StringToMouseButton(ActionObject["mouseButton"].get<std::string>());
            bool IsKeybindDown = ActionObject["isKeyDown"].get<bool>();

            if (const bool MoveBeforeClick = ActionObject.value("moveBeforeClick", false); MoveBeforeClick && ActionObject.contains("mouseX") && ActionObject.contains("mouseY")) {
                const int MouseX = ActionObject["mouseX"];
                const int MouseY = ActionObject["mouseY"];
                std::string PositionTypeString = ActionObject.value("positionType", "Absolute");
                const EMousePositionType PositionType = StringToMousePositionType(PositionTypeString);
                EMousePosition Position(MouseX, MouseY, PositionType);
                NewMacro.Actions.emplace_back(MouseButton, IsKeybindDown, Position, DelayMilliseconds);
            } else {
                NewMacro.Actions.emplace_back(MouseButton, IsKeybindDown, DelayMilliseconds);
            }
        } else if (InputTypeString == "MouseMove") {
            if (!ActionObject.contains("mouseX") || !ActionObject.contains("mouseY"))
                throw std::invalid_argument("MouseMove action missing coordinates");
            const int MouseX = ActionObject["mouseX"];
            const int MouseY = ActionObject["mouseY"];
            std::string PositionTypeString = ActionObject.value("positionType", "Absolute");
            const EMousePositionType PositionType = StringToMousePositionType(PositionTypeString);
            EMousePosition Position(MouseX, MouseY, PositionType);
            NewMacro.Actions.emplace_back(Position, DelayMilliseconds);
        } else {
            throw std::invalid_argument("Unknown input type in macro");
        }
    }

    return NewMacro;
}