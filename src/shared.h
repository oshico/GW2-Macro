#pragma once

#include "macro.h"
#include "mumble/Mumble.h"
#include "nexus/Nexus.h"
#include <atomic>
#include <mutex>

extern AddonAPI_t *ApiDefinition;
extern AddonDefinition_t AddonDefinition;
extern Mumble::Data *MumbleLink;
extern NexusLinkData_t *NexusLinkData;
extern bool ShowMainWindow;
extern bool ShowEditorWindow;
extern bool ShowSaveWindow;
extern int SelectedMacroIndex;
extern std::mutex MacroMutex;
extern std::atomic<bool> KillMacros;
extern std::vector<Macro> Macros;
extern char ImportJsonBuffer[8192];
extern char ExportJsonBuffer[8192];
extern int ExportSlot;
extern int ImportSlot;
extern int LastExportSlot;
extern char StatusMessage[256];
extern float StatusMessageTime;