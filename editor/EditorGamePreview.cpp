
bool approximateValue(float _valueA, float _valueB, float _epsilon = 0.01f) {
	return std::abs(_valueA - _valueB) < _epsilon;
}

float getEpsilon(float _imGuiViewportSize, float _sceneViewportSize) {
	auto _percentage = _imGuiViewportSize / _sceneViewportSize;
	if(_percentage > 0.15f) {
		return 0.01f;
	}

	return 0.1f;
}

void gameView(Editor* _editor) {
	static bool _firstEntry = true;
	static ImVec2 _lastImGuiViewportSize = { -1, -1 };
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Game Preview", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	auto* _viewport = _editor->editorCamera->getViewport();
	ImVec2 _viewportPanelSize = ImGui::GetContentRegionAvail();
	auto* _window = _editor->engine->getWindow();

	Vec2I _viewportSize = { (int)_viewportPanelSize.x, (int)_viewportPanelSize.y };
	Vec2I _finalImGuiViewSize = { _viewport->getSize().x, _viewport->getSize().y };

	if(_viewportSize != _viewport->getSize() || (_lastImGuiViewportSize.y != _viewportPanelSize.y || _lastImGuiViewportSize.x != _viewportPanelSize.x)) {
		_finalImGuiViewSize.x = _viewportSize.x;
		auto _i = 1;
		while(_i <= _finalImGuiViewSize.y) {
			if(approximateValue(_viewport->getAspectRatio(), (float)_finalImGuiViewSize.x / (float)_i, getEpsilon((float)_viewportSize.x, (float)_viewport->getSize().x))) {
				_finalImGuiViewSize.y = _i;
				break;
			}
			_i++;
		}
	}

	_lastImGuiViewportSize = _viewportPanelSize;

	ImVec2 _imageSize ={ (float)_finalImGuiViewSize.x, (float)_finalImGuiViewSize.y };
	ImVec2 _position = ImVec2 { ImGui::GetWindowSize().x - _imageSize.x, ImGui::GetWindowSize().y - _imageSize.y };
	ImGui::SetCursorPos(ImVec2 { _position.x * 0.5f, _position.y * 0.5f });

	uint32_t _textureID = _editor->engine->manager.renderManager.getFramebuffer(_editor->editorData.gameViewFramebufferID)->getColorAttachmentRendererID();
	ImGui::Image((void*)(intptr_t)_textureID, _imageSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
	ImGui::PopStyleVar();
}
