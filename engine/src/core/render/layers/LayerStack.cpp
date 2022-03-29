#include "core/render/layers/LayerStack.h"

namespace engine {

    LayerStack::~LayerStack() {
        for (Scene* _layer : layers) {
            _layer->onEnd();
            delete _layer;
        }
    }

    void LayerStack::pushLayer(Scene* _layer) {
        layers.emplace(layers.begin() + layerInsertIndex, _layer);
        layerInsertIndex++;
    }

    void LayerStack::pushOverlay(Scene* _overlay) {
        layers.emplace_back(_overlay);
    }

    void LayerStack::popLayer(Scene* _layer) {
        auto it = std::find(layers.begin(), layers.begin() + layerInsertIndex, _layer);
        if (it != layers.begin() + layerInsertIndex) {
            _layer->onEnd();
            layers.erase(it);
            layerInsertIndex--;
        }
    }

    void LayerStack::popOverlay(Scene* _overlay) {
        auto it = std::find(layers.begin() + layerInsertIndex, layers.end(), _overlay);
        if (it != layers.end()) {
            _overlay->onEnd();
            layers.erase(it);
        }
    }

}