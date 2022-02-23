// Created by borja on 22/2/22.

#include <future>
#include "projectManager/code//include/ProjectManagerLayer.h"
#include "core/Engine.h"
#include "core/systems/fileSystem/FilesSystem.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "imgui_internal.h"

namespace engine {

    imgui_addons::ImGuiFileBrowser fileBrowser;

    void ProjectManagerLayer::onInit() {
        showInstallationWindow = true;
        return;


        auto* _handler = FilesSystem::open("assets/data.config", FileMode::READ);

        if(_handler == nullptr) {
            showInstallationWindow = true;
            return;
        }

        auto _pathToGDE = SPLIT_S_I(FilesSystem::readLineInFile(_handler, 0).content, "=", 1);

        auto _path = APPEND_S(_pathToGDE, "/GDEAndroid/CMakeLists.txt");
        if(FilesSystem::fileExists(_path))
            modulesInstalled.emplace_back("ANDROID");

        _path = APPEND_S(_pathToGDE, "/GDEFirebase/CMakeLists.txt");
        if(FilesSystem::fileExists(_path))
            modulesInstalled.emplace_back("FIREBASE");

        FilesSystem::close(_handler);

        ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_DockingEnable;

        Renderer::setClearColor(Color::Black);
    }

    void ProjectManagerLayer::onEvent(Event& _event) {

    }

    void ProjectManagerLayer::onUpdate(Delta _dt) {

    }

