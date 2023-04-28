
#define CREATE_DISABLEABLE_HEADER(_title, _component, _codeBlock)               	\
	if(!_component) return;                                                         \
																					\
	ImGui::PushID(createID(_editor));                                               \
	static bool _enabled = _component->isEnabled();                                 \
	if(ImGui::Checkbox("##enabled", &_enabled)) {                                   \
	_component->setEnabled(_enabled);                                           	\
	}                                                                               \
	ImGui::PopID();                                                                 \
	ImGui::SameLine();                                                              \
																					\
	bool _opened = ImGui::CollapsingHeader(_title, ImGuiTreeNodeFlags_DefaultOpen); \
																					\
	if(_opened) {                                                                   \
	do { _codeBlock } while(0);                                                 	\
	}   


#define CREATE_NON_DISABLEABLE_HEADER(_title, _component, _codeBlock)           	\
	if(!_component) return;                                                         \
																					\
	bool _opened = ImGui::CollapsingHeader(_title, ImGuiTreeNodeFlags_DefaultOpen); \
																					\
	if(_opened) {                                                                   \
	do { _codeBlock } while(0);                                                 	\
	}   

static void helpMarker(const char* desc) {
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void activeComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);
        
	bool _active = _node->hasComponent<Active>();
	auto _tag = _node->getComponent<Tag>()->tag.c_str();
	ImGui::Text("%s", _tag);
	ImGui::SameLine(0, ImGui::GetWindowWidth() - ImGui::CalcTextSize(_tag).x - 40 - ImGui::CalcTextSize("(?)").x);
	ImGui::PushID(createID(_editor));
	if(ImGui::Checkbox("###Active", &_active)) {
		_node->setActive(_active);
	}
	helpMarker("This will set the Active property to true/false.\n Setting Active to false will make all of DisabledConfig elements to be disabled for the Node and children.");
	ImGui::PopID();
}

void tagComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	ImGui::Text("Tag"); ImGui::SameLine();
	char _buffer[256] = { 0 };
	auto& _tag = _node->getComponent<Tag>()->tag;
#if IS_WINDOWS()
	strcpy_s(_buffer, _tag.c_str());
#else
	strcpy(_buffer, _tag.c_str());
#endif

	if(ImGui::InputText("###tagName", _buffer, 256)) {
		_tag = std::string(_buffer);
	}
	ImGui::Separator();
}

void transformComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	Transform* _transform = nullptr;
	if(!_node->hasComponent<Transform>()) return;
	_transform = _node->getComponent<Transform>();

	CREATE_NON_DISABLEABLE_HEADER("Transform", _transform, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
		ImGui::Text("ID: %i", (int)_transform->node->getID());
		ImGui::Text("Position ");

		int _pos[2];
		_pos[0] = _transform->getPosition().x;
		_pos[1] = _transform->getPosition().y;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragInt2("##myInput", _pos, 0.5f)) {
			_transform->setPosition(_pos[0], _pos[1]);
		}
		ImGui::PopID();


		ImGui::Text("Rotation ");

		float _angle = _transform->getRotation();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragFloat("##myInput", &_angle, 0.1f))
			_transform->setRotation(_angle);
		ImGui::PopID();


		ImGui::Text("Scale ");

		float _scale[2]; 
		_scale[0] = _transform->getScale().x;
		_scale[1] = _transform->getScale().y;
		ImGui::SameLine(0, 30);
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragFloat2("##myInput", _scale, 0.05))
			_transform->setScale(_scale[0], _scale[1]);
		ImGui::PopID();

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	});
}


void cameraComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<Camera>()) return;

	auto _camera = _node->getComponent<Camera>();

	CREATE_NON_DISABLEABLE_HEADER("Camera", _camera, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
		ImGui::Text("Zoom Level");
		float _zoomLevel = _camera->getCurrentZoomLevel();
		ImGui::SameLine(0, 56);
		ImGui::SetNextItemWidth(50);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragFloat("##myInput", &_zoomLevel, 0.1f)) {
			_camera->setCurrentZoomLevel(_zoomLevel);
		}
		ImGui::PopID();

		auto* _viewport = _camera->getViewport();

		ImGui::Text("Viewport Size ");
		int _size[2];
		_size[0] = _viewport->getSize().x;
		_size[1] = _viewport->getSize().y;
		ImGui::SameLine(0, 28);
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if (ImGui::DragInt2("##myInput", _size, 1, 0)) {
			_viewport->setSize({ _size[0], _size[1] });
		}
		ImGui::PopID();

		ImGui::Text("Viewport Position ");
		int _position[2];
		_position[0] = _viewport->getPosition().x;
		_position[1] = _viewport->getPosition().y;
		ImGui::SameLine(0, 0);
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if (ImGui::DragInt2("##myInput", _position, 1, 0)) {
			_viewport->setPosition({ _position[0], _position[1] });
		}
		ImGui::PopID();

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void bodyComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<PhysicsBody>()) return;

	auto _body = _node->getComponent<PhysicsBody>();

	CREATE_DISABLEABLE_HEADER("Physics Body", _body, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void spriteComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<SpriteRenderer>()) return;

	auto _spriteRenderer = _node->getComponent<SpriteRenderer>();

	CREATE_DISABLEABLE_HEADER("Sprite Renderer", _spriteRenderer, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
		ImGui::Text("Texture"); ImGui::SameLine();
		auto _texturePath = _spriteRenderer->getTexturePath();
		ImGui::BeginDisabled(true);
		ImGui::InputText("###texture", const_cast<char*>(_texturePath.c_str()), _texturePath.size());
		ImGui::EndDisabled();

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void textComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<TextRenderer>()) return;

	auto _text = _node->getComponent<TextRenderer>();
	auto _textTransform = _node->getComponent<Transform>();

	CREATE_DISABLEABLE_HEADER("Text Renderer", _text, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
		ImGui::Text("Font size ");
		int _fontSize = _text->getFontSize();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if (ImGui::DragInt("##myInput", &_fontSize, 1, 0, 512)) {
			_text->setFontSize(_fontSize);
		}
		ImGui::PopID();
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void uiTransformComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<UIAnchoring>()) return;

	Transform* _transform = nullptr;
	UIAnchoring* _anchoring = nullptr;
	_transform = _node->getComponent<Transform>();
	_anchoring = _node->getComponent<UIAnchoring>();
	RDE_UI_ANCHOR_ _selectedAnchor = _anchoring->getAnchor();
	RDE_UI_STRETCH_ _selectedStretch = _anchoring->getStretch();

	CREATE_NON_DISABLEABLE_HEADER("UITransform", _transform, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

		ImGui::Text("ID: %i", (int)_transform->node->getID());

		ImGui::Text("Anchor "); ImGui::SameLine(0, 20);
		ImGui::Text("Stretch ");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImVec4 _selectedColor = ImVec4(204.f / 255.f, 204.f / 255.f, 0, 0.5f);
		ImVec4 _noSelectedColor = ImVec4(0, 0, 0, 0);
		auto* _topLeft = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomLeft");
		auto* _topMiddle = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomMiddle");
		auto* _topRight = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomRight");

		auto* _middleLeft = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleLeft");
		auto* _middle = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleMiddle");
		auto* _middleRight = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleRight");

		auto* _bottomLeft = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopLeft");
		auto* _bottomMiddle = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopMiddle");
		auto* _bottomRight = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopRight");

		auto* _stretchNone = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchNone");
		auto* _stretchHorizontal = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchHorizontal");
		auto* _stretchVertical = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchVertical");
		auto* _stretchFull = _editor->engine->manager.textureManager.getSubTexture("defaultAssets", "stretchFull");


		// Anchor top row
		auto _topLeftInit = Vec2F(_topLeft->getRegion().bottomLeftCorner.x / _topLeft->getSpriteSheetSize().x, _topLeft->getRegion().bottomLeftCorner.y / _topLeft->getSpriteSheetSize().y);
		auto _topLeftEnd = _topLeftInit + Vec2F(_topLeft->getRegion().size.x / _topLeft->getSpriteSheetSize().x, _topLeft->getRegion().size.y / _topLeft->getSpriteSheetSize().y);

		auto _topMiddleInit = Vec2F(_topMiddle->getRegion().bottomLeftCorner.x / _topMiddle->getSpriteSheetSize().x, _topMiddle->getRegion().bottomLeftCorner.y / _topMiddle->getSpriteSheetSize().y);
		auto _topMiddleEnd = _topMiddleInit + Vec2F(_topMiddle->getRegion().size.x / _topMiddle->getSpriteSheetSize().x, _topMiddle->getRegion().size.y / _topMiddle->getSpriteSheetSize().y);

		auto _topRightInit = Vec2F(_topRight->getRegion().bottomLeftCorner.x / _topRight->getSpriteSheetSize().x, _topRight->getRegion().bottomLeftCorner.y / _topRight->getSpriteSheetSize().y);
		auto _topRightEnd = _topRightInit + Vec2F(_topRight->getRegion().size.x / _topRight->getSpriteSheetSize().x, _topRight->getRegion().size.y / _topRight->getSpriteSheetSize().y);

		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topLeft->getGLTexture()), ImVec2(16, 16), {_topLeftInit.x, _topLeftInit.y}, {_topLeftEnd.x, _topLeftEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_LEFT_TOP ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_LEFT_TOP;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topMiddle->getGLTexture()), ImVec2(16, 16), {_topMiddleInit.x, _topMiddleInit.y}, {_topMiddleEnd.x, _topMiddleEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_TOP ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_TOP;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topRight->getGLTexture()), ImVec2(16, 16), {_topRightInit.x, _topRightInit.y}, {_topRightEnd.x, _topRightEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_RIGHT_TOP ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_RIGHT_TOP;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();


		// Stretch top row
		ImGui::SameLine(0, 20);
		auto _stretchNoneInit = Vec2F(_stretchNone->getRegion().bottomLeftCorner.x / _stretchNone->getSpriteSheetSize().x, _stretchNone->getRegion().bottomLeftCorner.y / _stretchNone->getSpriteSheetSize().y);
		auto _stretchNoneEnd = _stretchNoneInit + Vec2F(_stretchNone->getRegion().size.x / _stretchNone->getSpriteSheetSize().x, _stretchNone->getRegion().size.y / _stretchNone->getSpriteSheetSize().y);

		auto _stretchFullInit = Vec2F(_stretchFull->getRegion().bottomLeftCorner.x / _stretchFull->getSpriteSheetSize().x, _stretchFull->getRegion().bottomLeftCorner.y / _stretchFull->getSpriteSheetSize().y);
		auto _stretchFullEnd = _stretchFullInit + Vec2F(_stretchFull->getRegion().size.x / _stretchFull->getSpriteSheetSize().x, _stretchFull->getRegion().size.y / _stretchFull->getSpriteSheetSize().y);

		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchNone->getGLTexture()), ImVec2(16, 16), {_stretchNoneInit.x, _stretchNoneInit.y}, {_stretchNoneEnd.x, _stretchNoneEnd.y}, 0, (_selectedStretch == RDE_UI_STRETCH_NO_STRETCH ? _selectedColor : _noSelectedColor))) {
			_selectedStretch = RDE_UI_STRETCH_NO_STRETCH;
			_anchoring->setStretch(_selectedStretch);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchFull->getGLTexture()), ImVec2(16, 16), {_stretchFullInit.x, _stretchFullInit.y}, {_stretchFullEnd.x, _stretchFullEnd.y}, 0, (_selectedStretch == RDE_UI_STRETCH_FULL_STRETCH ? _selectedColor : _noSelectedColor))) {
			_selectedStretch = RDE_UI_STRETCH_FULL_STRETCH;
			_anchoring->setStretch(_selectedStretch);
		}
		ImGui::PopID();


		// Anchor middle row
		auto _middleLeftInit = Vec2F(_middleLeft->getRegion().bottomLeftCorner.x / _middleLeft->getSpriteSheetSize().x, _middleLeft->getRegion().bottomLeftCorner.y / _middleLeft->getSpriteSheetSize().y);
		auto _middleLeftEnd = _middleLeftInit + Vec2F(_middleLeft->getRegion().size.x / _middleLeft->getSpriteSheetSize().x, _middleLeft->getRegion().size.y / _middleLeft->getSpriteSheetSize().y);

		auto _middleMiddleInit = Vec2F(_middle->getRegion().bottomLeftCorner.x / _middle->getSpriteSheetSize().x, _middle->getRegion().bottomLeftCorner.y / _middle->getSpriteSheetSize().y);
		auto _middleMiddleEnd = _middleMiddleInit + Vec2F(_middle->getRegion().size.x / _middle->getSpriteSheetSize().x, _middle->getRegion().size.y / _middle->getSpriteSheetSize().y);

		auto _middleRightInit = Vec2F(_middleRight->getRegion().bottomLeftCorner.x / _middleRight->getSpriteSheetSize().x, _middleRight->getRegion().bottomLeftCorner.y / _middleRight->getSpriteSheetSize().y);
		auto _middleRightEnd = _middleRightInit + Vec2F(_middleRight->getRegion().size.x / _middleRight->getSpriteSheetSize().x, _middleRight->getRegion().size.y / _middleRight->getSpriteSheetSize().y);

		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middleLeft->getGLTexture()), ImVec2(16, 16), {_middleLeftInit.x, _middleLeftInit.y}, {_middleLeftEnd.x, _middleLeftEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_LEFT ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_LEFT;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middle->getGLTexture()), ImVec2(16, 16),   {_middleMiddleInit.x, _middleMiddleInit.y}, {_middleMiddleEnd.x, _middleMiddleEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_MIDDLE ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_MIDDLE;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middleRight->getGLTexture()), ImVec2(16, 16), {_middleRightInit.x, _middleRightInit.y}, {_middleRightEnd.x, _middleRightEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_RIGHT ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_RIGHT;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();



		// Stretch bottom row
		ImGui::SameLine(0, 20);
		auto _stretchHorInit = Vec2F(_stretchHorizontal->getRegion().bottomLeftCorner.x / _stretchHorizontal->getSpriteSheetSize().x, _stretchHorizontal->getRegion().bottomLeftCorner.y / _stretchHorizontal->getSpriteSheetSize().y);
		auto _stretchHorEnd = _stretchHorInit + Vec2F(_stretchHorizontal->getRegion().size.x / _stretchHorizontal->getSpriteSheetSize().x, _stretchHorizontal->getRegion().size.y / _stretchHorizontal->getSpriteSheetSize().y);

		auto _stretchVertInit = Vec2F(_stretchVertical->getRegion().bottomLeftCorner.x / _stretchVertical->getSpriteSheetSize().x, _stretchVertical->getRegion().bottomLeftCorner.y / _stretchVertical->getSpriteSheetSize().y);
		auto _stretchVertEnd = _stretchVertInit + Vec2F(_stretchVertical->getRegion().size.x / _stretchVertical->getSpriteSheetSize().x, _stretchVertical->getRegion().size.y / _stretchVertical->getSpriteSheetSize().y);

		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchHorizontal->getGLTexture()), ImVec2(16, 16), {_stretchHorInit.x, _stretchHorInit.y}, {_stretchHorEnd.x, _stretchHorEnd.y}, 0, (_selectedStretch == RDE_UI_STRETCH_HORIZONTAL_STRETCH ? _selectedColor : _noSelectedColor))) {
			_selectedStretch = RDE_UI_STRETCH_HORIZONTAL_STRETCH;
			_anchoring->setStretch(_selectedStretch);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchVertical->getGLTexture()), ImVec2(16, 16), {_stretchVertInit.x, _stretchVertInit.y}, {_stretchVertEnd.x, _stretchVertEnd.y}, 0, (_selectedStretch == RDE_UI_STRETCH_VERTICAL_STRETCH ? _selectedColor : _noSelectedColor))) {
			_selectedStretch = RDE_UI_STRETCH_VERTICAL_STRETCH;
			_anchoring->setStretch(_selectedStretch);
		}
		ImGui::PopID();



		// Anchor bottom row
		auto _bottomLeftInit = Vec2F(_bottomLeft->getRegion().bottomLeftCorner.x / _bottomLeft->getSpriteSheetSize().x, _bottomLeft->getRegion().bottomLeftCorner.y / _bottomLeft->getSpriteSheetSize().y);
		auto _bottomLeftEnd = _bottomLeftInit + Vec2F(_bottomLeft->getRegion().size.x / _bottomLeft->getSpriteSheetSize().x, _bottomLeft->getRegion().size.y / _bottomLeft->getSpriteSheetSize().y);

		auto _bottomMiddleInit = Vec2F(_bottomMiddle->getRegion().bottomLeftCorner.x / _bottomMiddle->getSpriteSheetSize().x, _bottomMiddle->getRegion().bottomLeftCorner.y / _bottomMiddle->getSpriteSheetSize().y);
		auto _bottomMiddleEnd = _bottomMiddleInit + Vec2F(_bottomMiddle->getRegion().size.x / _bottomMiddle->getSpriteSheetSize().x, _bottomMiddle->getRegion().size.y / _bottomMiddle->getSpriteSheetSize().y);

		auto _bottomRightInit = Vec2F(_bottomRight->getRegion().bottomLeftCorner.x / _bottomRight->getSpriteSheetSize().x, _bottomRight->getRegion().bottomLeftCorner.y / _bottomRight->getSpriteSheetSize().y);
		auto _bottomRightEnd = _bottomRightInit + Vec2F(_bottomRight->getRegion().size.x / _bottomRight->getSpriteSheetSize().x, _bottomRight->getRegion().size.y / _bottomRight->getSpriteSheetSize().y);

		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomLeft->getGLTexture()), ImVec2(16, 16),   {_bottomLeftInit.x, _bottomLeftInit.y}, {_bottomLeftEnd.x, _bottomLeftEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_LEFT_BOTTOM ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_LEFT_BOTTOM;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomMiddle->getGLTexture()), ImVec2(16, 16), {_bottomMiddleInit.x, _bottomMiddleInit.y}, {_bottomMiddleEnd.x, _bottomMiddleEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_BOTTOM ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_BOTTOM;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomRight->getGLTexture()), ImVec2(16, 16),  {_bottomRightInit.x, _bottomRightInit.y}, {_bottomRightEnd.x, _bottomRightEnd.y}, 0, (_selectedAnchor == RDE_UI_ANCHOR_RIGHT_BOTTOM ? _selectedColor : _noSelectedColor))) {
			_selectedAnchor = RDE_UI_ANCHOR_RIGHT_BOTTOM;
			_anchoring->setAnchor(_selectedAnchor);
		}
		ImGui::PopID();

		ImGui::PopStyleVar();

		ImGui::NewLine();
		ImGui::Text("Size ");
		float _size[2];
		_size[0] = _anchoring->getSize().x;
		_size[1] = _anchoring->getSize().y;
		ImGui::SameLine(0, 35);
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if (ImGui::DragFloat2("##myInput", _size, 1.f)) {
			_anchoring->setSize({ Util::Math::clampF(_size[0], 0.f, FLT_MAX), Util::Math::clampF(_size[1], 0.f, FLT_MAX) });
		}
		ImGui::PopID();

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();

		ImGui::NewLine();

		ImGui::Text("Position ");

		int _pos[2]; 
		_pos[0] = _transform->getPosition().x;
		_pos[1] = _transform->getPosition().y;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragInt2("##myInput", _pos, 0.5f)) {
			_transform->setPosition(_pos[0], _pos[1]);
		}
		ImGui::PopID();


		ImGui::Text("Rotation ");

		float _angle = _transform->getRotation();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragFloat("##myInput", &_angle, 0.1f))
			_transform->setRotation(_angle);
		ImGui::PopID();


		ImGui::Text("Scale ");

		float _scale[2];
		_scale[0] = _transform->getScale().x;
		_scale[1] = _transform->getScale().y;
		ImGui::SameLine(0, 30);
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if(ImGui::DragFloat2("##myInput", _scale, 0.05))
			_transform->setScale(_scale[0], _scale[1]);
		ImGui::PopID();
	})
}

