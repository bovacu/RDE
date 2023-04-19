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

    void Editor::redirectRendering(FrameBuffer* _frameBuffer) {
		sceneView(this, _frameBuffer);
    }

    void Editor::onInit() {
		auto* _editorCameraNode = new Node((NodeID)0, nullptr, &engine->manager, new Transform(nullptr));
		editorCamera = new Camera(_editorCameraNode, &engine->manager, nullptr, engine->getWindow());

        redirectRenderingDel.bind<&Editor::redirectRendering>(this);
        engine->setRenderingRedirectionToImGui(redirectRenderingDel);

		auto _textNode = graph->createNode("Duck");
		nodes.push_back(_textNode->getID());
		_textNode->getTransform()->setPosition(0, 0);
		auto _texture = engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
		auto* _sprite = _textNode->addComponent<SpriteRenderer>(SpriteRendererConfig {
			.texture = _texture
		});

		mseDel.bind<&Editor::mouseScrolled>(this);
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
    }

	void Editor::onEvent(Event& _event) {
		EventDispatcher dispatcher(_event);
		dispatcher.dispatchEvent<MouseScrolledEvent>(mseDel);
	}

    void Editor::onLateUpdate(Delta _dt) {
        Scene::onLateUpdate(_dt);
		engine->manager.renderManager.overwriteRenderingCamera(editorCamera);
    }

	void Editor::onImGuiRender(Delta _dt) {
		dockingSpaceView(this);
		hierarchyView(this);
		componentsView(this);
		consoleView(this);
	}

    void Editor::onDebugRender(Delta _dt, RenderManager* _renderManager) {
		Scene::onDebugRender(_dt, _renderManager);
		auto _windowSize = engine->getWindow()->getWindowSize();
		engine->manager.renderManager.drawLine( { 0, -(float)_windowSize.y }, { 0, (float)_windowSize.y }, Color::Blue);
		engine->manager.renderManager.drawLine( { -(float)_windowSize.x, 0 }, { (float)_windowSize.x, 0 }, Color::Blue);

		auto _mainCameraSize = mainCamera->getCameraSize();
		auto _mainCameraPos = mainCamera->node->getTransform()->getPosition();
		auto _mainCameraRot = mainCamera->node->getTransform()->getRotation();

		DebugShape _cameraShape;
		_cameraShape.makeSquare(_mainCameraPos, { (float)_mainCameraSize.x, (float)_mainCameraSize.y });
		_cameraShape.showOutsideColor(true);
		_cameraShape.setOutlineColor(Color::White);
		_cameraShape.showInnerColor(false);
		_cameraShape.setRotation(_mainCameraRot);
		engine->manager.renderManager.drawShape(_cameraShape);
    }

	bool Editor::mouseScrolled(MouseScrolledEvent& _event) {
		if(!editorFlags.isSceneViewHovered) {
			return true;
		}

		editorCamera->setCurrentZoomLevel(editorCamera->getCurrentZoomLevel() + _event.getScrollY() * 0.1f);
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
