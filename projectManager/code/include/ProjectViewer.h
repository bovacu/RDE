// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_VIEWER_H
#define ENGINE_PROJECT_VIEWER_H

#include "ProjectSelector.h"

namespace engine {

    class GlobalConfig;
    class ProjectViewer {
        private:
            bool show, showRelocate, showDelete;
            ProjectSelector* projectSelector;
            imgui_addons::ImGuiFileBrowser* fileBrowser;
            ProjectList* projectList;

            GlobalConfig* globalConfig;

            ImFont* defaultFont;
            ImFont* h1;
            ImFontConfig h1Conf;
            ImFont* h2;
            ImFontConfig h2Conf;
            ImFont* h3;
            ImFontConfig h3Conf;

            ProjectError error = ProjectError::NONE;

        public:
            void init(ProjectSelector* _projectSelector, imgui_addons::ImGuiFileBrowser* _fileBrowser, ProjectList* _projectList, GlobalConfig* _globalConfig);
            void render();
            void setShow(bool _show);

        private:
            void projectOk();
            void projectError();
            void relocateProject();
            void destroyProject();

            void checkErrors(const char* _newPath);
            void showErrors();
    };

}

#endif //ENGINE_PROJECT_VIEWER_H
