#include "Macro.h"
#include "StringConversions.h"
#include <stdexcept>

nlohmann::json MacroToJson(const Macro& macro, const int slot)
{
  nlohmann::json macroObj;
  const std::string identifier = "MACRO_" + std::to_string(slot + 1);

  macroObj["name"] = macro.name;
  macroObj["identifier"] = identifier;
  macroObj["enabled"] = macro.enabled;

  nlohmann::json actionsArray = nlohmann::json::array();
  for (const auto& action : macro.actions)
  {
    nlohmann::json actionObj;

    if (action.inputType == EInputType::GameBind)
    {
      actionObj["inputType"] = "GameBind";
      actionObj["gameBind"] = GameBindToString(action.gameBind);
      actionObj["isKeyDown"] = action.isKeyDown;
    }
    else if (action.inputType == EInputType::MouseButton)
    {
      actionObj["inputType"] = "MouseButton";
      actionObj["mouseButton"] = MouseButtonToString(action.mouseButton);
      actionObj["isKeyDown"] = action.isKeyDown;
      actionObj["moveBeforeClick"] = action.moveBeforeClick;

      if (action.moveBeforeClick)
      {
        actionObj["mouseX"] = action.mousePosition.x;
        actionObj["mouseY"] = action.mousePosition.y;
        actionObj["positionType"] = PositionTypeToString(action.mousePosition.positionType);
      }
    }
    else if (action.inputType == EInputType::MouseMove)
    {
      actionObj["inputType"] = "MouseMove";
      actionObj["mouseX"] = action.mousePosition.x;
      actionObj["mouseY"] = action.mousePosition.y;
      actionObj["positionType"] = PositionTypeToString(action.mousePosition.positionType);
    }

    actionObj["delayMs"] = action.delayMs;
    actionsArray.push_back(actionObj);
  }

  macroObj["actions"] = actionsArray;
  return macroObj;
}

Macro JsonToMacro(const nlohmann::json& j, const int slot)
{
  if (!j.is_object() || !j.contains("name") || !j.contains("actions"))
    throw std::invalid_argument("Invalid macro JSON");

  const auto name = j["name"].get<std::string>();
  if (name.empty() || name.length() > 128)
    throw std::invalid_argument("Invalid macro name");

  if (!j["actions"].is_array())
    throw std::invalid_argument("Actions must be an array");

  Macro newMacro(name, "MACRO_" + std::to_string(slot + 1));
  newMacro.enabled = j.value("enabled", false);

  for (const auto& actionObj : j["actions"])
  {
    if (!actionObj.is_object() || !actionObj.contains("inputType"))
      throw std::invalid_argument("Invalid action in macro");

    std::string inputTypeStr = actionObj["inputType"];
    int delayMs = actionObj.value("delayMs", 0);

    if (inputTypeStr == "GameBind")
    {
      if (!actionObj.contains("gameBind") || !actionObj.contains("isKeyDown"))
        throw std::invalid_argument("GameBind action missing required fields");
      EGameBinds gameBind = StringToGameBind(actionObj["gameBind"].get<std::string>());
      bool isKeyDown = actionObj["isKeyDown"].get<bool>();
      newMacro.actions.emplace_back(gameBind, isKeyDown, delayMs);
    }
    else if (inputTypeStr == "MouseButton")
    {
      if (!actionObj.contains("mouseButton") || !actionObj.contains("isKeyDown"))
        throw std::invalid_argument("MouseButton action missing required fields");
      EMouseButton mouseButton = StringToMouseButton(actionObj["mouseButton"].get<std::string>());
      bool isKeyDown = actionObj["isKeyDown"].get<bool>();

      if (const bool moveBeforeClick = actionObj.value("moveBeforeClick", false); moveBeforeClick && actionObj.
        contains("mouseX") && actionObj.contains("mouseY"))
      {
        const int mouseX = actionObj["mouseX"];
        const int mouseY = actionObj["mouseY"];
        std::string posTypeStr = actionObj.value("positionType", "Absolute");
        const EPositionType posType = StringToPositionType(posTypeStr);
        MousePosition pos(mouseX, mouseY, posType);
        newMacro.actions.emplace_back(mouseButton, isKeyDown, pos, delayMs);
      }
      else
      {
        newMacro.actions.emplace_back(mouseButton, isKeyDown, delayMs);
      }
    }
    else if (inputTypeStr == "MouseMove")
    {
      if (!actionObj.contains("mouseX") || !actionObj.contains("mouseY"))
        throw std::invalid_argument("MouseMove action missing coordinates");
      const int mouseX = actionObj["mouseX"];
      const int mouseY = actionObj["mouseY"];
      std::string posTypeStr = actionObj.value("positionType", "Absolute");
      const EPositionType posType = StringToPositionType(posTypeStr);
      MousePosition pos(mouseX, mouseY, posType);
      newMacro.actions.emplace_back(pos, delayMs);
    }
    else
    {
      throw std::invalid_argument("Unknown input type in macro");
    }
  }

  return newMacro;
}