//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/graph/components/Transform.h"
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

    Canvas::Canvas(Scene* _scene, const Window* _window, const std::string& _canvasTag) {
        graph = new Graph(_scene, _canvasTag, true);

        scene = _scene;
        auto* _cameraNode = graph->createNode("CanvasCamera");
        camera = _cameraNode->addComponent<Camera>(scene->engine->getWindow());
        camera->setCameraSize(scene->engine->getWindow()->getWindowSize());
    }

    Canvas::~Canvas() {
        delete graph;
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
        auto& _renderManager = graph->scene->engine->manager.renderManager;
        _renderManager.beginDraw(camera, nullptr);
        _renderManager.drawUI(batches);

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

    Camera* Canvas::getCamera() {
        return camera;
    }

    void Canvas::onResize(uint _width, uint _height) {
        ((UITransform*)graph->sceneRoot->getTransform())->setSize({ (float)_width, (float)_height });
        camera->setCameraSize((int)_width, (int)_height);
    }

    void Canvas::setCanvasResolution(const Vec2I& _resolution) {
        camera->getViewport()->virtualResolution = _resolution;
    }

    Vec2I Canvas::getCanvasResolution() {
        return camera->getViewport()->virtualResolution;
    }
}