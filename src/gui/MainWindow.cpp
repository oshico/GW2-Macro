#include "MainWindow.h"
#include "../macro/Macro.h"
#include "../macro/MacroManager.h"
#include "../compliance/GameModeCheck.h"
#include "../core/Context.h"
#include <imgui.h>
#include <cstring>

#include "Settings.h"

extern Context g_context;

void RenderMainWindow() {
    if (!g_context.showMainWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(650, 420), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Manager", &g_context.showMainWindow)) {
        ImGui::BeginChild("StatusBanner", ImVec2(0, 40), true);
        {
            ImVec4 statusColor;
            const char *statusText;

            if (IsInCompetitiveMode()) {
                statusColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                statusText = "COMPETITIVE MODE (PvP/WvW) - MACROS DISABLED";
            } else {
                statusColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
                statusText = "PvE MODE - MACROS ENABLED";
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
            ImGui::TextColored(statusColor, "%s", statusText);
        }
        ImGui::EndChild();

        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Macro Slots (10 total)");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                           "Assign keybinds through Nexus settings");

        ImGui::Spacing();

        if (ImGui::BeginChild("MacroList", ImVec2(0, 250), true)) {
            if (ImGui::BeginTable("MacrosTable", 5,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableSetupColumn("On", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < g_context.macros.size(); ++i) {
                    Macro &macro = g_context.macros[i];
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    bool enabled = macro.enabled;
                    if (ImGui::Checkbox(("##Enabled" + std::to_string(i)).c_str(), &enabled)) {
                        macro.enabled = enabled;
                        SaveMacrosToJson();
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(macro.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d actions", static_cast<int>(macro.actions.size()));

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::SmallButton(("Edit##" + std::to_string(i)).c_str()))
                        OpenMacroEditor(static_cast<int>(i));

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::SmallButton(("Delete##" + std::to_string(i)).c_str())) {
                        DeleteMacro(i);
                        --i;
                        SaveMacrosToJson();
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();

        ImGui::Spacing();

        if (ImGui::Button("Create New Macro", ImVec2(-1, 0)))
            OpenMacroEditor();

        if (ImGui::Button("Import / Export", ImVec2(-1, 0)))
            g_context.showImportExport = true;
    }
    ImGui::End();
}