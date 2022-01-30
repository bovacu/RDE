#include "LayerStack.h"

namespace engine {

    LayerStack::~LayerStack() {
        for (Layer* _layer : layers) {
            _layer->onEnd();
            delete _layer;
        }
    }

    void LayerStack::pushLayer(Layer* _layer) {
        layers.emplace(layers.begin() + layerInsertIndex, _layer);
        layerInsertIndex++;
    }

    void LayerStack::pushOverlay(Layer* _overlay) {
        layers.emplace_back(_overlay);
    }

    void LayerStack::popLayer(Layer* _layer) {
        auto it = std::find(layers.begin(), layers.begin() + layerInsertIndex, _layer);
        if (it != layers.begin() + layerInsertIndex) {
            _layer->onEnd();
            this->layers.erase(it);
            this->layerInsertIndex--;
        }
    }

    void LayerStack::popOverlay(Layer* _overlay) {
        auto it = std::find(this->layers.begin() + this->layerInsertIndex, this->layers.end(), _overlay);
        if (it != this->layers.end()) {
            _overlay->onEnd();
            this->layers.erase(it);
        }
    }

}