// Created by borja on 23/2/22.

#include <thread>
#include "projectManager/code/include/ProjectViewer.h"
#include "projectManager/code/include/ProjectManagerLayer.h"
#include "core/Engine.h"
#include "imgui_internal.h"
#include "code/include/Macros.h"

namespace engine {


    void ProjectViewer::init(ProjectSelector* _projectSelector, ProjectModules* _projectModules, imgui_addons::ImGuiFileBrowser* _fileBrowser, GlobalConfig* _globalConfig) {
        projectSelector = _projectSelector;
        projectModules = _projectModules;
        fileBrowser = _fileBrowser;
        globalConfig = _globalConfig;

        projectSelector->setOnChangeProjectCallback([&](const Project* _project) {
            defaultIDE = std::find_if(globalConfig->projects.begin(), globalConfig->projects.end(), [&](const Project& _project) {
                return std::equal(_project.projectName.begin(), _project.projectName.end(), projectSelector->getCurrentProject()->projectName.c_str());
            })->config.IDE;
        });

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

        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("###combo 1", defaultIDE.c_str())) {
            for (int n = 0; n < globalConfig->IDEs.size(); n++){
                bool is_selected = (defaultIDE == globalConfig->IDEs[n].name);
                if (ImGui::Selectable(globalConfig->IDEs[n].name.c_str(), is_selected)) {
                    defaultIDE = globalConfig->IDEs[n].name;
                    if(std::equal(globalConfig->IDEs[n].name.begin(), globalConfig->IDEs[n].name.end(), "Other...")) {
                        ImGui::OpenPopup("File Browser IDE");
                    }
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();

        if(ImGui::Button("Add..."))
            INIT_MODAL("Add new IDE")

        static char _newProjectPath[256];
        ImGui::Text("Path "); ImGui::SameLine();
        ImGui::SetNextItemWidth(275);
        ImGui::InputTextWithHint("###newPath", projectSelector->getCurrentProject()->projectPath.c_str(), _newProjectPath,
                                 IM_ARRAYSIZE(_newProjectPath)); ImGui::SameLine();
        if(ImGui::Button("Modify")) {
            ImGui::OpenPopup("File Browser Move To");
        }

        BROWSER_SELECT(fileBrowser, "File Browser Move To", {
            strcpy(_newProjectPath, fileBrowser->selected_path.c_str());
        })

        static char _projectName[256];
        ImGui::Text("Project Name: "); ImGui::SameLine(); ImGui::TextDisabled("%s", projectSelector->getCurrentProject()->projectName.c_str());
        ImGui::SetNextItemWidth(150);
        ImGui::InputTextWithHint("###projectName", "Modify Name...", _projectName, IM_ARRAYSIZE(_projectName));

        ImGui::NewLine();
        static float _buttonSize = 0;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _buttonSize - ImGui::GetFrameHeightWithSpacing());
        if(ImGui::Button("Update")) {
            bool _changes = false;
            if(!std::equal(defaultIDE.begin(), defaultIDE.end(), projectSelector->getCurrentProject()->config.IDE.c_str()))
                std::find_if(globalConfig->projects.begin(), globalConfig->projects.end(), [&](const Project& _project) {
                    return std::equal(_project.projectName.begin(), _project.projectName.end(), projectSelector->getCurrentProject()->projectName.c_str());
                })->config.IDE = defaultIDE;

            if(strcmp(_projectName, projectSelector->getCurrentProject()->projectName.c_str()) != 0 && strcmp(_projectName, "") != 0) {
                auto _pathWithoutProjectName = projectSelector->getCurrentProject()->projectPath.substr(0, projectSelector->getCurrentProject()->projectPath.find_last_of("/\\"));
                auto _path = APPEND_S(_pathWithoutProjectName, "/", _projectName);
                auto _command = APPEND_S("mv ", projectSelector->getCurrentProject()->projectPath, " ", _path);
                LOG_W(_command, " | ", _path, " | ", _projectName)
                projectSelector->getCurrentProject()->projectPath = _path;
                projectSelector->getCurrentProject()->projectName = _projectName;
                auto _project = APPEND_S(_projectName, "=", _path);
                projectSelector->getCurrentProject()->project = _project;
                strcpy(_projectName, "");
                std::system(_command.c_str());
            }

            if(strcmp(_newProjectPath, projectSelector->getCurrentProject()->projectPath.c_str()) != 0 && strcmp(_newProjectPath, "") != 0) {
                auto _path = APPEND_S(_newProjectPath, projectSelector->getCurrentProject()->projectName);
                auto _command = APPEND_S("mv ", projectSelector->getCurrentProject()->projectPath, " ", _path);
                LOG_E(_command)
                if(FilesSystem::fileExists(_newProjectPath)) {
                    std::system(_command.c_str());
                    strcpy(_newProjectPath, "");
                    projectSelector->getCurrentProject()->projectPath = _path;
                }
            }

            SAVE_CONFIG(globalConfig)
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
            if(defaultIDE.empty()) return;
            std::thread _openIDE([&]() {
                auto _command = APPEND_S(std::find_if(globalConfig->IDEs.begin(), globalConfig->IDEs.end(), [&](const IDE& _ide) {
                    return std::equal(_ide.name.begin(), _ide.name.end(), defaultIDE.c_str());
                })->path, " ", projectSelector->getCurrentProject()->projectPath);
                std::system(_command.c_str());
            });
            _openIDE.detach();
        }

        ImGui::SameLine(0, 20);

        if(ImGui::Button("Delete project")) {
            showActionDeleteProject = true;
            INIT_MODAL("Delete project");
        }

        ImGui::Separator();

        addNewIDE();

        androidActions();
        firebaseActions();

        actionsDeleteProject();

    }

    void ProjectViewer::addNewIDE() {
        INIT_CENTERED_WINDOW

        if(ImGui::BeginPopupModal("Add new IDE")) {
            static ProjectError _error = ProjectError::NONE;
            static char _ideName[256];
            static char _idePath[256];

            ImGui::Text("IDE name: "); ImGui::SameLine();
            ImGui::InputText("###ideName", _ideName, IM_ARRAYSIZE(_ideName));

            ImGui::Text("IDE path: "); ImGui::SameLine();
            ImGui::InputText("###idePath", _idePath, IM_ARRAYSIZE(_idePath)); ImGui::SameLine();
            if(ImGui::Button("Open")) {
                INIT_MODAL("File Browser IDE")
            }

            if((_error & ProjectError::NAME_NOT_SET) == ProjectError::NAME_NOT_SET)
                ImGui::TextColored({255, 0, 0, 255}, "Name cannot be empty");

            if((_error & ProjectError::NAME_NOT_SET) == ProjectError::PATH_NOT_SET)
                ImGui::TextColored({255, 0, 0, 255}, "Path cannot be empty");

            if((_error & ProjectError::WRONG_PATH) == ProjectError::WRONG_PATH)
                ImGui::TextColored({255, 0, 0, 255}, "Path cannot be empty");

            static float _addSize = 0;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _addSize / 2.f);
            if(ImGui::Button("Add")) {
                _error = ProjectError::NONE;
                if(strlen(_ideName) == 0) _error |= ProjectError::NAME_NOT_SET;
                if(strlen(_idePath) == 0) _error |= ProjectError::PATH_NOT_SET;
                if(_error == ProjectError::NONE) {
                    globalConfig->IDEs.emplace_back(IDE{_ideName, _idePath});
                    SAVE_CONFIG(globalConfig)
                    ImGui::CloseCurrentPopup();
                }
            }
            _addSize = ImGui::GetItemRectSize().x;

            BROWSER_OPEN(fileBrowser, "File Browser IDE", {
                strcpy(_idePath, fileBrowser->selected_path.c_str());
            })

            ImGui::EndPopup();
        }

        END_CENTERED_WINDOW
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
            INIT_MODAL("Delete");
            showDelete = true;
        }
        static float _deleteSize = ImGui::GetItemRectSize().x;

