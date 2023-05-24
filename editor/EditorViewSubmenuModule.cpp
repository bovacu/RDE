void setTheme(EditorTheme _theme) {
	switch(_theme) {
		case EditorTheme::Cherry: setStyleCherry(); break;
		case EditorTheme::Dark: setStyleDark(); break;
		case EditorTheme::DarkRed: setStyleDarkRed(); break;
		case EditorTheme::DarkRuda: setStyleDarkRuda(); break;
		case EditorTheme::DeepDark: setStyleDeepDark(); break;
		case EditorTheme::Dracula: setStyleDracula(); break;
		case EditorTheme::Green: setStyleGreen(); break;
		case EditorTheme::Gold: setStyleGold(); break;
		case EditorTheme::MaterialFlat: setStyleMaterialFlat(); break;
		case EditorTheme::PsycoLight: setStylePsycoLight(); break;
	}
}

void viewSubmenuTheme(Editor* _editor) {
	int _theme = (int)_editor->editorFlags.theme;
	ImGui::RadioButton("Cherry", &_theme, (int)EditorTheme::Cherry);
	ImGui::RadioButton("Dark", &_theme, (int)EditorTheme::Dark);
	ImGui::RadioButton("DarkRed", &_theme, (int)EditorTheme::DarkRed);
	ImGui::RadioButton("DarkRuda", &_theme, (int)EditorTheme::DarkRuda);
	ImGui::RadioButton("DeepDark", &_theme, (int)EditorTheme::DeepDark);
	ImGui::RadioButton("Dracula", &_theme, (int)EditorTheme::Dracula);
	ImGui::RadioButton("Green", &_theme, (int)EditorTheme::Green);
	ImGui::RadioButton("Gold", &_theme, (int)EditorTheme::Gold);
	ImGui::RadioButton("MaterialFlat", &_theme, (int)EditorTheme::MaterialFlat);
	ImGui::RadioButton("PsycoLight", &_theme, (int)EditorTheme::PsycoLight);

	if(_theme != (int)_editor->editorFlags.theme) {
		_editor->editorFlags.theme = (EditorTheme)_theme;
		setTheme(_editor->editorFlags.theme);
	}
}

void viewSubmenu(Editor* _editor) { 
	if(ImGui::BeginMenu("Theme")) {
		viewSubmenuTheme(_editor);
		ImGui::EndMenu();
	}
}