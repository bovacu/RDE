// Created by borja on 23/2/22.

#include <string>
#include <utility>
#include "projectManager/code/include/ProjectSelector.h"
#include "imgui.h"
#include "code/include/Macros.h"

namespace engine {


    void ProjectSelector::init(GlobalConfig* _globalConfig, ProjectCreator* _projectCreator) {
        globalConfig = _globalConfig;
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
                    for(auto& _project : globalConfig->projects) {
                        if(!_project.stillExists) ImGui::PushStyleColor(ImGuiCol_Text, {255, 0, 0, 255});
                        if(ImGui::Selectable(_project.projectName.c_str(), _project.selected)) {
                            project = &_project;
                            onChangeProjectCallback(project);
                        }
                        if(!_project.stillExists) ImGui::PopStyleColor();
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

    void ProjectSelector::selectProject(const std::string& _projectName) {
        if(_projectName.empty()) {
            project = nullptr;
            LOG_I("No project selected")
            return;
        }

        for(auto& _project : globalConfig->projects) {
            if(strcmp(_projectName.c_str(), _project.projectName.c_str()) == 0) {
                project = &_project;
                break;
            }
        }

        onChangeProjectCallback(project);
    }

    void ProjectSelector::loadProjects() {
        LOAD_CONFIG(globalConfig)
    }

    void ProjectSelector::setOnChangeProjectCallback(std::function<void(const Project*)> _callback) {
        onChangeProjectCallback = std::move(_callback);
    }
}
