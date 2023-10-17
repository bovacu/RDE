rde_camera model_viewer_camera;
rde_model* model_viewer_model = NULL;

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
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Only obj can be loaded at the time");
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
	static float _second = 1.f;

	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		model_viewer_camera.transform.position.x += model_viewer_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		model_viewer_camera.transform.position.y += model_viewer_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		model_viewer_camera.transform.position.z += model_viewer_camera_front.z * 10 * _dt * (_scrolled.y * 3);
	}

	if(_second >= 1.f) {
		char _title[16];
		memset(_title, 0, 16);
		snprintf(_title, 16, "FPS: %d", (int)(1.f / _dt));
		rde_window_set_title(current_window, _title);
		_second = 0.f;
	}

	_second += _dt;

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

	rde_transform _t = rde_struct_create_transform();
	_t.position.x = -10;
	_t.position.y = 0;

	rde_transform _t2 = rde_struct_create_transform();
	_t2.position.x = 5;
	_t2.position.y = 0;
	_t2.position.z = -9;

	if(model_viewer_model != NULL) {
		_t.position.z = -5;
		rde_rendering_begin_drawing_3d(&model_viewer_camera, _window);
		rde_rendering_draw_model_3d(&_t, model_viewer_model, NULL);
		rde_rendering_end_drawing_3d();
	}
}

void model_viewer_draw_imgui() {
	ImGui::Begin("Directional Light");

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
}

void model_viewer_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
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