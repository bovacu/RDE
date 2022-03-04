// Created by borja on 22/2/22.

#include <future>
#include "projectManager/code//include/ProjectManagerLayer.h"
#include "core/Engine.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "imgui_internal.h"
#include "code/include/Macros.h"


namespace engine {

    imgui_addons::ImGuiFileBrowser fileBrowser;

    void ProjectManagerLayer::onInit() {
        Renderer::setClearColor(Color::Black);

        globalConfig.handler = FilesSystem::open("assets/data.json", FileMode::READ);
        LOAD_CONFIG(&globalConfig)

        if(globalConfig.GDEPath.empty()) {
            projectInstallator.init(&fileBrowser, &globalConfig, this);
            loadModules();
            projectInstallator.setShow(true);
            return;
        }

        ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_DockingEnable;

        loadModules();
    }

    void ProjectManagerLayer::onEvent(Event& _event) {  }
    void ProjectManagerLayer::onUpdate(Delta _dt) {  }
    void ProjectManagerLayer::onFixedUpdate(Delta _dt) { Layer::onFixedUpdate(_dt); }
    void ProjectManagerLayer::onRender(Delta _dt) {  }

    void ProjectManagerLayer::onImGuiRender(Delta _dt) {
        Layer::onImGuiRender(_dt);
        projectMenuBar.render();
        mainImGuiWindow();
//        ImGui::ShowDemoWindow();
        projectInstallator.render();
        projectModules.render();
        projectCreator.render();
    }

    void ProjectManagerLayer::onEnd() {
        Layer::onEnd();
        if(globalConfig.handler != nullptr)
            FilesSystem::close(globalConfig.handler);
    }

    void ProjectManagerLayer::mainImGuiWindow() {
        ImVec2 _size = {(float) Engine::get().getWindowSize().x, (float) Engine::get().getWindowSize().y};
        if(!projectInstallator.isShown()) {
            ImGui::SetNextWindowSize({_size.x, _size.y - projectMenuBar.getHeight()},ImGuiCond_Always);
            ImGui::SetNextWindowPos({0, projectMenuBar.getHeight()}, ImGuiCond_Always);
        } else {
            ImGui::SetNextWindowSize({_size.x, _size.y},ImGuiCond_Always);
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
        }

        ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse);
        projectSelector.render();
        ImGui::SameLine();
        projectViewer.render();
        ImGui::End();
    }

    void ProjectManagerLayer::loadModules() {
        projectModules.init(&globalConfig, &fileBrowser);
        projectCreator.init(&globalConfig, &fileBrowser);
        projectSelector.init(&globalConfig, &projectCreator);
        projectCreator.setProjectSelector(&projectSelector);
        projectViewer.init(&projectSelector, &projectModules, &fileBrowser, &globalConfig);
        projectMenuBar.init(&projectModules);
    }
}