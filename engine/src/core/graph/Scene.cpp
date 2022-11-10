// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"
#include "core/graph/components/Transform.h"
#include "core/systems/uiSystem/Canvas.h"

namespace GDE {

    Scene::Scene(Engine* _engine, const std::string& _debugName)  : mainGraph(this, _debugName), debugName(_debugName), engine(_engine) {
        auto* _mainCameraNode = mainGraph.createNode("MainCamera");
        auto* _camera = mainGraph.addComponent<Camera>(_mainCameraNode->getID(), _mainCameraNode, &engine->manager, getMainGraph(), &_engine->getWindow());
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

    void Scene::switchMainCamera(Node* _camera) {
        mainCamera = _camera->getComponent<Camera>();
    }

    Camera* Scene::addCamera(Window* window) {
        auto _newCameraNode = mainGraph.createNode(APPEND_S("Camera", cameras.size()));
        auto* _camera = _newCameraNode->addComponent<Camera>(window);
        cameras.push_back(_camera);
        return cameras.back();
    }

    void Scene::enableCamera(Node* _camera, bool _enable) {
        if(_enable) {
            if(_camera->hasComponent<Active>()) return;
            _camera->addComponent<Active>();
            return;
        }

        if(_camera->hasComponent<Active>()) return;
        _camera->removeComponent<Active>();
    }

    void Scene::removeCamera(Node* _camera) {
        auto _it = std::find(cameras.begin(), cameras.end(), _camera->getComponent<Camera>());
        if(_it != cameras.end()) {
            cameras.erase(_it);
        }
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

        prefabs.clear();
    }

    Canvas* Scene::addNewCanvas(const std::string& _canvasTag) {
        auto* _canvas = new Canvas(this, &engine->getWindow(), _canvasTag);
        canvases.push_back(_canvas);
        return canvases.back();
    }

    void Scene::removeCanvas(const std::string& _canvasTag) {
        auto _found = std::find_if(canvases.begin(), canvases.end(), [&_canvasTag](Canvas* _c) {
            return _c->graph.name == _canvasTag;
        });
        if(_found == canvases.end()) return;

        canvases.erase(_found);
    }

    void Scene::removeCanvas(Canvas* _canvas) {
        auto _found = std::find(canvases.begin(), canvases.end(), _canvas);
        if(_found == canvases.end()) return;

        canvases.erase(_found);
    }

    std::vector<NodeID> Scene::getPrefabs() {
        std::vector<NodeID> _prefabs;

        for(auto& _pair : prefabs) _prefabs.push_back(_pair.second);

        return _prefabs;
    }

    NodeID Scene::getPrefab(const std::string& _prefabKey) {
        return prefabs[_prefabKey];
    }
}
