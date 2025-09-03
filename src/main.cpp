#include "Shared.h"
#include "Nexus.h"
#include "imgui.h"
#include <cstring>

// Addon Definition Structure
AddonDefinition_t AddonDef{};

// Required Export: GetAddonDef
extern "C" __declspec(dllexport) AddonDefinition_t *GetAddonDef()
{
    AddonDef.Signature = -12345;
    AddonDef.APIVersion = NEXUS_API_VERSION;
    AddonDef.Name = "Macro";
    AddonDef.Version.Major = 0;
    AddonDef.Version.Minor = 1;
    AddonDef.Version.Build = 1;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "A macro keybind manager for executing sequences of game actions with FPS management.";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    AddonDef.Provider = UP_None;
    AddonDef.UpdateLink = nullptr;

    return &AddonDef;
}

// Addon Load Function
void AddonLoad(AddonAPI_t *aApi)
{
    APIDefs = aApi;
    APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager loaded!");

    if (APIDefs->ImguiContext)
        ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);

    for (auto &slot : g_macros)
        UnregisterKeybind(slot.identifier);

    for (auto &macro : g_macros)
        macro.enabled = false;

    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);
    SetupKeybinds();
}

// Addon Unload Function
void AddonUnload()
{
    if (APIDefs)
    {

        for (auto &slot : g_macros)
            UnregisterKeybind(slot.identifier);

        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);
        APIDefs->InputBinds_Deregister("MACRO_SHOW_WINDOW");
        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

// Keybind Handler
void ProcessKeybind(const char *aIdentifier, bool aIsRelease)
{
    if (aIsRelease)
        return;

    if (strcmp(aIdentifier, "MACRO_SHOW_WINDOW") == 0)
    {
        g_showMainWindow = !g_showMainWindow;
        return;
    }

    for (const auto &macro : g_macros)
    {
        if (macro.identifier == aIdentifier)
        {
            ExecuteMacro(macro);
            break;
        }
    }
}

// Main Render Callback
void AddonRender()
{
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
        ImGui::Text("Macro Slots (10 total)");
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

                // Enabled checkbox
                ImGui::TableSetColumnIndex(0);
                bool enabled = macro.enabled;
                if (ImGui::Checkbox(("##Enabled" + std::to_string(i)).c_str(), &enabled))
                    macro.enabled = enabled;

                // Macro name
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", macro.name.c_str());

                // Actions count
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d actions", (int)macro.actions.size());

                // Edit button
                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button(("Edit##" + std::to_string(i)).c_str()))
                    OpenMacroEditor(static_cast<int>(i));

                // Delete button
                ImGui::TableSetColumnIndex(4);
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
                    DeleteMacro(i);
            }

            ImGui::EndTable();
        }
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
    static int selectedSlot = 0; // Slot 0-9

    // Initialize editor on selection change
    if (g_selectedMacroIndex != lastSelectedMacroIndex)
    {
        if (g_selectedMacroIndex >= 0 && g_selectedMacroIndex < static_cast<int>(g_macros.size()))
        {
            Macro &macro = g_macros[g_selectedMacroIndex];
            strcpy(macroName, macro.name.c_str());
            editingActions = macro.actions;

            // Set selectedSlot based on identifier: "MACRO_X"
            std::string id = macro.identifier;
            if (id.find("MACRO_") != std::string::npos)
                selectedSlot = std::stoi(id.substr(6)) - 1; // "MACRO_" is 6 chars
        }
        else
        {
            strcpy(macroName, "New Macro");
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
                    i--;
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
            editingActions.emplace_back(selectedBind, isKeyDown, delayMs);

        ImGui::SameLine();
        if (ImGui::Button("Clear All Actions"))
            editingActions.clear();

        ImGui::Separator();

        // Save button
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

// Options Render Callback
void AddonOptions()
{
    ImGui::Text("Macro Keybind Manager v0.1.1");
    ImGui::Text("Developed by oshico for Guild Wars 2 speed runs");
    ImGui::Text("Features: Macro sequences, FPS management, keybind automation");
    ImGui::Separator();

    if (ImGui::Button("Open Macro Manager"))
        g_showMainWindow = true;

    ImGui::SameLine();
    if (ImGui::Button("Edit Macros"))
        OpenMacroEditor();

    ImGui::Separator();
    ImGui::Text("Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");

    ImGui::Separator();
    ImGui::Text("Statistics:");
    ImGui::Text("Total Macros: %d", (int)g_macros.size());

    int totalActions = 0;
    for (const auto &macro : g_macros)
        totalActions += macro.actions.size();

    ImGui::Text("Total Actions: %d", totalActions);
}

// Setup permanent keybind for main menu
void SetupKeybinds()
{
    if (!APIDefs)
        return;

    // Register toggle key for main window
    APIDefs->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");

    // Register all MACRO_X slots
    for (size_t i = 0; i < g_macros.size(); ++i)
    {
        g_macros[i].identifier = "MACRO_" + std::to_string(i + 1);
        RegisterKeybind(g_macros[i]);
    }
}