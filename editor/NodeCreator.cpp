Node* createEmptyNode(Editor* _editor) {
	return _editor->graph->createNode(Util::String::appendToString("EmptyNode_", _editor->graph->getNodeContainer().size()));
}

SpriteRenderer* createSpriteRendererNode(Editor* _editor) {
	auto _node = _editor->graph->createNode(Util::String::appendToString("SpriteRenderer_", _editor->graph->getNodeContainer().size()));
	auto _texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
	auto* _sprite = _node->addComponent<SpriteRenderer>(SpriteRendererConfig {
		.texture = _texture
	});

	return _sprite;
}


UIPanel* createPanelNode(Editor* _editor) {
	auto _panelNode = _editor->canvas->graph->createNode(Util::String::appendToString("Panel_", _editor->graph->getNodeContainer().size()));
	return _panelNode->addComponent<UIPanel>(UIPanelConfig { .size = {256, 256} });
}