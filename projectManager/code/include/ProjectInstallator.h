// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_INSTALLATOR_H
#define ENGINE_PROJECT_INSTALLATOR_H

#include <string>
#include "FileBrowser/ImGuiFileBrowser.h"
#include "ProjectCreator.h"

namespace engine {

    class ProjectManagerLayer;
    class ProjectList;
    class ProjectInstallator {
        private:
            bool show = false;
            bool showLoading = false;
            float installationPercentage = 0.f;
            imgui_addons::ImGuiFileBrowser* fileBrowser = nullptr;
            GlobalConfig* globalConfig = nullptr;
            std::string installationStep;
            char pathToInstallGDE[256] {};
            ProjectError error = ProjectError::NONE;
            ProjectManagerLayer* projectManagerLayer;

        public:
            void init(imgui_addons::ImGuiFileBrowser* _fileBrowser, GlobalConfig* _globalConfig, ProjectManagerLayer* _projectManagerLayer);
            void render();
            void setShow(bool _show);
            [[nodiscard]] bool isShown() const;

        private:
            bool BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
            bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
            void checkError();
            void showError();
    };

}

#endif //ENGINE_PROJECT_INSTALLATOR_H
