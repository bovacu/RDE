// Created by borja on 23/2/22.

#include "projectManager/code/include/ProjectViewer.h"
#include "projectManager/code/include/ProjectManagerLayer.h"
#include "core/Engine.h"

namespace engine {


    void ProjectViewer::init(ProjectSelector* _projectSelector, imgui_addons::ImGuiFileBrowser* _fileBrowser, ProjectList* _projectList, GlobalConfig* _globalConfig) {
        projectSelector = _projectSelector;
        fileBrowser = _fileBrowser;
        projectList = _projectList;
        globalConfig = _globalConfig;

        defaultFont = ImGui::GetIO().Fonts->AddFontDefault();

        h1Conf.SizePixels = 25;
        h1Conf.OversampleH = h1Conf.OversampleV = 1;
        h1Conf.PixelSnapH = true;
        h1 = ImGui::GetIO().Fonts->AddFontDefault(&h1Conf);

        h2Conf.SizePixels = 20;
        h2Conf.OversampleH = h2Conf.OversampleV = 1;
        h2Conf.PixelSnapH = true;
        h2 = ImGui::GetIO().Fonts->AddFontDefault(&h2Conf);

        h3Conf.SizePixels = 17;
        h3Conf.OversampleH = h3Conf.OversampleV = 1;
        h3Conf.PixelSnapH = true;
        h3 = ImGui::GetIO().Fonts->AddFontDefault(&h3Conf);
    }

    void ProjectViewer::render() {
        if(ImGui::BeginChild("RightPanel", {-1, -1}, true)) {
            if(projectSelector->getCurrentProject() == nullptr) { ImGui::EndChild(); return; }
            if(projectSelector->getCurrentProject()->stillExists) projectOk();
            else projectError();
            ImGui::EndChild();
        }
    }

    void ProjectViewer::setShow(bool _show) {
        show = _show;
    }

    void ProjectViewer::projectOk() {
        ImGui::PushFont(h1);
        auto _textSize = ImGui::CalcTextSize(projectSelector->getCurrentProject()->projectName.c_str());
        auto _windowWidth = ImGui::GetWindowWidth();
        ImGui::SetCursorPosX(_windowWidth / 2.f - _textSize.x / 2.f);
        ImGui::Text("%s", projectSelector->getCurrentProject()->projectName.c_str());
        ImGui::PopFont();
        ImGui::NewLine();
        ImGui::Separator();

        ImGui::NewLine();
        ImGui::NewLine();

        ImGui::PushFont(h2);
        ImGui::Text("Configuration");
        ImGui::PopFont();

        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
        ImGui::Separator();

        ImGui::NewLine();

        ImGui::Text("IDE to open the project"); ImGui::SameLine();
        static const char* item_current = globalConfig->IDEs[0].c_str();            // Here our selection is a single pointer stored outside the object.
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("###combo 1", item_current)) {
            for (int n = 0; n < globalConfig->IDEs.size(); n++){
                bool is_selected = (item_current == globalConfig->IDEs[n]);
                if (ImGui::Selectable(globalConfig->IDEs[n].c_str(), is_selected))
                    item_current = globalConfig->IDEs[n].c_str();
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Path "); ImGui::SameLine(); ImGui::TextDisabled("%s", projectSelector->getCurrentProject()->projectPath.c_str()); ImGui::SameLine();
        if(ImGui::Button("Modify")) {

        }

        static char _projectName[256];
        ImGui::Text("Project Name: "); ImGui::SameLine(); ImGui::TextDisabled("%s", projectSelector->getCurrentProject()->projectName.c_str());
        ImGui::SetNextItemWidth(150);
        ImGui::InputTextWithHint("###projectName", "Modify Name...", _projectName, IM_ARRAYSIZE(_projectName));

        ImGui::NewLine();
        static float _buttonSize = 0;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _buttonSize - ImGui::GetFrameHeightWithSpacing());
        if(ImGui::Button("Update")) {

        }
        _buttonSize = ImGui::GetItemRectSize().x;

        ImGui::Separator();

        ImGui::NewLine();
        ImGui::NewLine();

        ImGui::PushFont(h2);
        ImGui::Text("Actions");
        ImGui::PopFont();

        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.25f);
        ImGui::Separator();

        ImGui::NewLine();

        if(ImGui::Button("Open project")) {

        }

        ImGui::SameLine(0, 20);

