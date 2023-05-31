Node* createEmptyNode(Editor* _editor, Node* _parent = nullptr) {
	return _editor->graph->createNode(Util::String::appendToString("EmptyNode_", _editor->graph->getNodeContainer().size()), _parent);
}

SpriteRenderer* createSpriteRendererNode(Editor* _editor, Node* _parent = nullptr) {
	auto _node = _editor->graph->createNode(Util::String::appendToString("SpriteRenderer_", _editor->graph->getNodeContainer().size() - 1), _parent);
	auto _texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
	auto* _sprite = _node->addComponent<SpriteRenderer>(SpriteRendererConfig {
		.texture = _texture
	});

	_sprite->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _sprite;
}


UIPanel* createPanelNode(Editor* _editor, Node* _parent = nullptr) {
	auto _panelNode = _editor->canvas->graph->createNode(Util::String::appendToString("Panel_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _panel = _panelNode->addComponent<UIPanel>(UIPanelConfig { .size = {256, 256} });
	_panel->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _panel;
}

UIText* createUITextNode(Editor* _editor, Node* _parent = nullptr) {
	auto _uiTextNode = _editor->canvas->graph->createNode(Util::String::appendToString("UIText_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _text = _uiTextNode->addComponent<UIText>(UITextConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_text->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _text;
}

UIButton* createUIButtonNode(Editor* _editor, Node* _parent = nullptr) {
	auto _uiButtonNode = _editor->canvas->graph->createNode(Util::String::appendToString("UIButton_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _button = _uiButtonNode->addComponent<UIButton>(UIButtonConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_button->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _button;
}

UIInput* createUIInputNode(Editor* _editor, Node* _parent = nullptr) {
	auto _uiInputNode = _editor->canvas->graph->createNode(Util::String::appendToString("UIInput_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _input = _uiInputNode->addComponent<UIInput>(UIInputConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_input->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _input;
}

UISlider* createUISliderNode(Editor* _editor, Node* _parent = nullptr) {
	auto _uiSliderNode = _editor->canvas->graph->createNode(Util::String::appendToString("UISlider_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _slider = _uiSliderNode->addComponent<UISlider>(UISliderConfig {  });
	_slider->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _slider;
}

UIImage* createUIImageNode(Editor* _editor, Node* _parent = nullptr) {
	auto _uiImageNode = _editor->canvas->graph->createNode(Util::String::appendToString("UIImage_", _editor->canvas->graph->getNodeContainer().size() - 1), _parent);
	auto* _image = _uiImageNode->addComponent<UIImage>(UIImageConfig { .texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck") });
	_image->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _image;
}