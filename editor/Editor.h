//
// Created by borja on 28/07/22.
//

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "RDE.h"
#include "core/graph/Scene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/util/Delta.h"
#include <vector>

using namespace RDE;

namespace RDEEditor {

	struct EditorFlags {
		bool isSceneViewActive = false;
		bool isSceneViewHovered = false;
	};

    class Editor : public Scene {
        UniqueDelegate<void(FrameBuffer*)> redirectRenderingDel;
		UIText* typedText = nullptr;
		UniqueDelegate<bool(MouseScrolledEvent&)> mseDel;
		UniqueDelegate<bool(WindowResizedEvent&)> wreDel;
		Texture gridTexture;
		SpriteRenderer* gridSprite = nullptr;
		ShaderID gridShaderID;
		Color backgroundColor = { 76, 76, 76, 255 };
		Vec2F sceneViewOffset;

		public:
			EditorFlags editorFlags;
			Camera* editorCamera;
			Vec2F lastClickOrMovedMousePosition;

        public:
            explicit Editor(Engine* _engine, const std::string& _debugName = "Editor") : Scene(_engine, _debugName) {  }
            void onInit() override;
			void onEvent(Event& _event) override;
			void onUpdate(Delta _dt) override;
            void onLateUpdate(Delta _dt) override;
            void onDebugRender(Delta _dt, RenderManager* _renderManager) override;
			void onImGuiRender(Delta _dt) override;

		private:
			bool mouseScrolled(MouseScrolledEvent& _event);
			bool windowResized(WindowResizedEvent& _event);
			void mouseHandler();


        private:
            void textStressTest(int _amount);
            void physicsTest();
            void particleSystemTest();
            void localizationTest();
            std::vector<NodeID> nodes;


			void onMouseClick(RDE_MOUSE_BUTTON_ _mouseCode);
            void onMouseEntered();
            void onMouseExited();

            void onCollisionEnter(PhysicsBody* _a, PhysicsBody* _b);
            void collisionB(PhysicsBody* _a, PhysicsBody* _b);
    };

}


#endif //ENGINE_EDITOR_H
