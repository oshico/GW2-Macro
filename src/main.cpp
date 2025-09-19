#include "Shared.h"
#include "Nexus.h"
#include "MacroManager.h"
#include "GameModeCheck.h"
#include "Settings.h"
#include "imgui.h"
#include "Resource.h"
#include "Module.h"
#include <cstring>

void AddonLoad(AddonAPI_t *aApi);
void AddonUnload();
void AddonRender();
void AddonOptions();

void RenderMainWindow();
void RenderMacroEditor();

// =============================================================================
// ADDON DEFINITION
// =============================================================================

AddonDefinition_t AddonDef{};

extern "C" __declspec(dllexport) AddonDefinition_t *GetAddonDef()
{
    AddonDef.Signature = 0xA1B2C3D4;
    AddonDef.APIVersion = NEXUS_API_VERSION;
    AddonDef.Name = "Macro Manager";
    AddonDef.Version.Major = 0;
    AddonDef.Version.Minor = 1;
    AddonDef.Version.Build = 8;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "A macro keybind manager for executing sequences of game actions with timing control.";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    AddonDef.Provider = EUpdateProvider::UP_GitHub;
    AddonDef.UpdateLink = "https://github.com/oshico/GW2-MACRO-Releases";

    return &AddonDef;
}

// =============================================================================
// ADDON LIFECYCLE
// =============================================================================

void AddonLoad(AddonAPI_t *aApi)
{
    APIDefs = aApi;
    APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v0.1.8 loaded!");
    ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);
    ImGui::SetAllocatorFunctions((void *(*)(size_t, void *))APIDefs->ImguiMalloc, (void (*)(void *, void *))APIDefs->ImguiFree);

    APIDefs->Textures_GetOrCreateFromResource("MACRO_MANAGER_ICON", IDB_MACRO_MANAGER_ICON, GetModuleHandle());

    APIDefs->QuickAccess_Add("MACRO_MANAGER_SHORTCUT", "MACRO_MANAGER_ICON", "MACRO_MANAGER_ICON", "MACRO_SHOW_WINDOW", "Open Macro Manager");

    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);

    LoadMacrosFromJson();

    for (const auto &macro : g_macros)
        UnregisterKeybind(macro.identifier);

    for (auto &macro : g_macros)
        macro.enabled = false;

    SetupKeybinds();
}

void AddonUnload()
{
    if (APIDefs)
    {
        SaveMacrosToJson();

        for (const auto &macro : g_macros)
            UnregisterKeybind(macro.identifier);

        APIDefs->QuickAccess_Remove("MACRO_MANAGER_SHORTCUT");

        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);

        APIDefs->InputBinds_Deregister("MACRO_SHOW_WINDOW");

        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

// =============================================================================
// GUI RENDERING
// =============================================================================

void AddonRender()
{
    ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);
    RenderMainWindow();
    RenderMacroEditor();
}

