// Created by borja on 22/2/22.


#ifndef ENGINE2_0_PROJECT_MANAGER_LAYER_H
#define ENGINE2_0_PROJECT_MANAGER_LAYER_H

#include "core/render/layers/Layer.h"
#include "imgui.h"
#include "core/systems/fileSystem/FilesSystem.h"
#include "ProjectCreator.h"
#include "ProjectSelector.h"
#include "ProjectViewer.h"
#include "ProjectMenuBar.h"
#include "ProjectInstallator.h"
#include "ProjectModules.h"

namespace engine {

    struct Android {
        std::string sdk;
        std::string ndk;
        std::string androidStudio;
        std::string jdk;
    };

    struct Firebase {
        std::string path;
    };

    struct IDE {
        std::string name;
        std::string path;
    };

    struct GlobalConfig {
        std::vector<Project> projects {};

        std::string GDEPath;
        std::vector<IDE> IDEs;
        Android android;
        Firebase firebase;

        GlobalConfig& operator= (const GlobalConfig& _globalConfig) {

            if(this == &_globalConfig) return *this;

            projects = _globalConfig.projects;
            GDEPath = _globalConfig.GDEPath;
            IDEs = _globalConfig.IDEs;
            android = _globalConfig.android;
            firebase = _globalConfig.firebase;

            return *this;
        }
    };

    struct ProjectConfig {
        std::string IDE;
    };

    struct Project {
        std::string project;
        std::string projectName;
        std::string projectPath;
        ProjectConfig config;
        bool selected;
        bool stillExists;

        Project& operator= (const Project& _project) {

            if(this == &_project) return *this;

            project = _project.project;
            projectName = _project.projectName;
            projectPath = _project.projectPath;
            return *this;
        }
    };

    class ProjectManagerLayer : public Layer {

        private:
            GlobalConfig globalConfig;

            ProjectInstallator projectInstallator;
            ProjectCreator projectCreator;
            ProjectSelector projectSelector;
            ProjectViewer projectViewer;
            ProjectModules projectModules;
            ProjectMenuBar projectMenuBar;

        public:
            ProjectManagerLayer() : Layer("ProjectManagerLayer") {  }
            void onInit() override;
            void onEvent(Event& _event) override;
            void onUpdate(Delta _dt) override;
            void onFixedUpdate(Delta _dt) override;
            void onRender(Delta _dt) override;
            void onImGuiRender(Delta _dt) override;
            void onEnd() override;
            ~ProjectManagerLayer() override {  };

        private:
            void mainImGuiWindow();
            void loadModules();
    };

}

#endif //ENGINE2_0_PROJECT_MANAGER_LAYER_H
