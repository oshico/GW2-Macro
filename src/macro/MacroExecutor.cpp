#include "MacroExecutor.h"
#include "GameModeCheck.h"
#include "StringConversions.h"
#include "../core/Context.h"
#include <mutex>
#include <thread>
#include <windows.h>

void MoveMouse(const MousePosition& pos)
{
  if (pos.positionType == EPositionType::Absolute)
  {
    SetCursorPos(pos.x, pos.y);
  }
  else
  {
    POINT currentPos;
    GetCursorPos(&currentPos);
    SetCursorPos(currentPos.x + pos.x, currentPos.y + pos.y);
  }
}

void SendMouseInput(const EMouseButton button, const bool isDown)
{
  if (!g_context.apiDefs)
    return;

  INPUT input = {};
  input.type = INPUT_MOUSE;

  switch (button)
  {
  case EMouseButton::Left:
    input.mi.dwFlags = isDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    break;
  case EMouseButton::Right:
    input.mi.dwFlags = isDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
    break;
  case EMouseButton::Middle:
    input.mi.dwFlags = isDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
    break;
  case EMouseButton::X1:
    input.mi.dwFlags = isDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
    input.mi.mouseData = XBUTTON1;
    break;
  case EMouseButton::X2:
    input.mi.dwFlags = isDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
    input.mi.mouseData = XBUTTON2;
    break;
  }

  SendInput(1, &input, sizeof(INPUT));
}

void SendMouseClickAtPosition(const EMouseButton button, const bool isDown, const MousePosition& pos)
{
  MoveMouse(pos);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  SendMouseInput(button, isDown);
}

void ExecuteMacro(const Macro& macro)
{
  if (!macro.enabled)
    return;

  if (!AreMacrosAllowed())
  {
    g_context.apiDefs->GUI_SendAlert("Macros disabled in PVP/WvW modes");
    return;
  }

  g_context.apiDefs->Log(LOGL_INFO, "MacroManager", ("Executing macro: " + macro.name).c_str());

  for (const auto& action : macro.actions)
  {
    if (g_context.killMacros.load())
    {
      g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "Macro execution stopped by Kill All");
      g_context.killMacros.store(false);
      return;
    }

    if (action.delayMs > 0)
    {
      int remainingDelay = action.delayMs;

      while (remainingDelay > 0 && !g_context.killMacros.load())
      {
        constexpr int chunkSize = 50;
        int currentChunk = (remainingDelay > chunkSize) ? chunkSize : remainingDelay;
        std::this_thread::sleep_for(std::chrono::milliseconds(currentChunk));
        remainingDelay -= currentChunk;
      }

      if (g_context.killMacros.load())
      {
        g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "Macro execution stopped during delay");
        g_context.killMacros.store(false);
        return;
      }
    }

    if (action.inputType == EInputType::GameBind)
    {
      if (action.isKeyDown)
        g_context.apiDefs->GameBinds_PressAsync(action.gameBind);
      else
        g_context.apiDefs->GameBinds_ReleaseAsync(action.gameBind);

      g_context.apiDefs->Log(
        LOGL_DEBUG,
        "MacroManager",
        ("Action executed: " +
          std::string(action.isKeyDown ? "PRESS " : "RELEASE ") +
          GetBindName(action.gameBind))
        .c_str());
    }
    else if (action.inputType == EInputType::MouseButton)
    {
      if (action.moveBeforeClick)
      {
        SendMouseClickAtPosition(action.mouseButton, action.isKeyDown, action.mousePosition);

        g_context.apiDefs->Log(
          LOGL_DEBUG,
          "MacroManager",
          ("Mouse action at (" + std::to_string(action.mousePosition.x) + ", " +
            std::to_string(action.mousePosition.y) + "): " +
            std::string(action.isKeyDown ? "PRESS " : "RELEASE ") +
            GetMouseButtonName(action.mouseButton))
          .c_str());
      }
      else
      {
        SendMouseInput(action.mouseButton, action.isKeyDown);

        g_context.apiDefs->Log(
          LOGL_DEBUG,
          "MacroManager",
          ("Mouse action executed: " +
            std::string(action.isKeyDown ? "PRESS " : "RELEASE ") +
            GetMouseButtonName(action.mouseButton))
          .c_str());
      }
    }
    else if (action.inputType == EInputType::MouseMove)
    {
      MoveMouse(action.mousePosition);

      g_context.apiDefs->Log(
        LOGL_DEBUG,
        "MacroManager",
        ("Mouse moved to (" + std::to_string(action.mousePosition.x) + ", " +
          std::to_string(action.mousePosition.y) + ") " +
          (action.mousePosition.positionType == EPositionType::Absolute ? "[Absolute]" : "[Relative]"))
        .c_str());
    }
  }

  g_context.apiDefs->Log(LOGL_INFO, "MacroManager", ("Macro completed: " + macro.name).c_str());
}

void KillAllMacros()
{
  g_context.killMacros.store(true);

  std::lock_guard<std::mutex> lock(g_context.macroMutex);

  g_context.killMacros.store(false);

  ReleaseAllGameKeys();

  g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "All macros killed");
  g_context.apiDefs->GUI_SendAlert("All macros stopped");
}

void ReleaseAllGameKeys()
{
  if (!g_context.apiDefs)
    return;

  EGameBinds allBinds[] = {
    GB_SkillWeapon1, GB_SkillWeapon2, GB_SkillWeapon3, GB_SkillWeapon4, GB_SkillWeapon5,
    GB_SkillHeal, GB_SkillUtility1, GB_SkillUtility2, GB_SkillUtility3, GB_SkillElite,
    GB_MoveDodge, GB_MoveJump_SwimUp_FlyUp, GB_MiscInteract, GB_SkillWeaponSwap,
    GB_MoveForward, GB_MoveBackward, GB_MoveLeft, GB_MoveRight,
    GB_SkillProfession1, GB_SkillProfession2, GB_SkillProfession3,
    GB_SkillProfession4, GB_SkillProfession5, GB_SkillProfession6, GB_SkillProfession7
  };

  g_context.apiDefs->Log(LOGL_DEBUG, "MacroManager", "Releasing all game keys...");

  for (const EGameBinds bind : allBinds)
  {
    g_context.apiDefs->GameBinds_ReleaseAsync(bind);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  g_context.apiDefs->Log(LOGL_INFO, "MacroManager", "All game keys released");
}