    void ProjectManagerLayer::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
    }

    void ProjectManagerLayer::onRender(Delta _dt) {

    }

    void ProjectManagerLayer::onImGuiRender(Delta _dt) {
        Layer::onImGuiRender(_dt);
        if(!showInstallationWindow) menuBar();
        mainImGuiWindow();
//        ImGui::ShowDemoWindow();
        if(showInstallationWindow) installationWindow();

        if(showGDEModules) GDEModules();
    }

    void ProjectManagerLayer::onEnd() {
        Layer::onEnd();
    }

    void ProjectManagerLayer::menuBar() {
        if(ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("GDE Modules..."))
                    showGDEModules = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About")) {
                ImGui::EndMenu();
            }

            mainMenuHeight = ImGui::GetWindowSize().y;

            ImGui::EndMainMenuBar();
        }
    }

    void ProjectManagerLayer::mainImGuiWindow() {
        if(!showInstallationWindow) {
            ImGui::SetNextWindowSize(
                    {(float) Engine::get().getWindowSize().x, (float) Engine::get().getWindowSize().y - mainMenuHeight},
                    ImGuiCond_Always);
            ImGui::SetNextWindowPos({0, mainMenuHeight}, ImGuiCond_Always);
        } else {
            ImGui::SetNextWindowSize({(float) Engine::get().getWindowSize().x, (float) Engine::get().getWindowSize().y},
                                     ImGuiCond_Always);
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
        }

        ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse);
        if(ImGui::BeginChild("LeftPanel", {ImGui::GetWindowSize().x * 0.25f, -1}, true)) {
            ImGui::EndChild();
        }

        ImGui::SameLine();

        if(ImGui::BeginChild("RightPanel", {-1, -1}, true)) {
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void ProjectManagerLayer::GDEModules() {
        static bool _selected[3] {
            std::find(modulesInstalled.begin(), modulesInstalled.end(), "ANDROID") != modulesInstalled.end(),
            std::find(modulesInstalled.begin(), modulesInstalled.end(), "FIREBASE") != modulesInstalled.end(),
            std::find(modulesInstalled.begin(), modulesInstalled.end(), "IOS") != modulesInstalled.end(),
        };
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        ImGui::SetNextWindowSize({Engine::get().getWindowSize().x * 0.25f, Engine::get().getWindowSize().y * 0.5f}, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::OpenPopup("Modules");
        if(ImGui::BeginPopupModal("Modules", &showGDEModules)) {
//            ImGui::Begin("Modules", &showGDEModules, ImGuiWindowFlags_NoCollapse);
            if(ImGui::BeginChild("ModulesTree", {0, -ImGui::GetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_NoMove)) {
                if(ImGui::TreeNode("Available modules")) {
                    ImGui::Checkbox("Android", &_selected[0]);
                    ImGui::Checkbox("Firebase", &_selected[1]);
                    ImGui::Checkbox("IOs", &_selected[2]);
                    ImGui::TreePop();
                }
                ImGui::EndChild();
            }

            static float _size = 0;
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - _size);
            if(ImGui::Button("Install Selected")) {

            }

            _size = ImGui::GetItemRectSize().x;

            ImGui::SameLine();

            if(ImGui::Button("Remove Selected")) {

            }

            _size += ImGui::GetItemRectSize().x + ImGui::GetFrameHeightWithSpacing();

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    static void asyncInstaller(std::string* installStep, float* installPercentage, const std::string& _path) {
        auto _command = APPEND_S("./installer.sh ", _path)
        std::system(_command.c_str());
        *installPercentage = 0.2f;

        _command = APPEND_S("./installationSteps/0.sh ", _path)
        *installStep = "Installing dependencies...";
        std::system(_command.c_str());
        *installPercentage = 0.3f;

        _command = APPEND_S("./installationSteps/1.sh ", _path)
        *installStep = "Installing submodules...";
        std::system(_command.c_str());
        *installPercentage = 0.5f;

        _command = APPEND_S("./installationSteps/2.sh ", _path)
        *installStep = "Installing and building SDL...";
        std::system(_command.c_str());
        *installPercentage = 0.6f;

        _command = APPEND_S("./installationSteps/3.sh ", _path)
        *installStep = "Installing and building SDL_image...";
        std::system(_command.c_str());
        *installPercentage = 0.7f;

        _command = APPEND_S("./installationSteps/4.sh ", _path)
        *installStep = "Installing and building SDL_mixer...";
        std::system(_command.c_str());
        *installPercentage = 0.8f;

        _command = APPEND_S("./installationSteps/5.sh ", _path)
        *installStep = "Installing and building SDL_net...";
        std::system(_command.c_str());
        *installPercentage = 0.9f;

        _command = APPEND_S("./installationSteps/6.sh ", _path)
        *installStep = "Installing and building freetype...";
        std::system(_command.c_str());
        *installPercentage = 1.f;
    }

    void ProjectManagerLayer::installationWindow() {
        static char _location[256];
        static float _installWidth = 0, _cancelWidth = 0;
        static bool _openFileBrowser = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize({-1, -1});

        if(!showInstallingLoadingBarModal) {
            ImGui::OpenPopup("Installation");
            if(ImGui::BeginPopupModal("Installation", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)) {
                ImGui::Text("Path to install"); ImGui::SameLine();
                ImGui::InputText("###path", _location, IM_ARRAYSIZE(_location)); ImGui::SameLine();

                if(ImGui::Button("Open...")) {
                    ImGui::OpenPopup("FileBrowser");
                }

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - (_installWidth + _cancelWidth));
                if(ImGui::Button("Install")) {
                    auto* _handler = FilesSystem::createFile("assets/data.config");
                    auto _line = APPEND_S("GDE_path=", std::string (_location))
                    FilesSystem::appendChunkToFileAtEnd(_handler, _line);
                    std::thread _asyncInstallation(asyncInstaller, &installStep, &installPercentage, _location);
                    _asyncInstallation.detach();
                    showInstallingLoadingBarModal = true;
                    FilesSystem::close(_handler);
                }
                _installWidth = ImGui::GetItemRectSize().x / 2.f;

                ImGui::SameLine();

                if(ImGui::Button("Cancel")) {
                    Engine::get().setRunning(false);
                }

                _cancelWidth = ImGui::GetItemRectSize().x / 2.f;

                if(fileBrowser.showFileDialog("FileBrowser", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT,
                                              ImVec2(Engine::get().getWindowSize().x * 0.75f, Engine::get().getWindowSize().y * 0.35f), "*.*")) {
                    strcpy(_location, fileBrowser.selected_path.c_str());
                }

                ImGui::EndPopup();
            }
        }

        ImGui::PopStyleVar();

        if(showInstallingLoadingBarModal) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
            ImGui::SetNextWindowSize({-1, -1});
            ImGui::OpenPopup("ProgressInstallation");
            if(ImGui::BeginPopupModal("ProgressInstallation")) {
                ImGui::Text("%s", installStep.c_str());
                const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

                BufferingBar("##buffer_bar", installPercentage, ImVec2(400, 6), bg, col);
                ImGui::EndPopup();
            }

            ImGui::PopStyleVar();

            if(installPercentage == 1.f) {
                showInstallingLoadingBarModal = false;
            }
        }
    }


    bool ProjectManagerLayer::BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;

        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart*value, bb.Max.y), fg_col);

        const float t = g.Time;
        const float r = size.y / 2;
        const float speed = 1.5f;

        const float a = speed*0;
        const float b = speed*0.333f;
        const float c = speed*0.666f;

        const float o1 = (circleWidth+r) * (t+a - speed * (int)((t+a) / speed)) / speed;
        const float o2 = (circleWidth+r) * (t+b - speed * (int)((t+b) / speed)) / speed;
        const float o3 = (circleWidth+r) * (t+c - speed * (int)((t+c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);

        return true;
    }

    bool ProjectManagerLayer::Spinner(const char* label, float radius, int thickness, const ImU32& color) {
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