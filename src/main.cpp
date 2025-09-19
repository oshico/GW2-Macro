#include "Shared.h"
#include "Nexus.h"
#include "MacroManager.h"
#include "GameModeCheck.h"
#include "Settings.h"
#include "imgui.h"
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
    AddonDef.Version.Build = 5;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "A macro keybind manager for executing sequences of game actions with timing control.";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    AddonDef.Provider = UP_None;
    AddonDef.UpdateLink = nullptr;

    return &AddonDef;
}

// =============================================================================
// ADDON LIFECYCLE
// =============================================================================

void AddonLoad(AddonAPI_t *aApi)
{
    APIDefs = aApi;
    APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v0.1.5 loaded!");
    ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);
    ImGui::SetAllocatorFunctions((void *(*)(size_t, void *))APIDefs->ImguiMalloc, (void (*)(void *, void *))APIDefs->ImguiFree);

    APIDefs->QuickAccess_Add("MACRO_MANAGER_SHORTCUT", "NEXUS_ICON_SETTINGS", "NEXUS_ICON_SETTINGS", "MACRO_SHOW_WINDOW", "Open Macro Manager");

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

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Manager", &g_showMainWindow))
    {
        ImVec4 statusColor;
        const char *statusText;

        if (IsInCompetitiveMode())
        {
            statusColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f); // Red
            statusText = "COMPETITIVE MODE (PVP/WVW) - MACROS DISABLED";
        }
        else
        {
            statusColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); // Green
            statusText = "PVE MODE - MACROS ENABLED";
        }

        ImGui::TextColored(statusColor, "%s", statusText);
        ImGui::Separator();

        ImGui::Text("Macro Slots (10 total)");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign keybinds through Nexus settings");
        ImGui::Separator();

        if (ImGui::BeginTable("MacrosTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 60.0f);
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

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", macro.name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d actions", static_cast<int>(macro.actions.size()));

                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button(("Edit##" + std::to_string(i)).c_str()))
                    OpenMacroEditor(static_cast<int>(i));

                ImGui::TableSetColumnIndex(4);
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
                {
                    DeleteMacro(i);
                    --i;
                    SaveMacrosToJson();
                }
            }

            ImGui::EndTable();
        }

        ImGui::Separator();
        if (ImGui::Button("Create New Macro"))
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
        if (g_selectedMacroIndex >= 0 && g_selectedMacroIndex < static_cast<int>(g_macros.size()))
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

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Editor", &g_showMacroEditor))
    {
        ImGui::InputText("Macro Name", macroName, sizeof(macroName));

        ImGui::Separator();

        ImGui::Text("Select Slot:");
        const char *slotNames[10] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
                                     "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
        ImGui::Combo("Macro Slot", &selectedSlot, slotNames, 10);

        ImGui::Separator();

        ImGui::Text("Action Sequence:");
        if (ImGui::BeginChild("ActionList", ImVec2(0, 200), true))
        {
            for (size_t i = 0; i < editingActions.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));

                ImGui::Text("%d.", static_cast<int>(i + 1));
                ImGui::SameLine();

                ImGui::TextColored(editingActions[i].isKeyDown ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                                   "%s", editingActions[i].isKeyDown ? "PRESS" : "RELEASE");
                ImGui::SameLine();

                ImGui::Text("%s", GetBindName(editingActions[i].gameBind));
                ImGui::SameLine();

                if (editingActions[i].delayMs > 0)
                    ImGui::Text("(%dms delay)", editingActions[i].delayMs);

                ImGui::SameLine(ImGui::GetWindowWidth() - 60);
                if (ImGui::SmallButton("X"))
                {
                    editingActions.erase(editingActions.begin() + i);
                    --i;
                }

                ImGui::PopID();
            }

            if (editingActions.empty())
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No actions added yet.");
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

        ImGui::Checkbox("Press (uncheck for Release)", &isKeyDown);
        ImGui::InputInt("Delay (ms)", &delayMs);

        if (ImGui::Button("Add Action"))
        {
            editingActions.emplace_back(selectedBind, isKeyDown, delayMs);
            delayMs = 0;
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All Actions"))
            editingActions.clear();

        ImGui::Separator();

        if (ImGui::Button("Save Macro"))
        {
            SaveMacro(macroName, selectedSlot, editingActions);
            editingActions.clear();
            lastSelectedMacroIndex = -2;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
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
    ImGui::Text("Macro Keybind Manager v0.1.5");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Execute sequences of game actions with precise timing control");
    ImGui::Separator();

    if (ImGui::Button("Open Macro Manager"))
        g_showMainWindow = true;

    ImGui::SameLine();
    if (ImGui::Button("Create New Macro"))
        OpenMacroEditor();

    ImGui::Separator();

    ImGui::Text("Default Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign macro keybinds through Nexus Input settings");

    ImGui::Separator();

    ImGui::Text("Statistics:");

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

    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.4f, 1.0f), "Macro Usage Policy Disclaimer:");
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Please read and understand the following policy before using macros:");

    if (ImGui::CollapsingHeader("Macro Usage Policy", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextWrapped("In general, our policy is as follows:");
        ImGui::TextWrapped("Attended macro use is permitted as long as it is not exploitative, and as long as it does not provide the user with an unfair advantage over other players. Unattended macro use is prohibited under any circumstances.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Allowed Macros:");
        ImGui::BulletText("You may bind dodge and jump to a single key.");
        ImGui::BulletText("You may create an auto-clicker that opens or consumes a stack of items.");
        ImGui::BulletText("You may use music macros to compose or perform in-game music (actively attended).");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Prohibited Macros:");
        ImGui::BulletText("Macro use cannot interfere with the gameplay of other players.");
        ImGui::BulletText("You cannot program a macro to perform actions on more than one account at a time.");
        ImGui::BulletText("You cannot use macros to automate skill use while away from your computer.");
        ImGui::BulletText("You cannot use macros to create skill chains in any competitive environment.");
        ImGui::BulletText("You cannot use macros to participate in activities or events (e.g., Bell Choir).");
        ImGui::BulletText("You cannot use macros for AFK farming of items, currencies, karma, or other benefits.");
        ImGui::BulletText("You cannot use macros to fully automate a character (botting).");
        ImGui::BulletText("Each macro should represent a single action that requires user input.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Important:");
        ImGui::TextWrapped("This addon automatically disables macros in competitive modes (PvP/WvW) to comply with game policies. However, you are ultimately responsible for using macros in accordance with the game's terms of service.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Warning:");
        ImGui::TextWrapped("Violation of these policies may result in account restrictions. Use macros responsibly!");
    }

    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.4f, 1.0f), "Usage Tips:");
    ImGui::BulletText("Each macro can contain up to any number of actions");
    ImGui::BulletText("Use delays between actions for timing-sensitive sequences");
    ImGui::BulletText("Press and Release actions allow for held key combinations");
    ImGui::BulletText("Disable macros to prevent accidental execution");
}