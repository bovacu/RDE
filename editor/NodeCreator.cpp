Node* createEmptyNode(Editor* _editor) {
	return _editor->graph->createNode(Util::String::appendToString("EmptyNode_", _editor->graph->getNodeContainer().size()));
}

SpriteRenderer* createSpriteRendererNode(Editor* _editor) {
	auto _node = _editor->graph->createNode(Util::String::appendToString("SpriteRenderer_", _editor->graph->getNodeContainer().size()));
	auto _texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
	auto* _sprite = _node->addComponent<SpriteRenderer>(SpriteRendererConfig {
		.texture = _texture
	});

	_sprite->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _sprite;
}


UIPanel* createPanelNode(Editor* _editor) {
	auto _panelNode = _editor->canvas->graph->createNode(Util::String::appendToString("Panel_", _editor->graph->getNodeContainer().size()));
	auto* _panel = _panelNode->addComponent<UIPanel>(UIPanelConfig { .size = {256, 256} });
	_panel->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _panel;
}

UIText* createUITextNode(Editor* _editor) {
	auto _uiTextNode = _editor->canvas->graph->createNode(Util::String::appendToString("UIText_", _editor->graph->getNodeContainer().size()));
	auto* _text = _uiTextNode->addComponent<UIText>(UITextConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_text->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _text;
}
