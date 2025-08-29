#include "Shared.h"
#include <cstring>

//==================================================================================================
// Addon Definition Structure
//==================================================================================================
AddonDefinition_t AddonDef{};

//==================================================================================================
// Required Export: GetAddonDef
// This function MUST be exported and is called by Nexus to get addon information
//==================================================================================================
extern "C" __declspec(dllexport) AddonDefinition_t* GetAddonDef()
{
    AddonDef.Signature = -12345;                               // Negative ID for non-Raidcore addons
    AddonDef.APIVersion = NEXUS_API_VERSION;                   // Use API version 6
    AddonDef.Name = "Nexus Addon Template";
    AddonDef.Version.Major = 1;
    AddonDef.Version.Minor = 0; 
    AddonDef.Version.Build = 0;
    AddonDef.Version.Revision = 1;
    AddonDef.Author = "Your Name";
    AddonDef.Description = "A barebones template addon for Nexus.";
    AddonDef.Load = AddonLoad;
    AddonDef.Unload = AddonUnload;
    AddonDef.Flags = AF_None;
    
    // Update settings (optional)
    AddonDef.Provider = UP_None;                               // No auto-updates
    AddonDef.UpdateLink = nullptr;

    return &AddonDef;
}

//==================================================================================================
// Addon Load Function
// Called when the addon is loaded
//==================================================================================================
void AddonLoad(AddonAPI_t* aApi)
{
    // Store the API pointer globally
    APIDefs = aApi;

    // Required ImGui setup if using ImGui rendering
    // ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext);
    // ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, 
    //                             (void(*)(void*, void*))APIDefs->ImguiFree);

    // Log addon load
    APIDefs->Log(LOGL_INFO, "NexusTemplate", "Template addon loaded successfully!");

    // Register render callbacks
    APIDefs->GUI_Register(RT_Render, AddonRender);
    APIDefs->GUI_Register(RT_OptionsRender, AddonOptions);

    // Setup keybinds and events
    SetupKeybinds();
    SetupEvents();

    // Example: Get Mumble data (optional)
    // MumbleLink* mumble = (MumbleLink*)APIDefs->DataLink_Get(DL_MUMBLE_LINK);
    // NexusLinkData_t* nexusLink = (NexusLinkData_t*)APIDefs->DataLink_Get(DL_NEXUS_LINK);
}

//==================================================================================================
// Addon Unload Function  
// Called when the addon is unloaded or game shuts down
//==================================================================================================
void AddonUnload()
{
    // Cleanup: Remove render callbacks
    if (APIDefs)
    {
        APIDefs->GUI_Deregister(AddonRender);
        APIDefs->GUI_Deregister(AddonOptions);
        
        // Remove keybinds
        APIDefs->InputBinds_Deregister("KB_TEMPLATE_TEST");
        
        // Unsubscribe from events
        APIDefs->Events_Unsubscribe(EV_MUMBLE_IDENTITY_UPDATED, HandleExampleEvent);
        
        APIDefs->Log(LOGL_INFO, "NexusTemplate", "Template addon unloaded successfully!");
    }
}

//==================================================================================================
// Main Render Callback
// Called every frame during the render phase
//==================================================================================================
void AddonRender()
{
    // Your main UI rendering code goes here
    // Example:
    // ImGui::Begin("Template Window");
    // ImGui::Text("Hello from Nexus Template!");
    // ImGui::End();
}

//==================================================================================================
// Options Render Callback
// Called when drawing in the addon options/settings window
//==================================================================================================
void AddonOptions()
{
    // Your addon settings/options UI goes here
    // This appears in the Nexus options window
    
    // Example:
    // static bool exampleSetting = false;
    // ImGui::Checkbox("Example Setting", &exampleSetting);
    // ImGui::Text("Template Addon v1.0.0");
}

//==================================================================================================
// Keybind Handler
// Called when registered keybinds are pressed/released
//==================================================================================================
void ProcessKeybind(const char* aIdentifier, bool aIsRelease)
{
    if (!aIsRelease) // Only process on key press, not release
    {
        if (strcmp(aIdentifier, "KB_TEMPLATE_TEST") == 0)
        {
            APIDefs->Log(LOGL_INFO, "NexusTemplate", "Test keybind pressed!");
            APIDefs->GUI_SendAlert("Template addon test keybind activated!");
        }
    }
}

//==================================================================================================
// Event Handler
// Called when subscribed events are raised
//==================================================================================================
void HandleExampleEvent(void* aEventArgs)
{
    // Handle the event
    // For Mumble identity updates, aEventArgs would be MumbleIdentity*
    // Suppress unused parameter warning since this is just a template
    (void)aEventArgs;
    
    APIDefs->Log(LOGL_DEBUG, "NexusTemplate", "Received example event");
}

//==================================================================================================
// Setup Functions
//==================================================================================================
void SetupKeybinds()
{
    if (APIDefs)
    {
        // Register a test keybind (Ctrl+Shift+T by default)
        APIDefs->InputBinds_RegisterWithString("KB_TEMPLATE_TEST", ProcessKeybind, "CTRL+SHIFT+T");
    }
}

void SetupEvents()
{
    if (APIDefs)
    {
        // Subscribe to Mumble identity updates as an example
        APIDefs->Events_Subscribe(EV_MUMBLE_IDENTITY_UPDATED, HandleExampleEvent);
    }
}