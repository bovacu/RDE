//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIMask.h"
#include "core/graph/components/ui/UIPanel.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Functions.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"

namespace RDE {

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
    }

    Graph* Canvas::getGraph() {
        return &graph;
    }





    void Canvas::onEvent(Engine* _engine, Event& _event) {
        EventDispatcher _eventDispatcher(_event);
        OnEventData _data { _engine, &_eventDispatcher, &_event };
        for(auto _it = uiInteractables.rbegin(); _it != uiInteractables.rend(); _it++) {
            _it->interactable->onEvent(_it->node, _data.engine, *_data.eventDispatcher, *_data.event);
        }
    }



    void Canvas::getUpdatable(Node* _node, CanvasElement* _canvasElement) {
        if(_node->hasComponent<UISlider>()) {
            auto* _uiSlider = _node->getComponent<UISlider>();
            if(_uiSlider->isEnabled() && _uiSlider->node->isActive()) {
                _canvasElement->updatableData.updatable = (void*)_node->getComponent<UISlider>();
                _canvasElement->updatableData.updatableType = UpdatableType::UT_UI_SLIDER;
                return;
            }
        }

        if(_node->hasComponent<UIInput>()) {
            auto* _uiInput = _node->getComponent<UIInput>();
            if(_uiInput->isEnabled() && _uiInput->node->isActive()) {
                _canvasElement->updatableData.updatable = (void*)_node->getComponent<UIInput>();
                _canvasElement->updatableData.updatableType = UpdatableType::UT_UI_INPUT;
                return;                
            }
        }
    }

    void Canvas::onUpdate(Delta _dt) {
        for(auto& _it : uiUpdatables) {
            switch (_it.updatableData.updatableType) {
                case UT_NONE:
                    break;
                case UT_UI_INPUT: {
                    ((UIInput*)_it.updatableData.updatable)->onUpdate(_dt);
                    break;
                }
                case UT_UI_SLIDER: {
                    ((UISlider*)_it.updatableData.updatable)->onUpdate(_dt);
                    break;
                }
            }
        }
    }




    void Canvas::onLateUpdate() {
        if(graph.isRenderizableTreeDirty) {
            graph.isRenderizableTreeDirty = false;
            uiInteractables.clear();
            uiUpdatables.clear();
            uiRenderizables.clear();
            recalculateRenderizableTree(graph.sceneRoot);
        }
    }




    void Canvas::getRenderizable(Node* _node, CanvasElement* _canvasElement) {
        if(_node->hasComponent<UIImage>()) {
            auto* _uiImage = _node->getComponent<UIImage>();
            if(_uiImage->isEnabled() && _uiImage->node->isActive()) {
                _uiImage->data.RenderizableInnerData.extraInfo = (void*)_uiImage;
                _canvasElement->renderizableInnerData = &_uiImage->data;
                return;
            }
        }

        if(_node->hasComponent<UIText>()) {
            auto* _uiText = _node->getComponent<UIText>();
            if(_uiText->isEnabled() && _uiText->node->isActive()) {
                _uiText->data.RenderizableInnerData.extraInfo = (void*)_uiText;
                _canvasElement->renderizableInnerData = &_uiText->data;
                return;
            }
        }
    }

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _data) {
        Batch* _currentBatch = &batches.back();

        // This checks if there is a beggining on the Cropping System, for masks.
        if(_canvasElement->cropping > 0) {
            if(_canvasElement->node->hasComponent<UIImage>() && _canvasElement->renderizableInnerData != nullptr) {
                forceRender();
                auto* _mask = _canvasElement->node->getComponent<UIMask>();

                _currentBatch = &batches.back();

                glEnable(GL_STENCIL_TEST);

                glColorMask(GL_FALSE , GL_FALSE , GL_FALSE , GL_FALSE);
                glStencilFunc(GL_ALWAYS , _mask->inverted ? 1 : 2, _mask->inverted ? 0xFF : ~0);
                glStencilOp(GL_REPLACE , GL_REPLACE , GL_REPLACE);

                auto& _data = _canvasElement->node->getComponent<UIImage>()->data;

                if (_currentBatch->shader == nullptr || _data.RenderizableInnerData.texture->getGLTexture() != _currentBatch->textureID || _currentBatch->shader->getShaderID() != _data.RenderizableInnerData.shader || _currentBatch->vertexBuffer.size() >= maxIndicesPerDrawCall) {
                    Batch _newBatch;
                    _newBatch.shader = scene->engine->manager.shaderManager.getShader(_data.RenderizableInnerData.shader);
                    _newBatch.textureID = _data.RenderizableInnerData.texture->getGLTexture();
                    batches.emplace_back(_newBatch);
                    _currentBatch = &batches.back();
                }

                switch (_data.RenderizableInnerData.renderizableType) {
                    case RT_NONE: {
                        Util::Log::debug("In Canvas, an element is trying to be rendered with a value of renerizableType == NONE");
                        break;
                    }
                    case RT_UI_IMAGE: {
                        drawBatchedUIImage(_data, _currentBatch, _canvasElement->node->getTransform(), camera->getViewport());
                        break;
                    }
                    case RT_UI_TEXT: {
                        drawBatchedUIText(_data, _currentBatch, _canvasElement->node->getTransform(), camera->getViewport());
                        break;
                    }
                    case RT_SPRITE:
                    case RT_TEXT:
                        break;
                }

                forceRender();

                glColorMask( GL_TRUE , GL_TRUE , GL_TRUE , GL_TRUE);
                glStencilFunc( GL_EQUAL , _mask->inverted ? 0 : 2 , _mask->inverted ? 0xFF : ~0);
                glStencilOp( GL_KEEP , GL_KEEP , GL_KEEP);
            }
        }

        if(_canvasElement->renderizableInnerData != nullptr) {
            auto* _data = _canvasElement->renderizableInnerData;

            if (_currentBatch->shader == nullptr || _data->RenderizableInnerData.texture->getGLTexture() != _currentBatch->textureID || _currentBatch->shader->getShaderID() != _data->RenderizableInnerData.shader || _currentBatch->vertexBuffer.size() >= maxIndicesPerDrawCall) {
                Batch _newBatch;
                _newBatch.shader = scene->engine->manager.shaderManager.getShader(_data->RenderizableInnerData.shader);
                _newBatch.textureID = _data->RenderizableInnerData.texture->getGLTexture();
                batches.emplace_back(_newBatch);
                _currentBatch = &batches.back();
            }

            switch (_data->RenderizableInnerData.renderizableType) {
                case RT_NONE: {
                    Util::Log::debug("In Canvas, an element is trying to be rendered with a value of renerizableType == NONE");
                    break;
                }
                case RT_UI_IMAGE: {
                    drawBatchedUIImage(*_data, _currentBatch, _canvasElement->node->getTransform(), camera->getViewport());
                    break;
                }
                case RT_UI_TEXT: {
                    drawBatchedUIText(*_data, _currentBatch, _canvasElement->node->getTransform(), camera->getViewport());
                    break;
                }
                case RT_SPRITE:
                case RT_TEXT:
                    break;
            }
        }
    }

    void Canvas::batchTreeElementPost(CanvasElement* _canvasElement, void* _data) {
        if(_canvasElement->cropping == 0) {
            forceRender();
            glDisable(GL_STENCIL_TEST);
        }
    }

    void Canvas::forceRender() {
        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(camera, nullptr);
        _renderManager.drawUI(batches);

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

    void Canvas::onRender() {
        auto& _renderManager = graph.scene->engine->manager.renderManager;
        _renderManager.beginDraw(camera, (Transform*)camera->node->getComponent<UITransform>());

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

        for(auto& _it : uiRenderizables) {
            batchTreeElementPre(&_it, nullptr);

            for(auto& _crop : _cropList) {
                _crop--;
                if(_crop == 0) batchTreeElementPost(&_it, nullptr);
            }

            if(_it.cropping > 0) _cropList.push_back(_it.cropping);
        }

        _renderManager.drawUI(batches);

        postRenderSync();
    }

    void Canvas::onDebugRender() {
        auto& _registry = graph.getNodeContainer();

        auto& _renderManager = graph.scene->engine->manager.renderManager;
       _renderManager.beginDebugDraw(camera, (Transform*)camera->node->getComponent<UITransform>());

        _registry.view<UIImage, UITransform, Active>().each([this, &_renderManager](const auto _entity, UIImage& _uiImage, UITransform& _transform, const Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _uiImage.getSize();
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
        });

        _registry.view<UICheckbox, UITransform, Active>().each([this, &_renderManager](const NodeID _nodeID, UICheckbox& _checkbox, UITransform& _transform, Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _checkbox.getSize();

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);
        });

        _registry.view<UIText, UITransform, Active>().each([this, &_renderManager](const NodeID _nodeID, UIText& _uiText, UITransform& _transform, Active& _) {
            if(!_uiText.isEnabled()) return;

            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _uiText.getSize();

            auto _viewport = camera->getViewport();
            auto _virtualRes = _viewport->getVirtualResolution();
            auto _physicalRes = _viewport->getDeviceResolution();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager.drawShape(_shape);

            Vec2F _pointSize = {4, 4};
            _renderManager.drawSquare(_pos, _pointSize, Color::Yellow);
            _renderManager.setPointSize(4);
        });

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

    void Canvas::recalculateRenderizableTree(Node* _node) {
        if(!_node->isActive()) {
            return;
        }

        CanvasElement _canvasElement { _node };

        if(graph.getNodeContainer().any_of<UIInteractable>(_node->getID())) {
            _canvasElement.interactable = _node->getComponent<UIInteractable>();
            uiInteractables.push_back(_canvasElement);
        }

        if(graph.getNodeContainer().any_of<UIText, UIButton, UIImage, UISlider, UIPanel>(_node->getID())) {
            getRenderizable(_node, &_canvasElement);

            if(graph.getNodeContainer().any_of<UIMask>(_node->getID()) && _node->getComponent<UIMask>()->isEnabled()) {
                _canvasElement.cropping = _node->getTransform()->getEnabledChildrenCount();
            }

            if(_canvasElement.renderizableInnerData != nullptr) {
                uiRenderizables.push_back(_canvasElement);
            }
        }

        if(graph.getNodeContainer().any_of<UISlider, UIInput>(_node->getID())) {
            getUpdatable(_node, &_canvasElement);

            if(_canvasElement.updatableData.updatable != nullptr) {
                uiUpdatables.push_back(_canvasElement);
            }
        }

         for(auto* _child : _node->getTransform()->children) {
             recalculateRenderizableTree(_child->node);
         }
    }

    void Canvas::postRenderSync() {
        for(auto* _dirtyTransform : graph.dirtyTransforms) {
            ((UITransform*)_dirtyTransform)->clearDirty();
        }
        graph.dirtyTransforms.clear();
    }

}