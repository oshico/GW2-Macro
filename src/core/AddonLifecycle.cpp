#include "AddonLifecycle.h"
#include "Context.h"
#include "../macro/MacroManager.h"
#include "../macro/KeybindManager.h"
#include "../gui/MainWindow.h"
#include "../gui/MacroEditor.h"
#include "../gui/ImportExport.h"
#include "../gui/AddonOptions.h"
#include "../persistence/Settings.h"
#include <imgui.h>
#include <cstring>

#include "MacroExecutor.h"
#include "Module.h"
#include "Resource.h"

AddonDefinition_t AddonDef{};
extern Context g_context;

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
    g_context.apiDefs = aApi;
    g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v0.3.0 loaded!");
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(g_context.apiDefs->ImguiContext));

    ImGui::SetAllocatorFunctions(reinterpret_cast<void*(*)(size_t, void *)>(g_context.apiDefs->ImguiMalloc),
                                 reinterpret_cast<void(*)(void *, void *)>(g_context.apiDefs->ImguiFree));

    g_context.apiDefs->Textures_GetOrCreateFromResource("MACRO_MANAGER_ICON", IDB_MACRO_MANAGER_ICON, GetAddonModuleHandle());

    g_context.apiDefs->QuickAccess_Add("MACRO_MANAGER_SHORTCUT", "MACRO_MANAGER_ICON", "MACRO_MANAGER_ICON", "MACRO_SHOW_WINDOW",
                             "Open Macro Manager");

    g_context.apiDefs->GUI_Register(RT_Render, AddonRender);
    g_context.apiDefs->GUI_Register(RT_OptionsRender, AddonOptions);

    LoadMacrosFromJson();

    for (const auto &macro: g_context.macros)
        UnregisterKeybind(macro.identifier);

    for (auto &macro: g_context.macros)
        macro.enabled = false;

    SetupKeybinds();
}

void AddonUnload() {
    if (g_context.apiDefs) {
        KillAllMacros();
        SaveMacrosToJson();

        for (const auto &macro: g_context.macros)
            UnregisterKeybind(macro.identifier);

        g_context.apiDefs->QuickAccess_Remove("MACRO_MANAGER_SHORTCUT");

        g_context.apiDefs->GUI_Deregister(AddonRender);
        g_context.apiDefs->GUI_Deregister(AddonOptions);

        g_context.apiDefs->InputBinds_Deregister("MACRO_SHOW_WINDOW");
        g_context.apiDefs->InputBinds_Deregister("MACRO_KILL");

        g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

void AddonRender() {
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(g_context.apiDefs->ImguiContext));
    RenderMainWindow();
    RenderMacroEditor();
    RenderImportExport();
}