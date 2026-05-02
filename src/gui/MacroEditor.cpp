#include "MacroEditor.h"
#include "../macro/Macro.h"
#include "../macro/MacroManager.h"
#include "../utils/StringConversions.h"
#include "../core/Context.h"
#include <imgui.h>
#include <cstring>
#include <windows.h>

extern Context g_context;

void RenderMacroEditor()
{
    if (!g_context.showMacroEditor)
        return;

    static char macroName[128] = "";
    static std::vector<KeyAction> editingActions;
    static int lastSelectedMacroIndex = -2;
    static int selectedSlot = 0;

    if (g_context.selectedMacroIndex != lastSelectedMacroIndex)
    {
        if (g_context.selectedMacroIndex >= 0 && g_context.selectedMacroIndex < static_cast<int>(g_context.macros.
            size()))
        {
            const Macro& macro = g_context.macros[g_context.selectedMacroIndex];
            strncpy_s(macroName, sizeof(macroName), macro.name.c_str(), _TRUNCATE);
            editingActions = macro.actions;

            if (const std::string id = macro.identifier; id.find("MACRO_") != std::string::npos)
                selectedSlot = std::stoi(id.substr(6)) - 1;
        }
        else
        {
            strcpy_s(macroName, sizeof(macroName), "New Macro");
            editingActions.clear();
            selectedSlot = 0;
        }
        lastSelectedMacroIndex = g_context.selectedMacroIndex;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 650), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Macro Editor", &g_context.showMacroEditor))
    {
        ImGui::InputText("Macro Name", macroName, sizeof(macroName));

        ImGui::Separator();
        const char* slotNames[10] = {
            "Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5",
            "Slot 6", "Slot 7", "Slot 8", "Slot 9", "Slot 10"
        };
        ImGui::Combo("Macro Slot", &selectedSlot, slotNames, 10);

        ImGui::Separator();
        ImGui::Text("Action Sequence:");
        if (ImGui::BeginChild("ActionList", ImVec2(0, 220), true))
        {
            for (size_t i = 0; i < editingActions.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));

                ImGui::Text("%d.", static_cast<int>(i) + 1);
                ImGui::SameLine();

                if (editingActions[i].inputType == EInputType::MouseMove)
                {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "MOVE");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f),
                                       "to (%d, %d) [%s]",
                                       editingActions[i].mousePosition.x,
                                       editingActions[i].mousePosition.y,
                                       editingActions[i].mousePosition.positionType == EPositionType::Absolute
                                           ? "Abs"
                                           : "Rel");
                }
                else
                {
                    ImGui::TextColored(editingActions[i].isKeyDown
                                           ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f)
                                           : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                                       "%s", editingActions[i].isKeyDown ? "PRESS" : "RELEASE");

                    ImGui::SameLine();

                    if (editingActions[i].inputType == EInputType::GameBind)
                    {
                        ImGui::TextUnformatted(GetBindName(editingActions[i].gameBind));
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f), "%s",
                                           GetMouseButtonName(editingActions[i].mouseButton));

                        if (editingActions[i].moveBeforeClick)
                        {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f),
                                               "@ (%d, %d)",
                                               editingActions[i].mousePosition.x,
                                               editingActions[i].mousePosition.y);
                        }
                    }
                }

                if (editingActions[i].delayMs > 0)
                {
                    ImGui::SameLine();
                    ImGui::Text("(%dms delay)", editingActions[i].delayMs);
                }

                ImGui::SameLine(ImGui::GetWindowWidth() - 60);
                if (ImGui::SmallButton("X"))
                {
                    editingActions.erase(editingActions.begin() + i);
                    --i;
                }

                ImGui::PopID();
            }

            if (editingActions.empty())
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No actions added yet.");
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Add Action:");

        static int inputTypeIndex = 0;
        const char* inputTypes[] = {"Keyboard/Game Action", "Mouse Button", "Mouse Move"};
        ImGui::Combo("Input Type", &inputTypeIndex, inputTypes, 3);

        static EGameBinds selectedBind = GB_SkillWeapon1;
        static auto selectedMouseButton = EMouseButton::Left;
        static bool isKeyDown = true;
        static int delayMs = 0;
        static int mouseX = 0;
        static int mouseY = 0;
        static int positionTypeIndex = 0;
        static bool usePosition = false;

        if (inputTypeIndex == 0)
        {
            const char* bindNames[] = {
                "Weapon 1", "Weapon 2", "Weapon 3", "Weapon 4", "Weapon 5",
                "Heal Skill", "Utility 1", "Utility 2", "Utility 3", "Elite Skill",
                "Profession 1", "Profession 2", "Profession 3", "Profession 4", "Profession 5",
                "Profession 6", "Profession 7",
                "Dodge", "Jump", "Interact", "Weapon Swap"
            };

            constexpr EGameBinds bindValues[] = {
                GB_SkillWeapon1, GB_SkillWeapon2, GB_SkillWeapon3, GB_SkillWeapon4, GB_SkillWeapon5,
                GB_SkillHeal, GB_SkillUtility1, GB_SkillUtility2, GB_SkillUtility3, GB_SkillElite,
                GB_SkillProfession1, GB_SkillProfession2, GB_SkillProfession3, GB_SkillProfession4,
                GB_SkillProfession5, GB_SkillProfession6, GB_SkillProfession7,
                GB_MoveDodge, GB_MoveJump_SwimUp_FlyUp, GB_MiscInteract, GB_SkillWeaponSwap
            };

            static int bindIndex = 0;
            if (ImGui::Combo("Game Action", &bindIndex, bindNames, IM_ARRAYSIZE(bindNames)))
                selectedBind = bindValues[bindIndex];

            ImGui::Checkbox("Press (uncheck = Release)", &isKeyDown);
        }
        else if (inputTypeIndex == 1)
        {
            const char* mouseButtonNames[] = {
                "Left Click", "Right Click", "Middle Click", "Side Button 1 (X1)", "Side Button 2 (X2)"
            };

            constexpr EMouseButton mouseButtonValues[] = {
                EMouseButton::Left, EMouseButton::Right, EMouseButton::Middle,
                EMouseButton::X1, EMouseButton::X2
            };

            static int mouseButtonIndex = 0;
            if (ImGui::Combo("Mouse Button", &mouseButtonIndex, mouseButtonNames, IM_ARRAYSIZE(mouseButtonNames)))
                selectedMouseButton = mouseButtonValues[mouseButtonIndex];

            ImGui::Checkbox("Press (uncheck = Release)", &isKeyDown);

            ImGui::Spacing();
            ImGui::Checkbox("Click at specific position", &usePosition);

            if (usePosition)
            {
                ImGui::Indent();
                ImGui::SetNextItemWidth(120);
                ImGui::InputInt("X Position", &mouseX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(120);
                ImGui::InputInt("Y Position", &mouseY);

                const char* posTypes[] = {"Absolute (Screen)", "Relative (Current)"};
                ImGui::Combo("Position Type", &positionTypeIndex, posTypes, 2);

                if (ImGui::Button("Get Current Mouse Position"))
                {
                    POINT cursorPos;
                    GetCursorPos(&cursorPos);
                    mouseX = cursorPos.x;
                    mouseY = cursorPos.y;
                }
                ImGui::Unindent();
            }
        }
        else if (inputTypeIndex == 2)
        {
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("X Position", &mouseX);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("Y Position", &mouseY);

            const char* posTypes[] = {"Absolute (Screen)", "Relative (Current)"};
            ImGui::Combo("Position Type", &positionTypeIndex, posTypes, 2);

            if (ImGui::Button("Get Current Mouse Position"))
            {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                mouseX = cursorPos.x;
                mouseY = cursorPos.y;
            }

            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                               "Current position will be: (%d, %d)", mouseX, mouseY);
        }

        ImGui::Spacing();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Delay (ms)", &delayMs);

        ImGui::Spacing();
        if (ImGui::Button("Add Action", ImVec2(120, 0)))
        {
            if (inputTypeIndex == 0)
            {
                editingActions.emplace_back(selectedBind, isKeyDown, delayMs);
            }
            else if (inputTypeIndex == 1)
            {
                if (usePosition)
                {
                    const EPositionType posType = (positionTypeIndex == 0)
                                                      ? EPositionType::Absolute
                                                      : EPositionType::Relative;
                    MousePosition pos(mouseX, mouseY, posType);
                    editingActions.emplace_back(selectedMouseButton, isKeyDown, pos, delayMs);
                }
                else
                {
                    editingActions.emplace_back(selectedMouseButton, isKeyDown, delayMs);
                }
            }
            else if (inputTypeIndex == 2)
            {
                const EPositionType posType = (positionTypeIndex == 0)
                                                  ? EPositionType::Absolute
                                                  : EPositionType::Relative;
                MousePosition pos(mouseX, mouseY, posType);
                editingActions.emplace_back(pos, delayMs);
            }
            delayMs = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All", ImVec2(120, 0)))
            editingActions.clear();

        ImGui::Separator();
        if (ImGui::Button("Save Macro", ImVec2(120, 0)))
        {
            SaveMacro(macroName, selectedSlot, editingActions);
            editingActions.clear();
            lastSelectedMacroIndex = -2;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            editingActions.clear();
            g_context.showMacroEditor = false;
            g_context.selectedMacroIndex = -1;
            lastSelectedMacroIndex = -2;
        }
    }
    ImGui::End();
}