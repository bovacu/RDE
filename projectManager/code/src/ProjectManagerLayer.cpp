// Created by borja on 22/2/22.

#include <future>
#include "projectManager/code//include/ProjectManagerLayer.h"
#include "core/Engine.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "imgui_internal.h"

namespace engine {

    imgui_addons::ImGuiFileBrowser fileBrowser;

    void ProjectManagerLayer::onInit() {
        Renderer::setClearColor(Color::Black);

        projectList.projectsHandler = FilesSystem::open("assets/data.config", FileMode::READ);

        projectInstallator.init(&fileBrowser, &projectList, this);

        if(projectList.projectsHandler == nullptr) {
            loadModules();
            projectInstallator.setShow(true);
            return;
        }

        auto _configFile = FilesSystem::readAllLinesFile(projectList.projectsHandler);
        globalConfig.GDEPath = SPLIT_S_I(_configFile.content[0], "=", 1);
        auto _ides = SPLIT_S_I(_configFile.content[1], "=", 1);
        globalConfig.IDEs = SPLIT_S(_ides, ",");
        globalConfig.IDEs.emplace_back("Other...");
        FilesSystem::close(projectList.projectsHandler);

        ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_DockingEnable;

        if(!FilesSystem::fileExists("assets/projects.config")) {
            projectList.projectsHandler = FilesSystem::createFile("assets/projects.config");
        } else
            projectList.projectsHandler = FilesSystem::open("assets/projects.config", FileMode::READ);

        loadModules();
        projectSelector.loadProjects();
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
        if(projectList.projectsHandler != nullptr)
            FilesSystem::close(projectList.projectsHandler);
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
        projectModules.init(globalConfig.GDEPath);
        projectCreator.init(&projectList, &fileBrowser);
        projectSelector.init(&projectList, &projectCreator);
        projectCreator.setProjectSelector(&projectSelector);
        projectViewer.init(&projectSelector, &fileBrowser, &projectList, &globalConfig);
        projectMenuBar.init(&projectModules);
    }
}