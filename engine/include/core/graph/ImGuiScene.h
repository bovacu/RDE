#pragma once

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#if !IS_MOBILE()

#include <chrono>
#include "core/graph/Scene.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/profiling/Profiler.h"
#include "submodules/imgui/backends/imgui_impl_opengl3.h"
#include "submodules/imgui/backends/imgui_impl_sdl.h"

namespace GDE {

    // This class is a fucking mess, and it needs to be reworked, but for now I just use it for debugging and it does the job.

    class Graph;
    class ImGuiScene : public Scene {
        private:
            bool anyWindowHovered = false;
            bool windowsHovered[4] {false, false, false, false};
            UDelegate<bool(MouseScrolledEvent&)> mseDel;
            UDelegate<bool(MouseButtonPressedEvent&)> mbpeDel;
            UDelegate<bool(MouseMovedEvent&)> mmeDel;
            NodeID selectedNode = NODE_ID_NULL;
            NodeID selectedNodeCanvas = NODE_ID_NULL;

        public:
            bool show = false;

        public:
            ImGuiScene(Engine* _engine);
            ~ImGuiScene() override {};

            void onInit() override;
            void onEvent(Event& _e) override;
            void onUpdate(Delta _dt) override {  }
            void onImGuiRender(Delta _dt) override {  }
            void onEnd() override;

            void begin();
            void end();

            bool onMouseScrolled(MouseScrolledEvent& _e);
            bool onMouseClicked(MouseButtonPressedEvent& _e);
            bool onMouseMovedEvent(MouseMovedEvent& _e);

            void drawDebugInfo(Scene* _scene);

        private:
            void console();
            static int consoleStub(ImGuiInputTextCallbackData* _data);
            int consoleIntro(ImGuiInputTextCallbackData* _data);

            void charToIntSize(const std::string& _size, int* _resolution);
            void metrics();
            void mouseInfo();
            void showFileExplorer();

            void hierarchy(Scene* _scene);
            void hierarchyRecursionStub(Scene* _scene, Graph* _graph, NodeID _node, NodeID& _selectedNode);
            void showLoadedPrefabs(Scene* _scene, Graph* _graph, NodeID _node, NodeID& _selectedNode);
            void nodeComponents(Graph* _graph, const NodeID _selectedNode);
            void activeComponent(Graph* _graph, const NodeID _selectedNode);
            void transformComponent(Graph* _graph, const NodeID _selectedNode);
            void tagComponent(Graph* _graph, const NodeID _selectedNode);
            void cameraComponent(Graph* _graph, const NodeID _selectedNode);
            void bodyComponent(Graph* _graph, const NodeID _selectedNode);
            void spriteComponent(Graph* _graph, const NodeID _selectedNode);
            void textComponent(Graph* _graph, const NodeID _selectedNode);

            static std::unordered_map<ProfilerState, RollingBuffer> plotBuffers;

            // Debug
            void printFPSDrawCallsAndRAM();
            void printAtlases();
            void printResolutionFullscreenAndVSync();

            bool checkForFocus();
    };

}

#endif

#endif //IMGUI_LAYER_H
