static Node* nodeRighClickContextMenuNode = nullptr;

void editorContextMenuNodeRightClicked(Editor* _editor, Node* _node, Graph* _graph) {
	if (nodeRighClickContextMenuNode != nullptr) return;
	nodeRighClickContextMenuNode = _node;
	
	ImGui::OpenPopup("NodeContextMenu");
}

void nodeRighClickContextMenu(Editor* _editor) {
	ImGui::SetNextWindowSize(ImVec2 { 200, 200 });
	if (ImGui::BeginPopup("NodeContextMenu", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar)) {
		
		ImGui::Text("Hello");
		
		ImGui::EndPopup();
	} else {
		nodeRighClickContextMenuNode = nullptr;
	}
}