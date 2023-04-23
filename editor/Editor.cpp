//
// Created by borja on 28/07/22.
//

#include "Editor.h"
#include "core/graph/Scene.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/render/elements/Texture.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/util/Functions.h"

#if !IS_MOBILE()
#include "imgui.h"
#endif

#include "EditorDockingspaceModule.cpp"
#include "EditorSceneViewModule.cpp"
#include "EditorHierarchyModule.cpp"
#include "EditorComponentsModule.cpp"
#include "EditorConsoleModule.cpp"

namespace RDEEditor {

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

		auto* _editorCameraNode = new Node((NodeID)0, nullptr, &engine->manager, new Transform(nullptr));
		editorCamera = new Camera(_editorCameraNode, &engine->manager, nullptr, engine->getWindow());

		gridTexture.loadTextureFromMemory(1920, 1080, _data);
		gridSprite = graph->createNode("Grid")->addComponent<SpriteRenderer>(SpriteRendererConfig {
			.texture = &gridTexture
		});
		gridSprite->setShaderID(gridShaderID);

		auto _textNode = graph->createNode("Duck");
		nodes.push_back(_textNode->getID());
		_textNode->getTransform()->setPosition(0, 0);
		auto _texture = engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
		auto* _sprite = _textNode->addComponent<SpriteRenderer>(SpriteRendererConfig {
			.texture = _texture
		});

		mseDel.bind<&Editor::mouseScrolled>(this);
		wreDel.bind<&Editor::windowResized>(this);

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
			editorCamera->node->getTransform()->setPosition(sceneViewOffset.x, -sceneViewOffset.y);
		}
    }

	void Editor::onEvent(Event& _event) {
		EventDispatcher dispatcher(_event);
		dispatcher.dispatchEvent<MouseScrolledEvent>(mseDel);
		dispatcher.dispatchEvent<WindowResizedEvent>(wreDel);
	}

    void Editor::onLateUpdate(Delta _dt) {
        Scene::onLateUpdate(_dt);
		engine->manager.renderManager.overwriteRenderingCamera(editorCamera);

		engine->manager.shaderManager.setFloat4(gridShaderID, "backgroundColor", glm::vec4{ backgroundColor.r / 255.f, backgroundColor.g / 255.f, backgroundColor.b / 255.f, backgroundColor.a / 255.f });
		engine->manager.shaderManager.setFloat(gridShaderID, "zoom", editorCamera->getCurrentZoomLevel());
		engine->manager.shaderManager.setFloat2(gridShaderID, "resolution", glm::vec2 { (float)engine->getWindow()->getWidth(), (float)engine->getWindow()->getHeight() });
		
		auto _cameraPos = editorCamera->node->getTransform()->getPosition();
		engine->manager.shaderManager.setFloat2(gridShaderID, "cameraDisplacement", glm::vec2 { _cameraPos.x, _cameraPos.y });

		auto _zoom = editorCamera->getCurrentZoomLevel();
		gridSprite->node->getTransform()->setScale(_zoom + std::abs(_cameraPos.x), _zoom + std::abs(_cameraPos.x));
    }

	void Editor::onImGuiRender(Delta _dt) {
		dockingSpaceView(this);
		sceneView(this, engine->frameBuffer, &sceneViewOffset);
		hierarchyView(this);
		componentsView(this);
		consoleView(this);
	}

    void Editor::onDebugRender(Delta _dt, RenderManager* _renderManager) {
		Scene::onDebugRender(_dt, _renderManager);

		for(auto* _camera : getCameras()) { 
			auto _cameraSize = mainCamera->getCameraSize();
			auto _cameraPos = mainCamera->node->getTransform()->getPosition();
			auto _cameraRot = mainCamera->node->getTransform()->getRotation();

			DebugShape _cameraShape;
			_cameraShape.makeSquare(_cameraPos, { (float)_cameraSize.x, (float)_cameraSize.y });
			_cameraShape.showOutsideColor(true);
			_cameraShape.setOutlineColor(Color::White);
			_cameraShape.showInnerColor(false);
			_cameraShape.setRotation(_cameraRot);
			engine->manager.renderManager.drawShape(_cameraShape);
		}
    }

	bool Editor::mouseScrolled(MouseScrolledEvent& _event) {
		if(!editorFlags.isSceneViewHovered) {
			return true;
		}

		editorCamera->setCurrentZoomLevel(editorCamera->getCurrentZoomLevel() + _event.getScrollY() * 0.1f);
		return true;
	}

	bool Editor::windowResized(WindowResizedEvent& _event) {
		Util::Log::info("Hello");
		editorCamera->onResize(_event.getWidth(), _event.getHeight());
		return true;
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
}
