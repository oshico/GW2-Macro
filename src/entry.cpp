#include "./imgui/imgui.h"
#include "./nexus/Nexus.h"
#include "game_mode_check.h"
#include "keybind_manager.h"
#include "macro_executor.h"
#include "macro_manager.h"
#include "macro_save.h"
#include "module.h"
#include "nlohmann/json.hpp"
#include "resource.h"
#include "shared.h"
#include "string_conversions.h"
#include <commdlg.h>
#include <fstream>
#include <string>
#include <windows.h>

void RenderMainWindow() {
    if (!ShowMainWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(650, 420), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Manager", &ShowMainWindow)) {
        ImGui::BeginChild("GameModeStatusBanner", ImVec2(0, 40), true);
        {
            ImVec4 GameModeStatusColor;
            const char *GameModeStatusText;

            if (IsPlayerInCompetitiveGameMode()) {
                GameModeStatusColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                GameModeStatusText = "COMPETITIVE MODE (PvP/WvW) - MACROS DISABLED";
            } else {
                GameModeStatusColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
                GameModeStatusText = "PvE MODE - MACROS ENABLED";
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
            ImGui::TextColored(GameModeStatusColor, "%s", GameModeStatusText);
        }
        ImGui::EndChild();

        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Macro Slots (10 total)");
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign keybinds through Nexus settings");

        ImGui::Spacing();

        if (ImGui::BeginChild("MacroList", ImVec2(0, 250), true)) {
            if (ImGui::BeginTable("MacrosTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableSetupColumn("On", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < Macros.size(); ++i) {
                    Macro &Macro = Macros[i];
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    bool Enabled = Macro.Enabled;
                    if (ImGui::Checkbox(("##Enabled" + std::to_string(i)).c_str(), &Enabled)) {
                        Macro.Enabled = Enabled;
                        SaveMacrosToJson();
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(Macro.Name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d actions", static_cast<int>(Macro.Actions.size()));

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::SmallButton(("Edit##" + std::to_string(i)).c_str()))
                        OpenMacroEditor(static_cast<int>(i));

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::SmallButton(("Delete##" + std::to_string(i)).c_str())) {
                        DeleteMacro(i);
                        --i;
                        SaveMacrosToJson();
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();

        ImGui::Spacing();

        if (ImGui::Button("Create New Macro", ImVec2(-1, 0)))
            OpenMacroEditor();

        if (ImGui::Button("Import / Export", ImVec2(-1, 0)))
            ShowSaveWindow = true;
    }
    ImGui::End();
}

void AddonOptions() {
    ImGui::SetCurrentContext(
        static_cast<ImGuiContext *>(ApiDefinition->ImguiContext));

    ImGui::Text("Macro Keybind Manager v2026.06.15.1400");
    ImGui::TextColored(
        ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Execute sequences of game actions with precise timing control");

    ImGui::Spacing();

    ImGui::Text("Default Keybinds:");
    ImGui::BulletText("CTRL+SHIFT+K - Toggle Macro Manager");
    ImGui::BulletText("CTRL+SHIFT+X - Kill All Running Macros");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Assign macro keybinds through Nexus Input settings");

    ImGui::Spacing();

    {
        int ActiveMacros = 0;
        int TotalMacroActions = 0;

        for (const auto &Macro : Macros) {
            if (!Macro.Actions.empty() && Macro.Name != "Empty")
                ++ActiveMacros;
            TotalMacroActions += static_cast<int>(Macro.Actions.size());
        }

        ImGui::Text("Active Macros: %d / 10", ActiveMacros);
        ImGui::Text("Total Actions: %d", TotalMacroActions);
    }

    ImGui::Spacing();

    if (ImGui::Button("Open Macro Manager"))
        ShowMainWindow = true;

    ImGui::SameLine();
    if (ImGui::Button("Create New Macro"))
        OpenMacroEditor();

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Usage Tips")) {
        ImGui::BulletText("Each macro can contain unlimited actions");
        ImGui::BulletText("Use delays between actions for timing-sensitive sequences");
        ImGui::BulletText("Press and Release actions allow for held key combinations");
        ImGui::BulletText("Mouse position support for clicking at specific coordinates");
        ImGui::BulletText("Disable macros to prevent accidental execution");
    }

    if (ImGui::CollapsingHeader("Macro Usage Policy")) {
        ImGui::TextWrapped("Attended macro use is permitted as long as it is not exploitative, "
                           "and as long as it does not provide the user with an unfair advantage. "
                           "Unattended macro use is prohibited.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Example of Allowed Macros:");
        ImGui::BulletText("Bind dodge and jump to a single key");
        ImGui::BulletText("Auto-clicker to open or consume items");
        ImGui::BulletText("Music macros for in-game performance (attended)");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Example of Prohibited Macros:");
        ImGui::BulletText("Cannot interfere with other players");
        ImGui::BulletText("Cannot act across multiple accounts");
        ImGui::BulletText("No automation of skills while away");
        ImGui::BulletText("No skill chains in competitive modes");
        ImGui::BulletText("No automation of events (e.g., Bell Choir)");
        ImGui::BulletText("No AFK farming of items/currencies");
        ImGui::BulletText("No full character automation (botting)");
        ImGui::BulletText("Each macro must represent one user action");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Important:");
        ImGui::TextWrapped("This addon automatically disables macros in PvP/WvW "
                           "to comply with ANET ToS policy, but you are still responsible "
                           "for following the game's ToS.");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Warning:");
        ImGui::TextWrapped("Violation of these policies may result in account restrictions.");
    }
}

struct KeybindEntry {
    const char *Name;
    EGameBinds Value;
};

struct KeybindCategory {
    const char *Name;
    const KeybindEntry *Entries;
    int Count;
};

static bool KeybindItemsGetter(void *data, int idx, const char **out_text) {
    *out_text = static_cast<const KeybindEntry *>(data)[idx].Name;
    return true;
}

void RenderMacroEditorWindow() {
    if (!ShowEditorWindow)
        return;

    static char MacroName[128] = "";
    static std::vector<KeybindAction> NewMacroActions;
    static int LastSelectedMacroIndex = -2;
    static int SelectedMacroSlot = 0;

    if (SelectedMacroIndex != LastSelectedMacroIndex) {
        if (SelectedMacroIndex >= 0 && SelectedMacroIndex < static_cast<int>(Macros.size())) {
            const Macro &Macro = Macros[SelectedMacroIndex];
            strncpy_s(MacroName, sizeof(MacroName), Macro.Name.c_str(), _TRUNCATE);
            NewMacroActions = Macro.Actions;

            if (const std::string id = Macro.Identifier; id.find("MACRO_") != std::string::npos)
                SelectedMacroSlot = std::stoi(id.substr(6)) - 1;
        } else {
            strcpy_s(MacroName, sizeof(MacroName), "New Macro");
            NewMacroActions.clear();
            SelectedMacroSlot = 0;
        }
        LastSelectedMacroIndex = SelectedMacroIndex;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 650), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Editor", &ShowEditorWindow)) {
        ImGui::InputText("Macro Name", MacroName, sizeof(MacroName));

        ImGui::Separator();
        const char *MacroSlotNames[10] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
        ImGui::Combo("Macro Slot", &SelectedMacroSlot, MacroSlotNames, 10);

        ImGui::Separator();
        ImGui::Text("Action Sequence:");
        if (ImGui::BeginChild("ActionList", ImVec2(0, 220), true)) {
            for (size_t i = 0; i < NewMacroActions.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));

                ImGui::Text("%d.", static_cast<int>(i) + 1);
                ImGui::SameLine();

                if (NewMacroActions[i].MacroInputType == EMacroInputType::MouseMove) {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "MOVE");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f), "to (%d, %d) [%s]", NewMacroActions[i].MousePosition.x, NewMacroActions[i].MousePosition.y, NewMacroActions[i].MousePosition.MousePositionType == EMousePositionType::Absolute ? "Abs" : "Rel");
                } else {
                    ImGui::TextColored(NewMacroActions[i].IsKeybindDown ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "%s", NewMacroActions[i].IsKeybindDown ? "PRESS" : "RELEASE");

                    ImGui::SameLine();

                    if (NewMacroActions[i].MacroInputType == EMacroInputType::GameBind) {
                        ImGui::TextUnformatted(GetKeybindName(NewMacroActions[i].GameBind));
                    } else {
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f), "%s", GetMouseButtonName(NewMacroActions[i].MouseButton));

                        if (NewMacroActions[i].MoveBeforeMouseClick) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "@ (%d, %d)", NewMacroActions[i].MousePosition.x, NewMacroActions[i].MousePosition.y);
                        }
                    }
                }

                if (NewMacroActions[i].DelayMilliseconds > 0) {
                    ImGui::SameLine();
                    ImGui::Text("(%dms delay)", NewMacroActions[i].DelayMilliseconds);
                }

                ImGui::SameLine(ImGui::GetWindowWidth() - 60);
                if (ImGui::SmallButton("X")) {
                    NewMacroActions.erase(NewMacroActions.begin() + i);
                    --i;
                }

                ImGui::PopID();
            }

            if (NewMacroActions.empty())
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No actions added yet.");
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Add Action:");

        static int MacroInputTypeIndex = 0;
        const char *MacroInputTypes[] = {"Keyboard/Game Action", "Mouse Button", "Mouse Move"};
        ImGui::Combo("Input Type", &MacroInputTypeIndex, MacroInputTypes, 3);

        static EGameBinds SelectedKeybind = GB_SkillWeapon1;
        static auto SelectedMouseButton = EMouseButton::Left;
        static bool IsKeybindDown = true;
        static int DelayMilliseconds = 0;
        static int MouseX = 0;
        static int MouseY = 0;
        static int MousePositionTypeIndex = 0;
        static bool UseMousePosition = false;

        if (MacroInputTypeIndex == 0) {
            static constexpr KeybindEntry MovementEntries[] = {
                {"Dodge", GB_MoveDodge},
                {"Jump / Swim Up / Fly Up", GB_MoveJump_SwimUp_FlyUp},
                {"About Face", GB_MoveAboutFace},
            };
            static constexpr KeybindEntry SkillsEntries[] = {
                {"Weapon 1", GB_SkillWeapon1},
                {"Weapon 2", GB_SkillWeapon2},
                {"Weapon 3", GB_SkillWeapon3},
                {"Weapon 4", GB_SkillWeapon4},
                {"Weapon 5", GB_SkillWeapon5},
                {"Heal", GB_SkillHeal},
                {"Utility 1", GB_SkillUtility1},
                {"Utility 2", GB_SkillUtility2},
                {"Utility 3", GB_SkillUtility3},
                {"Elite", GB_SkillElite},
                {"Profession 1", GB_SkillProfession1},
                {"Profession 2", GB_SkillProfession2},
                {"Profession 3", GB_SkillProfession3},
                {"Profession 4", GB_SkillProfession4},
                {"Profession 5", GB_SkillProfession5},
                {"Profession 6", GB_SkillProfession6},
                {"Profession 7", GB_SkillProfession7},
                {"Weapon Swap", GB_SkillWeaponSwap},
                {"Special Action", GB_SkillSpecialAction},
            };
            static constexpr KeybindEntry TargetingEntries[] = {
                {"Take Target", GB_TargetTake},
                {"Call Target", GB_TargetCall},
                {"Alert", GB_TargetAlert},
                {"Call Local Target", GB_TargetCallLocal},
                {"Take Local Target", GB_TargetTakeLocal},
                {"Nearest Enemy", GB_TargetEnemyNearest},
                {"Next Enemy", GB_TargetEnemyNext},
                {"Previous Enemy", GB_TargetEnemyPrev},
                {"Nearest Ally", GB_TargetAllyNearest},
                {"Next Ally", GB_TargetAllyNext},
                {"Previous Ally", GB_TargetAllyPrev},
                {"Lock Target", GB_TargetLock},
                {"Snap Ground Target", GB_TargetSnapGroundTarget},
                {"Snap Ground Target Toggle", GB_TargetSnapGroundTargetToggle},
                {"Auto-Targeting Disable", GB_TargetAutoTargetingDisable},
                {"Auto-Targeting Toggle", GB_TargetAutoTargetingToggle},
                {"Ally Targeting Mode", GB_TargetAllyTargetingMode},
                {"Ally Targeting Mode Toggle", GB_TargetAllyTargetingModeToggle},
            };
            static constexpr KeybindEntry UiEntries[] = {
                {"Trading Post", GB_UiCommerce},
                {"Contacts", GB_UiContacts},
                {"Guild", GB_UiGuild},
                {"Hero", GB_UiHero},
                {"Inventory", GB_UiInventory},
                {"Pets", GB_UiKennel},
                {"Logout", GB_UiLogout},
                {"Mail", GB_UiMail},
                {"Options", GB_UiOptions},
                {"Party", GB_UiParty},
                {"PvP", GB_UiPvp},
                {"PvP Build", GB_UiPvpBuild},
                {"Scoreboard", GB_UiScoreboard},
                {"Wizard's Vault", GB_UiSeasonalObjectivesShop},
                {"Information", GB_UiInformation},
                {"Chat Toggle", GB_UiChatToggle},
                {"Chat Command", GB_UiChatCommand},
                {"Chat Focus", GB_UiChatFocus},
                {"Chat Reply", GB_UiChatReply},
                {"UI Toggle", GB_UiToggle},
                {"Squad Broadcast Chat Toggle", GB_UiSquadBroadcastChatToggle},
                {"Squad Broadcast Chat Command", GB_UiSquadBroadcastChatCommand},
                {"Squad Broadcast Chat Focus", GB_UiSquadBroadcastChatFocus},
            };
            static constexpr KeybindEntry CameraEntries[] = {
                {"Free Camera", GB_CameraFree},
                {"Zoom In", GB_CameraZoomIn},
                {"Zoom Out", GB_CameraZoomOut},
                {"Reverse Camera", GB_CameraReverse},
                {"Action Mode", GB_CameraActionMode},
                {"Action Mode Disable", GB_CameraActionModeDisable},
            };
            static constexpr KeybindEntry ScreenshotEntries[] = {
                {"Normal Screenshot", GB_ScreenshotNormal},
                {"Stereoscopic Screenshot", GB_ScreenshotStereoscopic},
            };
            static constexpr KeybindEntry MapEntries[] = {
                {"Toggle Map", GB_MapToggle},
                {"Focus Player", GB_MapFocusPlayer},
                {"Floor Down", GB_MapFloorDown},
                {"Floor Up", GB_MapFloorUp},
                {"Map Zoom In", GB_MapZoomIn},
                {"Map Zoom Out", GB_MapZoomOut},
            };
            static constexpr KeybindEntry MountEntries[] = {
                {"Mount / Dismount", GB_SpumoniToggle},
                {"Mount Ability 1", GB_SpumoniMovement},
                {"Mount Ability 2", GB_SpumoniSecondaryMovement},
                {"Raptor", GB_SpumoniMAM01},
                {"Springer", GB_SpumoniMAM02},
                {"Skimmer", GB_SpumoniMAM03},
                {"Jackal", GB_SpumoniMAM04},
                {"Griffon", GB_SpumoniMAM05},
                {"Roller Beetle", GB_SpumoniMAM06},
                {"Warclaw", GB_SpumoniMAM07},
                {"Skyscale", GB_SpumoniMAM08},
                {"Siege Turtle", GB_SpumoniMAM09},
            };
            static constexpr KeybindEntry MasteryEntries[] = {
                {"Mastery", GB_MasteryAccess},
                {"Fishing", GB_MasteryAccess01},
                {"Skiff", GB_MasteryAccess02},
                {"Jade Bot Waypoint", GB_MasteryAccess03},
                {"Rift Scan", GB_MasteryAccess04},
                {"Skyscale", GB_MasteryAccess05},
                {"Homestead Doorway", GB_MasteryAccess06},
            };
            static constexpr KeybindEntry MiscEntries[] = {
                {"AoE Loot", GB_MiscAoELoot},
                {"Interact", GB_MiscInteract},
                {"Show Enemies", GB_MiscShowEnemies},
                {"Show Allies", GB_MiscShowAllies},
                {"Stow / Draw Weapons", GB_MiscCombatStance},
                {"Toggle Language", GB_MiscToggleLanguage},
                {"Toggle Pet Combat", GB_MiscTogglePetCombat},
                {"Toggle Fullscreen", GB_MiscToggleFullScreen},
                {"Toggle Decorate Mode", GB_MiscToggleDecorationMode},
            };
            static constexpr KeybindEntry ToyEntries[] = {
                {"Default Novelty", GB_ToyUseDefault},
                {"Chair", GB_ToyUseSlot1},
                {"Instrument", GB_ToyUseSlot2},
                {"Held Item", GB_ToyUseSlot3},
                {"Toy", GB_ToyUseSlot4},
                {"Tonic", GB_ToyUseSlot5},
            };
            static constexpr KeybindEntry BuildTemplateEntries[] = {
                {"Build Template 1", GB_Loadout1},
                {"Build Template 2", GB_Loadout2},
                {"Build Template 3", GB_Loadout3},
                {"Build Template 4", GB_Loadout4},
                {"Build Template 5", GB_Loadout5},
                {"Build Template 6", GB_Loadout6},
                {"Build Template 7", GB_Loadout7},
                {"Build Template 8", GB_Loadout8},
                {"Build Template 9", GB_Loadout9},
            };
            static constexpr KeybindEntry EquipmentTemplateEntries[] = {
                {"Equipment Template 1", GB_GearLoadout1},
                {"Equipment Template 2", GB_GearLoadout2},
                {"Equipment Template 3", GB_GearLoadout3},
                {"Equipment Template 4", GB_GearLoadout4},
                {"Equipment Template 5", GB_GearLoadout5},
                {"Equipment Template 6", GB_GearLoadout6},
                {"Equipment Template 7", GB_GearLoadout7},
                {"Equipment Template 8", GB_GearLoadout8},
                {"Equipment Template 9", GB_GearLoadout9},
            };

            static constexpr KeybindCategory Categories[] = {
                {"Movement", MovementEntries, IM_ARRAYSIZE(MovementEntries)},
                {"Skills", SkillsEntries, IM_ARRAYSIZE(SkillsEntries)},
                {"Targeting", TargetingEntries, IM_ARRAYSIZE(TargetingEntries)},
                {"UI", UiEntries, IM_ARRAYSIZE(UiEntries)},
                {"Camera", CameraEntries, IM_ARRAYSIZE(CameraEntries)},
                {"Screenshots", ScreenshotEntries, IM_ARRAYSIZE(ScreenshotEntries)},
                {"Map", MapEntries, IM_ARRAYSIZE(MapEntries)},
                {"Mounts", MountEntries, IM_ARRAYSIZE(MountEntries)},
                {"Mastery", MasteryEntries, IM_ARRAYSIZE(MasteryEntries)},
                {"Misc", MiscEntries, IM_ARRAYSIZE(MiscEntries)},
                {"Toys/Novelties", ToyEntries, IM_ARRAYSIZE(ToyEntries)},
                {"Build Templates", BuildTemplateEntries, IM_ARRAYSIZE(BuildTemplateEntries)},
                {"Equipment Templates", EquipmentTemplateEntries, IM_ARRAYSIZE(EquipmentTemplateEntries)},
            };
            static int CategoryIndex = 0;
            static int ActionIndex = 0;

            if (CategoryIndex >= IM_ARRAYSIZE(Categories))
                CategoryIndex = 0;
            if (ActionIndex >= Categories[CategoryIndex].Count)
                ActionIndex = 0;

            const char *CategoryItems = "Movement\0Skills\0Targeting\0UI\0Camera\0Screenshots\0Map\0Mounts\0Mastery\0Misc\0Toys/Novelties\0Build Templates\0Equipment Templates\0";
            if (ImGui::Combo("Category", &CategoryIndex, CategoryItems))
                ActionIndex = 0;

            if (ImGui::Combo("Action", &ActionIndex, KeybindItemsGetter, const_cast<KeybindEntry *>(Categories[CategoryIndex].Entries), Categories[CategoryIndex].Count))
                SelectedKeybind = Categories[CategoryIndex].Entries[ActionIndex].Value;

            ImGui::Checkbox("Press (uncheck = Release)", &IsKeybindDown);
        } else if (MacroInputTypeIndex == 1) {
            const char *MouseButtonNames[] = {"Left Click", "Right Click", "Middle Click", "Side Button 1 (X1)", "Side Button 2 (X2)"};

            constexpr EMouseButton MouseButtonValues[] = {EMouseButton::Left, EMouseButton::Right, EMouseButton::Middle, EMouseButton::X1, EMouseButton::X2};

            static int mouseButtonIndex = 0;
            if (ImGui::Combo("Mouse Button", &mouseButtonIndex, MouseButtonNames, IM_ARRAYSIZE(MouseButtonNames)))
                SelectedMouseButton = MouseButtonValues[mouseButtonIndex];

            ImGui::Checkbox("Press (uncheck = Release)", &IsKeybindDown);

            ImGui::Spacing();
            ImGui::Checkbox("Click at specific position", &UseMousePosition);

            if (UseMousePosition) {
                ImGui::Indent();
                ImGui::SetNextItemWidth(120);
                ImGui::InputInt("X Position", &MouseX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(120);
                ImGui::InputInt("Y Position", &MouseY);

                const char *MousePositionTypes[] = {"Absolute (Screen)", "Relative (Current)"};
                ImGui::Combo("Position Type", &MousePositionTypeIndex, MousePositionTypes, 2);

                if (ImGui::Button("Get Current Mouse Position")) {
                    POINT CursorPosition;
                    GetCursorPos(&CursorPosition);
                    MouseX = CursorPosition.x;
                    MouseY = CursorPosition.y;
                }
                ImGui::Unindent();
            }
        } else if (MacroInputTypeIndex == 2) {
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("X Position", &MouseX);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("Y Position", &MouseY);

            const char *posTypes[] = {"Absolute (Screen)", "Relative (Current)"};
            ImGui::Combo("Position Type", &MousePositionTypeIndex, posTypes, 2);

            if (ImGui::Button("Get Current Mouse Position")) {
                POINT CursorPosition;
                GetCursorPos(&CursorPosition);
                MouseX = CursorPosition.x;
                MouseY = CursorPosition.y;
            }

            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Current position will be: (%d, %d)", MouseX, MouseY);
        }

        ImGui::Spacing();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Delay (ms)", &DelayMilliseconds);

        ImGui::Spacing();
        if (ImGui::Button("Add Action", ImVec2(120, 0))) {
            if (MacroInputTypeIndex == 0) {
                NewMacroActions.emplace_back(SelectedKeybind, IsKeybindDown, DelayMilliseconds);
            } else if (MacroInputTypeIndex == 1) {
                if (UseMousePosition) {
                    const EMousePositionType posType = (MousePositionTypeIndex == 0) ? EMousePositionType::Absolute : EMousePositionType::Relative;
                    EMousePosition pos(MouseX, MouseY, posType);
                    NewMacroActions.emplace_back(SelectedMouseButton, IsKeybindDown, pos, DelayMilliseconds);
                } else {
                    NewMacroActions.emplace_back(SelectedMouseButton, IsKeybindDown, DelayMilliseconds);
                }
            } else if (MacroInputTypeIndex == 2) {
                const EMousePositionType posType = (MousePositionTypeIndex == 0) ? EMousePositionType::Absolute : EMousePositionType::Relative;
                EMousePosition pos(MouseX, MouseY, posType);
                NewMacroActions.emplace_back(pos, DelayMilliseconds);
            }
            DelayMilliseconds = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All", ImVec2(120, 0)))
            NewMacroActions.clear();

        ImGui::Separator();
        if (ImGui::Button("Save Macro", ImVec2(120, 0))) {
            SaveMacro(MacroName, SelectedMacroSlot, NewMacroActions);
            NewMacroActions.clear();
            LastSelectedMacroIndex = -2;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            NewMacroActions.clear();
            ShowEditorWindow = false;
            SelectedMacroIndex = -1;
            LastSelectedMacroIndex = -2;
        }
    }
    ImGui::End();
}

