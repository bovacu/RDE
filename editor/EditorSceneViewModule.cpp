
void sceneView(Editor* _editor, Vec2F* _sceneViewOffset) {
	static bool _firstEntry = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	auto viewportSize = Vec2F { viewportPanelSize.x, viewportPanelSize.y };
	auto& _renderManager = _editor->engine->manager.renderManager;
	uint32_t textureID = _renderManager.getFramebuffer(_editor->editorData.sceneViewFramebufferID)->getColorAttachmentRendererID();
	_editor->editorCamera->update();
	auto _windowSize = _editor->engine->getWindow()->getWindowSize();
	ImGui::Image((void*)(intptr_t)textureID, ImVec2{ (float)_windowSize.x, (float)_windowSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	if(_firstEntry) {
		Vec2I _windowSize = _editor->engine->getWindow()->getWindowSize();
		Vec2F _initPos = {
			-(-_windowSize.x * 0.5f + viewportPanelSize.x * 0.5f),
			-(_windowSize.y * 0.5f - viewportPanelSize.y * 0.5f),
		};
		_editor->editorCamera->node->getTransform()->setPosition(_initPos);
		_firstEntry = false;
	}

	_editor->editorFlags.isSceneViewActive = ImGui::IsWindowFocused();
	_editor->editorFlags.isSceneViewHovered = ImGui::IsWindowHovered();

	*_sceneViewOffset = { (_editor->engine->getWindow()->getWidth() - ImGui::GetWindowSize().x) * 0.5f, 
						  (_editor->engine->getWindow()->getHeight() - ImGui::GetWindowSize().y) * 0.5f };

	_editor->editorData.sizeOfSceneView = viewportSize;

	// TODO (Borja): Zoom not working, fix this.
	auto _mousePos = ImGui::GetIO().MousePos;
	auto _titleHeight = ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetFontSize();
	auto _windowHalfHeight = viewportSize.y * 0.5f - _titleHeight;

	auto _zoom = 1.f / _editor->editorCamera->getCurrentZoomLevel();
	auto _cameraOffset = _editor->editorCamera->node->getTransform()->getPosition();
	auto _mouseY = _mousePos.y - (ImGui::GetWindowPos().y + _editor->sceneViewOffset.y + _cameraOffset.y) - _titleHeight;

	_editor->editorData.mousePositionOnSceneView = Vec2F { _mousePos.x - (ImGui::GetWindowPos().x + _editor->sceneViewOffset.x - _cameraOffset.x) - viewportSize.x * 0.5f, _windowHalfHeight - _mouseY + _titleHeight * 1.5f };
	// Util::Log::info((_editor->sceneViewOffset.x - _cameraOffset.x) * _zoom);

	ImGui::End();
	ImGui::PopStyleVar();
}
