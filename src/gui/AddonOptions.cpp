#include "AddonOptions.h"
#include "../macro/Macro.h"
#include "../macro/MacroManager.h"
#include "../core/Shared.h"
#include <imgui.h>

void AddonOptions()
{
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(APIDefs->ImguiContext));

    ImGui::Text("Macro Keybind Manager v0.3.0");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                       "Execute sequences of game actions with precise timing control");

    ImGui::Spacing();

    ImGui::Text("Default Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");
    ImGui::BulletText("CTRL+SHIFT+X - Kill All Running Macros");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                       "Assign macro keybinds through Nexus Input settings");

    ImGui::Spacing();

    {
        int activeMacros = 0;
        int totalActions = 0;

        for (const auto &macro : g_macros)
        {
            if (!macro.actions.empty() && macro.name != "Empty")
                ++activeMacros;
            totalActions += static_cast<int>(macro.actions.size());
        }

        ImGui::Text("Active Macros: %d / 10", activeMacros);
        ImGui::Text("Total Actions: %d", totalActions);
    }

    ImGui::Spacing();

    if (ImGui::Button("Open Macro Manager"))
        g_showMainWindow = true;

    ImGui::SameLine();
    if (ImGui::Button("Create New Macro"))
        OpenMacroEditor();

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Usage Tips"))
    {
        ImGui::BulletText("Each macro can contain unlimited actions");
        ImGui::BulletText("Use delays between actions for timing-sensitive sequences");
        ImGui::BulletText("Press and Release actions allow for held key combinations");
        ImGui::BulletText("Mouse position support for clicking at specific coordinates");
        ImGui::BulletText("Disable macros to prevent accidental execution");
    }

    if (ImGui::CollapsingHeader("Macro Usage Policy"))
    {
        ImGui::TextWrapped("Attended macro use is permitted as long as it is not exploitative, "
                           "and as long as it does not provide the user with an unfair advantage. "
                           "Unattended macro use is prohibited.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Allowed Macros:");
        ImGui::BulletText("Bind dodge and jump to a single key");
        ImGui::BulletText("Auto-clicker to open or consume items");
        ImGui::BulletText("Music macros for in-game performance (attended)");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Prohibited Macros:");
        ImGui::BulletText("Cannot interfere with other players");
        ImGui::BulletText("Cannot act across multiple accounts");
        ImGui::BulletText("No automation of skills while away");
        ImGui::BulletText("No skill chains in competitive modes");
        ImGui::BulletText("No automation of events (e.g., Bell Choir)");
        ImGui::BulletText("No AFK farming of items/currencies");
        ImGui::BulletText("No full character automation (botting)");
        ImGui::BulletText("Each macro must represent one user action");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Important:");
        ImGui::TextWrapped("This addon automatically disables macros in PvP/WvW "
                           "to comply with policy, but you are still responsible "
                           "for following the game's ToS.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Warning:");
        ImGui::TextWrapped("Violation of these policies may result in account restrictions.");
    }
}