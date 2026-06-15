#include "string_conversions.h"
#include "nexus/Nexus.h"
#include <string>

const char *GetKeybindName(const EGameBinds Keybind) {
    switch (Keybind) {
    case GB_MoveDodge:
        return "Dodge";
    case GB_MoveJump_SwimUp_FlyUp:
        return "Jump";
    case GB_MoveAboutFace:
        return "About Face";
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
        return "Heal";
    case GB_SkillUtility1:
        return "Utility 1";
    case GB_SkillUtility2:
        return "Utility 2";
    case GB_SkillUtility3:
        return "Utility 3";
    case GB_SkillElite:
        return "Elite";
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
    case GB_SkillWeaponSwap:
        return "Weapon Swap";
    case GB_SkillSpecialAction:
        return "Special Action";
    case GB_TargetTake:
        return "Take Target";
    case GB_TargetCall:
        return "Call Target";
    case GB_TargetAlert:
        return "Alert";
    case GB_TargetCallLocal:
        return "Call Local Target";
    case GB_TargetTakeLocal:
        return "Take Local Target";
    case GB_TargetEnemyNearest:
        return "Nearest Enemy";
    case GB_TargetEnemyNext:
        return "Next Enemy";
    case GB_TargetEnemyPrev:
        return "Previous Enemy";
    case GB_TargetAllyNearest:
        return "Nearest Ally";
    case GB_TargetAllyNext:
        return "Next Ally";
    case GB_TargetAllyPrev:
        return "Previous Ally";
    case GB_TargetLock:
        return "Lock Target";
    case GB_TargetSnapGroundTarget:
        return "Snap Ground Target";
    case GB_TargetSnapGroundTargetToggle:
        return "Snap Ground Target Toggle";
    case GB_TargetAutoTargetingDisable:
        return "Auto-Targeting Disable";
    case GB_TargetAutoTargetingToggle:
        return "Auto-Targeting Toggle";
    case GB_TargetAllyTargetingMode:
        return "Ally Targeting Mode";
    case GB_TargetAllyTargetingModeToggle:
        return "Ally Targeting Mode Toggle";
    case GB_UiCommerce:
        return "Trading Post";
    case GB_UiContacts:
        return "Contacts";
    case GB_UiGuild:
        return "Guild";
    case GB_UiHero:
        return "Hero";
    case GB_UiInventory:
        return "Inventory";
    case GB_UiKennel:
        return "Pets";
    case GB_UiLogout:
        return "Logout";
    case GB_UiMail:
        return "Mail";
    case GB_UiOptions:
        return "Options";
    case GB_UiParty:
        return "Party";
    case GB_UiPvp:
        return "PvP";
    case GB_UiPvpBuild:
        return "PvP Build";
    case GB_UiScoreboard:
        return "Scoreboard";
    case GB_UiSeasonalObjectivesShop:
        return "Wizard's Vault";
    case GB_UiInformation:
        return "Information";
    case GB_UiChatToggle:
        return "Chat Toggle";
    case GB_UiChatCommand:
        return "Chat Command";
    case GB_UiChatFocus:
        return "Chat Focus";
    case GB_UiChatReply:
        return "Chat Reply";
    case GB_UiToggle:
        return "UI Toggle";
    case GB_UiSquadBroadcastChatToggle:
        return "Squad Broadcast Chat Toggle";
    case GB_UiSquadBroadcastChatCommand:
        return "Squad Broadcast Chat Command";
    case GB_UiSquadBroadcastChatFocus:
        return "Squad Broadcast Chat Focus";
    case GB_CameraFree:
        return "Free Camera";
    case GB_CameraZoomIn:
        return "Zoom In";
    case GB_CameraZoomOut:
        return "Zoom Out";
    case GB_CameraReverse:
        return "Reverse Camera";
    case GB_CameraActionMode:
        return "Action Mode";
    case GB_CameraActionModeDisable:
        return "Action Mode Disable";
    case GB_ScreenshotNormal:
        return "Normal Screenshot";
    case GB_ScreenshotStereoscopic:
        return "Stereoscopic Screenshot";
    case GB_MapToggle:
        return "Toggle Map";
    case GB_MapFocusPlayer:
        return "Focus Player";
    case GB_MapFloorDown:
        return "Floor Down";
    case GB_MapFloorUp:
        return "Floor Up";
    case GB_MapZoomIn:
        return "Map Zoom In";
    case GB_MapZoomOut:
        return "Map Zoom Out";
    case GB_SpumoniToggle:
        return "Mount / Dismount";
    case GB_SpumoniMovement:
        return "Mount Ability 1";
    case GB_SpumoniSecondaryMovement:
        return "Mount Ability 2";
    case GB_SpumoniMAM01:
        return "Raptor";
    case GB_SpumoniMAM02:
        return "Springer";
    case GB_SpumoniMAM03:
        return "Skimmer";
    case GB_SpumoniMAM04:
        return "Jackal";
    case GB_SpumoniMAM05:
        return "Griffon";
    case GB_SpumoniMAM06:
        return "Roller Beetle";
    case GB_SpumoniMAM07:
        return "Warclaw";
    case GB_SpumoniMAM08:
        return "Skyscale";
    case GB_SpumoniMAM09:
        return "Siege Turtle";
    case GB_MasteryAccess:
        return "Mastery";
    case GB_MasteryAccess01:
        return "Fishing";
    case GB_MasteryAccess02:
        return "Skiff";
    case GB_MasteryAccess03:
        return "Jade Bot Waypoint";
    case GB_MasteryAccess04:
        return "Rift Scan";
    case GB_MasteryAccess05:
        return "Skyscale";
    case GB_MasteryAccess06:
        return "Homestead Doorway";
    case GB_MiscAoELoot:
        return "AoE Loot";
    case GB_MiscInteract:
        return "Interact";
    case GB_MiscShowEnemies:
        return "Show Enemies";
    case GB_MiscShowAllies:
        return "Show Allies";
    case GB_MiscCombatStance:
        return "Stow / Draw Weapons";
    case GB_MiscToggleLanguage:
        return "Toggle Language";
    case GB_MiscTogglePetCombat:
        return "Toggle Pet Combat";
    case GB_MiscToggleFullScreen:
        return "Toggle Fullscreen";
    case GB_MiscToggleDecorationMode:
        return "Toggle Decorate Mode";
    case GB_ToyUseDefault:
        return "Default Novelty";
    case GB_ToyUseSlot1:
        return "Chair";
    case GB_ToyUseSlot2:
        return "Instrument";
    case GB_ToyUseSlot3:
        return "Held Item";
    case GB_ToyUseSlot4:
        return "Toy";
    case GB_ToyUseSlot5:
        return "Tonic";
    case GB_Loadout1:
        return "Build Template 1";
    case GB_Loadout2:
        return "Build Template 2";
    case GB_Loadout3:
        return "Build Template 3";
    case GB_Loadout4:
        return "Build Template 4";
    case GB_Loadout5:
        return "Build Template 5";
    case GB_Loadout6:
        return "Build Template 6";
    case GB_Loadout7:
        return "Build Template 7";
    case GB_Loadout8:
        return "Build Template 8";
    case GB_Loadout9:
        return "Build Template 9";
    case GB_GearLoadout1:
        return "Equipment Template 1";
    case GB_GearLoadout2:
        return "Equipment Template 2";
    case GB_GearLoadout3:
        return "Equipment Template 3";
    case GB_GearLoadout4:
        return "Equipment Template 4";
    case GB_GearLoadout5:
        return "Equipment Template 5";
    case GB_GearLoadout6:
        return "Equipment Template 6";
    case GB_GearLoadout7:
        return "Equipment Template 7";
    case GB_GearLoadout8:
        return "Equipment Template 8";
    case GB_GearLoadout9:
        return "Equipment Template 9";
    default:
        return "Unknown Bind";
    }
}