void RenderMainWindow()
{
    if (!g_showMainWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(650, 420), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Manager", &g_showMainWindow))
    {
        ImGui::BeginChild("StatusBanner", ImVec2(0, 40), true);
        {
            ImVec4 statusColor;
            const char *statusText;

            if (IsInCompetitiveMode())
            {
                statusColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                statusText = "COMPETITIVE MODE (PvP/WvW) - MACROS DISABLED";
            }
            else
            {
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

        if (ImGui::BeginChild("MacroList", ImVec2(0, 250), true))
        {
            if (ImGui::BeginTable("MacrosTable", 5,
                                  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableSetupColumn("On", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < g_macros.size(); ++i)
                {
                    Macro &macro = g_macros[i];
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    bool enabled = macro.enabled;
                    if (ImGui::Checkbox(("##Enabled" + std::to_string(i)).c_str(), &enabled))
                    {
                        macro.enabled = enabled;
                        SaveMacrosToJson();
                    }

                    // Name
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(macro.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d actions", (int)macro.actions.size());

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::SmallButton(("Edit##" + std::to_string(i)).c_str()))
                        OpenMacroEditor((int)i);

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::SmallButton(("Delete##" + std::to_string(i)).c_str()))
                    {
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
    }
    ImGui::End();
}

void RenderMacroEditor()
{
    if (!g_showMacroEditor)
        return;

    static char macroName[128] = "";
    static std::vector<KeyAction> editingActions;
    static int lastSelectedMacroIndex = -2;
    static int selectedSlot = 0;

    if (g_selectedMacroIndex != lastSelectedMacroIndex)
    {
        if (g_selectedMacroIndex >= 0 && g_selectedMacroIndex < (int)g_macros.size())
        {
            const Macro &macro = g_macros[g_selectedMacroIndex];
            strncpy_s(macroName, sizeof(macroName), macro.name.c_str(), _TRUNCATE);
            editingActions = macro.actions;

            std::string id = macro.identifier;
            if (id.find("MACRO_") != std::string::npos)
                selectedSlot = std::stoi(id.substr(6)) - 1;
        }
        else
        {
            strcpy_s(macroName, sizeof(macroName), "New Macro");
            editingActions.clear();
            selectedSlot = 0;
        }
        lastSelectedMacroIndex = g_selectedMacroIndex;
    }

    ImGui::SetNextWindowSize(ImVec2(720, 520), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Editor", &g_showMacroEditor))
    {
        ImGui::InputText("Macro Name", macroName, sizeof(macroName));

        ImGui::Separator();
        const char *slotNames[10] = {
            "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
            "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
        ImGui::Combo("Macro Slot", &selectedSlot, slotNames, 10);

        ImGui::Separator();
        ImGui::Text("Action Sequence:");
        if (ImGui::BeginChild("ActionList", ImVec2(0, 220), true))
        {
            for (size_t i = 0; i < editingActions.size(); ++i)
            {
                ImGui::PushID((int)i);

                ImGui::Text("%d.", (int)i + 1);
                ImGui::SameLine();

                ImGui::TextColored(editingActions[i].isKeyDown
                                       ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                                       : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                                   "%s", editingActions[i].isKeyDown ? "PRESS" : "RELEASE");

                ImGui::SameLine();
                ImGui::TextUnformatted(GetBindName(editingActions[i].gameBind));

                if (editingActions[i].delayMs > 0)
                {
                    ImGui::SameLine();
                    ImGui::Text("(%dms delay)", editingActions[i].delayMs);
                }

                ImGui::SameLine(ImGui::GetWindowWidth() - 60);
                if (ImGui::SmallButton("X"))
                {
                    editingActions.erase(editingActions.begin() + i);
                    --i;
                }

                ImGui::PopID();
            }

            if (editingActions.empty())
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No actions added yet.");
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Add Action:");

        static EGameBinds selectedBind = GB_SkillWeapon1;
        static bool isKeyDown = true;
        static int delayMs = 0;

        const char *bindNames[] = {
            "Weapon 1", "Weapon 2", "Weapon 3", "Weapon 4", "Weapon 5",
            "Heal Skill", "Utility 1", "Utility 2", "Utility 3", "Elite Skill",
            "Dodge", "Jump", "Interact", "Weapon Swap"};

        EGameBinds bindValues[] = {
            GB_SkillWeapon1, GB_SkillWeapon2, GB_SkillWeapon3, GB_SkillWeapon4, GB_SkillWeapon5,
            GB_SkillHeal, GB_SkillUtility1, GB_SkillUtility2, GB_SkillUtility3, GB_SkillElite,
            GB_MoveDodge, GB_MoveJump_SwimUp_FlyUp, GB_MiscInteract, GB_SkillWeaponSwap};

        static int bindIndex = 0;
        if (ImGui::Combo("Game Action", &bindIndex, bindNames, IM_ARRAYSIZE(bindNames)))
            selectedBind = bindValues[bindIndex];

        ImGui::Checkbox("Press (uncheck = Release)", &isKeyDown);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Delay (ms)", &delayMs);

        ImGui::Spacing();
        if (ImGui::Button("Add Action"))
        {
            editingActions.emplace_back(selectedBind, isKeyDown, delayMs);
            delayMs = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All"))
            editingActions.clear();

        ImGui::Separator();
        if (ImGui::Button("Save Macro", ImVec2(120, 0)))
        {
            SaveMacro(macroName, selectedSlot, editingActions);
            editingActions.clear();
            lastSelectedMacroIndex = -2;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            editingActions.clear();
            g_showMacroEditor = false;
            g_selectedMacroIndex = -1;
            lastSelectedMacroIndex = -2;
        }
    }
    ImGui::End();
}

void AddonOptions()
{
    ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);

    ImGui::Text("Macro Keybind Manager v0.1.8");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                       "Execute sequences of game actions with precise timing control");

    ImGui::Spacing();

    ImGui::Text("Default Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");
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
            totalActions += (int)macro.actions.size();
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

    if (ImGui::CollapsingHeader("Usage Tips", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BulletText("Each macro can contain up to any number of actions");
        ImGui::BulletText("Use delays between actions for timing-sensitive sequences");
        ImGui::BulletText("Press and Release actions allow for held key combinations");
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
