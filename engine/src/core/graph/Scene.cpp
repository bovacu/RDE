// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"
#include "core/graph/components/Transform.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/util/Functions.h"
#include "entt/entt.hpp"
#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"

namespace RDE {

    Scene::Scene(Engine* _engine, const std::string& _debugName)  : mainGraph(this, _debugName), debugName(_debugName), engine(_engine) {
        auto* _mainCameraNode = mainGraph.createNode("MainCamera");
        auto* _camera = _mainCameraNode->addComponent<Camera>(&_engine->getWindow());
        cameras.push_back(_camera);
        mainCamera = _camera;

        auto* _canvas = new Canvas(this, &engine->getWindow(), "Canvas");
        canvases.push_back(_canvas);
    }

    Scene::~Scene() {
        for(auto* _canvas : canvases)
            delete _canvas;

        prefabs.clear();
    }

    void Scene::onInnerEvent(Event& _event) {
        // for(auto* _canvas : canvases) {
        //     _canvas->onEvent(engine, _event);
        // }
    }

    void Scene::onInnerUpdate(Delta _dt) {
        auto _animations = mainGraph.registry.view<AnimationSystem, SpriteRenderer, Active>(entt::exclude<DisabledForRender>);
        auto _particleSystems = mainGraph.registry.view<ParticleSystem, Active>(entt::exclude<DisabledForRender>);

        _animations.each([&_dt](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _) {
            _animationSystem.update(_dt, &_spriteRenderer);
        });

        _particleSystems.each([&_dt](const auto _entity, ParticleSystem& _particleSystem, const Active& _) {
            _particleSystem.update(_dt);
        });

        onUpdate(_dt);

        // for(auto* _canvas : canvases) {
        //     _canvas->onUpdate(_dt);
        // }
    }

    void Scene::onInnerFixedUpdate(Delta _dt) {
        for(auto* _body : engine->manager.physics.bodies) {
            if(!_body->isEnabled()) continue;
            _body->update();
        }

        // for(auto* _canvas : canvases) {
        //    _canvas->onFixedUpdate();
        // }
    }

    void Scene::recalculateRenderizableTree(Node* _node, std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>>* _renderizables) {
        auto _id = _node->getID();

        if(!_node->hasComponent<DisabledForRender>() && _node->hasComponent<Active>()) {

            if(_node->hasComponent<SpriteRenderer>()) {
                _renderizables[0].emplace_back(&_node->getComponent<SpriteRenderer>()->data, _node->getTransform(), nullptr );
            }

            if(_node->hasComponent<DynamicSpriteRenderer>()) {
                _renderizables[1].emplace_back(&_node->getComponent<DynamicSpriteRenderer>()->data, _node->getTransform(), nullptr );
            }

            if(_node->hasComponent<ParticleSystem>()) {
                _renderizables[2].emplace_back(&_node->getComponent<ParticleSystem>()->data, _node->getTransform(), nullptr );
            }

            if(_node->hasComponent<TextRenderer>()) {
                auto* _textRenderer = _node->getComponent<TextRenderer>();
                _renderizables[3].emplace_back( &_textRenderer->data, _node->getTransform(), (void*)_textRenderer );
            }
        }

        for(auto* _child : _node->getTransform()->children) {
            recalculateRenderizableTree(_child->node, _renderizables);
        }
    }

    void Scene::onInnerLateUpdate(Delta _dt) {
        if(mainGraph.isRenderizableTreeDirty) {
            // [0] -> SpriteRenderer
            // [1] -> DynamicSpriteRenderer
            // [2] -> ParticleSystem
            // [3] -> TextRenderer
            const int RENDERIZABLES_COUNT = 4;

            mainGraph.renderizableTree[0].clear();
            mainGraph.renderizableTree[1].clear();

            std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> _renderizables[RENDERIZABLES_COUNT];
            recalculateRenderizableTree(mainGraph.getRoot(), _renderizables);

            mainGraph.renderizableTree[0].reserve((int)_renderizables[0].size() + (int)_renderizables[1].size() + (int)_renderizables[2].size());
            mainGraph.renderizableTree[1].reserve((int)_renderizables[3].size());

            mainGraph.renderizableTree[0].insert(mainGraph.renderizableTree[0].end(), _renderizables[0].begin(), _renderizables[0].end());
            mainGraph.renderizableTree[0].insert(mainGraph.renderizableTree[0].end(), _renderizables[1].begin(), _renderizables[1].end());
            mainGraph.renderizableTree[0].insert(mainGraph.renderizableTree[0].end(), _renderizables[2].begin(), _renderizables[2].end());
            mainGraph.renderizableTree[1].insert(mainGraph.renderizableTree[1].end(), _renderizables[3].begin(), _renderizables[3].end());

            mainGraph.isRenderizableTreeDirty = false;
        }

        mainGraph.registry.view<DynamicSpriteRenderer, Active>().each([](const NodeID _nodeID, DynamicSpriteRenderer& _dynamicSpriteRenderer, Active& _) {
            if(_dynamicSpriteRenderer.isEnabled()) {
                ((CPUTexture*)_dynamicSpriteRenderer.data.texture)->uploadToGPU();
            }
        });

        for(auto* _canvas : canvases) {
            _canvas->onLateUpdate();
        }

        onLateUpdate(_dt);
    }

    void Scene::onInnerRender(Delta _dt) {
        auto& _renderManager = engine->manager.renderManager;

        for(auto* _camera : cameras) {
            if(!_camera->node->hasComponent<Active>()) continue;

            _renderManager.beginDraw(_camera, _camera->node->getComponent<Transform>());
            _camera->update();
            {
                for(auto [_innerData, _transform, _extraData] : mainGraph.renderizableTree[0]) {
                    _renderManager.drawSpriteRenderer(*_innerData, _transform);
                }

                for(auto [_innerData, _transform, _extraData] : mainGraph.renderizableTree[1]) {
                    _innerData->extraInfo = _extraData;
                    _renderManager.drawTextRenderer(*_innerData, _transform);
                }
            }

            _renderManager.endDraw();
        }

        for(auto* _dirtyTransform : mainGraph.dirtyTransforms) {
            _dirtyTransform->clearDirty();
        }
        mainGraph.dirtyTransforms.clear();
    }

    void Scene::onInnerRenderUI(Delta _dt) {
        // for(auto* _canvas : canvases) {
        //     _canvas->onRender();
        // }
    }

    void Scene::onInnerDebugRender(Delta _dt) {
        auto& _renderManager = engine->manager.renderManager;
        _renderManager.beginDebugDraw(mainCamera, mainCamera->node->getComponent<Transform>());
        engine->manager.physics.debugRender(&_renderManager);
        _renderManager.endDebugDraw();

        // for(auto* _canvas : canvases) {
        //     _canvas->onDebugRender();
        // }
    }

    Graph* Scene::getGraph() {
        return &mainGraph;
    }

    Camera* Scene::getMainCamera() {
        return mainCamera;
    }

    void Scene::switchMainCamera(Node* _camera) {
        mainCamera = _camera->getComponent<Camera>();
    }

    Camera* Scene::addCamera(Window* window) {
        auto _newCameraNode = mainGraph.createNode(Util::String::appendToString("Camera", cameras.size()));
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

    // TODO (RDE): Implement canvas and graph refresh after the display is changed on runtime.
    void Scene::onDisplayChanged() {
        Util::Log::warn("The display changed, Scene has received the event but it has no effect as it needs to be implemented");        
    }
}
