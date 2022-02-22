// Created by borja on 22/2/22.


#ifndef ENGINE2_0_PROJECT_MANAGER_LAYER_H
#define ENGINE2_0_PROJECT_MANAGER_LAYER_H

#include "core/render/layers/Layer.h"

namespace engine {

    class ProjectManagerLayer : public Layer {
        private:
            float mainMenuHeight = 0;
            bool showInstallationWindow = false;
            bool showGDEModules = false;
            std::vector<std::string> modulesInstalled;

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
            void menuBar();
            void mainImGuiWindow();
            void installationWindow();
            void GDEModules();
    };

}

#endif //ENGINE2_0_PROJECT_MANAGER_LAYER_H
