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
        loadModules();

        if(projectList.projectsHandler == nullptr) {
            projectInstallator.setShow(true);
            return;
        }

        location = SPLIT_S_I(FilesSystem::readLineInFile(projectList.projectsHandler, 0).content, "=", 1);
        FilesSystem::close(projectList.projectsHandler);

        ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_DockingEnable;

        if(!FilesSystem::fileExists("assets/projects.config")) {
            projectList.projectsHandler = FilesSystem::createFile("assets/projects.config");
        } else
            projectList.projectsHandler = FilesSystem::open("assets/projects.config", FileMode::READ);

        reloadProjects();
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

    void ProjectManagerLayer::reloadProjects() {
        auto _projects = FilesSystem::readAllLinesFile(projectList.projectsHandler).content;
        projectList.content.clear();
        for(auto _project : _projects) projectList.content.emplace_back(Project{_project, false});
    }

    void ProjectManagerLayer::loadModules() {
        projectModules.init(location);
        projectCreator.init(&projectList, &fileBrowser);
        projectSelector.init(&projectList, &projectCreator);
        projectViewer.init(&projectSelector);
        projectMenuBar.init(&projectModules);
    }
}