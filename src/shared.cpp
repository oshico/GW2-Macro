#include "shared.h"
#include "macro.h"
#include <atomic>
#include <mutex>

AddonDefinition_t AddonDefinition = {};
AddonAPI_t *ApiDefinition = nullptr;
Mumble::Data *MumbleLink = nullptr;
NexusLinkData_t *NexusLink = nullptr;
bool ShowMainWindow = false;
bool ShowEditorWindow = false;
bool ShowSaveWindow = false;
int SelectedMacroIndex = -1;
std::atomic<bool> KillMacros{false};
std::mutex MacroMutex;

std::vector<Macro> Macros = {
    Macro("Empty", "MACRO_1"),
    Macro("Empty", "MACRO_2"),
    Macro("Empty", "MACRO_3"),
    Macro("Empty", "MACRO_4"),
    Macro("Empty", "MACRO_5"),
    Macro("Empty", "MACRO_6"),
    Macro("Empty", "MACRO_7"),
    Macro("Empty", "MACRO_8"),
    Macro("Empty", "MACRO_9"),
    Macro("Empty", "MACRO_10")};

char ImportJsonBuffer[8192] = "";
char ExportJsonBuffer[8192] = "";
int ExportSlot = 0;
int ImportSlot = 0;
int LastExportSlot = -1;
char StatusMessage[256] = "";
float StatusMessageTime = 0.0f;