static void ShowStatus(const char *msg) {
    strncpy_s(StatusMessage, msg, sizeof(StatusMessage));
    StatusMessageTime = ImGui::GetTime() + 3.0f;
}

static bool ClipboardSetText(const char *text) {
    if (!OpenClipboard(nullptr))
        return false;

    EmptyClipboard();

    const size_t TextLength = strlen(text);
    const HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, TextLength + 1);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    const auto pMem = static_cast<char *>(GlobalLock(hMem));
    if (!pMem) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    strcpy_s(pMem, TextLength + 1, text);
    GlobalUnlock(hMem);

    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    return true;
}

static std::string SaveFileDialog() {
    char MacroSaveFilePath[MAX_PATH] = "";

    OPENFILENAME OpenFileName;
    ZeroMemory(&OpenFileName, sizeof(OpenFileName));
    OpenFileName.lStructSize = sizeof(OpenFileName);
    OpenFileName.hwndOwner = nullptr;
    OpenFileName.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    OpenFileName.lpstrFile = MacroSaveFilePath;
    OpenFileName.nMaxFile = MAX_PATH;
    OpenFileName.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    OpenFileName.lpstrDefExt = "json";

    if (GetSaveFileName(&OpenFileName))
        return std::string(MacroSaveFilePath);

    return "";
}

