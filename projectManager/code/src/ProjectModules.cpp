// Created by borja on 23/2/22.

#include <thread>
#include "projectManager/code/include/ProjectModules.h"
#include "imgui.h"
#include "core/Engine.h"
#include "core/systems/fileSystem/FilesSystem.h"
#include "imgui_internal.h"
#include "projectManager/code/include/ProjectManagerLayer.h"
#include "code/include/Macros.h"

namespace engine {


    void ProjectModules::init(GlobalConfig* _globalConfig, imgui_addons::ImGuiFileBrowser* _fileBrowser) {
        fileBrowser = _fileBrowser;
        show = false;
        globalConfig = _globalConfig;
        GDEPath = globalConfig->GDEPath;

        auto* _dataHandler = FilesSystem::open("assets/data.config", FileMode::READ);
        auto _lines = FilesSystem::readAllLinesFile(_dataHandler).content;

        bool _foundAndroid = std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("ANDROID_SDK_PATH") != std::string::npos; }) != _lines.end();
        bool _foundFirebase = std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("FIREBASE_SDK_PATH") != std::string::npos; }) != _lines.end();
        bool _foundIOS = std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("IOS_SDK_PATH") != std::string::npos; }) != _lines.end();

        availableModules.emplace_back(Module{"Android", _foundAndroid, false, false, false, BIND_FUNC_0(ProjectModules::installAndroid)});
        availableModules.emplace_back(Module{"Firebase", _foundFirebase, false, false, false, BIND_FUNC_0(ProjectModules::installFirebase)});
        availableModules.emplace_back(Module{"IOs", _foundIOS, false, false, false, BIND_FUNC_0(ProjectModules::installIOSModule)});

        if(_foundAndroid) {
            globalConfig->android.sdk = SPLIT_S_I(*std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("ANDROID_SDK_PATH") != std::string::npos; }).base(), "=", 1);
            globalConfig->android.ndk = SPLIT_S_I(*std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("ANDROID_NDK_PATH") != std::string::npos; }).base(), "=", 1);
            globalConfig->android.androidStudio = SPLIT_S_I(*std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("ANDROID_STUDIO_PATH") != std::string::npos; }).base(), "=", 1);
            globalConfig->android.jdk = SPLIT_S_I(*std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("JDK_8_PATH") != std::string::npos; }).base(), "=", 1);

            LOG_W("Found Android -> SDK[", globalConfig->android.sdk, "], NDK[", globalConfig->android.ndk, "], AS[", globalConfig->android.androidStudio, "], JDK[", globalConfig->android.jdk, "]")
        }

        if(_foundFirebase) {
            globalConfig->firebase.path = SPLIT_S_I(*std::find_if(_lines.begin(), _lines.end(), [](const std::string& str) { return str.find("FIREBASE_SDK_PATH") != std::string::npos; }).base(), "=", 1);
            LOG_W("Found Firebase -> Path[", globalConfig->android.sdk)
        }
    }

    void ProjectModules::render() {
        if(show) {
            INIT_CENTERED_WINDOW_WITH_SIZE(Engine::get().getWindowSize().x * 0.25f, Engine::get().getWindowSize().y * 0.5f)
            INIT_MODAL("Modules");
            if(ImGui::BeginPopupModal("Modules", &show, ImGuiWindowFlags_NoResize)) {
                if(ImGui::BeginChild("ModulesTree", {0, -ImGui::GetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_NoMove)) {
                    for(auto& _module : availableModules) {
                        if(_module.installed) {
                            ImGui::PushStyleColor(ImGuiCol_Text, {0, 255, 0, 255});
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::Checkbox(_module.name.c_str(), &_module.installed);
                            ImGui::PopStyleColor();
                            ImGui::PopItemFlag();
                        } else
                            ImGui::Checkbox(_module.name.c_str(), &_module.setToBeInstalled);
                    }
                    ImGui::EndChild();
                }

                static float _size = 0;
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _size);
                if(ImGui::Button("Install Selected")) {
                    for(auto& _module : availableModules)
                        if(_module.setToBeInstalled && !_module.installed) modulesToInstall.push(_module);

                    if(!modulesToInstall.empty()) {
                        show = false;
                        currentModule = &modulesToInstall.front();
                        currentModule->needToBeInstalled = true;
                    }
                }
                _size = ImGui::GetItemRectSize().x + ImGui::GetStyle().WindowPadding.x;

                ImGui::EndPopup();
            }
            END_CENTERED_WINDOW
        }

        showAndroidInstallation();
        showFirebaseInstallation();
    }

    void ProjectModules::setShow(bool _show) {
        show = _show;
    }

    const std::vector<Module>& ProjectModules::getModules() {
        return availableModules;
    }

    void ProjectModules::showAndroidInstallation() {
        if(currentModule == nullptr) return;

        if(std::equal(currentModule->name.begin(), currentModule->name.end(), "Android") && currentModule->needToBeInstalled)
            INIT_MODAL("Android Installation")
        else
            return;

        INIT_CENTERED_WINDOW
        if(ImGui::BeginPopupModal("Android Installation")) {
            static float _leftAlign = ImGui::CalcTextSize("Android Studio").x + ImGui::GetFrameHeightWithSpacing();

            ImGui::Text("If you need to install any of the above, follow the steps, otherwise specify the path:");
            ImGui::Separator();
            ImGui::Text("Android Studio: "); ImGui::Indent();
            ImGui::Text("https://developer.android.com/studio"); ImGui::Unindent();
            ImGui::Text("Note*: This will also install the SDK");
            ImGui::Text("Android Studio"); ImGui::SameLine(_leftAlign);
            ImGui::InputText("###androidStudioPath", androidStudioPath, IM_ARRAYSIZE(androidStudioPath)); ImGui::SameLine();
            if(ImGui::Button("Open##AS")) ImGui::OpenPopup("File Browser Android Studio");

            ImGui::NewLine();
            ImGui::Text("Android SDK: ");
            ImGui::Text("Note*:It is inside the Android Studio directory");
            ImGui::Text("Android SDK"); ImGui::SameLine(_leftAlign);
            ImGui::InputText("###androidSDKPath", androidSDKPath, IM_ARRAYSIZE(androidSDKPath)); ImGui::SameLine();
            if(ImGui::Button("Open##SDK")) ImGui::OpenPopup("File Browser Android SDK");

            ImGui::NewLine();
            ImGui::Text("Android NDK: "); ImGui::Indent();
            ImGui::Text("https://androidsdkoffline.blogspot.com/p/android-ndk-side-by-side-direct-download.html"); ImGui::Unindent();
            ImGui::Text("Note*: Get the version 20.1.5948944, this is VERY IMPORTANT");
            ImGui::Text("Android NDK"); ImGui::SameLine(_leftAlign);
            ImGui::InputText("###androidNDKPath", androidNDKPath, IM_ARRAYSIZE(androidNDKPath)); ImGui::SameLine();
            if(ImGui::Button("Open##NDK")) ImGui::OpenPopup("File Browser Android NDK");

            ImGui::NewLine();
            ImGui::Text("JDK 8: "); ImGui::Indent();
            ImGui::Text("https://www.oracle.com/br/java/technologies/javase/javase8-archive-downloads.html"); ImGui::Unindent();
            ImGui::Text("Note*: This version is needed for the NDK to work properly (8u202)");
            ImGui::Text("Java JDK (8)"); ImGui::SameLine(_leftAlign);
            ImGui::InputText("###jdk8Path", jdk8Path, IM_ARRAYSIZE(jdk8Path)); ImGui::SameLine();
            if(ImGui::Button("Open##JDK")) ImGui::OpenPopup("File Browser JDK 8");

            ImGui::NewLine();

            showErrors();

            showAndroidWait();

            if(!showAndroidInstallationWait) {
                static float _buttonsSize = 0;

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _buttonsSize / 2.f);
                if(ImGui::Button("Install")) {
                    checkErrors();
                    if(error == ProjectAndroidErrors::PAE_NONE) {
                        currentModule->installationFunc();
                    }
                }

                _buttonsSize = ImGui::GetItemRectSize().x;

                ImGui::SameLine();

                if(ImGui::Button("Cancel")) {
                    modulesToInstall.pop();
                    currentModule = modulesToInstall.empty() ? nullptr : &modulesToInstall.front();
                    show = currentModule == nullptr;
                }

                _buttonsSize += ImGui::GetItemRectSize().x;
            }

            if(fileBrowser->showFileDialog("File Browser Android Studio", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*"))
                strcpy(androidStudioPath, fileBrowser->selected_path.c_str());

            if(fileBrowser->showFileDialog("File Browser Android SDK", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*"))
                strcpy(androidSDKPath, fileBrowser->selected_path.c_str());

            if(fileBrowser->showFileDialog("File Browser Android NDK", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*"))
                strcpy(androidNDKPath, fileBrowser->selected_path.c_str());

            if(fileBrowser->showFileDialog("File Browser JDK 8", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*"))
                strcpy(jdk8Path, fileBrowser->selected_path.c_str());

            ImGui::EndPopup();
        }
        END_CENTERED_WINDOW
    }

    void ProjectModules::installAndroid() {
        showAndroidInstallationWait = true;
        std::thread _installAndroid([&] {
            auto _command = APPEND_S("./androidInstallation/install.sh ", androidStudioPath, " ", androidSDKPath, " ", androidNDKPath, " ", jdk8Path);
            auto _result = std::system(_command.c_str()) / 256;
            switch(_result) {
                case 0: {
                    modulesToInstall.pop();
                    currentModule = modulesToInstall.empty() ? nullptr : &modulesToInstall.front();
                    if(currentModule != nullptr) currentModule->needToBeInstalled = true;
                    break;
                }
                case 1 : error |= ProjectAndroidErrors::PAE_PLATFORM_TOOLS_FAILED; break;
                case 2 : error |= ProjectAndroidErrors::PAE_BUILD_TOOLS_FAILED; break;
                case 3 : error |= ProjectAndroidErrors::PAE_CMAKE_TOOLS_FAILED; break;
                case 4 : error |= ProjectAndroidErrors::PAE_CMDLINE_TOOLS_FAILED; break;
                case 5 : error |= ProjectAndroidErrors::PAE_LICENSES_FAILED; break;
                default: error |= ProjectAndroidErrors::PAE_UNKNOWN_FAILED; break;
            }

            showAndroidInstallationWait = false;
        });
        _installAndroid.detach();
    }

    void engine::ProjectModules::installFirebase() {
        showFirebaseInstall = true;
        std::thread _installFirebase([&]() {
            auto _command = APPEND_S("./firebaseInstallation/install.sh ", globalConfig->GDEPath);
            int _result = std::system(_command.c_str()) / 256;
            showFirebaseInstall = false;

            if(_result == 0) {
                modulesToInstall.pop();
                currentModule = modulesToInstall.empty() ? nullptr : &modulesToInstall.front();
                if(currentModule != nullptr) currentModule->needToBeInstalled = true;
            }
        });
        _installFirebase.detach();
    }

    void ProjectModules::showFirebaseInstallation() {
        if(currentModule == nullptr) return;

        if(std::equal(currentModule->name.begin(), currentModule->name.end(), "Firebase") && currentModule->needToBeInstalled)
            INIT_MODAL("Firebase Installation")
        else
            return;

        INIT_CENTERED_WINDOW
        if(ImGui::BeginPopupModal("Firebase Installation", nullptr, ImGuiWindowFlags_NoResize)) {
            static float _yesWidth = 0, _noWidth = 0;
            auto _textSize = ImGui::CalcTextSize("Install Firebase Module?");

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - _textSize.x / 2.f);
            ImGui::Text("%s", "Install Firebase Module?");

            ImGui::NewLine();

            if(!showFirebaseInstall) {
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - (_yesWidth + _noWidth) / 2.f);
                if(ImGui::Button("Install")) {
                    currentModule->installationFunc();
                }
                _yesWidth = ImGui::GetItemRectSize().x;

                ImGui::SameLine();

                if(ImGui::Button("Cancel")) {
                    modulesToInstall.pop();
                    currentModule = modulesToInstall.empty() ? nullptr : &modulesToInstall.front();
                    if(currentModule != nullptr) currentModule->needToBeInstalled = true;
                }
                _noWidth = ImGui::GetItemRectSize().x;
            } else {
                const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 7.5f);
                Spinner("Installing Firebase Components", 15.f, 6, _col);
                ImGui::Text("Installing firebase, this might take a while...");
            }
            END_CENTERED_WINDOW
            ImGui::EndPopup();
        }
    }


    void engine::ProjectModules::installIOSModule() {

    }

    void ProjectModules::checkErrors() {
        error = ProjectAndroidErrors::PAE_NONE;

        std::ifstream _stream(androidStudioPath);
        if(!_stream.good()) error |= ProjectAndroidErrors::PAE_ANDROID_STUDIO_WRONG_PATH;
        _stream.close();
        if(strlen(androidStudioPath) == 0) error |= ProjectAndroidErrors::PAE_ANDROID_STUDIO_PATH_NOT_SET;

        _stream.open(androidSDKPath);
        if(!_stream.good()) error |= ProjectAndroidErrors::PAE_ANDROID_SDK_WRONG_PATH;
        _stream.close();
        if(strlen(androidSDKPath) == 0) error |= ProjectAndroidErrors::PAE_ANDROID_SDK_PATH_NOT_SET;

        _stream.open(androidNDKPath);
        if(!_stream.good()) error |= ProjectAndroidErrors::PAE_ANDROID_NDK_WRONG_PATH;
        _stream.close();
        if(strlen(androidNDKPath) == 0) error |= ProjectAndroidErrors::PAE_ANDROID_NDK_PATH_NOT_SET;

        _stream.open(jdk8Path);
        if(!_stream.good()) error |= ProjectAndroidErrors::PAE_JDK_WRONG_PATH;
        _stream.close();
        if(strlen(jdk8Path) == 0) error |= ProjectAndroidErrors::PAE_JDK_PATH_NOT_SET;
    }

    void ProjectModules::showErrors() {
        if((error & ProjectAndroidErrors::PAE_ANDROID_STUDIO_PATH_NOT_SET) == ProjectAndroidErrors::PAE_ANDROID_STUDIO_PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Android Studio path cannot be empty");
        if((error & ProjectAndroidErrors::PAE_ANDROID_STUDIO_WRONG_PATH) == ProjectAndroidErrors::PAE_ANDROID_STUDIO_WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "Android Studio path is not valid");

        if((error & ProjectAndroidErrors::PAE_ANDROID_SDK_PATH_NOT_SET) == ProjectAndroidErrors::PAE_ANDROID_SDK_PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Android SDK path cannot be empty");
        if((error & ProjectAndroidErrors::PAE_ANDROID_SDK_WRONG_PATH) == ProjectAndroidErrors::PAE_ANDROID_SDK_WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "Android SDK path is not valid");

        if((error & ProjectAndroidErrors::PAE_ANDROID_NDK_PATH_NOT_SET) == ProjectAndroidErrors::PAE_ANDROID_NDK_PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "Android NDK path cannot be empty");
        if((error & ProjectAndroidErrors::PAE_ANDROID_NDK_WRONG_PATH) == ProjectAndroidErrors::PAE_ANDROID_NDK_WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "Android NDK path is not valid");

        if((error & ProjectAndroidErrors::PAE_JDK_PATH_NOT_SET) == ProjectAndroidErrors::PAE_JDK_PATH_NOT_SET)
            ImGui::TextColored({255, 0, 0, 255}, "JDK 8 path cannot be empty");
        if((error & ProjectAndroidErrors::PAE_JDK_WRONG_PATH) == ProjectAndroidErrors::PAE_JDK_WRONG_PATH)
            ImGui::TextColored({255, 0, 0, 255}, "JDK 8 path is not valid");

        if((error & ProjectAndroidErrors::PAE_PLATFORM_TOOLS_FAILED) == ProjectAndroidErrors::PAE_PLATFORM_TOOLS_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "The platforms tools couldn't be installed, check the assets/androidInstallationLogs.txt to view the error");
        if((error & ProjectAndroidErrors::PAE_BUILD_TOOLS_FAILED) == ProjectAndroidErrors::PAE_BUILD_TOOLS_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "The build tools couldn't be installed, check the assets/androidInstallationLogs.txt to view the error");
        if((error & ProjectAndroidErrors::PAE_CMDLINE_TOOLS_FAILED) == ProjectAndroidErrors::PAE_CMDLINE_TOOLS_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "The cmdline tools couldn't be installed, check the assets/androidInstallationLogs.txt to view the error");
        if((error & ProjectAndroidErrors::PAE_CMAKE_TOOLS_FAILED) == ProjectAndroidErrors::PAE_CMAKE_TOOLS_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "The cmake tools couldn't be installed, check the assets/androidInstallationLogs.txt to view the error");
        if((error & ProjectAndroidErrors::PAE_LICENSES_FAILED) == ProjectAndroidErrors::PAE_LICENSES_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "The licenses couldn't be accepted, check the assets/androidInstallationLogs.txt to view the error");
        if((error & ProjectAndroidErrors::PAE_UNKNOWN_FAILED) == ProjectAndroidErrors::PAE_UNKNOWN_FAILED)
            ImGui::TextColored({255, 0, 0, 255}, "Unknown error, check the assets/androidInstallationLogs.txt to view the error");
    }

    void ProjectModules::showAndroidWait() {
        if(!showAndroidInstallationWait) return;
        const ImU32 _col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 7.5f);
        Spinner("Installing Android Components", 15.f, 6, _col);
    }

    bool ProjectModules::Spinner(const char* label, float radius, int thickness, const ImU32& color) {
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
