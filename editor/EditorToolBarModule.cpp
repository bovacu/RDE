
void toolBar(Editor* _editor) {
	
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2 { _editor->editorData.bottomBarWidth, (float)_editor->engine->getWindow()->getHeight() });
	ImGui::SetNextWindowPos(ImVec2 { 0, 0 });

	if(ImGui::Begin("BottomBar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
	                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking)) {
		
		const int _buttonSize = 28;

		ImVec4 _selectedColor = ImVec4(204.f / 255.f, 204.f / 255.f, 0, 0.5f);
		ImVec4 _noSelectedColor = ImVec4(0, 0, 0, 0);

		auto* _translation = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "translateIcon");
		auto _translationInit = Vec2F(_translation->getRegion().bottomLeftCorner.x / _translation->getSpriteSheetSize().x, _translation->getRegion().bottomLeftCorner.y / _translation->getSpriteSheetSize().y);
		auto _translationEnd = _translationInit + Vec2F(_translation->getRegion().size.x / _translation->getSpriteSheetSize().x, _translation->getRegion().size.y / _translation->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_translation->getGLTexture()), ImVec2(_buttonSize, _buttonSize), {_translationInit.x, _translationInit.y}, {_translationEnd.x, _translationEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Move ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Move;
		}
		ImGui::PopID();
		HINT("Translation Tool")


		auto* _rotation = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "rotateIcon");
		auto _rotationInit = Vec2F(_rotation->getRegion().bottomLeftCorner.x / _rotation->getSpriteSheetSize().x, _rotation->getRegion().bottomLeftCorner.y / _rotation->getSpriteSheetSize().y);
		auto _rotationEnd = _rotationInit + Vec2F(_rotation->getRegion().size.x / _rotation->getSpriteSheetSize().x, _rotation->getRegion().size.y / _rotation->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_rotation->getGLTexture()), ImVec2(_buttonSize, _buttonSize), {_rotationInit.x, _rotationInit.y}, {_rotationEnd.x, _rotationEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Rotate ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Rotate;
		}
		ImGui::PopID();
		HINT("Rotation Tool")


		auto* _scale = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "scaleIcon");
		auto _scaleInit = Vec2F(_scale->getRegion().bottomLeftCorner.x / _scale->getSpriteSheetSize().x, _scale->getRegion().bottomLeftCorner.y / _scale->getSpriteSheetSize().y);
		auto _scaleEnd = _scaleInit + Vec2F(_scale->getRegion().size.x / _scale->getSpriteSheetSize().x, _scale->getRegion().size.y / _scale->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_scale->getGLTexture()), ImVec2(_buttonSize, _buttonSize), {_scaleInit.x, _scaleInit.y}, {_scaleEnd.x, _scaleEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Scale ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Scale;
		}
		ImGui::PopID();
		HINT("Scaling Tool")


		ImGui::SetCursorPos({ ImGui::GetCursorPos().x, viewport->WorkSize.y * 0.5f - _buttonSize });

		auto* _compile = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "compileIcon");
		auto _compileInit = Vec2F(_compile->getRegion().bottomLeftCorner.x / _compile->getSpriteSheetSize().x, _compile->getRegion().bottomLeftCorner.y / _compile->getSpriteSheetSize().y);
		auto _compileEnd = _compileInit + Vec2F(_compile->getRegion().size.x / _compile->getSpriteSheetSize().x, _compile->getRegion().size.y / _compile->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_compile->getGLTexture()), ImVec2(_buttonSize, _buttonSize), {_compileInit.x, _compileInit.y}, {_compileEnd.x, _compileEnd.y}, 0, _noSelectedColor)) {
			
		}
		ImGui::PopID();
		HINT("Compile")


		auto* _play = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "playIcon");
		auto _playInit = Vec2F(_play->getRegion().bottomLeftCorner.x / _play->getSpriteSheetSize().x, _play->getRegion().bottomLeftCorner.y / _play->getSpriteSheetSize().y);
		auto _playEnd = _playInit + Vec2F(_compile->getRegion().size.x / _play->getSpriteSheetSize().x, _play->getRegion().size.y / _play->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_play->getGLTexture()), ImVec2(_buttonSize, _buttonSize), {_playInit.x, _playInit.y}, {_playEnd.x, _playEnd.y}, 0, _noSelectedColor)) {
			
		}
		ImGui::PopID();
		HINT("Play Scene")

		ImGui::End();
	}
}