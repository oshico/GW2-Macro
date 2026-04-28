#include "AddonLifecycle.h"
#include "Shared.h"
#include "../macro/MacroManager.h"
#include "../macro/KeybindManager.h"
#include "../gui/MainWindow.h"
#include "../gui/MacroEditor.h"
#include "../gui/AddonOptions.h"
#include "../persistence/Settings.h"
#include <imgui.h>
#include <cstring>

#include "MacroExecutor.h"
#include "Module.h"
#include "Resource.h"

AddonDefinition_t AddonDef{};

extern "C" __declspec(dllexport) AddonDefinition_t *GetAddonDef() {
    AddonDef.Signature = 0xA1B2C3D4;
    AddonDef.APIVersion = NEXUS_API_VERSION;
    AddonDef.Name = "Macro Manager";
    AddonDef.Version.Major = 0;
    AddonDef.Version.Minor = 3;
    AddonDef.Version.Build = 0;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "oshico";
    AddonDef.Description = "A macro keybind manager for executing sequences of game actions with timing control.";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    AddonDef.Provider = EUpdateProvider::UP_GitHub;
    AddonDef.UpdateLink = "https://github.com/oshico/GW2-MACRO";

    return &AddonDef;
}

void AddonLoad(AddonAPI_t *aApi) {
    APIDefs = aApi;
    APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v0.3.0 loaded!");
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(APIDefs->ImguiContext));

    ImGui::SetAllocatorFunctions(reinterpret_cast<void*(*)(size_t, void *)>(APIDefs->ImguiMalloc),
                                 reinterpret_cast<void(*)(void *, void *)>(APIDefs->ImguiFree));

    //ImGui::SetAllocatorFunctions((void *(*)(size_t, void *)) APIDefs->ImguiMalloc,(void (*)(void *, void *)) APIDefs->ImguiFree);
    //ImGui::SetAllocatorFunctions(APIDefs->ImguiMalloc, APIDefs->ImguiFree);

    APIDefs->Textures_GetOrCreateFromResource("MACRO_MANAGER_ICON", IDB_MACRO_MANAGER_ICON, GetAddonModuleHandle());

    APIDefs->QuickAccess_Add("MACRO_MANAGER_SHORTCUT", "MACRO_MANAGER_ICON", "MACRO_MANAGER_ICON", "MACRO_SHOW_WINDOW",
                             "Open Macro Manager");

    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);

    LoadMacrosFromJson();

    for (const auto &macro: g_macros)
        UnregisterKeybind(macro.identifier);

    for (auto &macro: g_macros)
        macro.enabled = false;

    SetupKeybinds();
}

void AddonUnload() {
    if (APIDefs) {
        KillAllMacros();
        SaveMacrosToJson();

        for (const auto &macro: g_macros)
            UnregisterKeybind(macro.identifier);

        APIDefs->QuickAccess_Remove("MACRO_MANAGER_SHORTCUT");

        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);

        APIDefs->InputBinds_Deregister("MACRO_SHOW_WINDOW");
        APIDefs->InputBinds_Deregister("MACRO_KILL");

        APIDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

void AddonRender() {
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(APIDefs->ImguiContext));
    RenderMainWindow();
    RenderMacroEditor();
}