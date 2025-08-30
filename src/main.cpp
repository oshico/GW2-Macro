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
    AddonDef.Version.Build = 0;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "A macro keybind manager for executing sequences of game actions.";
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
    {
        ImGui::SetCurrentContext((ImGuiContext *)APIDefs->ImguiContext);
    }

    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);
    SetupKeybinds();
}

// Addon Unload Function
void AddonUnload()
{
    if (APIDefs)
    {
        for (const auto &keybind : g_keybinds)
        {
            UnregisterKeybind(keybind.identifier);
        }

        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);
        APIDefs->InputBinds_Deregister("KB_SHOW_WINDOW");
        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

// Keybind Handler
void ProcessKeybind(const char *aIdentifier, bool aIsRelease)
{
    if (aIsRelease)
        return;

    if (strcmp(aIdentifier, "KB_SHOW_WINDOW") == 0)
    {
        g_showKeybindWindow = !g_showKeybindWindow;
        return;
    }

    for (const auto &keybind : g_keybinds)
    {
        if (keybind.identifier == aIdentifier && keybind.enabled)
        {
            APIDefs->Log(LOGL_INFO, "MacroManager",
                         ("Executing macro: " + keybind.name).c_str());
            ExecuteKeyActions();
            break;
        }
    }
}

// Main Render Callback
void AddonRender()
{
    if (g_showKeybindWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(550, 450), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Macro Manager", &g_showKeybindWindow))
        {
            ImGui::Text("Current Macros:");
            ImGui::Separator();

            // Display existing macros
            for (size_t i = 0; i < g_keybinds.size(); ++i)
            {
                auto &keybind = g_keybinds[i];
                ImGui::PushID(static_cast<int>(i + 1000));

                ImGui::Checkbox("##Enabled", &keybind.enabled);
                ImGui::SameLine();
                ImGui::Text("%s: %s", keybind.name.c_str(), keybind.keyCombo.c_str());

                ImGui::SameLine();
                if (ImGui::SmallButton("Delete"))
                {
                    DeleteMacro(i);
                    i--; // Adjust index after deletion
                }

                ImGui::PopID();
            }

            ImGui::Separator();
            ImGui::Text("Current Key Sequence:");

            // Display current key sequence
            for (size_t i = 0; i < g_keyActions.size(); ++i)
            {
                const auto &action = g_keyActions[i];
                ImGui::PushID(static_cast<int>(i + 2000));

                ImGui::Text("  %s %s (%dms delay)",
                            action.isKeyDown ? "PRESS" : "RELEASE",
                            GetBindName(action.gameBind),
                            action.delayMs);

                ImGui::SameLine();
                if (ImGui::SmallButton("X"))
                {
                    DeleteKeyAction(i);
                    i--; // Adjust index after deletion
                }

                ImGui::PopID();
            }

            ImGui::Separator();
            ImGui::Text("Add Key to Sequence:");

            // Static variables for the GUI form
            static EGameBinds selectedBind = GB_SkillWeapon1;
            static bool isKeyDown = true;
            static int delayMs = 0;
            static char macroName[128] = "New Macro";
            static char triggerCombo[128] = "CTRL+SHIFT+";

            // Available game binds
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
            {
                selectedBind = bindValues[bindIndex];
            }

            ImGui::Checkbox("Press (uncheck for Release)", &isKeyDown);
            ImGui::InputInt("Delay (ms)", &delayMs);

            if (ImGui::Button("Add to Sequence"))
            {
                g_keyActions.emplace_back(selectedBind, isKeyDown, delayMs);
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear All"))
            {
                ClearKeySequence();
            }

            ImGui::Separator();
            ImGui::InputText("Macro Name", macroName, sizeof(macroName));
            ImGui::InputText("Trigger Combo", triggerCombo, sizeof(triggerCombo));

            if (ImGui::Button("Save Macro"))
            {
                if (SaveMacro(macroName, triggerCombo))
                {
                    // Reset form on successful save
                    strcpy(macroName, "New Macro");
                    strcpy(triggerCombo, "CTRL+SHIFT+");
                }
            }
        }
        ImGui::End();
    }
}

// Options Render Callback
void AddonOptions()
{
    ImGui::Text("Macro Keybind Manager v0.1.0");
    ImGui::Separator();

    if (ImGui::Button("Open Macro Manager"))
    {
        g_showKeybindWindow = true;
    }

    ImGui::Text("Press CTRL+SHIFT+k to toggle window");
    ImGui::Text("Macros: %d | Key Actions: %d", (int)g_keybinds.size(), (int)g_keyActions.size());
}

// Setup Functions
void SetupKeybinds()
{
    if (APIDefs)
    {
        APIDefs->InputBinds_RegisterWithString("KB_SHOW_WINDOW", ProcessKeybind, "CTRL+SHIFT+K");

        // Example: Weapon swap combo
        g_keyActions.emplace_back(GB_SkillWeaponSwap, true, 0);   // Press weapon swap
        g_keyActions.emplace_back(GB_SkillWeaponSwap, false, 50); // Release weapon swap
        g_keyActions.emplace_back(GB_SkillWeapon1, true, 100);    // Press weapon 1
        g_keyActions.emplace_back(GB_SkillWeapon1, false, 50);    // Release weapon 1

        g_keybinds.emplace_back("Weapon Swap + Skill 1", "KB_MACRO_1", "CTRL+SHIFT+1");
        RegisterKeybind(g_keybinds.back());
    }
}