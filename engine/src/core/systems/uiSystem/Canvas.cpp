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
        auto _cameraNode = graph.createNode("CanvasCamera");
        camera = _cameraNode->addComponent<Camera>(_window);
        graph.onDataChanged.bind<&Canvas::onTreeChanged>(this);
    }

    void Canvas::traverseTree(Node* _node, bool _earlyBreak, void* _data, void (Canvas::*_preFunc)(Node* _node, bool&, void*), void (Canvas::*_postFunc)(Node* _node, void*)) {
        if(_node->getID() == NODE_ID_NULL) return;

        auto _transform = _node->getTransform();

        if(_preFunc != nullptr) (this->*_preFunc)(_node, _earlyBreak, _data);
        if(_earlyBreak) return;

        for(auto& _it : _transform->children) {
            traverseTree(_it->node, _earlyBreak, _data, _preFunc, _postFunc);
        }

        if(_postFunc != nullptr) (this->*_postFunc)(_node, _data);
    }

    void Canvas::createElementListTreePre(Node* _node, bool& _earlyBreak, void* _data) {
        if(!_node->hasComponent<Active>()) {
            _earlyBreak = true;
        }

        auto* _elements = (std::vector<CanvasElement>*)_data;
        CanvasElement _canvasElement { _node };

        if(_node->hasComponent<UIInteractable>()) {
            _canvasElement.interactable = _node->getComponent<UIInteractable>();
        }

        if(graph.getNodeContainer().any_of<TextRenderer, UIButton, NineSliceSprite, SpriteRenderer, UICheckbox, UISlider>(_node->getID())) {
            _canvasElement.renderizable = getRenderizable(_node);
        }

        if(graph.getNodeContainer().any_of<UISlider, UIInput>(_node->getID())) {
            _canvasElement.updatable = getUpdatable(_node);
        }

        _canvasElement.cropping = graph.getNodeContainer().any_of<UIInput>(_node->getID());

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
            _canvasElement->interactable->onEvent(_canvasElement->node, _onEventData->engine, *_onEventData->eventDispatcher, *_onEventData->event, this);
        }
    }

    void Canvas::onEvent(Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event) {
        OnEventData _data { _engine, &_eventDispatcher, &_event };
        for(auto _it = canvasElementsOrderedList.rbegin(); _it != canvasElementsOrderedList.rend(); _it++) {
            onEventTreeElement(&(*_it), (void*)&_data);
        }

        graph.onEventDel(graph.getNodeContainer(), _event);
    }




    IRenderizable* Canvas::getUpdatable(Node* _node) {
        if(_node->hasComponent<UISlider>()) {
            return _node->getComponent<UISlider>();
        }

        if(_node->hasComponent<UIInput>()) {
            return _node->getComponent<UIInput>();
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





    IRenderizable* Canvas::getRenderizable(Node* _node) {
        if(_node->hasComponent<NineSliceSprite>()) {
            return _node->getComponent<NineSliceSprite>();
        }

        if(_node->hasComponent<SpriteRenderer>()) {
            return _node->getComponent<SpriteRenderer>();
        }

        if(_node->hasComponent<TextRenderer>()) {
            return _node->getComponent<TextRenderer>();
        }

        return nullptr;
    }

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _data) {
        Batch* _currentBatch = &batches.back();

        if(_canvasElement->cropping) {
            stencils.push(_canvasElement->node->getID());
            auto* _transform = _canvasElement->node->getTransform();
            auto _position = _transform->getModelMatrixPosition();
            auto* _uiInput = _canvasElement->node->getComponent<UIInput>();

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

            _renderizable->drawBatched(_currentBatch->vertexBuffer, _currentBatch->indexBuffer, *_renderizable->node->getTransform(), *camera->getViewport());
        }
    }

    void Canvas::batchTreeElementPost(CanvasElement* _canvasElement, void* _data) {
        if(!stencils.empty() && stencils.top() == _canvasElement->node->getID()) {
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
        _renderManager.beginDraw(*camera, graph.getComponent<Transform>(camera->node->getID()));

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
       _renderManager.beginDebugDraw(*camera, graph.getComponent<Transform>(camera->node->getID()));

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