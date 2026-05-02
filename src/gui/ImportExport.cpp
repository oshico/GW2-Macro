#include "ImportExport.h"
#include "../macro/Macro.h"
#include "../macro/MacroManager.h"
#include "../macro/Macro.h"
#include "../utils/StringConversions.h"
#include "../core/Context.h"
#include "../persistence/Settings.h"
#include "../nlohmann/json.hpp"
#include <imgui.h>
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <fstream>

extern Context g_context;

static char g_importJsonBuffer[8192] = "";
static char g_exportJsonBuffer[8192] = "";
static int g_exportSlot = 0;
static int g_importSlot = 0;
static int g_lastExportSlot = -1;
static char g_statusMsg[256] = "";
static float g_statusMsgTime = 0.0f;

static void ShowStatus(const char* msg) {
    strncpy_s(g_statusMsg, msg, sizeof(g_statusMsg));
    g_statusMsgTime = ImGui::GetTime() + 3.0f;
}

static bool ClipboardSetText(const char* text) {
    if (!OpenClipboard(nullptr))
        return false;

    EmptyClipboard();

    size_t len = strlen(text);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len + 1);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    char* pMem = static_cast<char*>(GlobalLock(hMem));
    if (!pMem) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    strcpy_s(pMem, len + 1, text);
    GlobalUnlock(hMem);

    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    return true;
}

static std::string SaveFileDialog() {
    char filePath[MAX_PATH] = "";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "json";

    if (GetSaveFileName(&ofn))
        return std::string(filePath);

    return "";
}

static std::string OpenFileDialog() {
    char filePath[MAX_PATH] = "";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
        return std::string(filePath);

    return "";
}

static std::string ReadFileToString(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return "";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

static bool WriteStringToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open())
        return false;
    file << content;
    return true;
}

void RenderImportExport() {
    if (!g_context.showImportExport)
        return;

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Import / Export Macros", &g_context.showImportExport)) {

        if (ImGui::BeginTabBar("ImportExportTabs")) {

            if (ImGui::BeginTabItem("Export")) {
                const char* slotNames[10] = {
                    "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
                    "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"
                };
                if (ImGui::Combo("Select Macro", &g_exportSlot, slotNames, 10) || g_lastExportSlot != g_exportSlot) {
                    g_lastExportSlot = g_exportSlot;
                    nlohmann::json j = MacroToJson(g_context.macros[g_exportSlot], g_exportSlot);
                    std::string jsonStr = j.dump(2);
                    strncpy_s(g_exportJsonBuffer, jsonStr.c_str(), sizeof(g_exportJsonBuffer) - 1);
                }

                ImGui::Separator();

                ImGui::Text("Macro JSON:");
                ImGui::InputTextMultiline("##ExportJson",
                    g_exportJsonBuffer, sizeof(g_exportJsonBuffer),
                    ImVec2(-1, 250), ImGuiInputTextFlags_ReadOnly);

                ImGui::Spacing();

                if (ImGui::Button("Copy to Clipboard", ImVec2(150, 0))) {
                    if (ClipboardSetText(g_exportJsonBuffer))
                        ShowStatus("Copied to clipboard!");
                    else
                        ShowStatus("Failed to copy to clipboard!");
                }
                ImGui::SameLine();
                if (ImGui::Button("Save to File", ImVec2(150, 0))) {
                    std::string path = SaveFileDialog();
                    if (!path.empty()) {
                        if (WriteStringToFile(path, g_exportJsonBuffer))
                            ShowStatus(("Saved to: " + path.substr(path.find_last_of("/\\") + 1)).c_str());
                        else
                            ShowStatus("Failed to save file!");
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Import")) {
                const char* slotNames[10] = {
                    "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
                    "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"
                };
                ImGui::Combo("Import to Slot", &g_importSlot, slotNames, 10);

                ImGui::Separator();

                ImGui::Text("Paste Macro JSON:");
                ImGui::InputTextMultiline("##ImportJson", g_importJsonBuffer, sizeof(g_importJsonBuffer), ImVec2(-1, 250));

                ImGui::Spacing();

                if (ImGui::Button("Load from File", ImVec2(150, 0))) {
                    std::string path = OpenFileDialog();
                    if (!path.empty()) {
                        std::string content = ReadFileToString(path);
                        if (!content.empty()) {
                            strncpy_s(g_importJsonBuffer, content.c_str(), sizeof(g_importJsonBuffer));
                            ShowStatus(("Loaded: " + path.substr(path.find_last_of("/\\") + 1)).c_str());
                        } else {
                            ShowStatus("Failed to read file!");
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Import Macro", ImVec2(150, 0))) {
                    if (ImportMacroFromJson(g_importJsonBuffer, g_importSlot))
                        ShowStatus("Macro imported successfully!");
                    else
                        ShowStatus("Failed to import: invalid JSON!");
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if (g_statusMsg[0] && ImGui::GetTime() < g_statusMsgTime) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", g_statusMsg);
        } else if (ImGui::GetTime() >= g_statusMsgTime) {
            g_statusMsg[0] = '\0';
        }
    }
    ImGui::End();
}
