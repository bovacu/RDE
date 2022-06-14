// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"
#include "core/graph/Components.h"
#include "core/systems/uiSystem/Canvas.h"

namespace GDE {

    Scene::Scene(Engine* _engine, const std::string& _debugName)  : debugName(_debugName), mainGraph(this, _debugName), engine(_engine) {
        auto _mainCameraID = mainGraph.createNode("MainCamera");
        auto* _cameraTransform = mainGraph.getComponent<Transform>(_mainCameraID);
        auto* _camera = mainGraph.addComponent<Camera>(_mainCameraID, &_engine->getWindow(), _cameraTransform);
        cameras.push_back(_camera);
        mainCamera = _camera;

        auto* _canvas = new Canvas(this, &engine->getWindow(), "Canvas");
        canvases.push_back(_canvas);
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
        mainGraph.onRender();
    }

    void Scene::onDebugRender(Delta _dt) {
        mainGraph.onDebugRender();
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
        auto* _entityTransform = getMainGraph()->getComponent<Transform>(_newCameraID);
        auto* _camera = mainGraph.addComponent<Camera>(_newCameraID, window, _entityTransform);
        cameras.push_back(_camera);
        return cameras.back();
    }

    void Scene::enableCamera(const NodeID& _cameraID, bool _enable) {
        if(_enable) {
            if(mainGraph.hasComponent<Active>(_cameraID)) return;
            mainGraph.addComponent<Active>(_cameraID);
            return;
        }

        if(mainGraph.hasComponent<Active>(_cameraID)) return;
            mainGraph.removeComponent<Active>(_cameraID);
    }

    void Scene::removeCamera(const NodeID& _cameraID) {

    }

    std::vector<Camera*>& Scene::getCameras() {
        return cameras;
    }

    std::vector<Canvas*>& Scene::getCanvases() {
        return canvases;
    }

    Scene::~Scene() {
        for(auto* _canvas : canvases)
            delete _canvas;
    }
}
