//
// Created by borja on 28/07/22.
//

#include "Editor.h"
#include "core/graph/Scene.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIPanel.h"
#include "core/graph/components/ui/UIText.h"
#include "core/render/elements/Texture.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/util/Functions.h"
#include "core/util/Mat2.h"
#include "core/util/Vec.h"

#if !IS_MOBILE()
#include "imgui.h"
#endif



#define GRID_TEXTURE_SIZE 2500

// TODO (Borja): Create an ImGui graph/tree/thing showing which Framebuffer is being rendered and which cameras are rendering to it and which IRenderizables are being rendered by those cameras.

namespace RDEEditor {

	#include "EditorThemesModule.cpp"
	#include "NodeCreator.cpp"
	#include "EditorDockingspaceModule.cpp"
	#include "EditorSceneViewModule.cpp"
	#include "EditorHierarchyModule.cpp"
	#include "EditorComponentsModule.cpp"
	#include "EditorConsoleModule.cpp"
	#include "EditorGamePreview.cpp"
	#include "EditorOverlappingSelectionModule.cpp"

    void Editor::onInit() {
		
		engine->manager.renderManager.setClearColor(backgroundColor);
		gridShaderID = engine->manager.shaderManager.loadShader("gridShader", "editor/assets/shaders/gridVertex.glsl", "editor/assets/shaders/gridFragment.glsl");
		GLsizei _structSize = sizeof(OpenGLVertex);
		Util::GL::checkError("Error BEFORE ConfigBasicShader");
		engine->manager.shaderManager.loadShaderVertexConfig("gridShader", {
			VertexConfig {
			0, 2, GL_FLOAT, 0, _structSize
			},
			VertexConfig {
				1, 4, GL_UNSIGNED_BYTE, sizeof(float) * 2, _structSize
			},
			VertexConfig {
				2, 2, GL_FLOAT, sizeof(float) * 2 + sizeof(unsigned char) * 4, _structSize
			}
		},
		{
			"viewProjectionMatrix"
		},
		10);

		auto _size = 1920 * 1080 * 4;
		unsigned char* _data = new unsigned char[_size];
		for(auto _i = 0; _i < _size;) {
			_data[_i + 0] = 255;
			_data[_i + 1] = 255;
			_data[_i + 2] = 255;
			_data[_i + 3] = 255;
			_i += 4;
		}

		auto* _editorCameraNode = new Node((NodeID)0, graph, &engine->manager, new Transform(nullptr));
		editorCamera = new Camera(_editorCameraNode, &engine->manager, nullptr, engine->getWindow());
		generateGridTexture();
		gridSprite = graph->createNode("Grid")->addComponent<SpriteRenderer>(SpriteRendererConfig {
			.texture = &gridTexture
		});
		gridSprite->setShaderID(gridShaderID);

		duckNode = graph->createNode("Duck");
		nodes.push_back(duckNode->getID());
		duckNode->getTransform()->setPosition(0, 0);
		auto _texture = engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
		auto* _sprite = duckNode->addComponent<SpriteRenderer>(SpriteRendererConfig {
			.texture = _texture
		});

		mseDel.bind<&Editor::mouseScrolled>(this);
		wreDel.bind<&Editor::windowResized>(this);

		delete [] _data;

		addCamera(editorCamera);

		FrameBufferSpecification _specs = {(uint32_t)engine->getWindow()->getWindowSize().x,(uint32_t)engine->getWindow()->getWindowSize().y};
		_specs.drawDebug = false;
		auto _fbID = engine->manager.renderManager.createFrameBuffer(_specs);
		getCameras()[0]->framebufferID = _fbID;
		getCameras()[0]->getViewport()->autoResizeWhenWindowSizeChanges = false;

		editorData.sceneViewFramebufferID = editorCamera->framebufferID;
		editorData.gameViewFramebufferID = getCameras()[0]->framebufferID;

		_sprite->setFramebuffer(editorData.gameViewFramebufferID | editorData.sceneViewFramebufferID);
		
		generateTranslationGuizmo();

		setTheme(editorFlags.theme);
		engine->getWindow()->maximizeWindow();

		editorData.onHierarchyElementClicked.bind<onHierarchyElementClicked>();
    }

