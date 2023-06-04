
void sceneView(Editor* _editor, Vec2F* _sceneViewOffset) {
	static bool _firstEntry = true;
	static Vec2I _firstWindowSize = _editor->engine->getWindow()->getWindowSize();

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
		Vec2F _initPos = {
			-(-_windowSize.x * 0.5f + viewportPanelSize.x * 0.5f),
			-(_windowSize.y * 0.5f - viewportPanelSize.y * 0.5f),
		};
		_editor->editorCamera->node->getTransform()->setPosition(_initPos);
		_firstEntry = false;
	}

	_editor->editorFlags.isSceneViewActive = ImGui::IsWindowFocused();
	_editor->editorFlags.isSceneViewHovered = ImGui::IsWindowHovered();

	auto* _cameraViewport = _editor->getCameras()[0]->getViewport();
	*_sceneViewOffset = { (_cameraViewport->getSize().x - ImGui::GetWindowSize().x) * 0.5f, 
						  (_cameraViewport->getSize().y - ImGui::GetWindowSize().y) * 0.5f };

	_editor->editorData.sizeOfSceneView = viewportSize;

	auto _zoom = _editor->editorCamera->getCurrentZoomLevel();

	auto _mousePos = ImGui::GetIO().MousePos;
	auto _titleHeight = (ImGui::GetStyle().FramePadding.y + ImGui::GetFontSize() * 0.5f) * 0.5f * _zoom;

	// This calcultion I'm not really sure why is needed, because X axis does not need it, but it seems that the change of the window in the Y axis creates an ugly offset that gets fixed by adding
	// this _windowResizedOffsetY
	auto _windowResizedOffsetY = _windowSize.y - _firstWindowSize.y;
	auto _cameraOffset = _editor->editorCamera->node->getTransform()->getPosition();
	auto _mouseY = _mousePos.y * _zoom - ((ImGui::GetWindowPos().y + _editor->editorData.sceneViewOffset.y + _windowResizedOffsetY) * _zoom + _cameraOffset.y) - _titleHeight;
	auto _mouseX = _mousePos.x * _zoom - ((ImGui::GetWindowPos().x + _editor->editorData.sceneViewOffset.x) * _zoom - _cameraOffset.x);

	_editor->editorData.mousePositionOnSceneView = Vec2F { _mouseX - (viewportSize.x * 0.5f * _zoom), (viewportSize.y * 0.5f * _zoom) - _mouseY };

	if(_editor->engine->manager.inputManager.isMouseJustPressed(RDE_MOUSE_BUTTON_0)) {
		Util::Log::info("MposY: ", _editor->editorData.mousePositionOnSceneView.y, ", ImGUiWPosY: ", ImGui::GetWindowSize().y, ", ImGuiVPHeight: ", viewportSize.y, ", screenOffsetY: ", _sceneViewOffset->y, ", WHeight: ", _windowSize.y, ", VPHeight: ", _cameraViewport->getSize().y);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
