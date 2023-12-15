rde_camera ui_camera;

rde_camera ui_hud_camera;
rde_ui_container* ui_root_container;

float ui_yaw = -90.0f;
bool ui_first_mouse = true;
float ui_pitch =  0.0f;
float ui_last_x =  1280.f * 0.5f;
float ui_last_y =  720.f * 0.5f;
rde_vec_3F ui_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F ui_camera_up = { 0.0, 1.0f, 0.0f };

void ui_keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.x += ui_camera_front.x * 10 * _dt;
		_position.y += ui_camera_front.y * 10 * _dt;
		_position.z += ui_camera_front.z * 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.x -= ui_camera_front.x * 10 * _dt;
		_position.y -= ui_camera_front.y * 10 * _dt;
		_position.z -= ui_camera_front.z * 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.y -= 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.y += 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(ui_camera_front, ui_camera_up);
		rde_math_normalize(&_cp);
		
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.x -= _cp.x * 10 * _dt;
		_position.y -= _cp.y * 10 * _dt;
		_position.z -= _cp.z * 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(ui_camera_front, ui_camera_up);
		rde_math_normalize(&_cp);

		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.x += _cp.x * 10 * _dt;
		_position.y += _cp.y * 10 * _dt;
		_position.z += _cp.z * 10 * _dt;
		rde_engine_transform_set_position(ui_camera.transform, _position);
	}
}

void ui_mouse_controller(float _dt) {
	UNUSED(_dt)

	if(rde_events_is_mouse_button_just_released(current_window, RDE_MOUSE_BUTTON_1)) { 
		ui_first_mouse = true;
	}

	if(rde_events_is_mouse_button_pressed(current_window, RDE_MOUSE_BUTTON_1)) {
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)-_mouse_pos.y;

		if(ui_last_x == _x_pos && ui_last_y == _y_pos) {
			return;
		}

		if (ui_first_mouse) {
			ui_last_x = _x_pos;
			ui_last_y = _y_pos;
			ui_first_mouse = false;
		}

		float _x_offset = _x_pos - ui_last_x;
		float _y_offset = _y_pos - ui_last_y;
		ui_last_x = _x_pos;
		ui_last_y = _y_pos;

		float _sensitivity = 0.1f;
		_x_offset *= _sensitivity;
		_y_offset *= _sensitivity;

		ui_yaw -= _x_offset;
		ui_pitch += _y_offset;

		if (ui_pitch > 89.0f)
			ui_pitch = 89.0f;
		
		if (ui_pitch < -89.0f)
			ui_pitch = -89.0f;

		rde_vec_3F _front;
		_front.x = cos(rde_math_degrees_to_radians(ui_yaw)) * cos(rde_math_degrees_to_radians(ui_pitch));
		_front.y = sin(rde_math_degrees_to_radians(ui_pitch));
		_front.z = sin(rde_math_degrees_to_radians(ui_yaw)) * cos(rde_math_degrees_to_radians(ui_pitch));
		rde_math_normalize(&_front);

		ui_camera.direction = _front;
		ui_camera_front = _front;


	}
}

void ui_on_event(rde_event* _event, rde_window* _window) {
	
	if(ui_root_container != NULL) {
		rde_events_ui_poll(_window, _event, ui_root_container);
	}

}

void ui_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		rde_vec_3F _position = rde_engine_transform_get_position(ui_camera.transform);
		_position.x += ui_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		_position.y += ui_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		_position.z += ui_camera_front.z * 10 * _dt * (_scrolled.y * 3);
		rde_engine_transform_set_position(ui_camera.transform, _position);
	}

	ui_mouse_controller(_dt);
	ui_keyboard_controller(_dt);
}

void ui_on_fixed_update(float _dt) {
	UNUSED(_dt)
}

void ui_on_late_update(float _dt) {
	UNUSED(_dt)
}

void ui_draw_3d(rde_window* _window, float _dt) {
	UNUSED(_dt)

	rde_render_3d(_window, &ui_hud_camera, false, {
		
	})

	rde_render_ui_2d(_window, &ui_camera, {
		rde_rendering_draw_ui(ui_root_container);
		
	})
}

void ui_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
}

void ui_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	
	draw_grid(&ui_camera, _window);
	ui_draw_3d(_window, _dt);
}

void ui_unload() {
	rde_ui_container_unload_root(ui_root_container);

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void button_0_callback(rde_ui_container* _container, int _button_down) {
	UNUSED(_container)
	UNUSED(_button_down)
	rde_log_level(RDE_LOG_LEVEL_INFO, "Clicked on button 0");
}

void button_1_callback(rde_ui_container* _container, int _button_down) {
	UNUSED(_container)
	UNUSED(_button_down)
	rde_log_level(RDE_LOG_LEVEL_INFO, "Clicked on button 1");
}

void ui_init() {
	ui_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_PERSPECTIVE);
	ui_hud_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_ORTHOGRAPHIC);
	rde_engine_transform_set_position(ui_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });
	
	rde_vec_2I _screen_size = rde_window_get_window_size(current_window);
	ui_root_container = rde_ui_container_load_root((rde_vec_2UI) { _screen_size.x, _screen_size.y });

	rde_ui_container* _button_0 = rde_ui_add_button_default(ui_root_container, (rde_vec_2UI) { 170, 64 }, "Button");
	_button_0->callbacks.on_button_up = button_0_callback;

	rde_ui_container* _button_1 = rde_ui_add_button_default(ui_root_container, (rde_vec_2UI) { 170, 64 }, "Button");
	_button_1->callbacks.on_button_down = button_1_callback;
	rde_engine_transform_set_position(_button_1->transform, (rde_vec_3F) {
		250, 120, 0
	});

	events_callback = &ui_on_event;
	update_callback = &ui_on_update;
	fixed_update_callback = &ui_on_fixed_update;
	late_update_callback = &ui_on_late_update;
	render_callback = &ui_on_render;
	render_imgui_callback = &ui_draw_imgui;
	unload_callback = &ui_unload;
}