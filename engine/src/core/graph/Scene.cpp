// Created by borja on 30/3/22.

#include "core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/Engine.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/graph/components/Transform.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/util/Functions.h"
#include "entt/entt.hpp"
#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIMask.h"
#include "core/graph/components/ui/UIPanel.h"

namespace RDE {

    struct OnEventData {
        Engine* engine;
        EventDispatcher* eventDispatcher;
        Event* event;
    };

    Scene::Scene(Engine* _engine, const std::string& _debugName) : debugName(_debugName), engine(_engine) {
        graph = new Graph(this, _debugName);
        auto* _mainCameraNode = graph->createNode("MainCamera");
        auto* _camera = _mainCameraNode->addComponent<Camera>(_engine->getWindow());
        cameras.push_back(_camera);
        mainCamera = _camera;

        canvas = new Canvas(this, _engine->getWindow(), "Canvas");
        // canvas->onResize(_engine->getWindow()->getWidth(), _engine->getWindow()->getHeight());
    }

    Scene::~Scene() {
        delete graph;
        delete canvas;
        prefabs.clear();
    }



	void Scene::onInnerEvent(Event& _event) {
		onInnerEventUI(_event);
		onInnerEventHierarchy(_event);
		onEvent(_event);
	}


    void Scene::onInnerEventHierarchy(Event& _event) {
        
    }

    void Scene::onInnerEventUI(Event& _event) {
        // UI On Event
        EventDispatcher _eventDispatcher(_event);
        OnEventData _data { engine, &_eventDispatcher, &_event };
        for(auto _it = canvas->uiInteractables.rbegin(); _it != canvas->uiInteractables.rend(); _it++) {
            _it->interactable->onEvent(_it->node, _data.engine, *_data.eventDispatcher, *_data.event);
        }
    }




	void Scene::onInnerUpdate(Delta _dt) {
		onInnerUpdateHierarchy(_dt);
		onInnerUpdateUI(_dt);
		onUpdate(_dt);
	}

    void Scene::onInnerUpdateHierarchy(Delta _dt) {
        auto _animations = graph->registry.view<AnimationSystem, SpriteRenderer, Active>(entt::exclude<DisabledForRender>);
        auto _particleSystems = graph->registry.view<ParticleSystem, Active>(entt::exclude<DisabledForRender>);

        _animations.each([&_dt](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _) {
            _animationSystem.update(_dt, &_spriteRenderer);
        });

        _particleSystems.each([&_dt](const auto _entity, ParticleSystem& _particleSystem, const Active& _) {
            _particleSystem.update(_dt);
        });
    }

    void Scene::onInnerUpdateUI(Delta _dt) {
        for (auto& _it : canvas->uiUpdatables) {
            switch (_it.updatableData.updatableType) {
				case RDE_UI_UPDATABLE_NODE_NONE:
                break;
				case RDE_UI_UPDATABLE_NODE_INPUT: {
					((UIInput*)_it.updatableData.updatable)->onUpdate(_dt);
					break;
				}
				case RDE_UI_UPDATABLE_NODE_SLIDER: {
					((UISlider*)_it.updatableData.updatable)->onUpdate(_dt);
					break;
            	}
            }
        }
    }





	void Scene::onInnerFixedUpdate(Delta _fixedDt) {
		onInnerFixedUpdateHierarchy(_fixedDt);
		onInnerFixedUpdateUI(_fixedDt);
		onFixedUpdate(_fixedDt);
	}

    void Scene::onInnerFixedUpdateHierarchy(Delta _dt) {
        for(auto* _body : engine->manager.physics.bodies) {
            if(!_body->isEnabled()) continue;
            _body->update();
        }
    }

    void Scene::onInnerFixedUpdateUI(Delta _dt) {
        
    }



