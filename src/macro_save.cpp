#include "macro.h"
#include "shared.h"
#include <filesystem>
#include <fstream>

bool SaveMacrosToJson() {
    const std::string AddonConfigurationPath = ApiDefinition->Paths_GetAddonDirectory("MacroManager/macros.json");
    try {
        if (AddonConfigurationPath.empty()) {
            if (ApiDefinition)
                ApiDefinition->Log(LOGL_WARNING, "MacroManager", "No configuration file path available");
            return false;
        }

        std::filesystem::create_directories(std::filesystem::path(AddonConfigurationPath).parent_path());

        nlohmann::json Json;
        Json["version"] = "3.0.0";

        nlohmann::json MacrosArray = nlohmann::json::array();
        for (size_t i = 0; i < Macros.size(); ++i) {
            MacrosArray.push_back(
                MacroToJson(Macros[i], static_cast<int>(i)));
        }
        Json["macros"] = MacrosArray;

        std::ofstream MacrosJsonSaveFile(AddonConfigurationPath);
        MacrosJsonSaveFile << Json.dump(2);
        return true;
    } catch (const std::exception &e) {
        if (ApiDefinition)
            ApiDefinition->Log(LOGL_WARNING, "MacroManager", ("Failed to save macros: " + std::string(e.what())).c_str());
        return false;
    }
}

bool LoadMacrosFromJson() {
    std::string AddonConfigurationPath = ApiDefinition->Paths_GetAddonDirectory("MacroManager/macros.json");
    try {
        if (AddonConfigurationPath.empty()) {
            ApiDefinition->Log(LOGL_INFO, "MacroManager", "No config file path available, using defaults");
            return false;
        }

        std::ifstream MacrosJsonSaveFile(AddonConfigurationPath);
        if (!MacrosJsonSaveFile.is_open()) {
            ApiDefinition->Log(LOGL_INFO, "MacroManager", "Config file not found, using defaults");
            return false;
        }

        nlohmann::json Json;
        MacrosJsonSaveFile >> Json;

        if (Json.contains("macros") && Json["macros"].is_array()) {
            size_t Index = 0;
            for (const auto &MacroObject : Json["macros"]) {
                if (Index >= Macros.size())
                    break;

                Macros[Index] = JsonToMacro(MacroObject, static_cast<int>(Index));
                Index++;
            }
        }
        return true;
    } catch (const std::exception &e) {
        if (ApiDefinition)
            ApiDefinition->Log(LOGL_WARNING, "MacroManager", ("Failed to load macros: " + std::string(e.what())).c_str());

        for (size_t i = 0; i < Macros.size(); ++i) {
            Macros[i].Name = "Empty";
            Macros[i].Identifier = "MACRO_" + std::to_string(i + 1);
            Macros[i].Enabled = false;
            Macros[i].Actions.clear();
        }

        return false;
    }
}