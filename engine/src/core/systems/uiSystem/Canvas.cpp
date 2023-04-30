//
// Created by borja on 29/05/22.
//

#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UIAnchoring.h"
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
    }

    Canvas::~Canvas() {
        delete graph;
    }

    void Canvas::getUpdatable(Node* _node, CanvasElement* _canvasElement) {
        if(_node->hasComponent<UISlider>()) {
            auto* _uiSlider = _node->getComponent<UISlider>();
            if(_uiSlider->isEnabled() && _uiSlider->node->isActive()) {
                _canvasElement->updatableData.updatable = (void*)_node->getComponent<UISlider>();
				_canvasElement->updatableData.updatableType = RDE_UI_UPDATABLE_NODE_SLIDER;
                return;
            }
        }

        if(_node->hasComponent<UIInput>()) {
            auto* _uiInput = _node->getComponent<UIInput>();
            if(_uiInput->isEnabled() && _uiInput->node->isActive()) {
                _canvasElement->updatableData.updatable = (void*)_node->getComponent<UIInput>();
				_canvasElement->updatableData.updatableType = RDE_UI_UPDATABLE_NODE_INPUT;
                return;                
            }
        }
    }

	#define SET_FRAMEBUFFER_DATA(_type, _node, _data)								\
	if(_node->hasComponent<_type>()) {												\
	auto _t = _node->getComponent<_type>();											\
		if(_data == nullptr) {														\
			_data = &_t->data;														\
		}																			\
		_data->RenderizableInnerData.framebufferToRenderTo = _t->getFramebuffer();	\
	}

	#define SET_BASIC_UI_DRAWABLE_INFO(_type, _node, _data)					\
	if (_node->hasComponent<UIImage>()) { 									\
		auto* _t = _node->getComponent<_type>();							\
		auto _draw = _t->isEnabled() && _t->node->isActive();				\
		_t->data.RenderizableInnerData.extraInfo = (void*)_t;				\
		_data = &_t->data;													\
		_data->RenderizableInnerData.draw = _draw;							\
		SET_FRAMEBUFFER_DATA(_type, _node, _data)							\
		return;																\
	}
	

    void Canvas::getRenderizable(Node* _node, CanvasElement* _canvasElement) {
		SET_FRAMEBUFFER_DATA(UIPanel, _node, _canvasElement->renderizableInnerData)
		SET_FRAMEBUFFER_DATA(UISlider, _node, _canvasElement->renderizableInnerData)
		SET_FRAMEBUFFER_DATA(UIInput, _node, _canvasElement->renderizableInnerData)
		SET_FRAMEBUFFER_DATA(UIButton, _node, _canvasElement->renderizableInnerData)
		SET_FRAMEBUFFER_DATA(UICheckbox, _node, _canvasElement->renderizableInnerData)

		SET_BASIC_UI_DRAWABLE_INFO(UIImage, _node, _canvasElement->renderizableInnerData)
		SET_BASIC_UI_DRAWABLE_INFO(UIText, _node, _canvasElement->renderizableInnerData)
    }

    void Canvas::batchTreeElementPre(CanvasElement* _canvasElement, void* _extraData, FrameBuffer* _framebuffer) {
        Batch* _currentBatch = &batches.back();
		auto* _camera = scene->mainCamera;

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
                    case RDE_RENDERIZABLE_TYPE_NONE: {
                        Util::Log::debug("In Canvas, an element is trying to be rendered with a value of renerizableType == NONE");
                        break;
                    }
					case RDE_RENDERIZABLE_TYPE_UI_IMAGE: {
						auto _renderFbID = _data.RenderizableInnerData.framebufferToRenderTo;
						auto _fbID = _framebuffer->getID();
						if ((_renderFbID & _fbID) != _fbID) return;
                        drawBatchedUIImage(_data, _currentBatch, _canvasElement->anchoring, _canvasElement->node->getTransform(), _camera->getViewport());
                        break;
                    }
					case RDE_RENDERIZABLE_TYPE_UI_TEXT: {
						auto _renderFbID = _data.RenderizableInnerData.framebufferToRenderTo;
						auto _fbID = _framebuffer->getID();
						if ((_renderFbID & _fbID) != _fbID) return;
						drawBatchedUIText(_data, _currentBatch, _canvasElement->anchoring, _canvasElement->node->getTransform(), _camera->getViewport());
                        break;
                    }
					case RDE_RENDERIZABLE_TYPE_SPRITE:
					case RDE_RENDERIZABLE_TYPE_TEXT:
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
				case RDE_RENDERIZABLE_TYPE_NONE: {
                    Util::Log::debug("In Canvas, an element is trying to be rendered with a value of renerizableType == NONE");
                    break;
                }
				case RDE_RENDERIZABLE_TYPE_UI_IMAGE: {
					auto _renderFbID = _data->RenderizableInnerData.framebufferToRenderTo;
					auto _fbID = _framebuffer->getID();
					if ((_renderFbID & _fbID) != _fbID) {
						return;
					}
					drawBatchedUIImage(*_data, _currentBatch, _canvasElement->anchoring, _canvasElement->node->getTransform(), _camera->getViewport());
                    break;
                }
				case RDE_RENDERIZABLE_TYPE_UI_TEXT: {
					auto _renderFbID = _data->RenderizableInnerData.framebufferToRenderTo;
					auto _fbID = _framebuffer->getID();
					if ((_renderFbID & _fbID) != _fbID) {
						return;
					}
                    drawBatchedUIText(*_data, _currentBatch, _canvasElement->anchoring, _canvasElement->node->getTransform(), _camera->getViewport());
                    break;
                }
				case RDE_RENDERIZABLE_TYPE_SPRITE:
				case RDE_RENDERIZABLE_TYPE_TEXT:
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
		_renderManager.beginDraw(scene->mainCamera, nullptr);
        _renderManager.endDrawUI(batches);

        batches.clear();
        Batch _batch;
        _batch.shader = scene->engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = scene->engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        batches.emplace_back(_batch);
    }

    Camera* Canvas::getCamera() {
        return scene->mainCamera;
    }

    void Canvas::onResize(uint _width, uint _height) {
        graph->sceneRoot->getComponent<UIAnchoring>()->setSize({ (float)_width, (float)_height });
    }

    void Canvas::setCanvasResolution(const Vec2I& _resolution) {
		//auto* _camera = scene->mainCamera;
  //      _camera->getViewport()->virtualResolution = _resolution;
		//graph->renderingTreeData.isRenderizableTreeDirty = true;
		//graph->sceneRoot->getComponent<UIAnchoring>()->setSize({ (float)_resolution.x, (float)_resolution.y });
    }

    Vec2I Canvas::getCanvasResolution() {
        return scene->mainCamera->getViewport()->getSize();
    }
}