std::string IngameKeybindToString(const EGameBinds Keybind) {
    switch (Keybind) {
    case GB_MoveDodge:
        return "GB_MoveDodge";
    case GB_MoveJump_SwimUp_FlyUp:
        return "GB_MoveJump_SwimUp_FlyUp";
    case GB_MoveAboutFace:
        return "GB_MoveAboutFace";
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
    case GB_SkillWeaponSwap:
        return "GB_SkillWeaponSwap";
    case GB_SkillSpecialAction:
        return "GB_SkillSpecialAction";
    case GB_TargetTake:
        return "GB_TargetTake";
    case GB_TargetCall:
        return "GB_TargetCall";
    case GB_TargetAlert:
        return "GB_TargetAlert";
    case GB_TargetCallLocal:
        return "GB_TargetCallLocal";
    case GB_TargetTakeLocal:
        return "GB_TargetTakeLocal";
    case GB_TargetEnemyNearest:
        return "GB_TargetEnemyNearest";
    case GB_TargetEnemyNext:
        return "GB_TargetEnemyNext";
    case GB_TargetEnemyPrev:
        return "GB_TargetEnemyPrev";
    case GB_TargetAllyNearest:
        return "GB_TargetAllyNearest";
    case GB_TargetAllyNext:
        return "GB_TargetAllyNext";
    case GB_TargetAllyPrev:
        return "GB_TargetAllyPrev";
    case GB_TargetLock:
        return "GB_TargetLock";
    case GB_TargetSnapGroundTarget:
        return "GB_TargetSnapGroundTarget";
    case GB_TargetSnapGroundTargetToggle:
        return "GB_TargetSnapGroundTargetToggle";
    case GB_TargetAutoTargetingDisable:
        return "GB_TargetAutoTargetingDisable";
    case GB_TargetAutoTargetingToggle:
        return "GB_TargetAutoTargetingToggle";
    case GB_TargetAllyTargetingMode:
        return "GB_TargetAllyTargetingMode";
    case GB_TargetAllyTargetingModeToggle:
        return "GB_TargetAllyTargetingModeToggle";
    case GB_UiCommerce:
        return "GB_UiCommerce";
    case GB_UiContacts:
        return "GB_UiContacts";
    case GB_UiGuild:
        return "GB_UiGuild";
    case GB_UiHero:
        return "GB_UiHero";
    case GB_UiInventory:
        return "GB_UiInventory";
    case GB_UiKennel:
        return "GB_UiKennel";
    case GB_UiLogout:
        return "GB_UiLogout";
    case GB_UiMail:
        return "GB_UiMail";
    case GB_UiOptions:
        return "GB_UiOptions";
    case GB_UiParty:
        return "GB_UiParty";
    case GB_UiPvp:
        return "GB_UiPvp";
    case GB_UiPvpBuild:
        return "GB_UiPvpBuild";
    case GB_UiScoreboard:
        return "GB_UiScoreboard";
    case GB_UiSeasonalObjectivesShop:
        return "GB_UiSeasonalObjectivesShop";
    case GB_UiInformation:
        return "GB_UiInformation";
    case GB_UiChatToggle:
        return "GB_UiChatToggle";
    case GB_UiChatCommand:
        return "GB_UiChatCommand";
    case GB_UiChatFocus:
        return "GB_UiChatFocus";
    case GB_UiChatReply:
        return "GB_UiChatReply";
    case GB_UiToggle:
        return "GB_UiToggle";
    case GB_UiSquadBroadcastChatToggle:
        return "GB_UiSquadBroadcastChatToggle";
    case GB_UiSquadBroadcastChatCommand:
        return "GB_UiSquadBroadcastChatCommand";
    case GB_UiSquadBroadcastChatFocus:
        return "GB_UiSquadBroadcastChatFocus";
    case GB_CameraFree:
        return "GB_CameraFree";
    case GB_CameraZoomIn:
        return "GB_CameraZoomIn";
    case GB_CameraZoomOut:
        return "GB_CameraZoomOut";
    case GB_CameraReverse:
        return "GB_CameraReverse";
    case GB_CameraActionMode:
        return "GB_CameraActionMode";
    case GB_CameraActionModeDisable:
        return "GB_CameraActionModeDisable";
    case GB_ScreenshotNormal:
        return "GB_ScreenshotNormal";
    case GB_ScreenshotStereoscopic:
        return "GB_ScreenshotStereoscopic";
    case GB_MapToggle:
        return "GB_MapToggle";
    case GB_MapFocusPlayer:
        return "GB_MapFocusPlayer";
    case GB_MapFloorDown:
        return "GB_MapFloorDown";
    case GB_MapFloorUp:
        return "GB_MapFloorUp";
    case GB_MapZoomIn:
        return "GB_MapZoomIn";
    case GB_MapZoomOut:
        return "GB_MapZoomOut";
    case GB_SpumoniToggle:
        return "GB_SpumoniToggle";
    case GB_SpumoniMovement:
        return "GB_SpumoniMovement";
    case GB_SpumoniSecondaryMovement:
        return "GB_SpumoniSecondaryMovement";
    case GB_SpumoniMAM01:
        return "GB_SpumoniMAM01";
    case GB_SpumoniMAM02:
        return "GB_SpumoniMAM02";
    case GB_SpumoniMAM03:
        return "GB_SpumoniMAM03";
    case GB_SpumoniMAM04:
        return "GB_SpumoniMAM04";
    case GB_SpumoniMAM05:
        return "GB_SpumoniMAM05";
    case GB_SpumoniMAM06:
        return "GB_SpumoniMAM06";
    case GB_SpumoniMAM07:
        return "GB_SpumoniMAM07";
    case GB_SpumoniMAM08:
        return "GB_SpumoniMAM08";
    case GB_SpumoniMAM09:
        return "GB_SpumoniMAM09";
    case GB_MasteryAccess:
        return "GB_MasteryAccess";
    case GB_MasteryAccess01:
        return "GB_MasteryAccess01";
    case GB_MasteryAccess02:
        return "GB_MasteryAccess02";
    case GB_MasteryAccess03:
        return "GB_MasteryAccess03";
    case GB_MasteryAccess04:
        return "GB_MasteryAccess04";
    case GB_MasteryAccess05:
        return "GB_MasteryAccess05";
    case GB_MasteryAccess06:
        return "GB_MasteryAccess06";
    case GB_MiscAoELoot:
        return "GB_MiscAoELoot";
    case GB_MiscInteract:
        return "GB_MiscInteract";
    case GB_MiscShowEnemies:
        return "GB_MiscShowEnemies";
    case GB_MiscShowAllies:
        return "GB_MiscShowAllies";
    case GB_MiscCombatStance:
        return "GB_MiscCombatStance";
    case GB_MiscToggleLanguage:
        return "GB_MiscToggleLanguage";
    case GB_MiscTogglePetCombat:
        return "GB_MiscTogglePetCombat";
    case GB_MiscToggleFullScreen:
        return "GB_MiscToggleFullScreen";
    case GB_MiscToggleDecorationMode:
        return "GB_MiscToggleDecorationMode";
    case GB_ToyUseDefault:
        return "GB_ToyUseDefault";
    case GB_ToyUseSlot1:
        return "GB_ToyUseSlot1";
    case GB_ToyUseSlot2:
        return "GB_ToyUseSlot2";
    case GB_ToyUseSlot3:
        return "GB_ToyUseSlot3";
    case GB_ToyUseSlot4:
        return "GB_ToyUseSlot4";
    case GB_ToyUseSlot5:
        return "GB_ToyUseSlot5";
    case GB_Loadout1:
        return "GB_Loadout1";
    case GB_Loadout2:
        return "GB_Loadout2";
    case GB_Loadout3:
        return "GB_Loadout3";
    case GB_Loadout4:
        return "GB_Loadout4";
    case GB_Loadout5:
        return "GB_Loadout5";
    case GB_Loadout6:
        return "GB_Loadout6";
    case GB_Loadout7:
        return "GB_Loadout7";
    case GB_Loadout8:
        return "GB_Loadout8";
    case GB_Loadout9:
        return "GB_Loadout9";
    case GB_GearLoadout1:
        return "GB_GearLoadout1";
    case GB_GearLoadout2:
        return "GB_GearLoadout2";
    case GB_GearLoadout3:
        return "GB_GearLoadout3";
    case GB_GearLoadout4:
        return "GB_GearLoadout4";
    case GB_GearLoadout5:
        return "GB_GearLoadout5";
    case GB_GearLoadout6:
        return "GB_GearLoadout6";
    case GB_GearLoadout7:
        return "GB_GearLoadout7";
    case GB_GearLoadout8:
        return "GB_GearLoadout8";
    case GB_GearLoadout9:
        return "GB_GearLoadout9";
    default:
        return "GB_SkillWeapon1";
    }
}

