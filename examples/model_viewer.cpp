rde_camera model_viewer_camera;
rde_model* model_viewer_model = NULL;
rde_transform model_viewer_transform;

bool model_viewer_draw_wireframe = false;

float model_viewer_yaw = -90.0f;
bool model_viewer_first_mouse = true;
float model_viewer_pitch =  0.0f;
float model_viewer_last_x =  1280.f * 0.5f;
float model_viewer_last_y =  720.f * 0.5f;
rde_vec_3F model_viewer_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F model_viewer_camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F model_viewer_directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F model_viewer_directional_light_ambient_color = { 0.2f, 0.2f, 0.2f };
rde_vec_3F model_viewer_directional_light_diffuse_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F model_viewer_directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

void model_viewer_keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		model_viewer_camera.transform.position.x += model_viewer_camera_front.x * 10 * _dt;
		model_viewer_camera.transform.position.y += model_viewer_camera_front.y * 10 * _dt;
		model_viewer_camera.transform.position.z += model_viewer_camera_front.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		model_viewer_camera.transform.position.x -= model_viewer_camera_front.x * 10 * _dt;
		model_viewer_camera.transform.position.y -= model_viewer_camera_front.y * 10 * _dt;
		model_viewer_camera.transform.position.z -= model_viewer_camera_front.z * 10 * _dt;
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		model_viewer_camera.transform.position.y -= 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		model_viewer_camera.transform.position.y += 10 * _dt;
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(model_viewer_camera_front, model_viewer_camera_up);
		rde_math_normalize(&_cp);

		model_viewer_camera.transform.position.x -= _cp.x * 10 * _dt;
		model_viewer_camera.transform.position.y -= _cp.y * 10 * _dt;
		model_viewer_camera.transform.position.z -= _cp.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(model_viewer_camera_front, model_viewer_camera_up);
		rde_math_normalize(&_cp);

		model_viewer_camera.transform.position.x += _cp.x * 10 * _dt;
		model_viewer_camera.transform.position.y += _cp.y * 10 * _dt;
		model_viewer_camera.transform.position.z += _cp.z * 10 * _dt;
	}
}

void model_viewer_mouse_controller(float _dt) {
	UNUSED(_dt)

	if(rde_events_is_mouse_button_just_released(current_window, RDE_MOUSE_BUTTON_1)) { 
		model_viewer_first_mouse = true;
	}

	if(rde_events_is_mouse_button_pressed(current_window, RDE_MOUSE_BUTTON_1)) {
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)_mouse_pos.y;

		if(model_viewer_last_x == _x_pos && model_viewer_last_y == _y_pos) {
			return;
		}

		if (model_viewer_first_mouse) {
			model_viewer_last_x = _x_pos;
			model_viewer_last_y = _y_pos;
			model_viewer_first_mouse = false;
		}

		float _x_offset = _x_pos - model_viewer_last_x;
		float _y_offset = _y_pos - model_viewer_last_y;
		model_viewer_last_x = _x_pos;
		model_viewer_last_y = _y_pos;

		float _sensitivity = 0.1f;
		_x_offset *= _sensitivity;
		_y_offset *= _sensitivity;

		model_viewer_yaw -= _x_offset;
		model_viewer_pitch += _y_offset;

		if (model_viewer_pitch > 89.0f)
			model_viewer_pitch = 89.0f;
		
		if (model_viewer_pitch < -89.0f)
			model_viewer_pitch = -89.0f;

		rde_vec_3F _front;
		_front.x = cos(rde_math_degrees_to_radians(model_viewer_yaw)) * cos(rde_math_degrees_to_radians(model_viewer_pitch));
		_front.y = sin(rde_math_degrees_to_radians(model_viewer_pitch));
		_front.z = sin(rde_math_degrees_to_radians(model_viewer_yaw)) * cos(rde_math_degrees_to_radians(model_viewer_pitch));
		rde_math_normalize(&_front);

		model_viewer_camera.direction = _front;
		model_viewer_camera_front = _front;


	}
}

