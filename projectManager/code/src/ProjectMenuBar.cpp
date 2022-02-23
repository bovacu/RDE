// Created by borja on 23/2/22.

#include "projectManager/code/include/ProjectMenuBar.h"
#include "imgui.h"

namespace engine {


    void ProjectMenuBar::init(ProjectModules* _projectModules) {
        projectModules = _projectModules;
        show = true;
    }

    void ProjectMenuBar::render() {
        if(!show) return;

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
                if (ImGui::MenuItem("GDE Modules...")) projectModules->setShow(true);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About")) {
                ImGui::EndMenu();
            }

            mainMenuHeight = ImGui::GetWindowSize().y;

            ImGui::EndMainMenuBar();
        }
    }

    void ProjectMenuBar::setShow(bool _show) {
        show = _show;
    }

    float ProjectMenuBar::getHeight() {
        return mainMenuHeight;
    }
}