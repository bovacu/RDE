// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"
#include "core/graph/Components.h"

namespace GDE {

    Scene::Scene(const std::string& _debugName)  : debugName(_debugName), mainGraph(_debugName) {
        mainCameraID = mainGraph.createNode("MainCamera");
        auto* _camera = mainGraph.addComponent<Camera>(mainCameraID, &Engine::get().getWindow(), mainCameraID);
        _camera->getViewport()->update(Engine::get().getWindowSize());
        mainCamera = _camera;
        cameras.push_back(_camera);
    }

    void Scene::onEvent(Event& _event) {
        mainGraph.onEvent(_event);
    }

    void Scene::onUpdate(Delta _dt) {
        mainGraph.onUpdate(_dt);
    }

    void Scene::onFixedUpdate(Delta _dt) {
        mainGraph.onFixedUpdate(_dt);
    }

    void Scene::onRender(Delta _dt) {
        mainGraph.onRender(this);
    }

    void Scene::onDebugRender(Delta _dt) {
        mainGraph.onDebugRender(this);
    }

    Graph* Scene::getMainGraph() {
        return &mainGraph;
    }

    Camera* Scene::getMainCamera() {
        return mainCamera;
    }

    void Scene::switchMainCamera(const NodeID& _camera) {
        mainCamera = mainGraph.getComponent<Camera>(_camera);
    }

    Camera* Scene::addCamera(Window* window) {
        auto _newCameraID = mainGraph.createNode(APPEND_S("Camera", cameras.size()));
        auto* _camera = mainGraph.addComponent<Camera>(_newCameraID, window, _newCameraID);
        cameras.push_back(_camera);
        return cameras.back();
    }

    void Scene::enableCamera(const NodeID& _cameraID, bool _enable) {
        mainGraph.getComponent<Active>(_cameraID)->active = _enable;
    }

    void Scene::removeCamera(const NodeID& _cameraID) {

    }

    std::vector<Camera*>& Scene::getCameras() {
        return cameras;
    }
}
