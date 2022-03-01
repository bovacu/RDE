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

    struct GlobalConfig {
        std::string GDEPath;
        std::vector<std::string> IDEs;
    };

    struct ProjectConfig {
        std::string IDE;
    };

    struct Project {
        std::string project;
        std::string projectName;
        std::string projectPath;
        bool selected;
        bool stillExists;
        ProjectConfig config;
    };

    struct ProjectList {
        FileHandler* projectsHandler = nullptr;
        std::vector<Project> content {};
    };

    class ProjectManagerLayer : public Layer {

        private:
            GlobalConfig globalConfig;
            ProjectList projectList;

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
