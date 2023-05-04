
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

void hierarchyRecursionStub(Editor* _scene, Graph* _graph, Node* _node, NodeID& _selectedNode) {
	auto _prefabs = _scene->getPrefabs();
	if(std::find(_prefabs.begin(), _prefabs.end(), _node->getID()) != _prefabs.end()) return;

	auto* _transform =_node->getTransform();
	auto* _tag = _node->getComponent<Tag>();

	if(!_transform->children.empty()) {

		auto _flags = _node->getID() == _selectedNode ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
		if (ImGui::TreeNodeEx(_tag->tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

			if(ImGui::IsItemClicked()) {
				_selectedNode = _node->getID();
				_scene->editorData.sceneViewSelectedNode = _node;
				if(&_scene->editorData.selectedNode == &_selectedNode) _scene->editorData.selectedNodeCanvas = NODE_ID_NULL;
				else _scene->editorData.selectedNode = NODE_ID_NULL;
			}

			for(auto _child : _transform->children) {
				hierarchyRecursionStub(_scene, _graph, _child->node, _selectedNode);
			}

			ImGui::TreePop();
		}
	} else {
		if (ImGui::Selectable(_tag->tag.c_str(), _selectedNode == _node->getID())) {
			_selectedNode = _node->getID();
			_scene->editorData.sceneViewSelectedNode = _node;
			if(&_scene->editorData.selectedNode == &_selectedNode) _scene->editorData.selectedNodeCanvas = NODE_ID_NULL;
			else _scene->editorData.selectedNode = NODE_ID_NULL;
		}
	}
}

void hierarchy(Editor* _scene) {
	auto* _graph = _scene->graph;
	hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), _scene->editorData.selectedNode);
	_graph = _scene->canvas->graph;
	hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), _scene->editorData.selectedNodeCanvas);
	//showLoadedPrefabs(_scene, _scene->graph, _graph->getRoot(), _scene->editorData.selectedNode);
}

void hierarchyView(Editor* _editor) {
	ImGui::Begin("Nodes");
	hierarchy(_editor);
	ImGui::End();
}
