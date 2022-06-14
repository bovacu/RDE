//
// Created by borja on 29/05/22.
//

#ifndef ENGINE_CANVAS_H
#define ENGINE_CANVAS_H

#include "core/graph/Scene.h"
#include "core/graph/Components.h"


namespace GDE {

    class Canvas {
            friend class Graph;
            friend class Scene;

        private:
            Scene* scene;
            Camera* camera = nullptr;
            int layer = 0;
            NodeID ID;
            Graph graph;

        public:
            explicit Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag);

            Graph* getGraph();
            Camera* getCamera();

            void matchMainCameraViewPort();

            void onEvent(EventDispatcher& _eventDispatcher, Event& _event);
            void onUpdate(Delta _dt);
            void onRender();
            void onDebugRender();
    };

}

#endif //ENGINE_CANVAS_H