    void Scene::recalculateRenderizableTree(Node* _node, bool _force) {
        auto _id = _node->getID();
		auto _draw = !_node->hasComponent<DisabledForRender>() && _node->hasComponent<Active>();

		if(_node->hasComponent<SpriteRenderer>()) {
			auto* _data = &_node->getComponent<SpriteRenderer>()->data;
			_data->draw = _draw;
			_data->dirty |= _force;
			graph->renderingTreeData.sprites.emplace_back(_data, _node->getTransform(), nullptr );
		}

		if(_node->hasComponent<DynamicSpriteRenderer>()) {
			auto* _data = &_node->getComponent<DynamicSpriteRenderer>()->data;
			_data->draw = _draw;
			_data->dirty |= _force;
			graph->renderingTreeData.dynamicSprites.emplace_back(_data, _node->getTransform(), nullptr );
		}

		if(_node->hasComponent<ParticleSystem>()) {
			auto* _data = &_node->getComponent<ParticleSystem>()->data;
			_data->draw = _draw;
			_data->dirty |= _force;
			graph->renderingTreeData.particleSystmes.emplace_back(_data, _node->getTransform(), nullptr );
		}

		if(_node->hasComponent<TextRenderer>()) {
			auto* _textRenderer = _node->getComponent<TextRenderer>();
			auto* _data = &_textRenderer->data;
			_data->draw = _draw;
			_data->dirty |= _force;
			graph->renderingTreeData.texts.emplace_back(_data, _node->getTransform(), (void*)_textRenderer );
		}

        for(auto* _child : _node->getTransform()->children) {
            recalculateRenderizableTree(_child->node, _force);
        }
    }

	void Scene::recalculateRenderizableTreeUI(Node* _node, bool _force) {		
        CanvasElement _canvasElement { _node, _node->getComponent<UIAnchoring>() };

        if(canvas->graph->getNodeContainer().any_of<UIInteractable>(_node->getID())) {
            _canvasElement.interactable = _node->getComponent<UIInteractable>();
            canvas->uiInteractables.push_back(_canvasElement);
        }

        if(canvas->graph->getNodeContainer().any_of<UIText, UIButton, UIImage, UISlider, UIPanel>(_node->getID())) {
            canvas->getRenderizable(_node, &_canvasElement);

            if(canvas->graph->getNodeContainer().any_of<UIMask>(_node->getID()) && _node->getComponent<UIMask>()->isEnabled()) {
                _canvasElement.cropping = _node->getTransform()->getEnabledAndActiveChildrenCount() + 1;
            }

            if(_canvasElement.renderizableInnerData != nullptr) {
				_canvasElement.renderizableInnerData->RenderizableInnerData.dirty |= _force;
                canvas->uiRenderizables.push_back(_canvasElement);
            }
        }

        if(canvas->graph->getNodeContainer().any_of<UISlider, UIInput>(_node->getID())) {
            canvas->getUpdatable(_node, &_canvasElement);

            if(_canvasElement.updatableData.updatable != nullptr) {
                canvas->uiUpdatables.push_back(_canvasElement);
            }
        }

        for(auto* _child : _node->getTransform()->children) {
            recalculateRenderizableTreeUI(_child->node);
        }
    }





	void Scene::onInnerLateUpdate(Delta _dt) {
		onInnerLateUpdateHierarchy(_dt);
		onInnerLateUpdateUI(_dt);
		onLateUpdate(_dt);
	}

	void Scene::updateRenderTree(bool _force) {
		// [0] -> SpriteRenderer
		// [1] -> DynamicSpriteRenderer
		// [2] -> ParticleSystem
		// [3] -> TextRenderer
		const int RENDERIZABLES_COUNT = 4;

		graph->renderingTreeData.sprites.clear();
		graph->renderingTreeData.dynamicSprites.clear();
		graph->renderingTreeData.particleSystmes.clear();
		graph->renderingTreeData.texts.clear();

		recalculateRenderizableTree(graph->getRoot(), _force);
	}

    void Scene::onInnerLateUpdateHierarchy(Delta _dt) {
        if(graph->renderingTreeData.isRenderizableTreeDirty) {
			updateRenderTree();
            graph->renderingTreeData.isRenderizableTreeDirty = false;
        }

        graph->registry.view<DynamicSpriteRenderer, Active>().each([](const NodeID _nodeID, DynamicSpriteRenderer& _dynamicSpriteRenderer, Active& _) {
            if(_dynamicSpriteRenderer.isEnabled()) {
                ((CPUTexture*)_dynamicSpriteRenderer.data.texture)->uploadToGPU();
            }
        });
    }