        ImGui::BulletText("Reset the directory ->"); ImGui::SameLine();
        ImGui::SetNextItemWidth(_deleteSize);
        if(ImGui::Button("Open")) {
            INIT_MODAL("Relocate");
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

            BROWSER_SELECT(fileBrowser, "FileBrowser", {
                strcpy(_newPath, fileBrowser->selected_path.c_str());
            })

            showErrors();

            ImGui::NewLine();

            static float _updateWidth = ImGui::CalcTextSize("Update").x;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _updateWidth / 2.f);
            if(ImGui::Button("Update")) {
                checkErrors(_newPath);
                if(error == ProjectError::NONE) {
                    auto _it = std::find_if(globalConfig->projects.begin(), globalConfig->projects.end(), [&](const Project& _p) {
                        return std::equal(_p.projectName.begin(), _p.projectName.end(), projectSelector->getCurrentProject()->projectName.c_str());
                    });

                    _it->projectPath = _newPath;
                    SAVE_CONFIG(globalConfig)
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
        globalConfig->projects.erase(std::remove_if(globalConfig->projects.begin(), globalConfig->projects.end(), [&](const Project& _project) -> bool {
            return std::equal(_project.projectName.begin(), _project.projectName.end(), projectSelector->getCurrentProject()->projectName.c_str());
        }), globalConfig->projects.end());
        SAVE_CONFIG(globalConfig)
        showDelete = false;
        projectSelector->selectProject("");
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

            static int _buildingResult = -1;

            if(!FilesSystem::fileExists(projectSelector->getCurrentProject()->projectPath + "/targets/GDEAndroid/CMakeLists.txt")) {
                if(ImGui::Button("Add Android Target")) {
                    showAndroidInstallTarget = true;
                    INIT_MODAL("Android target installation");
                    std::thread _installAndroidTarget([&]() {
                        auto _command = APPEND_S("./androidInstallation/setupAndroidProject.sh ", projectSelector->getCurrentProject()->projectPath, " ", globalConfig->android.ndk, " ", globalConfig->GDEPath);
                        std::system(_command.c_str());
                        showAndroidInstallTarget = false;
                    });
                    _installAndroidTarget.detach();
                }
            } else {
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
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::InputText("###androidPackage", androidPackage, IM_ARRAYSIZE(androidPackage));
                TOOLTIP("If you change this, the google-services.json might break!");
                ImGui::PopItemFlag();

                ImGui::Text("APK Name"); ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::InputText("###apkName", androidApkName, IM_ARRAYSIZE(androidApkName));

                ImGui::Checkbox("Split build", &androidSplitBuild);

                if(ImGui::Button("Sign APK")) {

                }

                ImGui::SameLine();

                if(ImGui::Button("Building APK")) {
                    INIT_MODAL("Building APK")
                    std::thread _buildApk([&]() {
                        auto _command = APPEND_S("./androidInstallation/buildApk.sh ", projectSelector->getCurrentProject()->projectPath, " ", androidApkName);
                        _buildingResult = std::system(_command.c_str());
                    });
                    _buildApk.detach();
                }
            }

            static bool _buildingOpened = true;
            if(_buildingResult == -1) {
                INIT_CENTERED_WINDOW
                if(ImGui::BeginPopupModal("Building APK")) {
                    const float _textWidth = ImGui::CalcTextSize("Building the APK, this might take a while...").x;
                    const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 7.5f);
                    Spinner("Installing Android Components", 15.f, 6, _col);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textWidth / 2.f);
                    ImGui::Text("Building the APK, this might take a while...");
                    ImGui::EndPopup();
                }
                END_CENTERED_WINDOW
            } else if(_buildingResult == 0) {
                ImGui::OpenPopup("File Browser APK");
                BROWSER_SELECT(fileBrowser, "File Browser APK", {
                    auto _command = APPEND_S("cp ", projectSelector->getCurrentProject()->projectPath, "/targets/GDEAndroid/app/build/outputs/apk/debug/", androidApkName, ".apk ", fileBrowser->selected_path);
                    std::system(_command.c_str());
                    _buildingResult = -1;
                })
            } else {
                INIT_MODAL("Build failed")
                INIT_CENTERED_WINDOW
                if(ImGui::BeginPopupModal("Build failed")) {
                    const float _textWidth = ImGui::CalcTextSize("Building failed... check the logs in GDEAndroid/build.logs").x;
                    const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textWidth / 2.f);
                    ImGui::Text("Building failed... check the logs in GDEAndroid/build.logs");

                    static float _closeWidth = 0;
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _closeWidth / 2.f);
                    if(ImGui::Button("Close")) {
                        _buildingResult = -1;
                        ImGui::CloseCurrentPopup();
                    }
                    _closeWidth = ImGui::GetItemRectSize().x;

