
bool hasModule(const std::string& _module) {
	std::string _filePath = __FILE__;
	std::string _dirPath = _filePath.substr(0, _filePath.rfind("\\"));

	std::filesystem::path _path = _dirPath;

	// RDE/editor/EditorToolsMenuModule.cpp
	std::filesystem::path _parentPath = _path.parent_path().parent_path().parent_path();
	_parentPath += std::string("/");
	_parentPath += _module;

	return std::filesystem::is_directory(_parentPath);
}

void toolsMenu(Editor* _editor) {

	auto _hasAndroidModule = hasModule("RDEAndroid");
	auto _hasiOSModule = hasModule("RDEiOS");
	auto _hasWASMModule = hasModule("RDEWASM");
	auto _isMacPlatform = _editor->engine->getPlatform() == RDE_PLATFORM_TYPE_MAC;
	auto _isLinuxPlatform = _editor->engine->getPlatform() == RDE_PLATFORM_TYPE_LINUX;

	if(ImGui::MenuItem("Compile host platform")) {
		_editor->editorFlags.compilingPopupOpened = true;
	}

	if(ImGui::BeginPopupModal("CompilePopup")) {
		ImGui::Text("Compiling...");
		ImGui::EndPopup();
	}

	if(!_hasAndroidModule) { ImGui::BeginDisabled(true); }
	if(ImGui::MenuItem("Compile Android")) {

	}
	if(!_hasAndroidModule) { 
		ImGui::EndDisabled(); 
		HINT("To compile Android you need RDE Android module at same dir level as RDE")
	}

	if(!_isMacPlatform || !_hasiOSModule) { ImGui::BeginDisabled(true); }
	if(ImGui::MenuItem("Compile iOS")) {

	}
	if(!_isMacPlatform || !_hasiOSModule) { 
		ImGui::EndDisabled(); 
		HINT("To compile iOS you need Mac host and RDE iOS module at same dir level as RDE")
	}

	if(!_hasWASMModule || (!_isMacPlatform && !_isLinuxPlatform)) { ImGui::BeginDisabled(true); }
	if(ImGui::MenuItem("Compile WASM")) {

	}
	if(!_hasWASMModule || (!_isMacPlatform && !_isLinuxPlatform)) { 
		ImGui::EndDisabled(); 
		HINT("To compile WASM you need to be on Linux or Mac Platforms and have RDE WASM module at same dir level as RDE")
	}
}

int compilationThreadFn(void* _data) {
	std::string _output;
	auto _editor = (Editor*)_data;
	
	exec("python manager.py --action=build", _output);
	
	Util::Log::info(_output);
	_editor->editorFlags.compiling = false;

	return 0;
}

void compilePopup(Editor* _editor) {
	if(_editor->editorFlags.compilingPopupOpened) {
		ImGui::OpenPopup("CompilingPopup");
		_editor->editorFlags.compiling = true;
		_editor->editorFlags.compilingPopupOpened = false;
		if(SDL_CreateThreadWithStackSize(compilationThreadFn, "compile", 0, (void*)_editor) == nullptr) {
			Util::Log::error("Error launching compilation -> ", SDL_GetError());
		}
	}

	if(_editor->editorFlags.compiling) {
		if(ImGui::BeginPopupModal("CompilingPopup", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
			ImGui::Text("Compiling");
			ImGui::EndPopup();
		}
	}
}