#include "LayerStack.h"

namespace engine {

    LayerStack::~LayerStack() {
        for (Layer* _layer : this->layers) {
            _layer->onEnd();
            delete _layer;
        }
    }

    void LayerStack::pushLayer(Layer* _layer) {
        this->layers.emplace(this->layers.begin() + this->layerInsertIndex, _layer);
        this->layerInsertIndex++;
    }

    void LayerStack::pushOverlay(Layer* _overlay) {
        this->layers.emplace_back(_overlay);
    }

    void LayerStack::popLayer(Layer* _layer) {
        auto it = std::find(this->layers.begin(), this->layers.begin() + this->layerInsertIndex, _layer);
        if (it != this->layers.begin() + this->layerInsertIndex) {
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