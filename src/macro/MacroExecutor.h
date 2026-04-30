#ifndef MACRO_EXECUTOR_H
#define MACRO_EXECUTOR_H

#include "Macro.h"
#include <atomic>
#include <mutex>

extern std::vector<Macro> g_macros;
extern std::mutex g_macroMutex;
extern std::atomic<bool> g_killMacros;

void ExecuteMacro(const Macro &macro);

void KillAllMacros();

void ReleaseAllGameKeys();

void MoveMouse(const MousePosition &pos);

void SendMouseInput(EMouseButton button, bool isDown);

void SendMouseClickAtPosition(EMouseButton button, bool isDown, const MousePosition &pos);

#endif