// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_VIEWER_H
#define ENGINE_PROJECT_VIEWER_H

#include "ProjectSelector.h"

namespace engine {

    class GlobalConfig;
    class ProjectModules;
    class ProjectViewer {
        private:
            bool show, showRelocate, showDelete, showActionDeleteProject, showAndroidInstallTarget;
            bool blockOpenAndroidStudio;
            ProjectSelector* projectSelector;
            ProjectModules* projectModules = nullptr;
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

            char androidPackage[256] = "com.example.android";
            char androidApkName[256] = "app-debug";
            bool androidSplitBuild = false;

        public:
            void init(ProjectSelector* _projectSelector, ProjectModules* _projectModules, imgui_addons::ImGuiFileBrowser* _fileBrowser, ProjectList* _projectList, GlobalConfig* _globalConfig);
            void render();
            void setShow(bool _show);

        private:
            void projectOk();
            void projectError();
            void relocateProject();
            void destroyProject();

            void actionsDeleteProject();
            void deleteCurrentProjectFromManager();

            void androidActions();

            void firebaseActions();

            void checkErrors(const char* _newPath);
            void showErrors();

            bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
    };

}

#endif //ENGINE_PROJECT_VIEWER_H
