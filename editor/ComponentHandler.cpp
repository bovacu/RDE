
SpriteRenderer* addSpriteRenderer(Editor* _editor, Node* _node) {
	auto _texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
	auto* _sprite = _node->addComponent<SpriteRenderer>(SpriteRendererConfig {
		.texture = _texture
	});

	_sprite->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _sprite;
}

TextRenderer* addTextRenderer(Editor* _editor, Node* _node) {
	auto* _text = _node->addComponent<TextRenderer>(TextRendererConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_text->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _text;
}

UIImage* addUIImage(Editor* _editor, Node* _node) {
	auto _texture = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "duck");
	auto* _uiImage = _node->addComponent<UIImage>(UIImageConfig {
		.texture = _texture
	});

	_uiImage->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _uiImage;
}

UIText* addUIText(Editor* _editor, Node* _node) {
	auto* _text = _node->addComponent<UIText>(UITextConfig { .font = _editor->engine->manager.fontManager.getFont("MontserratRegular") });
	_text->setFramebuffer(_editor->editorData.gameViewFramebufferID | _editor->editorData.sceneViewFramebufferID);
	return _text;
}