void model_viewer_on_event(rde_event* _event, rde_window* _window) {
	UNUSED(_window);

	if(_event->type == RDE_EVENT_TYPE_DRAG_AND_DROP_FILE) {
		if(strcmp(rde_util_file_get_name_extension(_event->data.drag_and_drop_data.file_path), "obj") != 0) {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "%s", "Only obj can be loaded at the time");
			rde_engine_show_message_box(RDE_LOG_LEVEL_ERROR, 
			                         "Wrong Format", 
			                         "Model Viewer can only render OBJ files", 
			                         current_window
			);
		} else {
			if(model_viewer_model != NULL) {
				rde_rendering_unload_model(model_viewer_model);
			}
			model_viewer_model = rde_rendering_load_obj_model(_event->data.drag_and_drop_data.file_path);
		}
	}
}

void model_viewer_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		model_viewer_camera.transform.position.x += model_viewer_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		model_viewer_camera.transform.position.y += model_viewer_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		model_viewer_camera.transform.position.z += model_viewer_camera_front.z * 10 * _dt * (_scrolled.y * 3);
	}

	model_viewer_mouse_controller(_dt);
	model_viewer_keyboard_controller(_dt);
}

void model_viewer_on_fixed_update(float _dt) {
	UNUSED(_dt)
}

void model_viewer_on_late_update(float _dt) {
	UNUSED(_dt)
}

void model_viewer_draw_3d(rde_window* _window, float _dt) {
	UNUSED(_dt)

	if(model_viewer_model != NULL) {
		rde_rendering_begin_drawing_3d(&model_viewer_camera, _window, model_viewer_draw_wireframe);
		rde_rendering_draw_model_3d(&model_viewer_transform, model_viewer_model, NULL);
		rde_rendering_end_drawing_3d();
	}
}