EGameBinds StringToIngameKeybind(const std::string &KeybindString) {
    if (KeybindString == "GB_MoveDodge")
        return GB_MoveDodge;
    if (KeybindString == "GB_MoveJump_SwimUp_FlyUp")
        return GB_MoveJump_SwimUp_FlyUp;
    if (KeybindString == "GB_MoveAboutFace")
        return GB_MoveAboutFace;
    if (KeybindString == "GB_SkillWeapon1")
        return GB_SkillWeapon1;
    if (KeybindString == "GB_SkillWeapon2")
        return GB_SkillWeapon2;
    if (KeybindString == "GB_SkillWeapon3")
        return GB_SkillWeapon3;
    if (KeybindString == "GB_SkillWeapon4")
        return GB_SkillWeapon4;
    if (KeybindString == "GB_SkillWeapon5")
        return GB_SkillWeapon5;
    if (KeybindString == "GB_SkillHeal")
        return GB_SkillHeal;
    if (KeybindString == "GB_SkillUtility1")
        return GB_SkillUtility1;
    if (KeybindString == "GB_SkillUtility2")
        return GB_SkillUtility2;
    if (KeybindString == "GB_SkillUtility3")
        return GB_SkillUtility3;
    if (KeybindString == "GB_SkillElite")
        return GB_SkillElite;
    if (KeybindString == "GB_SkillProfession1")
        return GB_SkillProfession1;
    if (KeybindString == "GB_SkillProfession2")
        return GB_SkillProfession2;
    if (KeybindString == "GB_SkillProfession3")
        return GB_SkillProfession3;
    if (KeybindString == "GB_SkillProfession4")
        return GB_SkillProfession4;
    if (KeybindString == "GB_SkillProfession5")
        return GB_SkillProfession5;
    if (KeybindString == "GB_SkillProfession6")
        return GB_SkillProfession6;
    if (KeybindString == "GB_SkillProfession7")
        return GB_SkillProfession7;
    if (KeybindString == "GB_SkillWeaponSwap")
        return GB_SkillWeaponSwap;
    if (KeybindString == "GB_SkillSpecialAction")
        return GB_SkillSpecialAction;
    if (KeybindString == "GB_TargetTake")
        return GB_TargetTake;
    if (KeybindString == "GB_TargetCall")
        return GB_TargetCall;
    if (KeybindString == "GB_TargetAlert")
        return GB_TargetAlert;
    if (KeybindString == "GB_TargetCallLocal")
        return GB_TargetCallLocal;
    if (KeybindString == "GB_TargetTakeLocal")
        return GB_TargetTakeLocal;
    if (KeybindString == "GB_TargetEnemyNearest")
        return GB_TargetEnemyNearest;
    if (KeybindString == "GB_TargetEnemyNext")
        return GB_TargetEnemyNext;
    if (KeybindString == "GB_TargetEnemyPrev")
        return GB_TargetEnemyPrev;
    if (KeybindString == "GB_TargetAllyNearest")
        return GB_TargetAllyNearest;
    if (KeybindString == "GB_TargetAllyNext")
        return GB_TargetAllyNext;
    if (KeybindString == "GB_TargetAllyPrev")
        return GB_TargetAllyPrev;
    if (KeybindString == "GB_TargetLock")
        return GB_TargetLock;
    if (KeybindString == "GB_TargetSnapGroundTarget")
        return GB_TargetSnapGroundTarget;
    if (KeybindString == "GB_TargetSnapGroundTargetToggle")
        return GB_TargetSnapGroundTargetToggle;
    if (KeybindString == "GB_TargetAutoTargetingDisable")
        return GB_TargetAutoTargetingDisable;
    if (KeybindString == "GB_TargetAutoTargetingToggle")
        return GB_TargetAutoTargetingToggle;
    if (KeybindString == "GB_TargetAllyTargetingMode")
        return GB_TargetAllyTargetingMode;
    if (KeybindString == "GB_TargetAllyTargetingModeToggle")
        return GB_TargetAllyTargetingModeToggle;
    if (KeybindString == "GB_UiCommerce")
        return GB_UiCommerce;
    if (KeybindString == "GB_UiContacts")
        return GB_UiContacts;
    if (KeybindString == "GB_UiGuild")
        return GB_UiGuild;
    if (KeybindString == "GB_UiHero")
        return GB_UiHero;
    if (KeybindString == "GB_UiInventory")
        return GB_UiInventory;
    if (KeybindString == "GB_UiKennel")
        return GB_UiKennel;
    if (KeybindString == "GB_UiLogout")
        return GB_UiLogout;
    if (KeybindString == "GB_UiMail")
        return GB_UiMail;
    if (KeybindString == "GB_UiOptions")
        return GB_UiOptions;
    if (KeybindString == "GB_UiParty")
        return GB_UiParty;
    if (KeybindString == "GB_UiPvp")
        return GB_UiPvp;
    if (KeybindString == "GB_UiPvpBuild")
        return GB_UiPvpBuild;
    if (KeybindString == "GB_UiScoreboard")
        return GB_UiScoreboard;
    if (KeybindString == "GB_UiSeasonalObjectivesShop")
        return GB_UiSeasonalObjectivesShop;
    if (KeybindString == "GB_UiInformation")
        return GB_UiInformation;
    if (KeybindString == "GB_UiChatToggle")
        return GB_UiChatToggle;
    if (KeybindString == "GB_UiChatCommand")
        return GB_UiChatCommand;
    if (KeybindString == "GB_UiChatFocus")
        return GB_UiChatFocus;
    if (KeybindString == "GB_UiChatReply")
        return GB_UiChatReply;
    if (KeybindString == "GB_UiToggle")
        return GB_UiToggle;
    if (KeybindString == "GB_UiSquadBroadcastChatToggle")
        return GB_UiSquadBroadcastChatToggle;
    if (KeybindString == "GB_UiSquadBroadcastChatCommand")
        return GB_UiSquadBroadcastChatCommand;
    if (KeybindString == "GB_UiSquadBroadcastChatFocus")
        return GB_UiSquadBroadcastChatFocus;
    if (KeybindString == "GB_CameraFree")
        return GB_CameraFree;
    if (KeybindString == "GB_CameraZoomIn")
        return GB_CameraZoomIn;
    if (KeybindString == "GB_CameraZoomOut")
        return GB_CameraZoomOut;
    if (KeybindString == "GB_CameraReverse")
        return GB_CameraReverse;
    if (KeybindString == "GB_CameraActionMode")
        return GB_CameraActionMode;
    if (KeybindString == "GB_CameraActionModeDisable")
        return GB_CameraActionModeDisable;
    if (KeybindString == "GB_ScreenshotNormal")
        return GB_ScreenshotNormal;
    if (KeybindString == "GB_ScreenshotStereoscopic")
        return GB_ScreenshotStereoscopic;
    if (KeybindString == "GB_MapToggle")
        return GB_MapToggle;
    if (KeybindString == "GB_MapFocusPlayer")
        return GB_MapFocusPlayer;
    if (KeybindString == "GB_MapFloorDown")
        return GB_MapFloorDown;
    if (KeybindString == "GB_MapFloorUp")
        return GB_MapFloorUp;
    if (KeybindString == "GB_MapZoomIn")
        return GB_MapZoomIn;
    if (KeybindString == "GB_MapZoomOut")
        return GB_MapZoomOut;
    if (KeybindString == "GB_SpumoniToggle")
        return GB_SpumoniToggle;
    if (KeybindString == "GB_SpumoniMovement")
        return GB_SpumoniMovement;
    if (KeybindString == "GB_SpumoniSecondaryMovement")
        return GB_SpumoniSecondaryMovement;
    if (KeybindString == "GB_SpumoniMAM01")
        return GB_SpumoniMAM01;
    if (KeybindString == "GB_SpumoniMAM02")
        return GB_SpumoniMAM02;
    if (KeybindString == "GB_SpumoniMAM03")
        return GB_SpumoniMAM03;
    if (KeybindString == "GB_SpumoniMAM04")
        return GB_SpumoniMAM04;
    if (KeybindString == "GB_SpumoniMAM05")
        return GB_SpumoniMAM05;
    if (KeybindString == "GB_SpumoniMAM06")
        return GB_SpumoniMAM06;
    if (KeybindString == "GB_SpumoniMAM07")
        return GB_SpumoniMAM07;
    if (KeybindString == "GB_SpumoniMAM08")
        return GB_SpumoniMAM08;
    if (KeybindString == "GB_SpumoniMAM09")
        return GB_SpumoniMAM09;
    if (KeybindString == "GB_MasteryAccess")
        return GB_MasteryAccess;
    if (KeybindString == "GB_MasteryAccess01")
        return GB_MasteryAccess01;
    if (KeybindString == "GB_MasteryAccess02")
        return GB_MasteryAccess02;
    if (KeybindString == "GB_MasteryAccess03")
        return GB_MasteryAccess03;
    if (KeybindString == "GB_MasteryAccess04")
        return GB_MasteryAccess04;
    if (KeybindString == "GB_MasteryAccess05")
        return GB_MasteryAccess05;
    if (KeybindString == "GB_MasteryAccess06")
        return GB_MasteryAccess06;
    if (KeybindString == "GB_MiscAoELoot")
        return GB_MiscAoELoot;
    if (KeybindString == "GB_MiscInteract")
        return GB_MiscInteract;
    if (KeybindString == "GB_MiscShowEnemies")
        return GB_MiscShowEnemies;
    if (KeybindString == "GB_MiscShowAllies")
        return GB_MiscShowAllies;
    if (KeybindString == "GB_MiscCombatStance")
        return GB_MiscCombatStance;
    if (KeybindString == "GB_MiscToggleLanguage")
        return GB_MiscToggleLanguage;
    if (KeybindString == "GB_MiscTogglePetCombat")
        return GB_MiscTogglePetCombat;
    if (KeybindString == "GB_MiscToggleFullScreen")
        return GB_MiscToggleFullScreen;
    if (KeybindString == "GB_MiscToggleDecorationMode")
        return GB_MiscToggleDecorationMode;
    if (KeybindString == "GB_ToyUseDefault")
        return GB_ToyUseDefault;
    if (KeybindString == "GB_ToyUseSlot1")
        return GB_ToyUseSlot1;
    if (KeybindString == "GB_ToyUseSlot2")
        return GB_ToyUseSlot2;
    if (KeybindString == "GB_ToyUseSlot3")
        return GB_ToyUseSlot3;
    if (KeybindString == "GB_ToyUseSlot4")
        return GB_ToyUseSlot4;
    if (KeybindString == "GB_ToyUseSlot5")
        return GB_ToyUseSlot5;
    if (KeybindString == "GB_Loadout1")
        return GB_Loadout1;
    if (KeybindString == "GB_Loadout2")
        return GB_Loadout2;
    if (KeybindString == "GB_Loadout3")
        return GB_Loadout3;
    if (KeybindString == "GB_Loadout4")
        return GB_Loadout4;
    if (KeybindString == "GB_Loadout5")
        return GB_Loadout5;
    if (KeybindString == "GB_Loadout6")
        return GB_Loadout6;
    if (KeybindString == "GB_Loadout7")
        return GB_Loadout7;
    if (KeybindString == "GB_Loadout8")
        return GB_Loadout8;
    if (KeybindString == "GB_Loadout9")
        return GB_Loadout9;
    if (KeybindString == "GB_GearLoadout1")
        return GB_GearLoadout1;
    if (KeybindString == "GB_GearLoadout2")
        return GB_GearLoadout2;
    if (KeybindString == "GB_GearLoadout3")
        return GB_GearLoadout3;
    if (KeybindString == "GB_GearLoadout4")
        return GB_GearLoadout4;
    if (KeybindString == "GB_GearLoadout5")
        return GB_GearLoadout5;
    if (KeybindString == "GB_GearLoadout6")
        return GB_GearLoadout6;
    if (KeybindString == "GB_GearLoadout7")
        return GB_GearLoadout7;
    if (KeybindString == "GB_GearLoadout8")
        return GB_GearLoadout8;
    if (KeybindString == "GB_GearLoadout9")
        return GB_GearLoadout9;

    return GB_SkillWeapon1;
}

const char *GetMouseButtonName(const EMouseButton MouseButton) {
    switch (MouseButton) {
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

std::string MouseButtonToString(const EMouseButton MouseButton) {
    switch (MouseButton) {
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

EMouseButton StringToMouseButton(const std::string &MouseButtonString) {
    if (MouseButtonString == "MB_Left")
        return EMouseButton::Left;
    if (MouseButtonString == "MB_Right")
        return EMouseButton::Right;
    if (MouseButtonString == "MB_Middle")
        return EMouseButton::Middle;
    if (MouseButtonString == "MB_X1")
        return EMouseButton::X1;
    if (MouseButtonString == "MB_X2")
        return EMouseButton::X2;

    return EMouseButton::Left;
}

std::string MousePositionTypeToString(const EMousePositionType MousePositionType) {
    return (MousePositionType == EMousePositionType::Absolute) ? "Absolute" : "Relative";
}

EMousePositionType StringToMousePositionType(const std::string &MousePositionTypeString) {
    return (MousePositionTypeString == "Absolute") ? EMousePositionType::Absolute : EMousePositionType::Relative;
}