                    ImGui::EndPopup();
                }
                END_CENTERED_WINDOW
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

    void ProjectViewer::firebaseActions() {
        if(std::find_if(projectModules->getModules().begin(), projectModules->getModules().end(),
                        [](const Module& _module) { return std::equal(_module.name.begin(), _module.name.end(), "Firebase") && _module.installed; })
           != projectModules->getModules().end()) {
            ImGui::NewLine();
            ImGui::NewLine();

            ImGui::PushFont(h2);
            ImGui::Text("Firebase");
            ImGui::PopFont();

            ImGui::Separator();

            static bool _openPopup = false;
            if(FilesSystem::fileExists(projectSelector->getCurrentProject()->projectPath + "/targets/GDEAndroid/CMakeLists.txt")) {
                if(FilesSystem::fileExists(projectSelector->getCurrentProject()->projectPath + "/targets/GDEAndroid/firebase_cpp_sdk")) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(145.f/255.f, 133.f/255.f, 132.f/255.f));
                    ImGui::Button("Install to Android");
                    TOOLTIP("Module already installed!")
                    ImGui::PopItemFlag();
                    ImGui::PopStyleColor();

                    ImGui::SameLine();

                    if(FilesSystem::fileExists(projectSelector->getCurrentProject()->projectPath + "/targets/GDEAndroid/app/google-services.json")) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(145.f/255.f, 133.f/255.f, 132.f/255.f));
                        ImGui::Button("Add google-services.json");
                        TOOLTIP("Json already added!")
                        ImGui::PopItemFlag();
                        ImGui::PopStyleColor();
                    } else {
                        if(ImGui::Button("Add google-services.json")) {
                            ImGui::OpenPopup("File Browser google-services.json");
                        }

                        BROWSER_OPEN(fileBrowser, "File Browser google-services.json", {
                            if(std::equal(fileBrowser->selected_fn.begin(), fileBrowser->selected_fn.end(), "google-services.json")) {
                                auto _command = APPEND_S("cp ", fileBrowser->selected_path, " ", projectSelector->getCurrentProject()->projectPath, "/targets/GDEAndroid/app");
                                std::system(_command.c_str());
                            }
                        })
                    }

                } else {
                    if(ImGui::Button("Install to Android")) {
                        INIT_MODAL("Installing Firebase")
                        _openPopup = true;
                    }
                }
            } else {
                ImGui::Text("You need the Android, IOS or both modules to be able to use Firebase");
            }

            static bool _showLoad = false;
            static auto _yes = [&]() {
                _openPopup = false;
                _showLoad = true;
                std::thread _firebaseToAndroid([&]() {
                    auto _command = APPEND_S("./firebaseInstallation/setupAndroid.sh ", projectSelector->getCurrentProject()->projectPath, " ",
                                             globalConfig->firebase.path, " ", "com.example.android ", globalConfig->GDEPath);
                    std::system(_command.c_str());
                    _showLoad = false;
                });
                _firebaseToAndroid.detach();
                INIT_MODAL("Begin Firebase Installation")
            };

            static auto _no = [&]() {
                _openPopup = false;
            };

            YES_NO_MACRO("Installing Firebase", &_openPopup, "Do you want to add the Firebase module to the project?", "Yes", "No", _yes, _no);

            if(_showLoad) {
                INIT_CENTERED_WINDOW
                if(ImGui::BeginPopupModal("Begin Firebase Installation")) {
                    const float _textWidth = ImGui::CalcTextSize("Modifying android data...").x;
                    const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 7.5f);
                    Spinner("Installing Firebase Components", 15.f, 6, _col);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textWidth / 2.f);
                    ImGui::Text("Modifying android data...");
                    ImGui::EndPopup();
                }
                END_CENTERED_WINDOW
            }

            ImGui::Separator();
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
