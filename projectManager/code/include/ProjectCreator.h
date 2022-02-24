// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_CREATOR_H
#define ENGINE_PROJECT_CREATOR_H

#include "FileBrowser/ImGuiFileBrowser.h"

namespace engine {

    template<class T> inline T operator~ (T a) { return (T)~(int)a; }
    template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
    template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
    template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
    template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
    template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
    template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

    enum ProjectError {
        NONE =                  0,
        WRONG_PATH =            1 << 0,
        PATH_NOT_SET =          1 << 1,
        NAME_NOT_SET =          1 << 2,
        NAME_ALREADY_IN_USE =   1 << 3
    };

    class ProjectSelector;
    class ProjectList;
    class ProjectCreator {
        private:
            char projectPath[256] {};
            char projectName[256] {};
            bool showProjectCreator = false;
            ProjectError error = ProjectError::NONE;

            ProjectList* projectList = nullptr;
            imgui_addons::ImGuiFileBrowser* fileBrowser = nullptr;
            ProjectSelector* projectSelector;

        public:
            void init(ProjectList* _projectList, imgui_addons::ImGuiFileBrowser* _fileBrowser);
            void render();
            void setShow(bool _show);
            void setProjectSelector(ProjectSelector* _projectSelector);

        private:
            void createProject();
            void checkErrors();
            void showErrors();
    };

}

#endif //ENGINE_PROJECT_CREATOR_H
