#pragma once

#include "Macro.h"

void ExecuteMacro(const Macro& macro);

void KillAllMacros();

void ReleaseAllGameKeys();

void SendMouseInput(EMouseButton button, bool isDown);

void SendMouseClickAtPosition(EMouseButton button, bool isDown, const MousePosition& pos);