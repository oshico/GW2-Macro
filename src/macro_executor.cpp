#include "macro_executor.h"
#include "game_mode_check.h"
#include "macro.h"
#include "shared.h"
#include "string_conversions.h"
#include <mutex>
#include <thread>
#include <windows.h>

void MoveMouse(const EMousePosition &Position) {
    if (Position.MousePositionType == EMousePositionType::Absolute) {
        SetCursorPos(Position.x, Position.y);
    } else {
        POINT CurrentMousePosition;
        GetCursorPos(&CurrentMousePosition);
        SetCursorPos(CurrentMousePosition.x + Position.x, CurrentMousePosition.y + Position.y);
    }
}

void SendMouseInput(const EMouseButton MouseButton, const bool MouseButtonIsDown) {
    if (!ApiDefinition)
        return;

    INPUT MouseInput = {};
    MouseInput.type = INPUT_MOUSE;

    switch (MouseButton) {
    case EMouseButton::Left:
        MouseInput.mi.dwFlags = MouseButtonIsDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        break;
    case EMouseButton::Right:
        MouseInput.mi.dwFlags = MouseButtonIsDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
        break;
    case EMouseButton::Middle:
        MouseInput.mi.dwFlags = MouseButtonIsDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
        break;
    case EMouseButton::X1:
        MouseInput.mi.dwFlags = MouseButtonIsDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
        MouseInput.mi.mouseData = XBUTTON1;
        break;
    case EMouseButton::X2:
        MouseInput.mi.dwFlags = MouseButtonIsDown ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
        MouseInput.mi.mouseData = XBUTTON2;
        break;
    }

    SendInput(1, &MouseInput, sizeof(INPUT));
}

void SendMouseClickAtPosition(const EMouseButton MouseButton, const bool MouseButtonIsDown, const EMousePosition &MousePosition) {
    MoveMouse(MousePosition);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    SendMouseInput(MouseButton, MouseButtonIsDown);
}

void ExecuteMacro(const Macro &Macro) {
    if (!Macro.Enabled)
        return;

    if (!AreMacrosAllowed()) {
        ApiDefinition->GUI_SendAlert("Macros disabled in PVP/WvW modes");
        return;
    }

    ApiDefinition->Log(LOGL_INFO, "MacroManager", ("Executing macro: " + Macro.Name).c_str());

    for (const auto &Action : Macro.Actions) {
        if (KillMacros.load()) {
            ApiDefinition->Log(LOGL_INFO, "MacroManager", "Macro execution stopped by Kill All");
            KillMacros.store(false);
            return;
        }

        if (Action.DelayMilliseconds > 0) {
            int RemainingDelay = Action.DelayMilliseconds;

            while (RemainingDelay > 0 && !KillMacros.load()) {
                constexpr int ChunkSize = 50;
                int CurrentChink = (RemainingDelay > ChunkSize) ? ChunkSize : RemainingDelay;
                std::this_thread::sleep_for(std::chrono::milliseconds(CurrentChink));
                RemainingDelay -= CurrentChink;
            }

            if (KillMacros.load()) {
                ApiDefinition->Log(LOGL_INFO, "MacroManager", "Macro execution stopped during delay");
                KillMacros.store(false);
                return;
            }
        }

        if (Action.MacroInputType == EMacroInputType::GameBind) {
            if (Action.IsKeybindDown)
                ApiDefinition->GameBinds_PressAsync(Action.GameBind);
            else
                ApiDefinition->GameBinds_ReleaseAsync(Action.GameBind);

            ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Action executed: " + std::string(Action.IsKeybindDown ? "PRESS " : "RELEASE ") + GetKeybindName(Action.GameBind)).c_str());
        } else if (Action.MacroInputType == EMacroInputType::MouseButton) {
            if (Action.MoveBeforeMouseClick) {
                SendMouseClickAtPosition(Action.MouseButton, Action.IsKeybindDown, Action.MousePosition);

                ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Mouse action at (" + std::to_string(Action.MousePosition.x) + ", " + std::to_string(Action.MousePosition.y) + "): " + std::string(Action.IsKeybindDown ? "PRESS " : "RELEASE ") + GetMouseButtonName(Action.MouseButton)).c_str());
            } else {
                SendMouseInput(Action.MouseButton, Action.IsKeybindDown);

                ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Mouse action executed: " + std::string(Action.IsKeybindDown ? "PRESS " : "RELEASE ") + GetMouseButtonName(Action.MouseButton)).c_str());
            }
        } else if (Action.MacroInputType == EMacroInputType::MouseMove) {
            MoveMouse(Action.MousePosition);

            ApiDefinition->Log(LOGL_DEBUG, "MacroManager", ("Mouse moved to (" + std::to_string(Action.MousePosition.x) + ", " + std::to_string(Action.MousePosition.y) + ") " + (Action.MousePosition.MousePositionType == EMousePositionType::Absolute ? "[Absolute]" : "[Relative]")).c_str());
        }
    }

    ApiDefinition->Log(LOGL_INFO, "MacroManager", ("Macro completed: " + Macro.Name).c_str());
}

