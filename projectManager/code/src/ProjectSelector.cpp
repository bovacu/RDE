// Created by borja on 23/2/22.

#include <string>
#include "projectManager/code/include/ProjectSelector.h"
#include "imgui.h"
#include "projectManager/code/include/ProjectManagerLayer.h"

namespace engine {


    void ProjectSelector::init(ProjectList* _projectList, ProjectCreator* _projectCreator) {
        projectList = _projectList;
        projectCreator = _projectCreator;
    }

    void ProjectSelector::render() {
        if(ImGui::BeginChild("LeftPanel", {ImGui::GetWindowSize().x * 0.25f, -1}, true)) {
            if(ImGui::BeginChild("YourProjects", {-1, 30})) {
                ImGui::Text("Your projects");

                ImGui::SameLine();

                static float _addNewWidth = 0;
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _addNewWidth - ImGui::GetFrameHeightWithSpacing() / 2.f);
                if(ImGui::Button("Add new")) projectCreator->setShow(true);
                _addNewWidth = ImGui::GetItemRectSize().x;

                ImGui::EndChild();
            }

            if(ImGui::BeginChild("ProjectList", {-1, -1}, true)) {
                if(ImGui::BeginChild("ProjectList", {0, -ImGui::GetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_NoMove)) {
                    for(auto& _project : projectList->content) {
                        std::string _projectName = SPLIT_S_I(_project.project, "=", 0)
                        if(ImGui::Selectable(_projectName.c_str(), _project.selected)) {
                            project = &_project;
                            LOG_I("Selected project is: ", _projectName)
                        }
                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();
        }
    }

    Project* ProjectSelector::getCurrentProject() {
        return project;
    }
}
