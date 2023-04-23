using namespace RDE;
using namespace RDEEditor;

void sceneView(Editor* _editor, FrameBuffer* _frameBuffer, Vec2F* _sceneViewOffset) {
	static bool _firstEntry = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	auto viewportSize = Vec2F { viewportPanelSize.x, viewportPanelSize.y };
	if(_firstEntry) {
		Vec2I _windowSize = _editor->engine->getWindow()->getWindowSize();
		Vec2F _initPos = { 
			-(-_windowSize.x * 0.5f + viewportPanelSize.x * 0.5f), 
			-(_windowSize.y * 0.5f - viewportPanelSize.y * 0.5f), 
		};
		_editor->editorCamera->node->getTransform()->setPosition(_initPos);
		_firstEntry = false;
	}

	uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
	_editor->editorCamera->update();
	ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	_editor->editorFlags.isSceneViewActive = ImGui::IsWindowFocused();
	_editor->editorFlags.isSceneViewHovered = ImGui::IsWindowHovered();

	*_sceneViewOffset = { (_editor->engine->getWindow()->getWidth() - ImGui::GetWindowSize().x) * 0.5f, 
						  (_editor->engine->getWindow()->getHeight() - ImGui::GetWindowSize().y) * 0.5f };

	ImGui::End();
	ImGui::PopStyleVar();
}