void KillAllMacros() {
    KillMacros.store(true);

    std::lock_guard<std::mutex> lock(MacroMutex);

    KillMacros.store(false);

    ReleaseAllGameKeys();

    ApiDefinition->Log(LOGL_INFO, "MacroManager", "All macros killed");
    ApiDefinition->GUI_SendAlert("All macros stopped");
}

void ReleaseAllGameKeys() {
    if (!ApiDefinition)
        return;

    EGameBinds allBinds[] = {
        GB_SkillWeapon1,
        GB_SkillWeapon2,
        GB_SkillWeapon3,
        GB_SkillWeapon4,
        GB_SkillWeapon5,
        GB_SkillHeal,
        GB_SkillUtility1,
        GB_SkillUtility2,
        GB_SkillUtility3,
        GB_SkillElite,
        GB_MoveDodge,
        GB_MoveJump_SwimUp_FlyUp,
        GB_MoveAboutFace,
        GB_MiscInteract,
        GB_SkillWeaponSwap,
        GB_SkillSpecialAction,
        GB_MoveForward,
        GB_MoveBackward,
        GB_MoveLeft,
        GB_MoveRight,
        GB_SkillProfession1,
        GB_SkillProfession2,
        GB_SkillProfession3,
        GB_SkillProfession4,
        GB_SkillProfession5,
        GB_SkillProfession6,
        GB_SkillProfession7,
        GB_TargetTake,
        GB_TargetCall,
        GB_TargetAlert,
        GB_TargetCallLocal,
        GB_TargetTakeLocal,
        GB_TargetEnemyNearest,
        GB_TargetEnemyNext,
        GB_TargetEnemyPrev,
        GB_TargetAllyNearest,
        GB_TargetAllyNext,
        GB_TargetAllyPrev,
        GB_TargetLock,
        GB_TargetSnapGroundTarget,
        GB_TargetSnapGroundTargetToggle,
        GB_TargetAutoTargetingDisable,
        GB_TargetAutoTargetingToggle,
        GB_TargetAllyTargetingMode,
        GB_TargetAllyTargetingModeToggle,
        GB_UiCommerce,
        GB_UiContacts,
        GB_UiGuild,
        GB_UiHero,
        GB_UiInventory,
        GB_UiKennel,
        GB_UiLogout,
        GB_UiMail,
        GB_UiOptions,
        GB_UiParty,
        GB_UiPvp,
        GB_UiPvpBuild,
        GB_UiScoreboard,
        GB_UiSeasonalObjectivesShop,
        GB_UiInformation,
        GB_UiChatToggle,
        GB_UiChatCommand,
        GB_UiChatFocus,
        GB_UiChatReply,
        GB_UiToggle,
        GB_UiSquadBroadcastChatToggle,
        GB_UiSquadBroadcastChatCommand,
        GB_UiSquadBroadcastChatFocus,
        GB_CameraFree,
        GB_CameraZoomIn,
        GB_CameraZoomOut,
        GB_CameraReverse,
        GB_CameraActionMode,
        GB_CameraActionModeDisable,
        GB_ScreenshotNormal,
        GB_ScreenshotStereoscopic,
        GB_MapToggle,
        GB_MapFocusPlayer,
        GB_MapFloorDown,
        GB_MapFloorUp,
        GB_MapZoomIn,
        GB_MapZoomOut,
        GB_SpumoniToggle,
        GB_SpumoniMovement,
        GB_SpumoniSecondaryMovement,
        GB_SpumoniMAM01,
        GB_SpumoniMAM02,
        GB_SpumoniMAM03,
        GB_SpumoniMAM04,
        GB_SpumoniMAM05,
        GB_SpumoniMAM06,
        GB_SpumoniMAM07,
        GB_SpumoniMAM08,
        GB_SpumoniMAM09,
        GB_MasteryAccess,
        GB_MasteryAccess01,
        GB_MasteryAccess02,
        GB_MasteryAccess03,
        GB_MasteryAccess04,
        GB_MasteryAccess05,
        GB_MasteryAccess06,
        GB_MiscAoELoot,
        GB_MiscShowEnemies,
        GB_MiscShowAllies,
        GB_MiscCombatStance,
        GB_MiscToggleLanguage,
        GB_MiscTogglePetCombat,
        GB_MiscToggleFullScreen,
        GB_MiscToggleDecorationMode,
        GB_ToyUseDefault,
        GB_ToyUseSlot1,
        GB_ToyUseSlot2,
        GB_ToyUseSlot3,
        GB_ToyUseSlot4,
        GB_ToyUseSlot5,
        GB_Loadout1,
        GB_Loadout2,
        GB_Loadout3,
        GB_Loadout4,
        GB_Loadout5,
        GB_Loadout6,
        GB_Loadout7,
        GB_Loadout8,
        GB_Loadout9,
        GB_GearLoadout1,
        GB_GearLoadout2,
        GB_GearLoadout3,
        GB_GearLoadout4,
        GB_GearLoadout5,
        GB_GearLoadout6,
        GB_GearLoadout7,
        GB_GearLoadout8,
        GB_GearLoadout9};

    ApiDefinition->Log(LOGL_DEBUG, "MacroManager", "Releasing all game keys...");

    for (const EGameBinds bind : allBinds) {
        ApiDefinition->GameBinds_ReleaseAsync(bind);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ApiDefinition->Log(LOGL_INFO, "MacroManager", "All game keys released");
}