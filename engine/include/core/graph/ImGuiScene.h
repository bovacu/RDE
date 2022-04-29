#pragma once

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#if !IS_MOBILE()

#include <chrono>
#include "core/graph/Scene.h"
#include "core/render/window/event/MouseEvent.h"
#include "core/systems/profiling/Profiler.h"
#include "submodules/imgui/backends/imgui_impl_opengl3.h"
#include "submodules/imgui/backends/imgui_impl_sdl.h"

namespace GDE {

    class Graph;
    class ImGuiScene : public Scene {
        private:
            bool anyWindowHovered = false;
            UDelegate<bool(MouseScrolledEvent&)> mseDel;
            UDelegate<bool(MouseButtonPressedEvent&)> mbpeDel;
            UDelegate<bool(MouseMovedEvent&)> mmeDel;
            NodeID selectedNode = NODE_ID_NULL;

        public:
            ImGuiScene();
            ~ImGuiScene() override = default;

            void onInit() override;
            void onEvent(Event& _e) override;
            void onUpdate(Delta _dt) override {  }
            void onRender(Delta _dt) override {  }
            void onImGuiRender(Delta _dt) override {  }
            void onEnd() override;

            void begin();
            void end();

            bool onMouseScrolled(MouseScrolledEvent& _e);
            bool onMouseClicked(MouseButtonPressedEvent& _e);
            bool onMouseMovedEvent(MouseMovedEvent& _e);

            void drawDebugInfo(Graph* _mainGraph);

        private:
            void console();
            static int consoleStub(ImGuiInputTextCallbackData* _data);
            int consoleIntro(ImGuiInputTextCallbackData* _data);

            void charToIntSize(const std::string& _size, int* _resolution);
            void metrics();
            void mouseInfo();
            void showFileExplorer();

            void hierarchy(Graph* _graph);
            void hierarchyRecursionStub(Graph* _graph, NodeID _node, int& _childCount);
            void nodeComponents(Graph* _graph);

            static std::unordered_map<ProfilerState, RollingBuffer> plotBuffers;

            // Debug
            void printFPSDrawCallsAndRAM();
            void printAtlases();
            void printResolutionFullscreenAndVSync();
    };

}

#endif

#endif //IMGUI_LAYER_H
