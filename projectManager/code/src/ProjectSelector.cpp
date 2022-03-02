// Created by borja on 23/2/22.

#include <string>
#include <fstream>
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
                        std::string _projectName = SPLIT_S_I(_project.project, "=", 0);

                        if(!_project.stillExists) ImGui::PushStyleColor(ImGuiCol_Text, {255, 0, 0, 255});
                        if(ImGui::Selectable(_projectName.c_str(), _project.selected)) {
                            project = &_project;
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

        for(auto& _project : projectList->content) {
            std::string _pName = SPLIT_S_I(_project.project, "=", 0);
            if(strcmp(_projectName.c_str(), _pName.c_str()) == 0) {
                project = &_project;
                break;
            }
        }
    }

    void ProjectSelector::loadProjects() {
        auto _projects = FilesSystem::readAllLinesFile(projectList->projectsHandler).content;
        projectList->content.clear();
        for(auto& _project : _projects) {
            auto _path = SPLIT_S(_project, "=");
            auto _fullPath = APPEND_S(_path[1], "/", _path[0]);
            std::ifstream _stream(_fullPath + "/.config");
            projectList->content.emplace_back(Project{_project, _path[0], _fullPath, false, _stream.good()});
            _stream.close();
        }
    }
}
