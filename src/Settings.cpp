#include "Settings.h"
#include "Shared.h"
#include "MacroManager.h"
#include "json.hpp"
#include <fstream>
#include <filesystem>

// =============================================================================
// JSON SAVE FUNCTIONALITY
// =============================================================================
bool SaveMacrosToJson()
{
    std::string addonPath = APIDefs->Paths_GetAddonDirectory("MacroManager");
    std::string configPath = APIDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
    try
    {
        if (configPath.empty())
        {
            if (APIDefs)
                APIDefs->Log(LOGL_WARNING, "MacroManager", "No config file path available");
            return false;
        }

        std::filesystem::create_directories(std::filesystem::path(configPath).parent_path());

        nlohmann::json j;
        j["version"] = "0.3.0";

        nlohmann::json macrosArray = nlohmann::json::array();
        for (const auto &macro : g_macros)
        {
            nlohmann::json macroObj;
            macroObj["name"] = macro.name;
            macroObj["identifier"] = macro.identifier;
            macroObj["enabled"] = macro.enabled;
            
            nlohmann::json actionsArray = nlohmann::json::array();
            for (const auto &action : macro.actions)
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
            macrosArray.push_back(macroObj);
        }
        j["macros"] = macrosArray;

        std::ofstream file(configPath);
        file << j.dump(2);
        return true;
    }
    catch (const std::exception &e)
    {
        if (APIDefs)
            APIDefs->Log(LOGL_WARNING, "MacroManager",
                         ("Failed to save macros: " + std::string(e.what())).c_str());
        return false;
    }
}

// =============================================================================
// JSON LOAD FUNCTIONALITY
// =============================================================================

bool LoadMacrosFromJson()
{
    std::string addonPath = APIDefs->Paths_GetAddonDirectory("MacroManager");
    std::string configPath = APIDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
    try
    {
        if (configPath.empty())
        {
            APIDefs->Log(LOGL_INFO, "MacroManager", "No config file path available, using defaults");
            return false;
        }

        std::ifstream file(configPath);
        if (!file.is_open())
        {
            APIDefs->Log(LOGL_INFO, "MacroManager", "Config file not found, using defaults");
            return false;
        }

        nlohmann::json j;
        file >> j;

        if (j.contains("macros") && j["macros"].is_array())
        {
            size_t index = 0;
            for (const auto &macroObj : j["macros"])
            {
                if (index >= g_macros.size())
                    break;

                g_macros[index].name = macroObj.value("name", "Empty");
                g_macros[index].enabled = macroObj.value("enabled", false);

                if (macroObj.contains("actions") && macroObj["actions"].is_array())
                {
                    for (const auto &actionObj : macroObj["actions"])
                    {
                        if (!actionObj.is_object() || !actionObj.contains("inputType"))
                        {
                            APIDefs->Log(LOGL_WARNING, "MacroManager", "Skipping invalid action in macro");
                            continue;
                        }

                        std::string inputTypeStr = actionObj["inputType"];
                        int delayMs = actionObj.value("delayMs", 0);

                        if (inputTypeStr == "GameBind")
                        {
                            if (actionObj.contains("gameBind") && actionObj.contains("isKeyDown"))
                            {
                                std::string bindStr = actionObj["gameBind"];
                                bool isKeyDown = actionObj["isKeyDown"];
                                EGameBinds gameBind = StringToGameBind(bindStr);
                                g_macros[index].actions.emplace_back(gameBind, isKeyDown, delayMs);
                            }
                        }
                        else if (inputTypeStr == "MouseButton")
                        {
                            if (actionObj.contains("mouseButton") && actionObj.contains("isKeyDown"))
                            {
                                std::string buttonStr = actionObj["mouseButton"];
                                bool isKeyDown = actionObj["isKeyDown"];
                                bool moveBeforeClick = actionObj.value("moveBeforeClick", false);
                                EMouseButton mouseButton = StringToMouseButton(buttonStr);
                                
                                if (moveBeforeClick && actionObj.contains("mouseX") && actionObj.contains("mouseY"))
                                {
                                    int mouseX = actionObj["mouseX"];
                                    int mouseY = actionObj["mouseY"];
                                    std::string posTypeStr = actionObj.value("positionType", "Absolute");
                                    EPositionType posType = StringToPositionType(posTypeStr);
                                    
                                    MousePosition pos(mouseX, mouseY, posType);
                                    g_macros[index].actions.emplace_back(mouseButton, isKeyDown, pos, delayMs);
                                }
                                else
                                {
                                    g_macros[index].actions.emplace_back(mouseButton, isKeyDown, delayMs);
                                }
                            }
                        }
                        else if (inputTypeStr == "MouseMove")
                        {
                            if (actionObj.contains("mouseX") && actionObj.contains("mouseY"))
                            {
                                int mouseX = actionObj["mouseX"];
                                int mouseY = actionObj["mouseY"];
                                std::string posTypeStr = actionObj.value("positionType", "Absolute");
                                EPositionType posType = StringToPositionType(posTypeStr);
                                
                                MousePosition pos(mouseX, mouseY, posType);
                                g_macros[index].actions.emplace_back(pos, delayMs);
                            }
                        }
                        else
                        {
                            APIDefs->Log(LOGL_WARNING, "MacroManager", "Unknown input type in macro");
                        }
                    }
                }
                index++;
            }
        }
        return true;
    }
    catch (const std::exception &e)
    {
        if (APIDefs)
            APIDefs->Log(LOGL_WARNING, "MacroManager",
                         ("Failed to load macros: " + std::string(e.what())).c_str());

        for (size_t i = 0; i < g_macros.size(); ++i)
        {
            g_macros[i].name = "Empty";
            g_macros[i].identifier = "MACRO_" + std::to_string(i + 1);
            g_macros[i].enabled = false;
            g_macros[i].actions.clear();
        }

        return false;
    }
}