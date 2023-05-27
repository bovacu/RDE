
void toolBar(Editor* _editor) {
	
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(ImVec2 { _editor->editorData.bottomBarWidth, (float)_editor->engine->getWindow()->getHeight() });
	ImGui::SetNextWindowPos(ImVec2 { 0, 0 });

	if(ImGui::Begin("BottomBar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
	                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking)) {
		
		ImVec4 _selectedColor = ImVec4(204.f / 255.f, 204.f / 255.f, 0, 0.5f);
		ImVec4 _noSelectedColor = ImVec4(0, 0, 0, 0);

		auto* _translation = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchFull");
		auto _translationInit = Vec2F(_translation->getRegion().bottomLeftCorner.x / _translation->getSpriteSheetSize().x, _translation->getRegion().bottomLeftCorner.y / _translation->getSpriteSheetSize().y);
		auto _translationEnd = _translationInit + Vec2F(_translation->getRegion().size.x / _translation->getSpriteSheetSize().x, _translation->getRegion().size.y / _translation->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_translation->getGLTexture()), ImVec2(20, 20), {_translationInit.x, _translationInit.y}, {_translationEnd.x, _translationEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Move ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Move;
		}
		ImGui::PopID();


		auto* _rotation = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchFull");
		auto _rotationInit = Vec2F(_rotation->getRegion().bottomLeftCorner.x / _rotation->getSpriteSheetSize().x, _rotation->getRegion().bottomLeftCorner.y / _rotation->getSpriteSheetSize().y);
		auto _rotationEnd = _rotationInit + Vec2F(_rotation->getRegion().size.x / _rotation->getSpriteSheetSize().x, _rotation->getRegion().size.y / _rotation->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_rotation->getGLTexture()), ImVec2(20, 20), {_rotationInit.x, _rotationInit.y}, {_rotationEnd.x, _rotationEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Rotate ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Rotate;
		}
		ImGui::PopID();


		auto* _scale = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchFull");
		auto _scaleInit = Vec2F(_translation->getRegion().bottomLeftCorner.x / _scale->getSpriteSheetSize().x, _scale->getRegion().bottomLeftCorner.y / _scale->getSpriteSheetSize().y);
		auto _scaleEnd = _translationInit + Vec2F(_scale->getRegion().size.x / _scale->getSpriteSheetSize().x, _scale->getRegion().size.y / _scale->getSpriteSheetSize().y);
		
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_scale->getGLTexture()), ImVec2(20, 20), {_scaleInit.x, _scaleInit.y}, {_scaleEnd.x, _scaleEnd.y}, 0, (_editor->editorFlags.editMode == EditMode::Scale ? _selectedColor : _noSelectedColor))) {
			_editor->editorFlags.editMode = EditMode::Scale;
		}
		ImGui::PopID();


		ImGui::End();
	}
}