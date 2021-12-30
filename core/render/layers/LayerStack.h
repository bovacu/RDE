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

            std::vector<Layer*>::iterator begin() { return this->layers.begin(); }
            std::vector<Layer*>::iterator end() { return this->layers.end(); }
            std::vector<Layer*>::reverse_iterator rbegin() { return this->layers.rbegin(); }
            std::vector<Layer*>::reverse_iterator rend() { return this->layers.rend(); }

            std::vector<Layer*>::const_iterator begin() const { return this->layers.begin(); }
            std::vector<Layer*>::const_iterator end()	const { return this->layers.end(); }
            std::vector<Layer*>::const_reverse_iterator rbegin() const { return this->layers.rbegin(); }
            std::vector<Layer*>::const_reverse_iterator rend() const { return this->layers.rend(); }
    };

}

#endif //LAYER_STACK_H
