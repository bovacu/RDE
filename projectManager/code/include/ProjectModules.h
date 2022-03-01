// Created by borja on 23/2/22.


#ifndef ENGINE_PROJECT_MODULES_H
#define ENGINE_PROJECT_MODULES_H

#include <string>
#include <vector>
#include <queue>
#include <functional>
#include "ProjectCreator.h"

namespace engine {

    struct Module {
        std::string name;
        bool installed, setToBeInstalled, needToBeInstalled, hasFinishedInstalling;
        std::function<void()> installationFunc;
    };

    enum ProjectAndroidErrors {
        PAE_NONE = 0,

        PAE_ANDROID_STUDIO_WRONG_PATH = 1 << 0,
        PAE_ANDROID_STUDIO_PATH_NOT_SET = 1 << 1,

        PAE_ANDROID_SDK_WRONG_PATH = 1 << 2,
        PAE_ANDROID_SDK_PATH_NOT_SET = 1 << 3,

        PAE_ANDROID_NDK_WRONG_PATH = 1 << 4,
        PAE_ANDROID_NDK_PATH_NOT_SET = 1 << 5,

        PAE_JDK_WRONG_PATH = 1 << 6,
        PAE_JDK_PATH_NOT_SET = 1 << 7,

        PAE_PLATFORM_TOOLS_FAILED = 1 << 8,
        PAE_BUILD_TOOLS_FAILED = 1 << 9,
        PAE_CMDLINE_TOOLS_FAILED = 1 << 10,
        PAE_CMAKE_TOOLS_FAILED = 1 << 11,
        PAE_LICENSES_FAILED = 1 << 12,
        PAE_UNKNOWN_FAILED = 1 << 13,
    };

    class ProjectModules {
        private:
            bool show;
            imgui_addons::ImGuiFileBrowser* fileBrowser = nullptr;

            std::vector<Module> availableModules;
            std::queue<Module> modulesToInstall;
            Module* currentModule = nullptr;
            std::string GDEPath;
            ProjectAndroidErrors error = ProjectAndroidErrors::PAE_NONE;

            char androidStudioPath[256] = {0};
            char androidSDKPath[256] = {0};
            char androidNDKPath[256] = {0};
            char jdk8Path[256] = {0};
            bool showAndroidInstallationWait = false;

        public:
            void init(const std::string& _gdePath, imgui_addons::ImGuiFileBrowser* _fileBrowser);
            void render();
            void setShow(bool _show);
            const std::vector<Module>& getModules();

        private:
            void installAndroidModule();
            void checkErrors();
            void showErrors();
            void showAndroidWait();

            void installFirebaseModule();
            void installIOSModule();

            bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
    };

}

#endif //ENGINE_PROJECT_MODULES_H
