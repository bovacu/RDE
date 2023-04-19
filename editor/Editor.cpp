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

static void myFunc(int _a) {
    Util::Log::warn("Int: ", _a);
}

static void myFunc2(int _a) {
    Util::Log::warn("Int2: ", _a);
}

namespace Editor {

    void Editor::redirectRendering(FrameBuffer* _frameBuffer) {
		static bool _firstEntry = true;
        #if !IS_MOBILE()
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
            ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            auto viewportSize = Vec2F{ viewportPanelSize.x, viewportPanelSize.y };
			if(_firstEntry) {
				Vec2I _windowSize = engine->getWindow()->getWindowSize();
				Vec2F _initPos = { 
					-(-_windowSize.x * 0.5f + viewportPanelSize.x * 0.5f), 
					-(_windowSize.y * 0.5f - viewportPanelSize.y * 0.5f), 
				};
				editorCamera->node->getTransform()->setPosition(_initPos);
				_firstEntry = false;
			}

            uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
			editorCamera->update();
			ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
            ImGui::PopStyleVar();
        #endif
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

    void Editor::onDebugRender(Delta _dt, RenderManager* _renderManager) {
		Scene::onDebugRender(_dt, _renderManager);
		auto _windowSize = engine->getWindow()->getWindowSize();
		engine->manager.renderManager.drawLine( { 0, -(float)_windowSize.y }, { 0, (float)_windowSize.y }, Color::Blue);
		engine->manager.renderManager.drawLine( { -(float)_windowSize.x, 0 }, { (float)_windowSize.x, 0 }, Color::Blue);
    }

	bool Editor::mouseScrolled(MouseScrolledEvent& _event) {
		editorCamera->setCurrentZoomLevel(editorCamera->getCurrentZoomLevel() + _event.getScrollY() * 0.1f);
		return true;
	}

	void Editor::mouseHandler() {
		
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
