#ifndef SHARED_H
#define SHARED_H

#include "Nexus.h"

// Global API pointer - accessible from all files that include this header
extern AddonAPI_t* APIDefs;

// Function prototypes for the addon lifecycle
void AddonLoad(AddonAPI_t* aApi);
void AddonUnload();

// Render callback prototypes
void AddonRender();
void AddonOptions();

// Event handler prototypes  
void HandleExampleEvent(void* aEventArgs);

// Keybind handler prototype
void ProcessKeybind(const char* aIdentifier, bool aIsRelease);

// Setup function prototypes
void SetupKeybinds();
void SetupEvents();

#endif // SHARED_H