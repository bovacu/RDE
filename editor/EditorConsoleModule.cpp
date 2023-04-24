
int consoleIntro(ImGuiInputTextCallbackData* _data, Editor* _editor) {
	switch (_data->EventFlag){
		case ImGuiInputTextFlags_CallbackHistory:{
			if (_data->EventKey == ImGuiKey_UpArrow){
				_data->DeleteChars(0, _data->BufTextLen);
				_data->InsertChars(0, _editor->engine->manager.consoleManager.getUpCommand().c_str());
			}
			else if (_data->EventKey == ImGuiKey_DownArrow) {
				_data->DeleteChars(0, _data->BufTextLen);
				_data->InsertChars(0, _editor->engine->manager.consoleManager.getDownCommand().c_str());
			}
		}
	}
	return 0;
}

int consoleStub(ImGuiInputTextCallbackData* _data) {
	return consoleIntro(_data, (Editor*)_data->UserData);
}

void consoleView(Editor* _editor) {
	ImGui::Begin("Console");

	static bool autoscroll = true;
	static bool scrollToBottom = false;

	static char _input[256];
	bool reclaim_focus = false;
	ImGuiInputTextFlags _inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Command").x * 1.5f);
	ImGui::PushID(createID(_editor));
	if(ImGui::InputText("Command", _input, IM_ARRAYSIZE(_input), _inputTextFlags, &consoleStub, (void*)_editor)) {
		std::stringstream _ss(_input);
		std::vector<std::string> _splits;

		std::string _s;
		while (std::getline(_ss, _s, ' '))
			_splits.push_back(_s);

		if(_splits.empty()) return;

		Command _command;
		_command.name = _splits[0];
		_command.arguments = std::vector<std::string>(_splits.begin() + 1, _splits.end());
		auto _result = _editor->engine->manager.consoleManager.call(_command);
		for(auto& _r : _result)
			_editor->engine->manager.consoleManager.logs.push_back(_r);

		reclaim_focus = true;
		#if IS_WINDOWS()
			strcpy_s(_input, "");
		#else
			strcpy(_input, "");
		#endif
	}
	ImGui::PopID();

	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1);

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	auto wPos = ImGui::GetWindowPos();
	auto wSize = ImGui::GetWindowSize();
	ImGui::GetWindowDrawList()->AddRect(wPos, { wPos.x + wSize.x, wPos.y + wSize.y }, ImColor(1.f, 1.f, 1.f, 1.f));

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	ImGui::Indent(2.5f);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	for (size_t i = 0; i < _editor->engine->manager.consoleManager.logs.size(); i++) {
		const char* item = _editor->engine->manager.consoleManager.logs[i].c_str();

		ImVec4 color;
		bool has_color = false;
		if (strstr(item, "[error]")){ color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
		else if (strstr(item, "#")) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
		if (has_color)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(item);
		if (has_color)
			ImGui::PopStyleColor();
	}
	ImGui::Unindent(2.5f);
	if (scrollToBottom || (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	scrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();

	_editor->editorFlags.isConsoleViewActive = ImGui::IsWindowFocused();
	_editor->editorFlags.isConsoleViewHovered = ImGui::IsWindowHovered();

	ImGui::End();
}