    void Scene::onInnerLateUpdateUI(Delta _dt) {
        if(canvas->graph->renderingTreeData.isRenderizableTreeDirty) {
            canvas->graph->renderingTreeData.isRenderizableTreeDirty = false;
            canvas->uiInteractables.clear();
            canvas->uiUpdatables.clear();
            canvas->uiRenderizables.clear();
            recalculateRenderizableTreeUI(canvas->graph->sceneRoot);
        }
    }





	void Scene::onInnerRender(Delta _dt, FrameBuffer* _framebuffer) {
		onInnerRenderHierarchy(_dt, _framebuffer); 
		onInnerRenderUI(_dt, _framebuffer); 
	}

	void Scene::onInnerRenderHierarchy(Delta _dt, FrameBuffer* _framebuffer) {
        auto& _renderManager = engine->manager.renderManager;

        for(auto* _camera : cameras) {
            if(!_camera->node->hasComponent<Active>() && _camera->isEnabled()) continue;
			if (_camera->framebufferID != _framebuffer->getID()) continue;

            _renderManager.beginDraw(_camera, _camera->node->getComponent<Transform>());
            _camera->update();
            {
                for(auto [_innerData, _transform, _extraData] : graph->renderingTreeData.sprites) {
					auto _renderFbID = _innerData->framebufferToRenderTo;
					auto _fbID = _framebuffer->getID();
					if ((_renderFbID & _fbID) != _fbID) continue;
                    _renderManager.drawSpriteRenderer(*_innerData, _transform);
                }

                for(auto [_innerData, _transform, _extraData] : graph->renderingTreeData.texts) {
					auto _renderFbID = _innerData->framebufferToRenderTo;
					auto _fbID = _framebuffer->getID();
					if ((_renderFbID & _fbID) != _fbID) continue;
                    _innerData->extraInfo = _extraData;
                    _renderManager.drawTextRenderer(*_innerData, _transform);
                }
            }

            _renderManager.endDraw();
        }
    }

	void Scene::onInnerRenderUI(Delta _dt, FrameBuffer* _framebuffer) {
        auto& _renderManager = engine->manager.renderManager;

		if(!mainCamera->node->hasComponent<Active>() && mainCamera->isEnabled()) return;
		if (mainCamera->framebufferID != _framebuffer->getID()) return;

        _renderManager.beginDraw(mainCamera, (Transform*)mainCamera->node->getComponent<Transform>());

        canvas->batches.clear();
        Batch _batch;
        _batch.shader = engine->manager.shaderManager.getShader(SPRITE_RENDERER_SHADER);
        _batch.textureID = engine->manager.textureManager.getSubTexture("defaultAssets", "buttonDark")->getGLTexture();
        canvas->batches.emplace_back(_batch);

        // Depending on the element, we will need to restrict the area that can be rendered. In that case and as we
        // precalculate a list in the correct order of rendering, we need to know when to begin the cropping and when to
        // finish it, in this case we create a list of root elements that need to be cropped. We store how many children
        // the root cropped element has, and we simply make a countdown until all children have been drawn, then we apply
        // the crop.
        std::vector<int> _cropList;

        for(auto& _it : canvas->uiRenderizables) {
			auto _renderFbID = _it.renderizableInnerData->RenderizableInnerData.framebufferToRenderTo;;
			auto _fbID = _framebuffer->getID();
			if ((_renderFbID & _fbID) != _fbID) continue;

            canvas->batchTreeElementPre(&_it, nullptr, _framebuffer);

            for(auto& _crop : _cropList) {
                _crop--;
                if(_crop == 0) canvas->batchTreeElementPost(&_it, nullptr);
            }

            if(_it.cropping > 0) _cropList.push_back(_it.cropping);
        }

		_renderManager.endDrawUI(canvas->batches);
    }



	void Scene::onInnerDebugRender(Delta _dt, FrameBuffer* _framebuffer) {
		auto& _renderManager = engine->manager.renderManager;

		if (mainCamera->framebufferID != _framebuffer->getID()) return;

		_renderManager.beginDebugDraw(mainCamera, mainCamera->node->getTransform());
		onInnerDebugRenderHierarchy(_dt, &_renderManager, _framebuffer);
		onDebugRender(_dt, &_renderManager);
		_renderManager.endDebugDraw();

		_renderManager.beginDebugDraw(mainCamera, mainCamera->node->getTransform());
		onInnerDebugRenderUI(_dt, &_renderManager, _framebuffer);
		_renderManager.endDebugDraw();
	}

