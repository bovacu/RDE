
void gameView(Editor* _editor, FrameBuffer* _frameBuffer) {
	static bool _firstEntry = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Game Preview", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	uint32_t textureID = _frameBuffer->getColorAttachmentRendererID(1);
	auto _windowSize = _editor->engine->getWindow()->getWindowSize();
	ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 500, 500 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
	ImGui::PopStyleVar();
}