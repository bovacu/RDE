#include "EditorViewSubmenuModule.cpp"
#include <filesystem>

void createNodeSubMenu(Editor* _editor, Node* _parent = nullptr) {
	if(ImGui::MenuItem("Empty Node")) {
		createEmptyNode(_editor, _editor->graph, _parent);
	}

	if(ImGui::MenuItem("Sprite Renderer Node")) {
		createSpriteRendererNode(_editor, _parent);
	}
}

void createUINodeSubMenu(Editor* _editor, Node* _parent = nullptr) {
	if(ImGui::MenuItem("Empty Node")) {
		createEmptyNode(_editor, _editor->canvas->graph, _parent);
	}

	if(ImGui::MenuItem("Button Node")) {
		createUIButtonNode(_editor, _parent);
	}

	if(ImGui::MenuItem("Image Node")) {
		createUIImageNode(_editor, _parent);
	}

	if(ImGui::MenuItem("InputText Node")) {
		createUIInputNode(_editor, _parent);
	}

	if(ImGui::MenuItem("Panel Node")) {
		createPanelNode(_editor, _parent);
	}

	if(ImGui::MenuItem("Slider Node")) {
		createUISliderNode(_editor, _parent);
	}

	if(ImGui::MenuItem("Text Node")) {
		createUITextNode(_editor, _parent);
	}
}

void nodesSubMenu(Editor* _editor) {
	if(ImGui::BeginMenu("Create")) {
		if(ImGui::BeginMenu("Node")) {
			createNodeSubMenu(_editor);
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("UI Node")) {
			createUINodeSubMenu(_editor);
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void menuBarView(Editor* _editor) {
	if (ImGui::BeginMenuBar()) {

		_editor->editorData.menuBarHeight = ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetFontSize();

		if (ImGui::BeginMenu("File")) {
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Nodes")) {
			nodesSubMenu(_editor);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Tools")) {
			toolsMenu(_editor);
			ImGui::EndMenu();
		}



		if (ImGui::BeginMenu("View")) {
			viewSubmenu(_editor);
			ImGui::EndMenu();
		}



		if (ImGui::BeginMenu("Help")) {
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}
