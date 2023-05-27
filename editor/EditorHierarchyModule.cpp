
void showLoadedPrefabs(Editor* _scene, Graph* _graph, Node* _node, NodeID& _selectedNode) {
	auto _flags = _node->getID() == _selectedNode ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
	if (ImGui::TreeNodeEx("Prefabs loaded in memory", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

		auto _prefabs = _scene->getPrefabs();
		for(auto& _nodeID : _prefabs) {
			auto _tag = _node->getComponent<Tag>();
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", _tag->tag.c_str());
		}

		ImGui::TreePop();
	}
}

void editorHierarchyModuleNodeLeftClicked(Editor* _editor, Node* _node, Graph* _graph) {
	_editor->editorData.selectedNode.node = _node;
	_editor->editorData.selectedNode.graph = _graph;
}

void hierarchyRecursionStub(Editor* _scene, Graph* _graph, Node* _node, bool* _invalidClick) {
	if (_node == _scene->gridSprite->node) return;

	auto _prefabs = _scene->getPrefabs();
	if(std::find(_prefabs.begin(), _prefabs.end(), _node->getID()) != _prefabs.end()) return;

	auto* _transform =_node->getTransform();
	auto* _tag = _node->getComponent<Tag>();

	if(!_transform->children.empty()) {

		auto _flags = _node == _scene->editorData.selectedNode.node ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
		if (ImGui::TreeNodeEx(_tag->tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

			if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				*_invalidClick = false;
				onHierarchyNodeLeftClicked(_scene, _node, _graph);
			} else if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				onHierarchyNodeRightClicked(_scene, _node, _graph);
			}

			for(auto _child : _transform->children) {
				hierarchyRecursionStub(_scene, _graph, _child->node, _invalidClick);
			}

			ImGui::TreePop();
		}
	} else {
		ImGui::Selectable(_tag->tag.c_str(), _scene->editorData.selectedNode.node == _node);

		if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			*_invalidClick = false;
			onHierarchyNodeLeftClicked(_scene, _node, _graph);
		} else if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			*_invalidClick = false;
			onHierarchyNodeRightClicked(_scene, _node, _graph);
		}
	}
}

void hierarchy(Editor* _scene) {
	auto _clickReleased = _scene->engine->manager.inputManager.isMouseJustReleased(RDE::RDE_MOUSE_BUTTON_0);
	auto _clickOnHierarchyWindowAndGotNoTarget = ImGui::IsWindowHovered() && _clickReleased;
	auto* _graph = _scene->graph;

	auto _clickClone = _clickOnHierarchyWindowAndGotNoTarget;
	hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), &_clickClone);

	_graph = _scene->canvas->graph;
	hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), &_clickClone);

	if(_clickClone) {
		onHierarchyNodeLeftClicked(_scene, nullptr, nullptr);
	}

	//showLoadedPrefabs(_scene, _scene->graph, _graph->getRoot(), _scene->editorData.selectedNode);
}

void hierarchyView(Editor* _editor) {
	ImGui::Begin("Nodes");
	hierarchy(_editor);
	ImGui::End();
}
