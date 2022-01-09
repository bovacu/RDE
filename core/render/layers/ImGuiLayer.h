#pragma once

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <chrono>
#include "Layer.h"
#include "core/render/window/event/MouseEvent.h"
#include "external/imgui/backends/imgui_impl_opengl3.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "core/systems/Profiler.h"

namespace engine {

    class ImGuiLayer : public Layer {
        public:
            ImGuiLayer();
            ~ImGuiLayer()                       override = default;

            void onInit()                       override;
            void onEvent(Event& _e)             override;
            void onUpdate(Delta _dt)         override {  }
            void onRender(Delta _dt)         override {  }
            void onImGuiRender(Delta _dt)    override {  }
            void onEnd()                        override;

            void begin();
            void end();

            bool onMouseScrolled(MouseScrolledEvent& _e);
            bool onMouseClicked(MouseButtonPressedEvent& _e);
            bool onMouseMovedEvent(MouseMovedEvent& _e);

            static void drawDebugInfo();

        private:
            static void charToIntSize(const std::string& _size, int* _resolution);
            static void metrics();
            static void mouseInfo();

            static std::unordered_map<ProfilerState, RollingBuffer> plotBuffers;

            // Debug
            static void printFPSDrawCallsAndRAM();
            static void printAtlases();
            static void printResolutionFullscreenAndVSync();
    };

}

#endif //IMGUI_LAYER_H