	void Editor::generateGridTexture() {
		auto _windowSize = engine->getWindow()->getWindowSize();
		auto _size = _windowSize.x * _windowSize.y * 4;
		unsigned char* _data = new unsigned char[_size];
		for(auto _i = 0; _i < _size;) {
			_data[_i + 0] = 255;
			_data[_i + 1] = 255;
			_data[_i + 2] = 255;
			_data[_i + 3] = 255;
			_i += 4;
		}

		gridTexture.loadTextureFromMemory(_windowSize.x, _windowSize.y, _data);

		delete [] _data;
	}

    void Editor::onUpdate(Delta _dt) {
        Scene::onUpdate(_dt);

        static float _timer = 0.f;
        static int _fpsCounter = 0;
        static int _frameCounter = 0;
        if (_timer >= 1.f) {
            _fpsCounter = _frameCounter;
            engine->getWindow()->setTitle("Engine: " + std::to_string(_fpsCounter));
            _frameCounter = 0;
            _timer = 0;
        }
        ++_frameCounter;
        _timer += _dt;

		mouseHandler();

		if(engine->manager.inputManager.isKeyJustPressed(RDE_KEYBOARD_KEY_O)) {
			centerCamera();
		}
		
		selectNodeWithClick();
		editModeInputHandler();
    }

    bool isPointInside(const Vec2F& _rectangleSize, const Transform* _rectangleTransform, const Vec2F& _pointPos) {
		auto _rectPos =_rectangleTransform->getWorldPosition();
		return  _pointPos.x >= _rectPos.x - _rectangleSize.x * 0.5f && _pointPos.x <= _rectPos.x + _rectangleSize.x * 0.5f &&
				_pointPos.y >= _rectPos.y - _rectangleSize.y * 0.5f && _pointPos.y <= _rectPos.y + _rectangleSize.y * 0.5f;
	}

