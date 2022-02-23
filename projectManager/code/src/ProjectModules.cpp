// Created by borja on 23/2/22.

#include "projectManager/code/include/ProjectModules.h"
#include "imgui.h"
#include "core/Engine.h"
#include "core/systems/fileSystem/FilesSystem.h"

namespace engine {


    void ProjectModules::init(const std::string& _gdePath) {
        show = false;

        GDEPath = _gdePath;
        auto _path = APPEND_S(GDEPath, "/GDEAndroid/CMakeLists.txt");
        availableModules.emplace_back(Module{"Android", FilesSystem::fileExists(_path)});

        _path = APPEND_S(GDEPath, "/GDEFirebase/CMakeLists.txt");
        availableModules.emplace_back(Module{"Firebase", FilesSystem::fileExists(_path)});

        _path = APPEND_S(GDEPath, "/GDEIOS/CMakeLists.txt");
        availableModules.emplace_back(Module{"IOs", FilesSystem::fileExists(_path)});
    }

    void ProjectModules::render() {
        if(!show) return;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
        ImGui::SetNextWindowSize({Engine::get().getWindowSize().x * 0.25f, Engine::get().getWindowSize().y * 0.5f}, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::OpenPopup("Modules");
        if(ImGui::BeginPopupModal("Modules", &show)) {
            if(ImGui::BeginChild("ModulesTree", {0, -ImGui::GetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_NoMove)) {
                for(auto& _module : availableModules)
                    ImGui::Checkbox(_module.name.c_str(), &_module.installed);
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

    void ProjectModules::setShow(bool _show) {
        show = _show;
    }

    const std::vector<Module>& ProjectModules::getModules() {
        return availableModules;
    }
}