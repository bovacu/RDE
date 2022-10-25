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
#include "core/graph/components/ui/UISlider.h"

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
        graph.onDataChanged.bind<&Canvas::onTreeChanged>(this);
    }

    void Canvas::traverseTree(const NodeID& _nodeID, bool _earlyBreak, void* _data, void (Canvas::*_preFunc)(const NodeID&, bool&, void*), void (Canvas::*_postFunc)(const NodeID&, void*)) {
        if(_nodeID == NODE_ID_NULL) return;

        auto _transform = graph.getComponent<Transform>(_nodeID);

        if(_preFunc != nullptr) (this->*_preFunc)(_nodeID, _earlyBreak, _data);
        if(_earlyBreak) return;

        for(auto& _it : _transform->children) {
            traverseTree(_it->ID, _earlyBreak, _data, _preFunc, _postFunc);
        }

        if(_postFunc != nullptr) (this->*_postFunc)(_nodeID, _data);
    }

    void Canvas::createElementListTreePre(const NodeID& _nodeID, bool& _earlyBreak, void* _data) {
        if(!graph.hasComponent<Active>(_nodeID)) {
            _earlyBreak = true;
        }

        auto* _elements = (std::vector<CanvasElement>*)_data;
        CanvasElement _canvasElement { _nodeID };

        if(graph.hasComponent<UIInteractable>(_nodeID)) {
            _canvasElement.interactable = graph.getComponent<UIInteractable>(_nodeID);
        }

        if(graph.getNodeContainer().any_of<TextRenderer, UIButton, NineSliceSprite, SpriteRenderer, UICheckbox, UISlider>(_nodeID)) {
            _canvasElement.renderizable = getRenderizable(_nodeID);
        }

        if(graph.getNodeContainer().any_of<UISlider, UIInput>(_nodeID)) {
            _canvasElement.updatable = getUpdatable(_nodeID);
        }

        _canvasElement.cropping = graph.getNodeContainer().any_of<UIInput>(_nodeID);

        if(!_canvasElement.renderizable && !_canvasElement.interactable && !_canvasElement.updatable) return;

        _elements->push_back(_canvasElement);
    }

    void Canvas::onTreeChanged(void* _data) {
        bool _earlyBreak = false;
        canvasElementsOrderedList.clear();
        traverseTree(graph.sceneRoot, _earlyBreak, (void*)&canvasElementsOrderedList, &Canvas::createElementListTreePre, nullptr);
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }






    void Canvas::onEventTreeElement(CanvasElement* _canvasElement, void* _data) {
        auto* _onEventData = (OnEventData*)_data;
        if(_canvasElement->interactable) {
            _canvasElement->interactable->onEvent(_canvasElement->nodeID, _onEventData->engine, *_onEventData->eventDispatcher, *_onEventData->event, this);
        }
    }

    void Canvas::onEvent(Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event) {
        OnEventData _data { _engine, &_eventDispatcher, &_event };
        for(auto _it = canvasElementsOrderedList.rbegin(); _it != canvasElementsOrderedList.rend(); _it++) {
            onEventTreeElement(&(*_it), (void*)&_data);
        }

        graph.onEventDel(graph.getNodeContainer(), _event);
    }




    IRenderizable* Canvas::getUpdatable(const NodeID& _nodeID) {
        if(graph.hasComponent<UISlider>(_nodeID)) {
            return graph.getComponent<UISlider>(_nodeID);
        }

        if(graph.hasComponent<UIInput>(_nodeID)) {
            return graph.getComponent<UIInput>(_nodeID);
        }

        return nullptr;
    }

    void Canvas::updateTreeElement(CanvasElement* _canvasElement, void* _data) {
        if(_canvasElement->updatable) {
            _canvasElement->updatable->onUpdate(*(Delta*)_data);
        }
    }

    void Canvas::onUpdate(Delta _dt) {
        for(auto& _it : canvasElementsOrderedList) {
            updateTreeElement(&_it, (void*)&_dt);
        }
        graph.onUpdateDel(graph.getNodeContainer(), _dt);
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

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _data) {
        Batch* _currentBatch = &batches.back();

        if(_canvasElement->cropping) {
            stencils.push(_canvasElement->nodeID);
            auto* _transform = graph.getComponent<Transform>(_canvasElement->nodeID);
            auto _position = _transform->getModelMatrixPosition();
            auto* _uiInput = graph.getComponent<UIInput>(_canvasElement->nodeID);

            forceRender();
            _currentBatch = &batches.back();

            glEnable(GL_SCISSOR_TEST);
            glScissor((GLint)((float)scene->getMainCamera()->getViewport()->getVirtualResolution().x / 2.f + _position.x - (_uiInput->getSize().x * _uiInput->pivot.x)),
                      (GLint)((float)scene->getMainCamera()->getViewport()->getVirtualResolution().y / 2.f + _position.y - (_uiInput->getSize().y * _uiInput->pivot.y)),
                      (GLint)_uiInput->getSize().x,
                      (GLint)_uiInput->getSize().y
            );
        }

        if(_canvasElement->renderizable != nullptr) {
            auto* _renderizable = _canvasElement->renderizable;

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

    void Canvas::batchTreeElementPost(CanvasElement* _canvasElement, void* _data) {
        if(!stencils.empty() && stencils.top() == _canvasElement->nodeID) {
            stencils.pop();
            forceRender();
            glDisable(GL_SCISSOR_TEST);
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

    void Canvas::onRender() {
        auto& _registry = graph.getNodeContainer();

        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, graph.getComponent<Transform>(camera->ID));

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("assets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);

        for(auto& _it : canvasElementsOrderedList) {
            batchTreeElementPre(&_it, nullptr);
            batchTreeElementPost(&_it, nullptr);
        }

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

}