	// TODO (Borja): When stacking nodes are selected, create a small list to select wich one we really want to select.
	// TODO (Borja): Fix, separate selectedNonUI from selected UI as when Node is selected in view, components are mixed between nonUI and UI, a chaos.
    void Editor::selectNodeWithClick() {
		if(!editorFlags.isSceneViewActive && !editorFlags.isSceneViewHovered) return;

		if(engine->manager.inputManager.isMouseJustReleased(RDE::RDE_MOUSE_BUTTON_0) && editorFlags.editModeAxis == EditModeAxis::None) {
			auto& _nodes = graph->getNodeContainer();
			auto& _canvasNodes = canvas->graph->getNodeContainer();
			editorData.overlappedNodesSceneSelection.clear();
			
			_nodes.view<SpriteRenderer, Transform>().each([this](const auto _entity, SpriteRenderer& _sprite, Transform& _transform) {
				if(isPointInside(_sprite.getSize(), &_transform, editorData.mousePositionOnSceneView) && &_sprite != gridSprite) {
					if(std::find(editorData.overlappedNodesSceneSelection[0].begin(), editorData.overlappedNodesSceneSelection[0].end(), &_transform) == editorData.overlappedNodesSceneSelection[0].end()) {
						editorData.overlappedNodesSceneSelection[0].push_back(&_transform);
					}
				}
			});

			_canvasNodes.view<UIPanel, Transform>().each([this](const auto _entity, UIPanel& _uiPanel, Transform& _transform) {
				if(isPointInside(_uiPanel.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});

			_canvasNodes.view<UISlider, Transform>().each([this](const auto _entity, UISlider& _uiSlider, Transform& _transform) {
				if(isPointInside(_uiSlider.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});


			_canvasNodes.view<UIButton, Transform>().each([this](const auto _entity, UIButton& _uiButton, Transform& _transform) {
				if(isPointInside(_uiButton.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});
			
			_canvasNodes.view<UIText, Transform>().each([this](const auto _entity, UIText& _uiText, Transform& _transform) {
				if(isPointInside(_uiText.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});

			_canvasNodes.view<UIInput, Transform>().each([this](const auto _entity, UIInput& _uiInput, Transform& _transform) {
				if(isPointInside(_uiInput.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});

			_canvasNodes.view<UICheckbox, Transform>().each([this](const auto _entity, UICheckbox& _uiCheckbox, Transform& _transform) {
				if(isPointInside(_uiCheckbox.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
					if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
						editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});
			
			_canvasNodes.view<UIImage, Transform>().each([this](const auto _entity, UIImage& _uiImage, Transform& _transform) {
				if(isPointInside(_uiImage.getSize(), &_transform, editorData.mousePositionOnSceneView)) {
				    if(std::find(editorData.overlappedNodesSceneSelection[1].begin(), editorData.overlappedNodesSceneSelection[1].end(), &_transform) == editorData.overlappedNodesSceneSelection[1].end()) {
				        editorData.overlappedNodesSceneSelection[1].push_back(&_transform);
					}
				}
			});
			
			if(editorData.overlappedNodesSceneSelection.empty()) {
				editModeInputHandler();
				if(editorFlags.editModeAxis == EditModeAxis::None) {
					if(editorFlags.isSceneViewHovered) {
						editorData.selectedNode.node = nullptr;
					}
				}
			} else {
				if(editorFlags.editModeAxis == EditModeAxis::None && editorData.overlappedNodesSceneSelection.size() == 1) {
					auto _graph = editorData.overlappedNodesSceneSelection.begin();
					if(_graph->second.size() == 1) {
						editorData.selectedNode.node = _graph->second[0]->node;
						editorData.selectedNode.graph = _graph->second[0]->graph;
					}
				}
			}
		}
	}

	void Editor::onEvent(Event& _event) {
		EventDispatcher dispatcher(_event);
		dispatcher.dispatchEvent<MouseScrolledEvent>(mseDel);
		dispatcher.dispatchEvent<WindowResizedEvent>(wreDel);
	}

    void Editor::onLateUpdate(Delta _dt) {
        Scene::onLateUpdate(_dt);

		engine->manager.shaderManager.setFloat4(gridShaderID, "gridColor", glm::vec4{ gridColor.r / 255.f, gridColor.g / 255.f, gridColor.b / 255.f, gridColor.a / 255.f });
		engine->manager.shaderManager.setFloat(gridShaderID, "zoom", editorCamera->getCurrentZoomLevel());
		engine->manager.shaderManager.setFloat2(gridShaderID, "resolution", glm::vec2 { (float)engine->getWindow()->getWidth(), (float)engine->getWindow()->getHeight() });
		
		auto _cameraPos = editorCamera->node->getTransform()->getPosition();
		engine->manager.shaderManager.setFloat2(gridShaderID, "cameraDisplacement", glm::vec2 { _cameraPos.x, _cameraPos.y });

		auto _zoom = editorCamera->getCurrentZoomLevel();
		gridSprite->node->getTransform()->setScale(_zoom + std::abs(_cameraPos.x), _zoom + std::abs(_cameraPos.x));    

		for(auto* _camera : getCameras()) {
			_camera->recalculateViewProjectionMatrix();
		}
	}

	void Editor::onImGuiRender(Delta _dt) {
		dockingSpaceView(this);
		gameView(this);
		sceneView(this, &sceneViewOffset);
		hierarchyView(this);
		componentsView(this);
		consoleView(this);
		overlappingSelectionWindow(this);

		resetID(this);
	}

    void Editor::onDebugRender(Delta _dt, RenderManager* _renderManager) {
		Scene::onDebugRender(_dt, _renderManager);

		for(auto* _camera : getCameras()) {
			if(_camera == editorCamera) continue;

			auto _cameraSize = _camera->getViewport()->getSize();
			auto _cameraPos = _camera->node->getTransform()->getPosition();
			auto _cameraRot = _camera->node->getTransform()->getRotation();

			DebugShape _cameraShape;
			_cameraShape.makeSquare(-_cameraPos, { (float)_cameraSize.x, (float)_cameraSize.y });
			_cameraShape.showOutsideColor(true);
			_cameraShape.setOutlineColor(Color::White);
			_cameraShape.showInnerColor(false);
			_cameraShape.setRotation(_cameraRot);
			engine->manager.renderManager.drawShape(_cameraShape);
		}

		if(editorData.selectedNode.node != nullptr && editorFlags.editMode == EditMode::Move) {
			auto _isRootNode = editorData.selectedNode.node == graph->getRoot() || editorData.selectedNode.node == canvas->graph->getRoot();
			if(!_isRootNode) {
				engine->manager.renderManager.drawShape(editorData.gizmos.translationX);
				engine->manager.renderManager.drawShape(editorData.gizmos.translationY);
				engine->manager.renderManager.drawShape(editorData.gizmos.translationXY);
			}
		}
    }

    void Editor::generateTranslationGuizmo() {
		DebugShape _arrowUp;
		_arrowUp.setPosition({0, 0});
		_arrowUp.setRotation(0);
		_arrowUp.addPoint({ 4, 0});
		_arrowUp.addPoint({ 4, 48});
		_arrowUp.addPoint({ 16, 48});
		_arrowUp.addPoint({ 0, 80});
		_arrowUp.addPoint({-16, 48});
		_arrowUp.addPoint({-4, 48});
		_arrowUp.addPoint({-4, 0});
		
		_arrowUp.addPoint({-4, -48});
		_arrowUp.addPoint({-16, -48});
		_arrowUp.addPoint({ 0, -80});
		_arrowUp.addPoint({ 16, -48});
		_arrowUp.addPoint({ 4, -48});
		
		auto _color = Color::Blue;
		_color.a = 110;
		_arrowUp.setOutlineColor(Color::Black);
		_arrowUp.setInnerColor(_color);
		editorData.gizmos.translationY = _arrowUp;

		DebugShape _arrowRight;
		_arrowRight.setPosition({0, 0});
		_arrowRight.setRotation(0);
		_arrowRight.addPoint({0,   4});
		_arrowRight.addPoint({48, 4});
		_arrowRight.addPoint({48, 16});
		_arrowRight.addPoint({80, 0});
		_arrowRight.addPoint({48, -16});
		_arrowRight.addPoint({48, -4});
		_arrowRight.addPoint({0,   -4});
		
		_arrowRight.addPoint({-48, -4});
		_arrowRight.addPoint({-48, -16});
		_arrowRight.addPoint({-80, -0});
		_arrowRight.addPoint({-48,  16});
		_arrowRight.addPoint({-48,  4});
		
		_color = Color::Green;
		_color.a = 110;
		_arrowRight.setInnerColor(_color);
		_arrowRight.setOutlineColor(Color::Black);
		_arrowRight.setRotation(0);
		editorData.gizmos.translationX = _arrowRight;

		DebugShape _intersection;
		_intersection.makeSquare({0, 0}, {16, 16});
		_intersection.setOutlineColor(Color::Black);
		_intersection.setInnerColor(Color::Gray);
		editorData.gizmos.translationXY = _intersection;
	}

	bool Editor::mouseScrolled(MouseScrolledEvent& _event) {
		if(!editorFlags.isSceneViewHovered) {
			return true;
		}

		editorCamera->setCurrentZoomLevel(editorCamera->getCurrentZoomLevel() + _event.getScrollY() * 0.1f);

		auto _zoom = editorCamera->getCurrentZoomLevel();
		editorData.gizmos.translationX.setScale({_zoom, _zoom});
		editorData.gizmos.translationY.setScale({_zoom, _zoom});
		editorData.gizmos.translationXY.setScale({_zoom, _zoom});
		
		return true;
	}

	bool Editor::windowResized(WindowResizedEvent& _event) {
		for(auto* _camera : getCameras()) {
			_camera->onResize(_event.getWidth(), _event.getHeight());
		}

		generateGridTexture();
		editorCamera->getViewport()->matchVirtualResolutionToDeviceResolution();
		canvas->setCanvasResolution( { (int)_event.getWidth(), (int)_event.getHeight() });
		return true;
	}
	
	void Editor::editModeTranslationInputHandler() {
		if(editorData.selectedNode.node == graph->getRoot() || 
			editorData.selectedNode.node == canvas->graph->getRoot()) return;

		if(engine->manager.inputManager.isMouseJustPressed(RDE::RDE_MOUSE_BUTTON_0)) {
			editorFlags.editModeAxis = EditModeAxis::None;
			editorData.gizmos.lastClickOrMovedMousePositionForTranslation = engine->manager.inputManager.getMousePosScreenCoords();
			
			if(editorData.gizmos.translationXY.isPointInside(editorData.mousePositionOnSceneView)) {
				editorFlags.editModeAxis = EditModeAxis::X | EditModeAxis::Y;
			} else {
				if(editorData.gizmos.translationX.isPointInside(editorData.mousePositionOnSceneView)) {
					editorFlags.editModeAxis = EditModeAxis::X;
				} else if(editorData.gizmos.translationY.isPointInside(editorData.mousePositionOnSceneView)) {
					editorFlags.editModeAxis = EditModeAxis::Y;
				}
			}
		} else if(engine->manager.inputManager.isMouseJustReleased(RDE::RDE_MOUSE_BUTTON_0)) {
			editorFlags.editModeAxis = EditModeAxis::None;
		}
		
		if(engine->manager.inputManager.isMousePressed(RDE_MOUSE_BUTTON_0) && editorFlags.editModeAxis != EditModeAxis::None) {
			auto _current = engine->manager.inputManager.getMousePosScreenCoords();
			auto _diff = _current - editorData.gizmos.lastClickOrMovedMousePositionForTranslation;
			editorData.gizmos.lastClickOrMovedMousePositionForTranslation = _current;
			
			_diff.y = editorFlags.editModeAxis == EditModeAxis::X ? 0.f : _diff.y;
			_diff.x = editorFlags.editModeAxis == EditModeAxis::Y ? 0.f : _diff.x;
			
			auto _rot = editorData.selectedNode.node->getTransform()->getWorldRotation();
			auto _pos = editorData.selectedNode.node->getTransform()->getWorldPosition();
			auto _newPos = _diff * editorCamera->getCurrentZoomLevel();
			Mat2 _rotMatrix(1, _pos.x, 0, _pos.y);
			_rotMatrix.rotate(editorFlags.editModeAxis == EditModeAxis::X || editorFlags.editModeAxis == EditModeAxis::Y ? _rot : 0.f);
			
			editorData.selectedNode.node->getTransform()->translate(_rotMatrix * _newPos);
		}
		
		if(editorData.selectedNode.node != nullptr) {
			auto _pos = editorData.selectedNode.node->getTransform()->getWorldPosition();
			auto _rot = editorData.selectedNode.node->getTransform()->getWorldRotation();
			
			editorData.gizmos.translationX.setPosition(_pos);
			editorData.gizmos.translationX.setRotation(_rot);
			
			editorData.gizmos.translationY.setPosition(_pos);
			editorData.gizmos.translationY.setRotation(_rot);
			
			editorData.gizmos.translationXY.setPosition(_pos);
			editorData.gizmos.translationXY.setRotation(_rot);
		}
	}
	
	void Editor::editModeInputHandler() {
		if(editorData.selectedNode.node == nullptr) {
			return;
		}

		switch (editorFlags.editMode) {
			case Move: {
				editModeTranslationInputHandler();
				break;
			}
			case Rotate:
			case Scale:
				break;
        }
	}

	void Editor::mouseHandler() {
		if(!editorFlags.isSceneViewHovered) {
			return;
		}

		if(engine->manager.inputManager.isMouseJustPressed(RDE_MOUSE_BUTTON_1)) {
			lastClickOrMovedMousePosition = engine->manager.inputManager.getMousePosScreenCoords();
		} else if(engine->manager.inputManager.isMousePressed(RDE_MOUSE_BUTTON_1)) {
			auto _current = engine->manager.inputManager.getMousePosScreenCoords();
			auto _diff = _current - lastClickOrMovedMousePosition;
			lastClickOrMovedMousePosition = _current;
			editorCamera->node->getTransform()->translate(_diff * -editorCamera->getCurrentZoomLevel());
		}
	}

	void Editor::centerCamera() {
		if(!editorFlags.isSceneViewHovered && !editorFlags.isSceneViewActive) {
			return;
		}

		auto _zoom = editorCamera->getCurrentZoomLevel();
		editorCamera->node->getTransform()->setPosition(sceneViewOffset.x * _zoom, -sceneViewOffset.y * _zoom);
	}
}