	void Scene::onInnerDebugRenderHierarchy(Delta _dt, RenderManager* _renderManager, FrameBuffer* _framebuffer) {
        engine->manager.physics.debugRender(_renderManager);
    }

	void Scene::onInnerDebugRenderUI(Delta _dt, RenderManager* _renderManager, FrameBuffer* _framebuffer) {
		auto& _registry = canvas->graph->getNodeContainer();

        _registry.view<UIImage, Transform, Active>().each([this, _renderManager](const auto _entity, UIImage& _uiImage, Transform& _transform, const Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _uiImage.getSize();
            Vec2F _pointSize = {4, 4};

            auto _viewport = mainCamera->getViewport();
            auto _virtualRes = _viewport->getSize();
			auto _physicalRes = _viewport->getSize();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;
            _pointSize *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager->drawShape(_shape);

            _renderManager->drawSquare(_pos, _pointSize, Color::Blue);
            _renderManager->setPointSize(4);
        });

        _registry.view<UICheckbox, Transform, Active>().each([this, _renderManager](const NodeID _nodeID, UICheckbox& _checkbox, Transform& _transform, Active& _) {
            DebugShape _shape;
            Vec2F _pos = _transform.getModelMatrixPosition();
            Vec2F _size = _checkbox.getSize();

            auto _viewport = mainCamera->getViewport();
            auto _virtualRes = _viewport->getSize();
            auto _physicalRes = _viewport->getSize();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

            _pos       *= _scale.y;
            _size      *= _scale.y;

            _shape.makeSquare(_pos, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager->drawShape(_shape);
        });

        _registry.view<UIText, Transform, UIAnchoring, Active>().each([this, _renderManager](const NodeID _nodeID, UIText& _uiText, Transform& _transform, UIAnchoring& _anchoring, Active& _) {
            if(!_uiText.isEnabled()) return;

            DebugShape _shape;
            Vec2F _posRect = _transform.getModelMatrixPosition();
            Vec2F _posPoint = _transform.getModelMatrixPosition();
			Vec2F _size = _uiText.getSize();

			if((_anchoring.getAnchor() & RDE_UI_ANCHOR_LEFT) == RDE_UI_ANCHOR_LEFT) {
				_posRect.x += _size.x * 0.5f;
			} else if((_anchoring.getAnchor() & RDE_UI_ANCHOR_RIGHT) == RDE_UI_ANCHOR_RIGHT) {
				_posRect.x -= _size.x * 0.5f;
			} else if((_anchoring.getAnchor() & RDE_UI_ANCHOR_TOP) == RDE_UI_ANCHOR_TOP) {
				_posRect.y += _size.y * 0.5f;
			} else if((_anchoring.getAnchor() & RDE_UI_ANCHOR_BOTTOM) == RDE_UI_ANCHOR_BOTTOM) {
				_posRect.y -= _size.y * 0.5f;
			}

            auto _viewport = mainCamera->getViewport();
            auto _virtualRes = _viewport->getSize();
            auto _physicalRes = _viewport->getSize();
            auto _scale = Vec2F { (float)_virtualRes.x / (float)_physicalRes.x, (float)_virtualRes.y / (float)_physicalRes.y };

			_posRect   *= _scale.y;
            _size      *= _scale.y;

			_shape.makeSquare(_posRect, _size);
            _shape.showOutsideColor(true);
            _shape.setOutlineColor(Color::Blue);
            _shape.showInnerColor(false);
            _shape.setRotation(_transform.getModelMatrixRotation());
            _renderManager->drawShape(_shape);

            Vec2F _pointSize = {4, 4};
			_renderManager->drawSquare(_posPoint, _pointSize, Color::Yellow);
            _renderManager->setPointSize(4);
        });
    }



    void Scene::switchMainCamera(Camera* _camera) {
        mainCamera = _camera;
    }

    Camera* Scene::addCamera(Window* _window) {
        auto _newCameraNode = graph->createNode(Util::String::appendToString("Camera", cameras.size()));
        auto* _camera = _newCameraNode->addComponent<Camera>(_window);
        cameras.push_back(_camera);
        return cameras.back();
    }

	void Scene::addCamera(Camera* _camera) {
		cameras.push_back(_camera);
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