void model_viewer_draw_imgui() {
	ImGui::Begin("Directional Light", NULL, ImGuiWindowFlags_AlwaysAutoResize);

	float _vec[3] = { model_viewer_directional_light_direction.x, model_viewer_directional_light_direction.y, model_viewer_directional_light_direction.z };
	if(ImGui::DragFloat3("Direction", _vec, 0.25f)) {
		model_viewer_directional_light_direction.x = _vec[0];
		model_viewer_directional_light_direction.y = _vec[1];
		model_viewer_directional_light_direction.z = _vec[2];
		rde_rendering_lighting_set_directional_light_direction(model_viewer_directional_light_direction);
	}

	float _vec_1[3] = { model_viewer_directional_light_ambient_color.x, model_viewer_directional_light_ambient_color.y, model_viewer_directional_light_ambient_color.z };
	if(ImGui::DragFloat3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f)) {
		model_viewer_directional_light_ambient_color.x = _vec_1[0];
		model_viewer_directional_light_ambient_color.y = _vec_1[1];
		model_viewer_directional_light_ambient_color.z = _vec_1[2];
		rde_rendering_lighting_set_directional_light_ambient_color_f(model_viewer_directional_light_ambient_color);
	}

	float _vec_2[3] = { model_viewer_directional_light_diffuse_color.x, model_viewer_directional_light_diffuse_color.y, model_viewer_directional_light_diffuse_color.z };
	if(ImGui::DragFloat3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f)) {
		model_viewer_directional_light_diffuse_color.x = _vec_2[0];
		model_viewer_directional_light_diffuse_color.y = _vec_2[1];
		model_viewer_directional_light_diffuse_color.z = _vec_2[2];
		rde_rendering_lighting_set_directional_light_diffuse_color_f(model_viewer_directional_light_diffuse_color);
	}

	float _vec_3[3] = { model_viewer_directional_light_specular_color.x, model_viewer_directional_light_specular_color.y, model_viewer_directional_light_specular_color.z };
	if(ImGui::DragFloat3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f)) {
		model_viewer_directional_light_specular_color.x = _vec_3[0];
		model_viewer_directional_light_specular_color.y = _vec_3[1];
		model_viewer_directional_light_specular_color.z = _vec_3[2];
		rde_rendering_lighting_set_directional_light_specular_color_f(model_viewer_directional_light_specular_color);
	}
	ImGui::End();

	ImGui::Begin("Model Transform", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	float _position[3] = { model_viewer_transform.position.x, model_viewer_transform.position.y, model_viewer_transform.position.z };
	if(ImGui::DragFloat3("Position", _position, 1.f)) {
		model_viewer_transform.position.x = _position[0];
		model_viewer_transform.position.y = _position[1];
		model_viewer_transform.position.z = _position[2];
	}

	ImGui::Separator();
	float _radians = rde_math_degrees_to_radians(model_viewer_transform.rotation.x);

	if(ImGui::SliderAngle("Rotation X", &_radians, 0, 360)) {
		model_viewer_transform.rotation.x = rde_math_radians_to_degrees(_radians);
	}

	_radians = rde_math_degrees_to_radians(model_viewer_transform.rotation.y);
	if(ImGui::SliderAngle("Rotation Y", &_radians, 0, 360)) {
		model_viewer_transform.rotation.y = rde_math_radians_to_degrees(_radians);
	}

	_radians = rde_math_degrees_to_radians(model_viewer_transform.rotation.z);
	if(ImGui::SliderAngle("Rotation Z", &_radians, 0, 360)) {
		model_viewer_transform.rotation.z = rde_math_radians_to_degrees(_radians);
	}

	ImGui::Separator();
	static bool _proportional_scale = true;

	if(!_proportional_scale) {
		float _scale[3] = { model_viewer_transform.scale.x, model_viewer_transform.scale.y, model_viewer_transform.scale.z };
		if(ImGui::DragFloat3("Scale", &model_viewer_transform.scale.x, 0.25f)) {
			model_viewer_transform.scale.x = _scale[0];
			model_viewer_transform.scale.y = _scale[1];
			model_viewer_transform.scale.z = _scale[2];
		}
	} else {
		if(ImGui::DragFloat("Scale", &model_viewer_transform.scale.x, 0.25f)) {
			model_viewer_transform.scale.x = model_viewer_transform.scale.x;
			model_viewer_transform.scale.y = model_viewer_transform.scale.x;
			model_viewer_transform.scale.z = model_viewer_transform.scale.x;
		}
	}

	ImGui::Checkbox("Proportional Scale", &_proportional_scale);
	ImGui::End();

	ImGui::Begin("Model Material", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	rde_material_light_data _light = model_viewer_model != NULL ? rde_rendering_model_get_light_data(model_viewer_model) : rde_struct_create_material_light_data();
	float _vec_4[4] = { _light.ka, _light.kd, _light.ks, _light.shininess };
	if(ImGui::DragFloat4("K Indices", _vec_4, 0.0005f, 0.0f, 1.0f)) {
		rde_material_light_data _new_light {
			.shininess = _vec_4[3],
			.ka = _vec_4[0],
			.kd = _vec_4[1],
			.ks = _vec_4[2]
		};
		if(model_viewer_model != NULL) {
			rde_rendering_model_set_light_data(model_viewer_model, _new_light);
		}
	}
	ImGui::Checkbox("Wireframe", &model_viewer_draw_wireframe);
	ImGui::End();

	ImGui::Begin("Camera Options", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat("FOV", &model_viewer_camera.fov, 1, 0, 180);

	float _near_far[2] = { model_viewer_camera.near_far.x, model_viewer_camera.near_far.y };
	if(ImGui::DragFloat2("Near/Far", _near_far, 1, 0, 10000)) {
		model_viewer_camera.near_far.x = _near_far[0];
		model_viewer_camera.near_far.y = _near_far[1];
	}
	ImGui::End();
}

void model_viewer_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	model_viewer_draw_grid(&model_viewer_camera, _window);
	model_viewer_draw_3d(_window, _dt);
	model_viewer_draw_imgui();
}

void model_viewer_unload() {
	if(model_viewer_model != NULL) {
		rde_rendering_unload_model(model_viewer_model);
	}

	model_viewer_model = NULL;

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void model_viewer_init() {
	model_viewer_camera = rde_struct_create_camera();
	model_viewer_camera.transform.position = (rde_vec_3F) { -3.0, 8.0f, 14.0f };

	model_viewer_transform = rde_struct_create_transform();

	events_callback = &model_viewer_on_event;
	update_callback = &model_viewer_on_update;
	fixed_update_callback = &model_viewer_on_fixed_update;
	late_update_callback = &model_viewer_on_late_update;
	render_callback = &model_viewer_on_render;
	unload_callback = &model_viewer_unload;

	rde_engine_show_message_box(RDE_LOG_LEVEL_INFO, 
	                         "Instructions", 
	                         "Drag and Drop a .obj file", 
	                         current_window
	);
}