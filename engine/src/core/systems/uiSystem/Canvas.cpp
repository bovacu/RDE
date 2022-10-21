//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/render/Camera.h"
#include "core/graph/components/Transform.h"
#include "core/graph/Scene.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/NineSliceSprite.h"
#include "core/render/RenderManager.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"

namespace GDE {

    struct OnEventData {
        Engine* engine;
        EventDispatcher* eventDispatcher;
        Event* event;
    };

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag) {
        scene = _scene;
        auto _cameraID = graph.createNode("CanvasCamera");
        auto* _canvasTransform = graph.getComponent<Transform>(_cameraID);
        camera = graph.addComponent<Camera>(_cameraID, _window, _canvasTransform);
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }

    void Canvas::onEvent(Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event) {
        OnEventData _data { _engine, &_eventDispatcher, &_event };
        traverseTreeReverse(graph.sceneRoot, (void*)&_data, &Canvas::onEventTreeElement, nullptr);
        graph.onEventDel(graph.getNodeContainer(), _event);
    }

    void Canvas::onUpdate(Delta _dt) {

    }

    void Canvas::onRender() {
        auto& _registry = graph.getNodeContainer();

        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, graph.getComponent<Transform>(camera->ID));

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("assets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
        traverseTree(graph.sceneRoot, nullptr, &Canvas::drawTreeElementPre, &Canvas::drawTreeElementPost);

        _renderManager.drawUI(batches);

        graph.onRenderDel(_registry);
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();
        auto _texts = _registry.group<TextRenderer>(entt::get<Transform, Active>);
        auto _interactables = _registry.group<UIInteractable>(entt::get<Transform, Active>);

        auto& _renderManager = graph.scene->engine->manager.renderManager;
       _renderManager.beginDebugDraw(*camera, graph.getComponent<Transform>(camera->ID));

//        _interactables.each([&_renderManager](const auto _entity, UIInteractable& _interactable, Transform& _transform, const Active& _) {
//            DebugShape _shape;
//            _shape.rotation = _transform.getModelMatrixRotation();
//            _shape.makeSquare(_transform.getModelMatrixPosition(), _interactable.sizeOfInteraction);
//            _shape.showInnerColor(false);
//            _shape.showOutsideColor(true);
//            _shape.setOutlineColor(Color::Blue);
//            _renderManager.drawShape(_shape);
//        });
//
//        _texts.each([&_renderManager](const auto _entity, TextRenderer& _textRenderer, Transform& _transform, const Active& _) {
//            DebugShape _shape;
//            _shape.rotation = _transform.getModelMatrixRotation();
//            _shape.makeSquare(_transform.getModelMatrixPosition(), _textRenderer.getSize());
//            _shape.showInnerColor(false);
//            _shape.showOutsideColor(true);
//            _shape.setOutlineColor(Color::Green);
//            _renderManager.drawShape(_shape);
//        });

//        _sprites.each([&_renderManager](const auto _entity, NineSliceSprite& _nineSlice, const Transform& _transform, const Active& _) {
//            if(!_nineSlice.getNineSlice().isInteractable()) return;
//
//            auto _rects = _nineSlice.getNineSlice().subRects;
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[0].bottomLeftCorner.x + (float)_rects[0].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[0].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[0].size.y / 2.f},
//                                      {(float)_rects[0].size.x, (float)_rects[0].size.y}, Color::Green);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[1].bottomLeftCorner.x + (float)_rects[1].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[1].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[1].size.y / 2.f},
//                                      {(float)_rects[1].size.x, (float)_rects[1].size.y}, Color::Black);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[2].bottomLeftCorner.x + (float)_rects[2].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[2].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[2].size.y / 2.f},
//                                      {(float)_rects[2].size.x, (float)_rects[2].size.y}, Color::Blue);
////
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[3].bottomLeftCorner.x + (float)_rects[3].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[3].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[3].size.y / 2.f},
//                                      {(float)_rects[3].size.x, (float)_rects[3].size.y}, Color::Yellow);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[4].bottomLeftCorner.x + (float)_rects[4].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[4].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[4].size.y / 2.f},
//                                      {(float)_rects[4].size.x, (float)_rects[4].size.y}, Color::Orange);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[5].bottomLeftCorner.x + (float)_rects[5].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[5].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[5].size.y / 2.f},
//                                      {(float)_rects[5].size.x, (float)_rects[5].size.y}, Color::Gray);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[6].bottomLeftCorner.x + (float)_rects[6].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[6].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[6].size.y / 2.f},
//                                      {(float)_rects[6].size.x, (float)_rects[6].size.y}, Color::Magenta);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[7].bottomLeftCorner.x + (float)_rects[7].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[7].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[7].size.y / 2.f},
//                                      {(float)_rects[7].size.x, (float)_rects[7].size.y}, Color::Purple);
//
//            _renderManager.drawSquare({_transform.getPosition().x - (float)_nineSlice.getRegion().size.x / 2.f + (float)_rects[8].bottomLeftCorner.x + (float)_rects[8].size.x / 2.f,
//                                       _transform.getPosition().y + ((float)_rects[8].bottomLeftCorner.y - _nineSlice.getRegion().size.y) - (float)_nineSlice.getRegion().size.y / 2.f + (float)_rects[8].size.y / 2.f},
//                                      {(float)_rects[8].size.x, (float)_rects[8].size.y}, Color::White);
//
//        });
        _renderManager.endDebugDraw();
    }

    Camera* Canvas::getCamera() {
        return camera;
    }

    void Canvas::matchMainCameraViewPort() {
        auto* _mainCameraViewPort = scene->getMainCamera()->getViewport();

        if(_mainCameraViewPort == nullptr) return;
        camera->setAdaptiveViewport(_mainCameraViewPort->getVirtualResolution(), _mainCameraViewPort->getDeviceResolution());
    }

    void Canvas::traverseTree(const NodeID& _nodeID, void* _data, void (Canvas::*_preFunc)(const NodeID&, void*), void (Canvas::*_postFunc)(const NodeID&, void*)) {
        if(_nodeID == NODE_ID_NULL) return;

        auto _transform = graph.getComponent<Transform>(_nodeID);

        if(_preFunc != nullptr) (this->*_preFunc)(_nodeID, _data);

        for(auto& _it : _transform->children) {
            traverseTree(_it->ID, _data, _preFunc, _postFunc);
        }

        if(_postFunc != nullptr) (this->*_postFunc)(_nodeID, _data);
    }

    void Canvas::traverseTreeReverse(const NodeID& _nodeID, void* _data, void (Canvas::*_preFunc)(const NodeID&, void*), void (Canvas::*_postFunc)(const NodeID&, void*)) {
        if(_nodeID == NODE_ID_NULL) return;

        auto _transform = graph.getComponent<Transform>(_nodeID);

        if(_preFunc != nullptr) (this->*_preFunc)(_nodeID, _data);

        for(auto _it = _transform->children.rbegin(); _it != _transform->children.rend(); _it++) {
            traverseTree((*_it)->ID, _data, _preFunc, _postFunc);
        }

        if(_postFunc != nullptr) (this->*_postFunc)(_nodeID, _data);
    }

    IRenderizable* Canvas::getRenderizable(const NodeID& _nodeID) {
        if(graph.hasComponent<NineSliceSprite>(_nodeID)) {
            return graph.getComponent<NineSliceSprite>(_nodeID);
        }

        if(graph.hasComponent<SpriteRenderer>(_nodeID)) {
            return graph.getComponent<SpriteRenderer>(_nodeID);
        }

        if(graph.hasComponent<TextRenderer>(_nodeID)) {
            return graph.getComponent<TextRenderer>(_nodeID);
        }

        return nullptr;
    }

    void Canvas::drawTreeElementPre(const NodeID& _nodeID, void* _data) {
        Batch* _currentBatch = &batches.back();

        if(graph.hasComponent<UIInput>(_nodeID)) {
            stencils.push(_nodeID);
            auto* _transform = graph.getComponent<Transform>(_nodeID);
            auto _position = _transform->getModelMatrixPosition();
            auto* _uiInput = graph.getComponent<UIInput>(_nodeID);

            forceRender();
            _currentBatch = &batches.back();

            glEnable(GL_SCISSOR_TEST);
            glScissor((GLint)((float)scene->getMainCamera()->getViewport()->getVirtualResolution().x / 2.f + _position.x - (_uiInput->getSize().x * _uiInput->pivot.x)),
                      (GLint)((float)scene->getMainCamera()->getViewport()->getVirtualResolution().y / 2.f + _position.y - (_uiInput->getSize().y * _uiInput->pivot.y)),
                      (GLint)_uiInput->getSize().x,
                      (GLint)_uiInput->getSize().y
                      );
        }

        if(graph.getNodeContainer().any_of<TextRenderer, UIButton, NineSliceSprite, SpriteRenderer, UICheckbox>(_nodeID)){
            auto* _renderizable = getRenderizable(_nodeID);

            if(_renderizable != nullptr) {
                if (_currentBatch->shader == nullptr || _renderizable->getTexture() != _currentBatch->textureID || _currentBatch->shader->getShaderID() != _renderizable->shaderID || _currentBatch->indexBuffer.size() + 6 >= maxIndicesPerDrawCall) {
                    Batch _newBatch;
                    _newBatch.shader = scene->engine->manager.shaderManager.getShader(_renderizable->shaderID);
                    _newBatch.textureID = _renderizable->getTexture();
                    batches.emplace_back(_newBatch);
                    _currentBatch = &batches.back();
                }

                _renderizable->drawBatched(_currentBatch->vertexBuffer, _currentBatch->indexBuffer, *_renderizable->transform, *camera->getViewport());
            }
        }
    }

    void Canvas::drawTreeElementPost(const NodeID& _nodeID, void* _data) {
        if(!stencils.empty() && stencils.top() == _nodeID) {
            stencils.pop();
            forceRender();
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void Canvas::onEventTreeElement(const NodeID& _nodeID, void* _data) {
        auto* _onEventData = (OnEventData*)_data;
        if(graph.hasComponent<UIInteractable>(_nodeID)) {
            graph.getComponent<UIInteractable>(_nodeID)->onEvent(_nodeID, _onEventData->engine, *_onEventData->eventDispatcher, *_onEventData->event, this);
        }
    }

    void Canvas::forceRender() {
        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, nullptr);
        _renderManager.drawUI(batches);

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("assets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

}