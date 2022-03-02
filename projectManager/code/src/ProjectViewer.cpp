// Created by borja on 23/2/22.

#include <thread>
#include "projectManager/code/include/ProjectViewer.h"
#include "projectManager/code/include/ProjectManagerLayer.h"
#include "core/Engine.h"
#include "imgui_internal.h"
#include "code/include/Macros.h"

namespace engine {


    void ProjectViewer::init(ProjectSelector* _projectSelector, ProjectModules* _projectModules, imgui_addons::ImGuiFileBrowser* _fileBrowser, ProjectList* _projectList, GlobalConfig* _globalConfig) {
        projectSelector = _projectSelector;
        projectModules = _projectModules;
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
            showActionDeleteProject = true;
            ImGui::OpenPopup("Delete project");
        }

        ImGui::Separator();

        androidActions();

        actionsDeleteProject();
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
        static auto _no = [&]() {
            showDelete = false;
        };
        static float _width = ImGui::GetWindowWidth() * 0.2f;
        static float _height = ImGui::GetWindowHeight() * 0.15f;

        YES_NO_MACRO_WITH_SIZE("Delete", &showDelete, _width, _height, "Are you sure?", "Yes", "No", deleteCurrentProjectFromManager, _no);
    }

    void ProjectViewer::relocateProject() {
        float _height = error == ProjectError::NONE ? 0.14f : 0.19f;
        INIT_CENTERED_WINDOW_WITH_SIZE(Engine::get().getWindowSize().x * 0.35f, Engine::get().getWindowSize().y * _height)
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
        END_CENTERED_WINDOW
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

    void ProjectViewer::deleteCurrentProjectFromManager() {
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
        }
    }

    void ProjectViewer::actionsDeleteProject() {
        static auto _fromDisk = [&]() {
            auto _pathToProject = projectSelector->getCurrentProject()->projectPath;
            auto _command = APPEND_S("rm -rf ", _pathToProject);
            std::system(_command.c_str());
            deleteCurrentProjectFromManager();
        };

        YES_NO_MACRO("Delete project", &showActionDeleteProject, "Choose what kind of deletion:", "Delete from manager", "Delete from disk",  deleteCurrentProjectFromManager, _fromDisk)
    }

    void ProjectViewer::androidActions() {
        if(std::find_if(projectModules->getModules().begin(), projectModules->getModules().end(),
                        [](const Module& _module) { return std::equal(_module.name.begin(), _module.name.end(), "Android") && _module.installed; })
           != projectModules->getModules().end()) {
            ImGui::NewLine();
            ImGui::NewLine();

            ImGui::PushFont(h2);
            ImGui::Text("Android");
            ImGui::PopFont();

            ImGui::Separator();

            if(!FilesSystem::fileExists(projectSelector->getCurrentProject()->projectPath + "/targets/GDEAndroid/CMakeLists.txt")) {
                if(ImGui::Button("Add Android Target")) {
                    showAndroidInstallTarget = true;
                    ImGui::OpenPopup("Android target installation");
                    std::thread _installAndroidTarget([&]() {
                        auto _command = APPEND_S("./androidInstallation/1.sh ", projectSelector->getCurrentProject()->projectPath, " ", globalConfig->android.ndk, " ", globalConfig->GDEPath);
                        std::system(_command.c_str());
                        showAndroidInstallTarget = false;
                    });
                    _installAndroidTarget.detach();
                }
            } else {
                static char _androidPackage[256];
                static char _apkName[256];
                static bool _split = false;

                if(blockOpenAndroidStudio) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::Button("Open With Android Studio");
                    ImGui::PopItemFlag();
                } else {
                    if(ImGui::Button("Open With Android Studio")) {
                        blockOpenAndroidStudio = true;
                        std::thread _openAndroidStudio([&]{
                            auto _command = APPEND_S("sh ", globalConfig->android.androidStudio, "/bin/studio.sh ", projectSelector->getCurrentProject()->projectPath, "/targets/GDEAndroid");
                            std::system(_command.c_str());
                            blockOpenAndroidStudio = false;
                        });
                        _openAndroidStudio.detach();
                    }
                }

                ImGui::SameLine();
                static float _deleteAndroidModuleSize = 0;
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _deleteAndroidModuleSize - ImGui::GetStyle().WindowPadding.x);

                static bool _sureDeleteAndroidModule = true;
                if(ImGui::Button("Delete Android Module")) {
                    INIT_MODAL("Delete Android Module")
                }

                static auto _yes = [&]() {
                    auto _command = APPEND_S("rm -rf ", projectSelector->getCurrentProject()->projectPath, "/targets/GDEAndroid");
                    std::system(_command.c_str());
                };
                static auto _no = [](){
                    _sureDeleteAndroidModule = false;
                };
                YES_NO_MACRO("Delete Android Module", &_sureDeleteAndroidModule, "Are you sure you want to delete it?", "Yes", "No", _yes, _no)

                _deleteAndroidModuleSize = ImGui::GetItemRectSize().x;

                ImGui::Text("Package"); ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::InputText("###androidPackage", _androidPackage, IM_ARRAYSIZE(_androidPackage));

                ImGui::Text("APK Name"); ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::InputText("###apkName", _apkName, IM_ARRAYSIZE(_apkName));

                ImGui::Checkbox("Split build", &_split);

                if(ImGui::Button("Sign APK")) {

                }

                ImGui::SameLine();

                if(ImGui::Button("Build APK")) {

                }
            }

            ImGui::Separator();

            if(showAndroidInstallTarget) {
                INIT_CENTERED_WINDOW
                if(ImGui::BeginPopupModal("Android target installation")) {
                    const float _textWidth = ImGui::CalcTextSize("Downloading and installing android target, might take some time...").x;
                    const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 7.5f);
                    Spinner("Installing Android Components", 15.f, 6, _col);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textWidth / 2.f);
                    ImGui::Text("Downloading and installing android target, might take some time...");
                    ImGui::EndPopup();
                }
                END_CENTERED_WINDOW
            }
        }
    }

    bool ProjectViewer::Spinner(const char* label, float radius, int thickness, const ImU32& color) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));

        const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
                                                centre.y + ImSin(a+g.Time*8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);

        return true;
    }
}
