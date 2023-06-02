static Node* nodeRighClickContextMenuNode = nullptr;
static Graph* nodeRightClickGraph = nullptr;
static bool nodeContextMenuOpened = false;

void editorContextMenuNodeRightClicked(Editor* _editor, Node* _node, Graph* _graph) {
	if (nodeRighClickContextMenuNode != nullptr) return;
	onHierarchyNodeLeftClicked(_editor, _node, _graph);
	nodeRighClickContextMenuNode = _node;
	nodeRightClickGraph = _graph;
}

void nodeRighClickContextMenu(Editor* _editor) {
	if(!nodeContextMenuOpened && nodeRighClickContextMenuNode != nullptr) {
		ImGui::OpenPopup("NodeContextMenu");
		nodeContextMenuOpened = true;
	}

	if (ImGui::BeginPopup("NodeContextMenu", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize)) {
		if (ImGui::BeginMenu("Add Child")) {
			
			if(_editor->graph == nodeRightClickGraph) {
				if(ImGui::BeginMenu("Node")) {
					createNodeSubMenu(_editor, nodeRighClickContextMenuNode);
					ImGui::EndMenu();
				}
			} else {
				if(ImGui::BeginMenu("UI Node")) {
					createUINodeSubMenu(_editor, nodeRighClickContextMenuNode);
					ImGui::EndMenu();
				}
			}

			ImGui::EndMenu();
		}

		if(nodeRighClickContextMenuNode == nodeRightClickGraph->getRoot()) { ImGui::BeginDisabled(true); }
		if (ImGui::Selectable("Set parent to root")) {
			nodeRightClickGraph->setParent(nodeRighClickContextMenuNode, nodeRightClickGraph->getRoot());
			nodeRighClickContextMenuNode = nullptr;
			nodeContextMenuOpened = false;
		}
		if(nodeRighClickContextMenuNode == nodeRightClickGraph->getRoot()) { ImGui::EndDisabled(); }

		if(nodeRighClickContextMenuNode == nodeRightClickGraph->getRoot()) { ImGui::BeginDisabled(true); }
		if (ImGui::Selectable("Remove node")) {
			nodeRightClickGraph->removeNode(nodeRighClickContextMenuNode);
			nodeRighClickContextMenuNode = nullptr;
			nodeContextMenuOpened = false;
			_editor->editorData.selectedNode.node = nullptr;
			_editor->editorData.selectedNode.graph = nullptr;
		}
		if(nodeRighClickContextMenuNode == nodeRightClickGraph->getRoot()) { ImGui::EndDisabled(); }

		ImGui::EndPopup();
	} else {
		nodeRighClickContextMenuNode = nullptr;
		nodeContextMenuOpened = false;
	}
}