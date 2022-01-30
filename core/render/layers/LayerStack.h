#pragma once

#ifndef LAYER_STACK_H
#define LAYER_STACK_H

#include "Layer.h"

namespace engine {

    class LayerStack {
        private:
            std::vector<Layer*> layers;
            unsigned int layerInsertIndex = 0;

        public:
            LayerStack() = default;
            ~LayerStack();

            void pushLayer(Layer* _layer);
            void pushOverlay(Layer* _overlay);
            void popLayer(Layer* _layer);
            void popOverlay(Layer* _overlay);

            std::vector<Layer*>::iterator begin() { return layers.begin(); }
            std::vector<Layer*>::iterator end() { return layers.end(); }
            std::vector<Layer*>::reverse_iterator rbegin() { return layers.rbegin(); }
            std::vector<Layer*>::reverse_iterator rend() { return layers.rend(); }

            std::vector<Layer*>::const_iterator begin() const { return layers.begin(); }
            std::vector<Layer*>::const_iterator end()	const { return layers.end(); }
            std::vector<Layer*>::const_reverse_iterator rbegin() const { return layers.rbegin(); }
            std::vector<Layer*>::const_reverse_iterator rend() const { return layers.rend(); }
    };

}

#endif //LAYER_STACK_H
