rde_camera performance_test_3d_camera;
rde_model* performance_test_3d_model = NULL;

#define performance_test_3d_MAX_3D 60000

float performance_test_3d_yaw = -90.0f;
bool performance_test_3d_first_mouse = true;
float performance_test_3d_pitch =  0.0f;
float performance_test_3d_last_x =  1280.f * 0.5f;
float performance_test_3d_last_y =  720.f * 0.5f;
rde_vec_3F performance_test_3d_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F performance_test_3d_camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F performance_test_3d_directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F performance_test_3d_directional_light_ambient_color = { 0.2f, 0.2f, 0.2f };
rde_vec_3F performance_test_3d_directional_light_diffuse_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F performance_test_3d_directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

rde_transform* performance_test_3d_transforms = NULL;
int performance_test_3d_amount_to_show = 1;

void performance_test_3d_keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		performance_test_3d_camera.transform.position.x += performance_test_3d_camera_front.x * 10 * _dt;
		performance_test_3d_camera.transform.position.y += performance_test_3d_camera_front.y * 10 * _dt;
		performance_test_3d_camera.transform.position.z += performance_test_3d_camera_front.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		performance_test_3d_camera.transform.position.x -= performance_test_3d_camera_front.x * 10 * _dt;
		performance_test_3d_camera.transform.position.y -= performance_test_3d_camera_front.y * 10 * _dt;
		performance_test_3d_camera.transform.position.z -= performance_test_3d_camera_front.z * 10 * _dt;
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		performance_test_3d_camera.transform.position.y -= 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		performance_test_3d_camera.transform.position.y += 10 * _dt;
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(performance_test_3d_camera_front, performance_test_3d_camera_up);
		rde_math_normalize(&_cp);

		performance_test_3d_camera.transform.position.x -= _cp.x * 10 * _dt;
		performance_test_3d_camera.transform.position.y -= _cp.y * 10 * _dt;
		performance_test_3d_camera.transform.position.z -= _cp.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(performance_test_3d_camera_front, performance_test_3d_camera_up);
		rde_math_normalize(&_cp);

		performance_test_3d_camera.transform.position.x += _cp.x * 10 * _dt;
		performance_test_3d_camera.transform.position.y += _cp.y * 10 * _dt;
		performance_test_3d_camera.transform.position.z += _cp.z * 10 * _dt;
	}
}

void performance_test_3d_mouse_controller(float _dt) {
	UNUSED(_dt)

	if(rde_events_is_mouse_button_just_released(current_window, RDE_MOUSE_BUTTON_1)) { 
		performance_test_3d_first_mouse = true;
	}

	if(rde_events_is_mouse_button_pressed(current_window, RDE_MOUSE_BUTTON_1)) {
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)_mouse_pos.y;

		if(performance_test_3d_last_x == _x_pos && performance_test_3d_last_y == _y_pos) {
			return;
		}

		if (performance_test_3d_first_mouse) {
			performance_test_3d_last_x = _x_pos;
			performance_test_3d_last_y = _y_pos;
			performance_test_3d_first_mouse = false;
		}

		float _x_offset = _x_pos - performance_test_3d_last_x;
		float _y_offset = _y_pos - performance_test_3d_last_y;
		performance_test_3d_last_x = _x_pos;
		performance_test_3d_last_y = _y_pos;

		float _sensitivity = 0.1f;
		_x_offset *= _sensitivity;
		_y_offset *= _sensitivity;

		performance_test_3d_yaw -= _x_offset;
		performance_test_3d_pitch += _y_offset;

		if (performance_test_3d_pitch > 89.0f)
			performance_test_3d_pitch = 89.0f;
		
		if (performance_test_3d_pitch < -89.0f)
			performance_test_3d_pitch = -89.0f;

		rde_vec_3F _front;
		_front.x = cos(rde_math_degrees_to_radians(performance_test_3d_yaw)) * cos(rde_math_degrees_to_radians(performance_test_3d_pitch));
		_front.y = sin(rde_math_degrees_to_radians(performance_test_3d_pitch));
		_front.z = sin(rde_math_degrees_to_radians(performance_test_3d_yaw)) * cos(rde_math_degrees_to_radians(performance_test_3d_pitch));
		rde_math_normalize(&_front);

		performance_test_3d_camera.direction = _front;
		performance_test_3d_camera_front = _front;


	}
}

