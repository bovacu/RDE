static bool overlappingPopupOpened = false;

void overlappingSelectionWindow(Editor* _editor) {
	
	auto _showOverlap = _editor->editorData.overlappedNodesSceneSelection.size() > 1 ||
		(_editor->editorData.overlappedNodesSceneSelection.size() == 1 && 
			_editor->editorData.overlappedNodesSceneSelection.begin()->second.size() > 1);

	if (!_showOverlap) return;

	if(_editor->editorFlags.editModeAxis != EditModeAxis::None) {
		_editor->editorData.overlappedNodesSceneSelection.clear();
		return;
	}

	if(!overlappingPopupOpened) {
		overlappingPopupOpened = true;
		ImGui::OpenPopup("NodeSelection");
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2 { 10, 10 }, ImVec2 { 300, 300 });
	if(ImGui::BeginPopup("NodeSelection", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar)) {
		if(_editor->editorData.overlappedNodesSceneSelection.find(0) != _editor->editorData.overlappedNodesSceneSelection.end()) {
			if(ImGui::CollapsingHeader("No UI Nodes", ImGuiTreeNodeFlags_DefaultOpen)) {
				for(auto* _transform : _editor->editorData.overlappedNodesSceneSelection[0]) {
					auto _foo = false;
					if(ImGui::Selectable(_transform->node->getComponent<Tag>()->tag.c_str(), &_foo)) {
						_editor->editorData.selectedNode.node = _transform->node;
						_editor->editorData.selectedNode.graph = _transform->graph;
						_editor->editorData.overlappedNodesSceneSelection.clear();
						overlappingPopupOpened = false;
						break;
					}
				}
			}
		}

		if(_editor->editorData.overlappedNodesSceneSelection.find(1) != _editor->editorData.overlappedNodesSceneSelection.end()) {
			if(ImGui::CollapsingHeader("UI Nodes", ImGuiTreeNodeFlags_DefaultOpen)) {
				for(auto* _transform : _editor->editorData.overlappedNodesSceneSelection[1]) {
					auto _foo = false;
					if(ImGui::Selectable(_transform->node->getComponent<Tag>()->tag.c_str(), &_foo)) {
						_editor->editorData.selectedNode.node = _transform->node;
						_editor->editorData.selectedNode.graph = _transform->graph;
						_editor->editorData.overlappedNodesSceneSelection.clear();
						overlappingPopupOpened = false;
						break;
					}
				}
			}
		}
		ImGui::EndPopup();
	} else {
		_editor->editorData.overlappedNodesSceneSelection.clear();
		overlappingPopupOpened = false;
	}
}