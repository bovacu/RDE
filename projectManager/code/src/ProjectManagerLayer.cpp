// Created by borja on 22/2/22.

#include "projectManager/code//include/ProjectManagerLayer.h"
#include "imgui.h"
#include "core/Engine.h"
#include "core/systems/fileSystem/FilesSystem.h"
#include "FileBrowser/ImGuiFileBrowser.h"

namespace engine {

    imgui_addons::ImGuiFileBrowser fileBrowser;

    void ProjectManagerLayer::onInit() {
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
    }

    void ProjectManagerLayer::onEvent(Event& _event) {

    }

    void ProjectManagerLayer::onUpdate(Delta _dt) {

    }

    void ProjectManagerLayer::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
    }

    void ProjectManagerLayer::onRender(Delta _dt) {
        Renderer::clear(Color::White);
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
        ImGui::Begin("Modules", &showGDEModules, ImGuiWindowFlags_NoCollapse);
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

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ProjectManagerLayer::installationWindow() {
        static char _location[256];
        static float _installWidth = 0, _cancelWidth = 0;
        static bool _openFileBrowser = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize({-1, -1});
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
                auto _command = APPEND_S("./installer.sh ", _location)
                std::system(_command.c_str());
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

        ImGui::PopStyleVar();
    }

}