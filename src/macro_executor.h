#pragma once

#include "macro.h"

void ExecuteMacro(const Macro &Macro);

void KillAllMacros();

void ReleaseAllGameKeys();

void SendMouseInput(EMouseButton MouseButton, bool MouseButtonIsDown);

void SendMouseClickAtPosition(EMouseButton MouseButton, bool MouseButtonIsDown, const EMousePosition &MousePosition);