        if(ImGui::Button("Delete project")) {

        }
    }

    void ProjectViewer::projectError() {
        ImGui::PushFont(h1);
        auto _text = APPEND_S("Project ", projectSelector->getCurrentProject()->projectName," not found!");
        auto _textSize = ImGui::CalcTextSize(_text.c_str());
        auto _windowWidth = ImGui::GetWindowWidth();
        ImGui::SetCursorPosX(_windowWidth / 2.f - _textSize.x / 2.f);
        ImGui::Text("%s", _text.c_str());
        ImGui::PopFont();
        ImGui::NewLine();
        ImGui::Separator();

        ImGui::Text("The project is missing, this could be due to these reasons:");
        ImGui::BulletText("The .config file of the project is missing");
        ImGui::BulletText("The project is not any more in the initial directory (%s)", projectSelector->getCurrentProject()->projectPath.c_str());

        ImGui::NewLine();

        ImGui::Text("The suggested actions are:");
        ImGui::BulletText("Delete the project -> "); ImGui::SameLine();
        if(ImGui::Button("Delete")) {
            ImGui::OpenPopup("Delete");
            showDelete = true;
        }
        static float _deleteSize = ImGui::GetItemRectSize().x;

        ImGui::BulletText("Reset the directory ->"); ImGui::SameLine();
        ImGui::SetNextItemWidth(_deleteSize);
        if(ImGui::Button("Open")) {
            ImGui::OpenPopup("Relocate");
            showRelocate = true;
        }

        destroyProject();
        relocateProject();
    }

    void ProjectViewer::destroyProject() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        ImGui::SetNextWindowSize({Engine::get().getWindowSize().x * 0.35f, Engine::get().getWindowSize().y * 0.15f}, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        if(ImGui::BeginPopupModal("Delete", &showDelete, ImGuiWindowFlags_NoResize)) {
            static const char* _text = "Are you sure?";
            static float _yesWidth = 0, _noWidth = 0;
            auto _textSize = ImGui::CalcTextSize(_text);

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textSize.x / 2.f);
            ImGui::Text("%s", _text);

            ImGui::NewLine();

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - (_yesWidth + _noWidth) / 2.f);
            if(ImGui::Button("Yes")) {
                int _line = -1;
                for(int _i = 0; _i < projectList->content.size(); _i++) {
                    if(strcmp(projectList->content[_i].projectName.c_str(), projectSelector->getCurrentProject()->projectName.c_str()) == 0) {
                        _line = _i;
                        break;
                    }
                }

                if(_line >= 0) {
                    FilesSystem::removeChunkLineInFile(projectList->projectsHandler, _line);
                    projectSelector->loadProjects();
                    showDelete = false;
                    projectSelector->selectProject("");
                } else
                    LOG_E("Ups, couldn't find the project ", projectSelector->getCurrentProject()->projectName, " in the data.config file")
            }
            _yesWidth = ImGui::GetItemRectSize().x;

            ImGui::SameLine();

            if(ImGui::Button("No")) {
                showDelete = false;
            }
            _noWidth = ImGui::GetItemRectSize().x;
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    void ProjectViewer::relocateProject() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        float _height = error == ProjectError::NONE ? 0.14f : 0.19f;
        ImGui::SetNextWindowSize({Engine::get().getWindowSize().x * 0.35f, Engine::get().getWindowSize().y * _height}, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        if(ImGui::BeginPopupModal("Relocate", &showRelocate, ImGuiWindowFlags_NoResize)) {
            static char _newPath[256];
            ImGui::Text("New Path: "); ImGui::SameLine();

            ImGui::InputText("###newPath", _newPath, IM_ARRAYSIZE(_newPath)); ImGui::SameLine();
            if(ImGui::Button("Open")) {
                ImGui::OpenPopup("FileBrowser");
            }

            if(fileBrowser->showFileDialog("FileBrowser", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*")) {
                strcpy(_newPath, fileBrowser->selected_path.c_str());
            }

            showErrors();

            ImGui::NewLine();

            static float _updateWidth = ImGui::CalcTextSize("Update").x;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _updateWidth / 2.f);
            if(ImGui::Button("Update")) {
                checkErrors(_newPath);
                if(error == ProjectError::NONE) {
                    auto _projectsFileHandler = FilesSystem::open("assets/projects.config", FileMode::READ);
                    auto _lines = FilesSystem::readAllLinesFile(_projectsFileHandler);

                    int _line = -1;
                    int _i = 0;
                    for(auto& _l : _lines.content) {
                        auto _projectName = SPLIT_S_I(_l, "=", 0);
                        if(strcmp(_projectName.c_str(), projectSelector->getCurrentProject()->projectName.c_str()) == 0) {
                            _line = _i;
                            break;
                        }
                        _i++;
                    }

                    FilesSystem::removeChunkLineInFile(_projectsFileHandler, _line);
                    auto _newLine = APPEND_S(projectSelector->getCurrentProject()->projectName, "=", _newPath);
                    FilesSystem::appendChunkInLineToFile(_projectsFileHandler, _newLine, _line);

                    FilesSystem::close(_projectsFileHandler);

                    projectSelector->loadProjects();
                }
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    void ProjectViewer::checkErrors(const char* _newPath) {
        error = ProjectError::NONE;

        std::ifstream _stream(_newPath);
        _stream.close();
        if(!_stream.good()) error |= ProjectError::WRONG_PATH;

        if(strlen(_newPath) == 0) error |= ProjectError::PATH_NOT_SET;
    }

    void ProjectViewer::showErrors() {
        if((error & ProjectError::PATH_NOT_SET) == ProjectError::PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Path cannot be empty");

        if((error & ProjectError::WRONG_PATH) == ProjectError::WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "Selected path is not valid");
    }
}
