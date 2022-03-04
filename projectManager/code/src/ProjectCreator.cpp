// Created by borja on 23/2/22.

#include "projectManager/code/include/ProjectCreator.h"
#include "imgui.h"
#include "core/Engine.h"
#include "core/systems/fileSystem/FilesSystem.h"
#include "projectManager/code/include/ProjectSelector.h"
#include "code/include/Macros.h"

namespace engine {

    void ProjectCreator::init(GlobalConfig* _globalConfig, imgui_addons::ImGuiFileBrowser* _fileBrowser) {
        globalConfig = _globalConfig;
        fileBrowser = _fileBrowser;
    }

    void ProjectCreator::render() {
        if(!showProjectCreator) return;

        static float _projectNameWidth = 0, _projectPathWidth = 0, _offset = 0;

        INIT_CENTERED_WINDOW_WITH_SIZE(Engine::get().getWindowSize().x * 0.35f, Engine::get().getWindowSize().y * (error == ProjectError::NONE ? 0.17f : 0.2f))
        INIT_MODAL("CreateNewProject")
        if(ImGui::BeginPopupModal("CreateNewProject", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar)) {

            ImGui::Text("Project Name"); ImGui::SameLine();
            _projectNameWidth = ImGui::GetItemRectSize().x;
            ImGui::SetCursorPosX(_projectNameWidth + _offset + ImGui::GetFrameHeightWithSpacing() * 0.7f);
            ImGui::SetNextItemWidth(240);
            ImGui::InputText("###ProjectName", projectName, IM_ARRAYSIZE(projectName));

            ImGui::Text("Project Destination"); ImGui::SameLine();
            _projectPathWidth = ImGui::GetItemRectSize().x;
            ImGui::SetNextItemWidth(240);
            ImGui::InputText("###ProjectDestination", projectPath, IM_ARRAYSIZE(projectPath));
            ImGui::SameLine();

            if(ImGui::Button("Open")) ImGui::OpenPopup("FileBrowser");

            _offset = _projectPathWidth - _projectNameWidth;

            showErrors();

            ImGui::NewLine();

            static float _createWidth = 0;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _createWidth);
            if(ImGui::Button("Create")) {
                checkErrors();
                if(error == ProjectError::NONE) createProject();
            }
            _createWidth = ImGui::GetItemRectSize().x;

            ImGui::SameLine();

            if(ImGui::Button("Cancel")) showProjectCreator = false;

            BROWSER_SELECT(fileBrowser, "FileBrowser", {
                strcpy(projectPath, fileBrowser->selected_path.c_str());
            })

            ImGui::EndPopup();
        }
        END_CENTERED_WINDOW
    }

    void ProjectCreator::createProject() {
        auto _command = APPEND_S("./projectCreator.sh ", globalConfig->GDEPath, " ", projectName, " ", projectPath);
        std::system(_command.c_str());
        globalConfig->projects.emplace_back(Project{std::string(projectName) + "=" + std::string(projectPath), projectName, std::string(projectPath) + "/" + std::string(projectName), {}, false, true});
        SAVE_CONFIG(globalConfig)

        showProjectCreator = false;
        auto _configFile = APPEND_S(projectPath, "/", projectName, "/.config");
        FilesSystem::close(FilesSystem::createFile(_configFile));

        projectSelector->loadProjects();
        projectSelector->selectProject(projectName);
    }

    void ProjectCreator::showErrors() {
        if((error & ProjectError::NAME_NOT_SET) == ProjectError::NAME_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Name cannot be empty");

        if((error & ProjectError::PATH_NOT_SET) == ProjectError::PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Path cannot be empty");

        if((error & ProjectError::WRONG_PATH) == ProjectError::WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "Selected path is not valid");

        if((error & ProjectError::NAME_ALREADY_IN_USE) == ProjectError::NAME_ALREADY_IN_USE)
            ImGui::TextColored({255, 0, 0, 255}, "Name is already in list");
    }

    void ProjectCreator::checkErrors() {
        error = ProjectError::NONE;

        std::ifstream _stream(projectPath);
        if(!_stream.good()) error |= ProjectError::WRONG_PATH;
        _stream.close();

        if(strlen(projectName) == 0) error |= ProjectError::NAME_NOT_SET;
        if(strlen(projectPath) == 0) error |= ProjectError::PATH_NOT_SET;

        for(auto& _project : globalConfig->projects) {
            if (strcmp(_project.projectName.c_str(), projectName) == 0) {
                error |= ProjectError::NAME_ALREADY_IN_USE;
                break;
            }
        }
    }

    void ProjectCreator::setShow(bool _show) {
        showProjectCreator = _show;
    }

    void ProjectCreator::setProjectSelector(ProjectSelector* _projectSelector) {
        projectSelector = _projectSelector;
    }
}