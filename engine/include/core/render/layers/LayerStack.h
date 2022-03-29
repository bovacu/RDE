#pragma once

#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include <vector>
#include "engine/include/core/graph/Scene.h"

namespace engine {

    class LayerStack {
        private:
            std::vector<Scene*> layers;
            unsigned int layerInsertIndex = 0;

        public:
            LayerStack() = default;
            ~LayerStack();

            void pushLayer(Scene* _layer);
            void pushOverlay(Scene* _overlay);
            void popLayer(Scene* _layer);
            void popOverlay(Scene* _overlay);

            Scene* getLayer(int _index) { return layers[_index]; }

            std::vector<Scene*>::iterator begin() { return layers.begin(); }
            std::vector<Scene*>::iterator end() { return layers.end(); }
            std::vector<Scene*>::reverse_iterator rbegin() { return layers.rbegin(); }
            std::vector<Scene*>::reverse_iterator rend() { return layers.rend(); }

            std::vector<Scene*>::const_iterator begin() const { return layers.begin(); }
            std::vector<Scene*>::const_iterator end()	const { return layers.end(); }
            std::vector<Scene*>::const_reverse_iterator rbegin() const { return layers.rbegin(); }
            std::vector<Scene*>::const_reverse_iterator rend() const { return layers.rend(); }
    };

}

#endif //LAYER_STACK_H
