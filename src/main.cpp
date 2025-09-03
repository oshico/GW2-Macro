/**
 * @file main.cpp
 * @brief Main entry point and GUI implementation for Macro Keybind Manager
 * @author oshico
 * @version 0.1.1
 * 
 * This addon provides a macro system for Guild Wars 2 that allows players to create
 * and execute sequences of game actions with timing control. Designed primarily for
 * speedrunning and efficient gameplay automation.
 */

#include "Shared.h"
#include "Nexus.h"
#include "imgui.h"
#include <cstring>

// =============================================================================
// ADDON DEFINITION
// =============================================================================

AddonDefinition_t AddonDef{};

/**
 * @brief Required export function for Nexus addon framework
 * @return Pointer to addon definition structure
 */
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
    APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v0.1.1 loaded!");

    // Set ImGui context if available
    if (APIDefs->ImguiContext)
        ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);

    // Clean up any existing keybinds from previous sessions
    for (const auto &macro : g_macros)
        UnregisterKeybind(macro.identifier);

    // Disable all macros on startup for safety
    for (auto &macro : g_macros)
        macro.enabled = false;

    // Register GUI callbacks
    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);
    
    // Set up keybind system
    SetupKeybinds();
}

void AddonUnload()
{
    if (APIDefs)
    {
        // Unregister all macro keybinds
        for (const auto &macro : g_macros)
            UnregisterKeybind(macro.identifier);

        // Unregister GUI callbacks
        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);
        
        // Unregister main window toggle keybind
        APIDefs->InputBinds_Deregister("MACRO_SHOW_WINDOW");
        
        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

// =============================================================================
// KEYBIND HANDLING
// =============================================================================

void ProcessKeybind(const char *aIdentifier, bool aIsRelease)
{
    // Ignore key release events
    if (aIsRelease)
        return;

    // Handle main window toggle
    if (strcmp(aIdentifier, "MACRO_SHOW_WINDOW") == 0)
    {
        g_showMainWindow = !g_showMainWindow;
        return;
    }

    // Handle macro execution
    for (const auto &macro : g_macros)
    {
        if (macro.identifier == aIdentifier)
        {
            ExecuteMacro(macro);
            break;
        }
    }
}

void SetupKeybinds()
{
    if (!APIDefs)
        return;

    // Register main window toggle keybind (CTRL+SHIFT+K)
    APIDefs->InputBinds_RegisterWithString("MACRO_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");

    // Register keybinds for all macro slots (users will assign keys through Nexus)
    for (const auto& macro : g_macros)
        RegisterKeybind(macro);
}

// =============================================================================
// GUI RENDERING
// =============================================================================

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
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign keybinds through Nexus settings");
        ImGui::Separator();

        // Create table for macro display
        if (ImGui::BeginTable("MacrosTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            // Set up table columns
            ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableHeadersRow();

            // Render each macro slot
            for (size_t i = 0; i < g_macros.size(); ++i)
            {
                Macro &macro = g_macros[i];
                ImGui::TableNextRow();

                // Enabled checkbox column
                ImGui::TableSetColumnIndex(0);
                bool enabled = macro.enabled;
                if (ImGui::Checkbox(("##Enabled" + std::to_string(i)).c_str(), &enabled))
                    macro.enabled = enabled;

                // Macro name column
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", macro.name.c_str());

                // Actions count column
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d actions", static_cast<int>(macro.actions.size()));

                // Edit button column
                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button(("Edit##" + std::to_string(i)).c_str()))
                    OpenMacroEditor(static_cast<int>(i));

                // Delete button column
                ImGui::TableSetColumnIndex(4);
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
                {
                    DeleteMacro(i);
                    --i; // Adjust index after deletion
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

    // Static variables for editor state
    static char macroName[128] = "";
    static std::vector<KeyAction> editingActions;
    static int lastSelectedMacroIndex = -2;
    static int selectedSlot = 0;

    // Initialize editor when macro selection changes
    if (g_selectedMacroIndex != lastSelectedMacroIndex)
    {
        if (g_selectedMacroIndex >= 0 && g_selectedMacroIndex < static_cast<int>(g_macros.size()))
        {
            // Editing existing macro
            const Macro &macro = g_macros[g_selectedMacroIndex];
            strncpy_s(macroName, sizeof(macroName), macro.name.c_str(), _TRUNCATE);
            editingActions = macro.actions;

            // Extract slot number from identifier (MACRO_X format)
            std::string id = macro.identifier;
            if (id.find("MACRO_") != std::string::npos)
                selectedSlot = std::stoi(id.substr(6)) - 1;
        }
        else
        {
            // Creating new macro
            strcpy_s(macroName, sizeof(macroName), "New Macro");
            editingActions.clear();
            selectedSlot = 0;
        }
        lastSelectedMacroIndex = g_selectedMacroIndex;
    }

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Editor", &g_showMacroEditor))
    {
        // Macro name input
        ImGui::InputText("Macro Name", macroName, sizeof(macroName));

        ImGui::Separator();
        
        // Slot selection
        ImGui::Text("Select Slot:");
        const char *slotNames[10] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
                                     "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
        ImGui::Combo("Macro Slot", &selectedSlot, slotNames, 10);

        ImGui::Separator();
        
        // Action sequence display
        ImGui::Text("Action Sequence:");
        if (ImGui::BeginChild("ActionList", ImVec2(0, 200), true))
        {
            for (size_t i = 0; i < editingActions.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                
                // Action number
                ImGui::Text("%d.", static_cast<int>(i + 1));
                ImGui::SameLine();
                
                // Action type (PRESS/RELEASE) with color coding
                ImGui::TextColored(editingActions[i].isKeyDown ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                                   "%s", editingActions[i].isKeyDown ? "PRESS" : "RELEASE");
                ImGui::SameLine();
                
                // Action bind name
                ImGui::Text("%s", GetBindName(editingActions[i].gameBind));
                ImGui::SameLine();
                
                // Delay information if present
                if (editingActions[i].delayMs > 0)
                    ImGui::Text("(%dms delay)", editingActions[i].delayMs);
                
                // Delete action button
                ImGui::SameLine(ImGui::GetWindowWidth() - 60);
                if (ImGui::SmallButton("X"))
                {
                    editingActions.erase(editingActions.begin() + i);
                    --i; // Adjust index after deletion
                }
                
                ImGui::PopID();
            }
            
            if (editingActions.empty())
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No actions added yet.");
        }
        ImGui::EndChild();

        ImGui::Separator();
        
        // Add new action section
        ImGui::Text("Add Action:");

        static EGameBinds selectedBind = GB_SkillWeapon1;
        static bool isKeyDown = true;
        static int delayMs = 0;

        // Available game binds
        const char *bindNames[] = {
            "Weapon 1", "Weapon 2", "Weapon 3", "Weapon 4", "Weapon 5",
            "Heal Skill", "Utility 1", "Utility 2", "Utility 3", "Elite Skill",
            "Dodge", "Jump", "Interact", "Weapon Swap"
        };

        EGameBinds bindValues[] = {
            GB_SkillWeapon1, GB_SkillWeapon2, GB_SkillWeapon3, GB_SkillWeapon4, GB_SkillWeapon5,
            GB_SkillHeal, GB_SkillUtility1, GB_SkillUtility2, GB_SkillUtility3, GB_SkillElite,
            GB_MoveDodge, GB_MoveJump_SwimUp_FlyUp, GB_MiscInteract, GB_SkillWeaponSwap
        };

        // Action selection controls
        static int bindIndex = 0;
        if (ImGui::Combo("Game Action", &bindIndex, bindNames, IM_ARRAYSIZE(bindNames)))
            selectedBind = bindValues[bindIndex];

        ImGui::Checkbox("Press (uncheck for Release)", &isKeyDown);
        ImGui::InputInt("Delay (ms)", &delayMs);

        // Add action button
        if (ImGui::Button("Add Action"))
        {
            editingActions.emplace_back(selectedBind, isKeyDown, delayMs);
            delayMs = 0; // Reset delay for next action
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All Actions"))
            editingActions.clear();

        ImGui::Separator();

        // Save and cancel buttons
        if (ImGui::Button("Save Macro"))
        {
            SaveMacro(macroName, selectedSlot, editingActions);
            editingActions.clear();
            lastSelectedMacroIndex = -2; // Reset state
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            editingActions.clear();
            g_showMacroEditor = false;
            g_selectedMacroIndex = -1;
            lastSelectedMacroIndex = -2; // Reset state
        }
    }
    ImGui::End();
}

void AddonOptions()
{
    ImGui::Text("Macro Keybind Manager v0.1.1");
    ImGui::Text("Developed by oshico for Guild Wars 2");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Execute sequences of game actions with precise timing control");
    ImGui::Separator();

    // Quick access buttons
    if (ImGui::Button("Open Macro Manager"))
        g_showMainWindow = true;

    ImGui::SameLine();
    if (ImGui::Button("Create New Macro"))
        OpenMacroEditor();

    ImGui::Separator();
    
    // Keybind information
    ImGui::Text("Default Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign macro keybinds through Nexus Input settings");

    ImGui::Separator();
    
    // Statistics display
    ImGui::Text("Statistics:");
    
    // Count active macros
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
    
    // Usage information
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.4f, 1.0f), "Usage Tips:");
    ImGui::BulletText("Each macro can contain up to any number of actions");
    ImGui::BulletText("Use delays between actions for timing-sensitive sequences");
    ImGui::BulletText("Press and Release actions allow for held key combinations");
    ImGui::BulletText("Disable macros to prevent accidental execution");
}