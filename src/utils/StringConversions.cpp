#include "StringConversions.h"

const char *GetBindName(const EGameBinds bind) {
    switch (bind) {
        case GB_SkillWeapon1:
            return "Weapon 1";
        case GB_SkillWeapon2:
            return "Weapon 2";
        case GB_SkillWeapon3:
            return "Weapon 3";
        case GB_SkillWeapon4:
            return "Weapon 4";
        case GB_SkillWeapon5:
            return "Weapon 5";
        case GB_SkillHeal:
            return "Heal Skill";
        case GB_SkillUtility1:
            return "Utility 1";
        case GB_SkillUtility2:
            return "Utility 2";
        case GB_SkillUtility3:
            return "Utility 3";
        case GB_SkillElite:
            return "Elite Skill";
        case GB_SkillProfession1:
            return "Profession 1";
        case GB_SkillProfession2:
            return "Profession 2";
        case GB_SkillProfession3:
            return "Profession 3";
        case GB_SkillProfession4:
            return "Profession 4";
        case GB_SkillProfession5:
            return "Profession 5";
        case GB_SkillProfession6:
            return "Profession 6";
        case GB_SkillProfession7:
            return "Profession 7";
        case GB_MoveDodge:
            return "Dodge";
        case GB_MoveJump_SwimUp_FlyUp:
            return "Jump";
        case GB_MiscInteract:
            return "Interact";
        case GB_SkillWeaponSwap:
            return "Weapon Swap";
        default:
            return "Unknown Bind";
    }
}

std::string GameBindToString(const EGameBinds bind) {
    switch (bind) {
        case GB_SkillWeapon1:
            return "GB_SkillWeapon1";
        case GB_SkillWeapon2:
            return "GB_SkillWeapon2";
        case GB_SkillWeapon3:
            return "GB_SkillWeapon3";
        case GB_SkillWeapon4:
            return "GB_SkillWeapon4";
        case GB_SkillWeapon5:
            return "GB_SkillWeapon5";
        case GB_SkillHeal:
            return "GB_SkillHeal";
        case GB_SkillUtility1:
            return "GB_SkillUtility1";
        case GB_SkillUtility2:
            return "GB_SkillUtility2";
        case GB_SkillUtility3:
            return "GB_SkillUtility3";
        case GB_SkillElite:
            return "GB_SkillElite";
        case GB_MoveDodge:
            return "GB_MoveDodge";
        case GB_MoveJump_SwimUp_FlyUp:
            return "GB_MoveJump_SwimUp_FlyUp";
        case GB_MiscInteract:
            return "GB_MiscInteract";
        case GB_SkillWeaponSwap:
            return "GB_SkillWeaponSwap";
        case GB_MoveForward:
            return "GB_MoveForward";
        case GB_MoveBackward:
            return "GB_MoveBackward";
        case GB_MoveLeft:
            return "GB_MoveLeft";
        case GB_MoveRight:
            return "GB_MoveRight";
        case GB_SkillProfession1:
            return "GB_SkillProfession1";
        case GB_SkillProfession2:
            return "GB_SkillProfession2";
        case GB_SkillProfession3:
            return "GB_SkillProfession3";
        case GB_SkillProfession4:
            return "GB_SkillProfession4";
        case GB_SkillProfession5:
            return "GB_SkillProfession5";
        case GB_SkillProfession6:
            return "GB_SkillProfession6";
        case GB_SkillProfession7:
            return "GB_SkillProfession7";
        default:
            return "GB_SkillWeapon1";
    }
}

EGameBinds StringToGameBind(const std::string &bindStr) {
    if (bindStr == "GB_SkillWeapon1")
        return GB_SkillWeapon1;
    if (bindStr == "GB_SkillWeapon2")
        return GB_SkillWeapon2;
    if (bindStr == "GB_SkillWeapon3")
        return GB_SkillWeapon3;
    if (bindStr == "GB_SkillWeapon4")
        return GB_SkillWeapon4;
    if (bindStr == "GB_SkillWeapon5")
        return GB_SkillWeapon5;
    if (bindStr == "GB_SkillHeal")
        return GB_SkillHeal;
    if (bindStr == "GB_SkillUtility1")
        return GB_SkillUtility1;
    if (bindStr == "GB_SkillUtility2")
        return GB_SkillUtility2;
    if (bindStr == "GB_SkillUtility3")
        return GB_SkillUtility3;
    if (bindStr == "GB_SkillElite")
        return GB_SkillElite;
    if (bindStr == "GB_MoveDodge")
        return GB_MoveDodge;
    if (bindStr == "GB_MoveJump_SwimUp_FlyUp")
        return GB_MoveJump_SwimUp_FlyUp;
    if (bindStr == "GB_MiscInteract")
        return GB_MiscInteract;
    if (bindStr == "GB_SkillWeaponSwap")
        return GB_SkillWeaponSwap;
    if (bindStr == "GB_MoveForward")
        return GB_MoveForward;
    if (bindStr == "GB_MoveBackward")
        return GB_MoveBackward;
    if (bindStr == "GB_MoveLeft")
        return GB_MoveLeft;
    if (bindStr == "GB_MoveRight")
        return GB_MoveRight;
    if (bindStr == "GB_SkillProfession1")
        return GB_SkillProfession1;
    if (bindStr == "GB_SkillProfession2")
        return GB_SkillProfession2;
    if (bindStr == "GB_SkillProfession3")
        return GB_SkillProfession3;
    if (bindStr == "GB_SkillProfession4")
        return GB_SkillProfession4;
    if (bindStr == "GB_SkillProfession5")
        return GB_SkillProfession5;
    if (bindStr == "GB_SkillProfession6")
        return GB_SkillProfession6;
    if (bindStr == "GB_SkillProfession7")
        return GB_SkillProfession7;

    return GB_SkillWeapon1;
}

const char *GetMouseButtonName(const EMouseButton button) {
    switch (button) {
        case EMouseButton::Left:
            return "Mouse Left";
        case EMouseButton::Right:
            return "Mouse Right";
        case EMouseButton::Middle:
            return "Mouse Middle";
        case EMouseButton::X1:
            return "Mouse X1";
        case EMouseButton::X2:
            return "Mouse X2";
        default:
            return "Unknown Mouse Button";
    }
}

std::string MouseButtonToString(const EMouseButton button) {
    switch (button) {
        case EMouseButton::Left:
            return "MB_Left";
        case EMouseButton::Right:
            return "MB_Right";
        case EMouseButton::Middle:
            return "MB_Middle";
        case EMouseButton::X1:
            return "MB_X1";
        case EMouseButton::X2:
            return "MB_X2";
        default:
            return "MB_Left";
    }
}

EMouseButton StringToMouseButton(const std::string &buttonStr) {
    if (buttonStr == "MB_Left")
        return EMouseButton::Left;
    if (buttonStr == "MB_Right")
        return EMouseButton::Right;
    if (buttonStr == "MB_Middle")
        return EMouseButton::Middle;
    if (buttonStr == "MB_X1")
        return EMouseButton::X1;
    if (buttonStr == "MB_X2")
        return EMouseButton::X2;

    return EMouseButton::Left;
}

std::string PositionTypeToString(const EPositionType type) {
    return (type == EPositionType::Absolute) ? "Absolute" : "Relative";
}

EPositionType StringToPositionType(const std::string &typeStr) {
    return (typeStr == "Absolute") ? EPositionType::Absolute : EPositionType::Relative;
}