void performance_test_3d_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		performance_test_3d_camera.transform.position.x += performance_test_3d_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		performance_test_3d_camera.transform.position.y += performance_test_3d_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		performance_test_3d_camera.transform.position.z += performance_test_3d_camera_front.z * 10 * _dt * (_scrolled.y * 3);
	}

	performance_test_3d_mouse_controller(_dt);
	performance_test_3d_keyboard_controller(_dt);
}

void performance_test_3d_on_fixed_update(float _dt) {
	UNUSED(_dt)
}

void performance_test_3d_on_late_update(float _dt) {
	UNUSED(_dt)
}

void performance_test_3d_draw_3d(rde_window* _window, float _dt) {
	UNUSED(_dt)
	if(performance_test_3d_model != NULL) {
		rde_rendering_3d_begin_drawing(&performance_test_3d_camera, _window, false);
		
		for(int _i = 0; _i < performance_test_3d_amount_to_show; _i++) {
			rde_rendering_3d_draw_model(&performance_test_3d_transforms[_i], performance_test_3d_model, NULL);
		}

		rde_rendering_3d_end_drawing();
	}
}

void performance_test_3d_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	ImGui::Begin("Performance Control", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragInt("Amount To Render", &performance_test_3d_amount_to_show, 1, 0, performance_test_3d_MAX_3D);
	unsigned int _total_vertices = rde_rendering_model_get_vertices_count(performance_test_3d_model);
	ImGui::Text("Total Vertices: %u", _total_vertices * performance_test_3d_amount_to_show);
	ImGui::Text("Total Triangles: %u", (_total_vertices / 3) * performance_test_3d_amount_to_show);
	ImGui::End();
}

void performance_test_3d_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	performance_test_3d_draw_3d(_window, _dt);
}

void performance_test_3d_unload() {
	if(performance_test_3d_model != NULL) {
		rde_rendering_model_unload(performance_test_3d_model);
	}

	performance_test_3d_model = NULL;

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;

	free(performance_test_3d_transforms);
}

void performance_test_3d_init() {
	performance_test_3d_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_PERSPECTIVE);
	performance_test_3d_camera.transform.position = (rde_vec_3F) { -3.0, 8.0f, 14.0f };

	update_callback = &performance_test_3d_on_update;
	fixed_update_callback = &performance_test_3d_on_fixed_update;
	late_update_callback = &performance_test_3d_on_late_update;
	render_callback = &performance_test_3d_on_render;
	render_imgui_callback = &performance_test_3d_draw_imgui;
	unload_callback = &performance_test_3d_unload;

	performance_test_3d_transforms = (rde_transform*)malloc(sizeof(rde_transform) * performance_test_3d_MAX_3D);
	for(size_t _i = 0; _i < performance_test_3d_MAX_3D; _i++) {
		performance_test_3d_transforms[_i] = rde_struct_create_transform();
		performance_test_3d_transforms[_i].position = (rde_vec_3F) { (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-50, 0) };
	}

	performance_test_3d_model = rde_rendering_model_load("hub_assets/performance_test/pumpkin_orange_jackolantern.obj");
	rde_rendering_model_set_light_data(performance_test_3d_model, (rde_material_light_data) {
	                                   .shininess = 1.f,
	                                   .ka = (rde_vec_3F) { 1.0f, 1.0f, 1.0f },
	                                   .kd = (rde_vec_3F) { 1.0f, 1.0f, 1.0f },
	                                   .ks = (rde_vec_3F) { 1.0f, 1.0f, 1.0f },
	});
	performance_test_3d_amount_to_show = 1;
}