static RDE_IMAGE_RENDERING_TYPE_ renderingTypeStringToEnum(std::string* _types, const std::string& _type) {
	if(_type == _types[0]) return RDE_IMAGE_RENDERING_TYPE_NORMAL;
	if(_type == _types[1]) return RDE_IMAGE_RENDERING_TYPE_NINE_SLICE;
	if(_type == _types[2]) return RDE_IMAGE_RENDERING_TYPE_PARTIAL_VERTICAL;
	if(_type == _types[3]) return RDE_IMAGE_RENDERING_TYPE_PARTIAL_HORIZONTAL;
	if(_type == _types[4]) return RDE_IMAGE_RENDERING_TYPE_PARTIAL_RADIAL;

	return RDE_IMAGE_RENDERING_TYPE_NORMAL;
}

void uiImageComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<UIImage>()) return;

	auto _uiImage = _node->getComponent<UIImage>();

	CREATE_DISABLEABLE_HEADER("UI Image", _uiImage, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

		ImGui::Text("Origin Offset ");
		float _originOffset[2];
		_originOffset[0] = _uiImage->getOriginOffset().x;
		_originOffset[1] = _uiImage->getOriginOffset().y;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::PushID(createID(_editor));
		if (ImGui::DragFloat2("##myInput", _originOffset, 1.f)) {
			_uiImage->setOriginOffset({Util::Math::clampF(_originOffset[0], -FLT_MAX, FLT_MAX), Util::Math::clampF(_originOffset[1], -FLT_MAX, FLT_MAX) });
		}
		ImGui::PopID();


		std::string _imageRenderingTypes[5]; 
		_imageRenderingTypes[0] = "Normal";
		_imageRenderingTypes[1] = "9-Slice";
		_imageRenderingTypes[2] = "Partial Vert";
		_imageRenderingTypes[3] = "Partial Horiz";
		_imageRenderingTypes[4] = "Partial Radial";

		int _partialType = 0;

		std::string _imageRenderingSelected;

		switch(_uiImage->getImageRenderingType()) {
			case RDE_IMAGE_RENDERING_TYPE_NORMAL:
				_imageRenderingSelected = _imageRenderingTypes[0];
				break;
			case RDE_IMAGE_RENDERING_TYPE_NINE_SLICE:
				_imageRenderingSelected = _imageRenderingTypes[1];
				break;
			case RDE_IMAGE_RENDERING_TYPE_PARTIAL_VERTICAL:
				_imageRenderingSelected = _imageRenderingTypes[2];
				_partialType = 0;
				break;
			case RDE_IMAGE_RENDERING_TYPE_PARTIAL_HORIZONTAL:
				_imageRenderingSelected = _imageRenderingTypes[3];
				break;
			case RDE_IMAGE_RENDERING_TYPE_PARTIAL_RADIAL:
				_imageRenderingSelected = _imageRenderingTypes[4];
				break;
		}

		ImGui::PushID(createID(_editor));
		ImGui::Text("Rendering Type"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo("##combo", _imageRenderingSelected.c_str())) { // The second parameter is the label previewed before opening the combo. {
			for (auto& _renderType : _imageRenderingTypes) {
				bool is_selected = (_imageRenderingSelected == _renderType);
				if (ImGui::Selectable(_renderType.c_str(), is_selected)) {
					_imageRenderingSelected = _renderType;
					_uiImage->setImageRenderingType(renderingTypeStringToEnum(_imageRenderingTypes, _imageRenderingSelected));
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();

		if(_imageRenderingSelected == _imageRenderingTypes[2] || _imageRenderingSelected == _imageRenderingTypes[3] || _imageRenderingSelected == _imageRenderingTypes[4]) {
			ImGui::PushID(createID(_editor));
			static float _percentage = _uiImage->getPartialRenderingPercentage();

			ImGui::Text("Percentage");
			ImGui::SameLine(114);
			ImGui::SetNextItemWidth(100);
			if(ImGui::SliderFloat("##Percentage", &_percentage, 0.f, 1.f)) {
				_uiImage->setPartialRenderingPercentage(_percentage);
			}

			ImGui::Text("Inverted");
			ImGui::SameLine(114);
			static bool _inverted = _uiImage->isPartialRenderingInverted();
			if(ImGui::Checkbox("##Inverted", &_inverted)) {
				_uiImage->setPartialRenderingInverted(_inverted);
			}

			ImGui::PopID();
		}

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void uiTextComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<UIText>()) return;

	auto _uiText = _node->getComponent<UIText>();

	CREATE_DISABLEABLE_HEADER("UI Text", _uiText, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

		{
			ImGui::Text("Origin Offset ");
			float _originOffset[2];
			_originOffset[0] = _uiText->getOriginOffset().x;
			_originOffset[1] = _uiText->getOriginOffset().y;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::PushID(createID(_editor));
			if (ImGui::DragFloat2("##myInput", _originOffset, 1.f)) {
				_uiText->setOriginOffset({Util::Math::clampF(_originOffset[0], -FLT_MAX, FLT_MAX), Util::Math::clampF(_originOffset[1], -FLT_MAX, FLT_MAX) });
			}
			ImGui::PopID();

			ImGui::NewLine();

			ImGui::Text("Font size ");
			int _fontSize = _uiText->getFontSize();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::PushID(createID(_editor));
			if (ImGui::DragInt("##myInput", &_fontSize, 1, 0, 512)) {
				_uiText->setFontSize(_fontSize);
			}
			ImGui::PopID();
		}

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void uiMaskComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<UIMask>()) return;

	auto _uiMask = _node->getComponent<UIMask>();

	CREATE_DISABLEABLE_HEADER("UI Mask", _uiMask, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

		ImGui::Text("Inverted ");
		ImGui::SameLine();
		ImGui::PushID(createID(_editor));
		if (ImGui::Checkbox("##Inverted", &_uiMask->inverted)) {}
		ImGui::PopID();

		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void uiCanvasStopperComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<CanvasEventStopper>()) return;

	auto _canvasStopper = _node->getComponent<CanvasEventStopper>();

	CREATE_DISABLEABLE_HEADER("Canvas Event Stopper", _canvasStopper, {
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
		if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
	})
}

void uiButtonComponent(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	auto* _node = _graph->getNode(_selectedNode);

	if(!_node->hasComponent<UIButton>()) return;

	auto _uiButton = _node->getComponent<UIButton>();

	CREATE_DISABLEABLE_HEADER("UI Button", _uiButton, {
	if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

	//            ImGui::Text("Size ");
	//            float _size[2] = { _uiButton->uiImage->getSize().x, _uiButton->uiImage->getSize().y };
	//            ImGui::SameLine();
	//            ImGui::SetNextItemWidth(100);
	//            ImGui::PushID(createID(_editor));
	//            if(ImGui::DragFloat2("##myInput", _size, 0.5f)) {
	//                auto _config = _uiButton->getConfig();
	//                clampF(_size[0], 0, FLT_MAX);
	//                clampF(_size[1], 0, FLT_MAX);
	//                _config.buttonSize = { _size[0], _size[1] };
	//                _uiButton->setConfig(&_editor->engine->manager, _config);
	//            }
	//            ImGui::PopID();

	if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();  
	})
}

void nodeComponents(Editor* _editor, Graph* _graph, const NodeID _selectedNode) {
	if(_selectedNode == NODE_ID_NULL) return;

	activeComponent(_editor, _graph, _selectedNode);
	tagComponent(_editor, _graph, _selectedNode);
	transformComponent(_editor, _graph, _selectedNode);
	cameraComponent(_editor, _graph, _selectedNode);
	spriteComponent(_editor, _graph, _selectedNode);
	bodyComponent(_editor, _graph, _selectedNode);
	textComponent(_editor, _graph, _selectedNode);

	uiTransformComponent(_editor, _graph, _selectedNode);
	uiButtonComponent(_editor, _graph, _selectedNode);
	uiImageComponent(_editor, _graph, _selectedNode);
	uiTextComponent(_editor, _graph, _selectedNode);
	uiMaskComponent(_editor, _graph, _selectedNode);
	uiCanvasStopperComponent(_editor, _graph, _selectedNode);
}

void componentsView(Editor* _editor) {
	ImGui::Begin("Components");
	auto* _mainGraph = _editor->graph;
	if(_editor->editorData.selectedNode != NODE_ID_NULL) nodeComponents(_editor, _mainGraph, _editor->editorData.selectedNode);
	
	if(_editor->editorData.selectedNodeCanvas != NODE_ID_NULL) {
	    auto* _graph = _editor->canvas->graph;
		nodeComponents(_editor, _graph, _editor->editorData.selectedNodeCanvas);
	}
	ImGui::End();
}