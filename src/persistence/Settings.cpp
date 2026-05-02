#include "Settings.h"
#include "../core/Context.h"
#include "../macro/Macro.h"
#include <filesystem>
#include <fstream>

extern Context g_context;

bool SaveMacrosToJson() {
  std::string configPath =
      g_context.apiDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
  try {
    if (configPath.empty()) {
      if (g_context.apiDefs)
        g_context.apiDefs->Log(LOGL_WARNING, "MacroManager",
                               "No config file path available");
      return false;
    }

    std::filesystem::create_directories(
        std::filesystem::path(configPath).parent_path());

    nlohmann::json j;
    j["version"] = "0.3.0";

    nlohmann::json macrosArray = nlohmann::json::array();
    for (size_t i = 0; i < g_context.macros.size(); ++i) {
      macrosArray.push_back(
          MacroToJson(g_context.macros[i], static_cast<int>(i)));
    }
    j["macros"] = macrosArray;

    std::ofstream file(configPath);
    file << j.dump(2);
    return true;
  } catch (const std::exception &e) {
    if (g_context.apiDefs)
      g_context.apiDefs->Log(
          LOGL_WARNING, "MacroManager",
          ("Failed to save macros: " + std::string(e.what())).c_str());
    return false;
  }
}

bool LoadMacrosFromJson() {
  std::string configPath =
      g_context.apiDefs->Paths_GetAddonDirectory("MacroManager/macros.json");
  try {
    if (configPath.empty()) {
      g_context.apiDefs->Log(LOGL_INFO, "MacroManager",
                             "No config file path available, using defaults");
      return false;
    }

    std::ifstream file(configPath);
    if (!file.is_open()) {
      g_context.apiDefs->Log(LOGL_INFO, "MacroManager",
                             "Config file not found, using defaults");
      return false;
    }

    nlohmann::json j;
    file >> j;

    if (j.contains("macros") && j["macros"].is_array()) {
      size_t index = 0;
      for (const auto &macroObj : j["macros"]) {
        if (index >= g_context.macros.size())
          break;

        g_context.macros[index] =
            JsonToMacro(macroObj, static_cast<int>(index));
        index++;
      }
    }
    return true;
  } catch (const std::exception &e) {
    if (g_context.apiDefs)
      g_context.apiDefs->Log(
          LOGL_WARNING, "MacroManager",
          ("Failed to load macros: " + std::string(e.what())).c_str());

    for (size_t i = 0; i < g_context.macros.size(); ++i) {
      g_context.macros[i].name = "Empty";
      g_context.macros[i].identifier = "MACRO_" + std::to_string(i + 1);
      g_context.macros[i].enabled = false;
      g_context.macros[i].actions.clear();
    }

    return false;
  }
}
