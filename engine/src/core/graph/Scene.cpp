// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"

namespace engine {

    Scene::Scene(const std::string& _debugName)  : debugName(_debugName), mainGraph(_debugName) {
        mainCameraID = mainGraph.createNode("MainCamera");
        auto* _camera = mainGraph.addComponent<Camera>(mainCameraID, &Engine::get().getWindow(), mainCameraID);
        _camera->getViewport()->update(Engine::get().getWindowSize());
//        _camera->recalculateViewMatrix();
    }

    Graph* Scene::getMainGraph() {
        return &mainGraph;
    }

    Camera* Scene::getMainCamera() {
        return mainCameraID == NODE_ID_NULL ? nullptr : mainGraph.getComponent<Camera>(mainCameraID);
    }
}
