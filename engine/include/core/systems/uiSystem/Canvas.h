//
// Created by borja on 29/05/22.
//

#ifndef ENGINE_CANVAS_H
#define ENGINE_CANVAS_H

#include "core/graph/Scene.h"

namespace GDE {

    class Canvas {
            friend class Graph;

        private:
            Camera* camera = nullptr;
            int layer = 0;
            NodeID ID;
            Graph graph;

        public:
            explicit Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag);

            Graph* getGraph();
            void onRender();
    };

}

#endif //ENGINE_CANVAS_H
