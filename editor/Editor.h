//
// Created by borja on 28/07/22.
//

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "RDE.h"
#include "core/graph/Scene.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/render/shapes/DebugShape.h"
#include "core/util/Delta.h"
#include "core/util/Vec.h"
#include <vector>

using namespace RDE;

namespace RDEEditor {

	enum EditMode {
		Move,
		Rotate,
		Scale
	};
	
	enum EditModeAxis {
		None,
		X, Y, Z
	};

	enum EditorTheme {
		Cherry,
		Dark,
		DarkRed,
		DarkRuda,
		DeepDark,
		Dracula,
		Green,
		Gold,
		MaterialFlat,
		PsycoLight
	};
	
	struct Gizmos {
		DebugShape translationX, translationY, translationXY;
		Vec2F lastClickOrMovedMousePositionForTranslation;
	};
	
	struct EditorFlags {
		bool isSceneViewActive = false;
		bool isSceneViewHovered = false;

		bool isConsoleViewActive = false;
		bool isConsoleViewHovered = false;
		
		EditMode editMode = EditMode::Move;
		uint32_t editModeAxis = EditModeAxis::None;

		EditorTheme theme = EditorTheme::DarkRuda;
	};

	struct SelectedNode {
		Node* node = nullptr;
		Graph* graph = nullptr;
	};

	class Editor;
	struct EditorData {
		SelectedNode selectedNode;
		std::map<int, std::vector<Transform*>> overlappedNodesSceneSelection;
		int idIndex = 0;

		uint32_t sceneViewFramebufferID = 0;
		uint32_t gameViewFramebufferID = 0;

		Vec2F sizeOfSceneView {0, 0};
		Vec2F mousePositionOnSceneView {0, 0};
		float menuBarHeight = 0.f;
		float bottomBarWidth = 35.f;
		
		Gizmos gizmos;
		Delegate<void(Editor*, Node*, Graph*)> onHierarchyElementClicked;
	};

    class Editor : public Scene {
        UniqueDelegate<void(FrameBuffer*)> redirectRenderingDel;
		UIText* typedText = nullptr;
		UniqueDelegate<bool(MouseScrolledEvent&)> mseDel;
		UniqueDelegate<bool(WindowResizedEvent&)> wreDel;
		Texture gridTexture;
		Node* duckNode = nullptr;
		ShaderID gridShaderID;
		Color backgroundColor = { 76, 76, 76, 255 };
		Color gridColor = { 63, 63, 63, 255 };

		public:
			EditorFlags editorFlags;
			EditorData editorData;
			Camera* editorCamera;
			Vec2F lastClickOrMovedMousePosition;
			Vec2F sceneViewOffset;
			SpriteRenderer* gridSprite = nullptr;

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
			void centerCamera();
			void generateGridTexture();


        private:
            void textStressTest(int _amount);
            void physicsTest();
            void particleSystemTest();
            void localizationTest();
            std::vector<NodeID> nodes;

			void generateTranslationGuizmo();
			void selectNodeWithClick();
			void editModeInputHandler();
			void editModeTranslationInputHandler();


			void onMouseClick(RDE_MOUSE_BUTTON_ _mouseCode);
            void onMouseEntered();
            void onMouseExited();

            void onCollisionEnter(PhysicsBody* _a, PhysicsBody* _b);
            void collisionB(PhysicsBody* _a, PhysicsBody* _b);
    };

}


#endif //ENGINE_EDITOR_H