static std::string OpenFileDialog() {
    char MacroSaveFilePath[MAX_PATH] = "";

    OPENFILENAME OpenFileName;
    ZeroMemory(&OpenFileName, sizeof(OpenFileName));
    OpenFileName.lStructSize = sizeof(OpenFileName);
    OpenFileName.hwndOwner = nullptr;
    OpenFileName.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    OpenFileName.lpstrFile = MacroSaveFilePath;
    OpenFileName.nMaxFile = MAX_PATH;
    OpenFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&OpenFileName))
        return std::string(MacroSaveFilePath);

    return "";
}

static std::string ReadFileToString(const std::string &path) {
    std::ifstream MacroSaveFile(path, std::ios::binary);
    if (!MacroSaveFile.is_open())
        return "";
    return std::string((std::istreambuf_iterator<char>(MacroSaveFile)), std::istreambuf_iterator<char>());
}

static bool WriteStringToFile(const std::string &path, const std::string &content) {
    std::ofstream MacroSaveFile(path);
    if (!MacroSaveFile.is_open())
        return false;
    MacroSaveFile << content;
    return true;
}

void RenderMacroSaveWindow() {
    if (!ShowSaveWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Import / Export Macros", &ShowSaveWindow)) {
        if (ImGui::BeginTabBar("ImportExportTabs")) {
            if (ImGui::BeginTabItem("Export")) {
                const char *MacroSlotNames[10] = {"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5", "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
                if (ImGui::Combo("Select Macro", &ExportSlot, MacroSlotNames, 10) || LastExportSlot != ExportSlot) {
                    LastExportSlot = ExportSlot;
                    const nlohmann::json Json = MacroToJson(Macros[ExportSlot], ExportSlot);
                    const std::string JsonStrings = Json.dump(2);
                    strncpy_s(ExportJsonBuffer, JsonStrings.c_str(), sizeof(ExportJsonBuffer) - 1);
                }

                ImGui::Separator();

                ImGui::Text("Macro JSON:");
                ImGui::InputTextMultiline("##ExportJson", ExportJsonBuffer, sizeof(ExportJsonBuffer), ImVec2(-1, 250), ImGuiInputTextFlags_ReadOnly);

                ImGui::Spacing();

                if (ImGui::Button("Copy to Clipboard", ImVec2(150, 0))) {
                    if (ClipboardSetText(ExportJsonBuffer))
                        ShowStatus("Copied to clipboard!");
                    else
                        ShowStatus("Failed to copy to clipboard!");
                }
                ImGui::SameLine();
                if (ImGui::Button("Save to File", ImVec2(150, 0))) {
                    std::string MacroSaveFilePath = SaveFileDialog();
                    if (!MacroSaveFilePath.empty()) {
                        if (WriteStringToFile(MacroSaveFilePath, ExportJsonBuffer))
                            ShowStatus(("Saved to: " + MacroSaveFilePath.substr(MacroSaveFilePath.find_last_of("/\\") + 1)).c_str());
                        else
                            ShowStatus("Failed to save file!");
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Import")) {
                const char *MacroSlotNames[10] = {
                    "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
                    "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"};
                ImGui::Combo("Import to Slot", &ImportSlot, MacroSlotNames, 10);

                ImGui::Separator();

                ImGui::Text("Paste Macro JSON:");
                ImGui::InputTextMultiline("##ImportJson", ImportJsonBuffer, sizeof(ImportJsonBuffer),
                                          ImVec2(-1, 250));

                ImGui::Spacing();

                if (ImGui::Button("Load from File", ImVec2(150, 0))) {
                    std::string MacroSaveFilePath = OpenFileDialog();
                    if (!MacroSaveFilePath.empty()) {
                        std::string MacroSaveFileContent = ReadFileToString(MacroSaveFilePath);
                        if (!MacroSaveFileContent.empty()) {
                            strncpy_s(ImportJsonBuffer, MacroSaveFileContent.c_str(), sizeof(ImportJsonBuffer));
                            ShowStatus(("Loaded: " + MacroSaveFilePath.substr(MacroSaveFilePath.find_last_of("/\\") + 1)).c_str());
                        } else {
                            ShowStatus("Failed to read file!");
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Import Macro", ImVec2(150, 0))) {
                    if (ImportMacroFromJson(ImportJsonBuffer, ImportSlot))
                        ShowStatus("Macro imported successfully!");
                    else
                        ShowStatus("Failed to import: invalid JSON!");
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if (StatusMessage[0] && ImGui::GetTime() < StatusMessageTime) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", StatusMessage);
        } else if (ImGui::GetTime() >= StatusMessageTime) {
            StatusMessage[0] = '\0';
        }
    }
    ImGui::End();
}

void AddonRender() {
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(ApiDefinition->ImguiContext));
    RenderMainWindow();
    RenderMacroEditorWindow();
    RenderMacroSaveWindow();
}

void AddonUnload() {
    if (ApiDefinition) {
        KillAllMacros();
        SaveMacrosToJson();

        for (const auto &Macro : Macros)
            UnregisterKeybind(Macro.Identifier);

        ApiDefinition->QuickAccess_Remove("MACRO_MANAGER_SHORTCUT");
        ApiDefinition->GUI_Deregister(AddonRender);
        ApiDefinition->GUI_Deregister(AddonOptions);
        ApiDefinition->InputBinds_Deregister("MACRO_SHOW_WINDOW");
        ApiDefinition->InputBinds_Deregister("MACRO_KILL");

        ApiDefinition->Log(LOGL_INFO, "MacroManager", "Macro Manager unloaded!");
    }
}

void AddonLoad(AddonAPI_t *AddonApi) {
    ApiDefinition = AddonApi;
    ApiDefinition->Log(LOGL_INFO, "MacroManager", "Macro Keybind Manager v2025.05.02.1230 loaded!");
    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(ApiDefinition->ImguiContext));
    ImGui::SetAllocatorFunctions(reinterpret_cast<void *(*)(size_t, void *)>(ApiDefinition->ImguiMalloc), reinterpret_cast<void (*)(void *, void *)>(ApiDefinition->ImguiFree));
    ApiDefinition->Textures_GetOrCreateFromResource("MACRO_MANAGER_ICON", IDI_MACRO_MANAGER, GetAddonModuleHandle());
    ApiDefinition->QuickAccess_Add("MACRO_MANAGER_SHORTCUT", "MACRO_MANAGER_ICON", "MACRO_MANAGER_ICON", "MACRO_SHOW_WINDOW", "Open Macro Manager");
    ApiDefinition->GUI_Register(RT_Render, AddonRender);
    ApiDefinition->GUI_Register(RT_OptionsRender, AddonOptions);

    LoadMacrosFromJson();

    for (const auto &Macro : Macros)
        UnregisterKeybind(Macro.Identifier);

    for (auto &Macro : Macros)
        Macro.Enabled = false;

    SetupKeybinds();
}

extern "C" __declspec(dllexport) AddonDefinition_t *GetAddonDef() {
    AddonDefinition.Signature = 0xA1B2C3D4;
    AddonDefinition.APIVersion = NEXUS_API_VERSION;
    AddonDefinition.Name = "Macro Manager";
    AddonDefinition.Version.Major = 2026;
    AddonDefinition.Version.Minor = 06;
    AddonDefinition.Version.Build = 15;
    AddonDefinition.Version.Revision = 1400;
    AddonDefinition.Author = "oshico";
    AddonDefinition.Description = "A macro manager for executing sequences of game actions with timing control.";
    AddonDefinition.Load = AddonLoad;
    AddonDefinition.Unload = AddonUnload;
    AddonDefinition.Flags = AF_None;
    AddonDefinition.Provider = EUpdateProvider::UP_GitHub;
    AddonDefinition.UpdateLink = "https://github.com/oshico/GW2-MACRO";

    return &AddonDefinition;
}