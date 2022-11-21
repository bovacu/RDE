//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UI9Slice.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"

namespace GDE {

    struct OnEventData {
        Engine* engine;
        EventDispatcher* eventDispatcher;
        Event* event;
    };

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) : graph(_scene, _canvasTag, true) {
        scene = _scene;
        auto _cameraNode = graph.createNode("CanvasCamera");
        camera = _cameraNode->addComponent<Camera>(_window);
        camera->setCameraSize(_window->getWindowSize());
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

        if(graph.getNodeContainer().any_of<UIText, UIButton, UI9Slice, UIImage, UICheckbox, UISlider>(_node->getID()) && _node->isEnabledStateOn(EnabledStates::DS_RENDER)) {
            _canvasElement.renderizable = getRenderizable(_node);
        }

        if(graph.getNodeContainer().any_of<UISlider, UIInput>(_node->getID()) && _node->isEnabledStateOn(EnabledStates::DS_UPDATE)) {
            _canvasElement.updatable = getUpdatable(_node);
        }

        if(graph.getNodeContainer().any_of<UIInput>(_node->getID()) && _node->isEnabledStateOn(EnabledStates::DS_RENDER)) {
            _canvasElement.cropping = _node->getTransform()->getChildrenCount();
        }

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
        if(_canvasElement->interactable && !_canvasElement->node->hasComponent<DisabledForEvent>()) {
            _canvasElement->interactable->onEvent(_canvasElement->node, _onEventData->engine, *_onEventData->eventDispatcher, *_onEventData->event);
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
        if(_canvasElement->updatable && !_canvasElement->node->hasComponent<DisabledForUpdate>()) {
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
        if(_node->hasComponent<UI9Slice>()) {
            return _node->getComponent<UI9Slice>();
        }

        if(_node->hasComponent<UIImage>()) {
            return _node->getComponent<UIImage>();
        }

        if(_node->hasComponent<UIText>()) {
            return _node->getComponent<UIText>();
        }

        return nullptr;
    }

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _data) {
        Batch* _currentBatch = &batches.back();

        if(_canvasElement->cropping) {
            stencils.push(_canvasElement->node->getID());
            auto* _transform = (UITransform*)_canvasElement->node->getTransform();
            auto _position = _transform->getModelMatrixPosition();
            auto* _uiInput = _canvasElement->node->getComponent<UIInput>();

            forceRender();
            _currentBatch = &batches.back();

            glEnable(GL_SCISSOR_TEST);
            auto _resolution = scene->getMainCamera()->getViewport()->getDeviceResolution();
            auto _bottomLeftCorner = Vec2<GLint> {
                    (GLint)(((float)_resolution.x - _uiInput->getSize().x) * 0.5f + _position.x),
                    (GLint)(((float)_resolution.y - _uiInput->getSize().y) * 0.5f + _position.y)
            };
            auto _size = Vec2<GLint> {
                    (GLint)(_uiInput->getSize().x),
                    (GLint)(_uiInput->getSize().y)
            };
            glScissor(_bottomLeftCorner.x, _bottomLeftCorner.y, _size.x, _size.y);
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
        if(_canvasElement->cropping == 0) {
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
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

    void Canvas::onRender() {
        auto& _registry = graph.getNodeContainer();

        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(*camera, (Transform*)graph.getComponent<UITransform>(camera->node->getID()));

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);

        // Depending on the element, we will need to restrict the area that can be rendered. In that case and as we
        // precalculate a list in the correct order of rendering, we need to know when to begin the cropping and when to
        // finish it, in this case we create a list of root elements that need to be cropped. We store how many children
        // the root cropped element has, and we simply make a countdown until all children have been drawn, then we apply
        // the crop.
        std::vector<int> _cropList;

        for(auto& _it : canvasElementsOrderedList) {
            batchTreeElementPre(&_it, nullptr);

            for(auto& _crop : _cropList) {
                _crop--;
                if(_crop == 0) batchTreeElementPost(&_it, nullptr);
            }

            if(_it.cropping > 0) _cropList.push_back(_it.cropping);
        }

        _renderManager.drawUI(batches);

        graph.onRenderDel(_registry);
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();
        auto _interactables = _registry.group<UIInteractable>(entt::get<UITransform, Active>);

        auto& _renderManager = graph.scene->engine->manager.renderManager;
       _renderManager.beginDebugDraw(*camera, (Transform*)graph.getComponent<UITransform>(camera->node->getID()));

        _registry.group<UI9Slice>(entt::get<UITransform, Active>).each([this, &_renderManager](const auto _entity, UI9Slice& _ui9Slice, UITransform& _transform, const Active& _) {
            if(!_ui9Slice.enabled) return;
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _ui9Slice.getSize();
            Vec2F _pointSize = {4, 4};

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;
            _pointSize *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);

            _renderManager.drawSquare(_pos, _pointSize, Color::Blue);
            _renderManager.setPointSize(4);
            // TODO: PIVOT CHANGES
//            _renderManager.drawPoint({_transform.getModelMatrixPosition().x - (_ui9Slice.getSize().x * 0.5f - _ui9Slice.getSize().x * _transform.getPivot().x),
//                                      _transform.getModelMatrixPosition().y - (_ui9Slice.getSize().y * 0.5f - _ui9Slice.getSize().y * _transform.getPivot().y) }, Color::Green);
//            _renderManager.drawPoint({_transform.getModelMatrixPosition().x - (_ui9Slice.getSize().x * 0.5f),
//                                      _transform.getModelMatrixPosition().y - (_ui9Slice.getSize().y * 0.5f)}, Color::Green);
        });

        // TODO: PIVOT CHANGES
//        _registry.group<UIImage>(entt::get<UITransform, Active>).each([&_renderManager](const auto _entity, UIImage& _uiImage, UITransform& _transform, const Active& _) {
//            if(!_uiImage.enabled) return;
//            DebugShape _shape;
//            _shape.makeSquare(_transform.getModelMatrixPosition(), _uiImage.getSize());
//            _shape.showOutsideColor(true);
//            _shape.setOutlineColor(Color::Blue);
//            _shape.showInnerColor(false);
//            _shape.setRotation(_transform.getModelMatrixRotation());
//            _renderManager.drawShape(_shape);
//
//            _renderManager.drawSquare(_transform.getModelMatrixPosition(), {4, 4}, Color::Blue);
//            _renderManager.setPointSize(4);
//            _renderManager.drawPoint({_transform.getModelMatrixPosition().x - (_uiImage.getSize().x * 0.5f - _uiImage.getSize().x * _transform.getPivot().x),
//                                      _transform.getModelMatrixPosition().y - (_uiImage.getSize().y * 0.5f - _uiImage.getSize().y * _transform.getPivot().y) }, Color::Green);
//        });
//
//        _registry.group<UIButton>(entt::get<UITransform, Active>).each([&_renderManager](const auto _entity, UIButton& _uiButton, UITransform& _transform, const Active& _) {
//            if(!_uiButton.enabled) return;
//            DebugShape _shape;
//            _shape.makeSquare(_transform.getModelMatrixPosition(), _uiButton.getSize());
//            _shape.showOutsideColor(true);
//            _shape.setOutlineColor(Color::Yellow);
//            _shape.showInnerColor(false);
//            _shape.setRotation(_transform.getModelMatrixRotation());
//            _renderManager.drawShape(_shape);
//
//            _renderManager.drawSquare(_transform.getModelMatrixPosition(), {4, 4}, Color::Blue);
//            _renderManager.setPointSize(4);
//            _renderManager.drawPoint({_transform.getModelMatrixPosition().x - (_uiButton.getSize().x * 0.5f - _uiButton.getSize().x * _transform.getPivot().x),
//                                      _transform.getModelMatrixPosition().y - (_uiButton.getSize().y * 0.5f - _uiButton.getSize().y * _transform.getPivot().y) }, Color::Green);
//        });

//        _registry.group<UIText>(entt::get<UITransform, Active>).each([&_renderManager](const auto _entity, UIText& _textRenderer, UITransform& _transform, const Active& _) {
//            DebugShape _shape;
//            _shape.rotation = _transform.getModelMatrixRotation();
//            _shape.makeSquare(_transform.getModelMatrixPosition(), _textRenderer.getSize());
//            _shape.showInnerColor(false);
//            _shape.showOutsideColor(true);
//            _shape.setOutlineColor(Color::Blue);
//            _renderManager.drawShape(_shape);
//
//            _renderManager.drawSquare(_transform.getModelMatrixPosition(), {4, 4}, Color::Blue);
//            _renderManager.setPointSize(4);
//            _renderManager.drawPoint({_transform.getModelMatrixPosition().x - _textRenderer.getSize().x * _transform.getPivot().y,
//                                      _transform.getModelMatrixPosition().y - _textRenderer.getSize().y * _transform.getPivot().x }, Color::Green);
//        });

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

//        _sprites.each([&_renderManager](const auto _entity, UI9Slice& _nineSlice, const Transform& _transform, const Active& _) {
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

    void Canvas::onResize(uint _width, uint _height) {
        ((UITransform*)graph.sceneRoot->getTransform())->setSize({ (float)_width, (float)_height });
        camera->setCameraSize((int)_width, (int)_height);
    }

    void Canvas::setCanvasResolution(const Vec2I& _resolution) {
        camera->getViewport()->virtualResolution = _resolution;
    }

    Vec2I Canvas::getCanvasResolution() {
        return camera->getViewport()->virtualResolution;
    }

}