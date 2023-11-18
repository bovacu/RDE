//rde_camera physics_camera;
//
//float physics_yaw = -90.0f;
//bool physics_first_mouse = true;
//float physics_pitch =  0.0f;
//float physics_last_x =  1280.f * 0.5f;
//float physics_last_y =  720.f * 0.5f;
//rde_vec_3F physics_camera_front = { -0.31f, -0.24f, -0.91f };
//rde_vec_3F physics_camera_up = { 0.0, 1.0f, 0.0f };
//
//rde_transform INITIAL_TRANSFORM;
//
//rde_transform physics_cube_body_transform;
//rde_transform physics_floor_body_transform;
//rde_model* physics_body_model;
//rde_mesh* physics_floor_mesh;
//rde_physics_3d_body* physics_cube_body;
//rde_physics_3d_body* physics_floor_body;
//
//bool physics_simulation_running = false;
//
//void physics_keyboard_controller(float _dt) {
//	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
//		physics_camera.transform.position.x += physics_camera_front.x * 10 * _dt;
//		physics_camera.transform.position.y += physics_camera_front.y * 10 * _dt;
//		physics_camera.transform.position.z += physics_camera_front.z * 10 * _dt;
//	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
//		physics_camera.transform.position.x -= physics_camera_front.x * 10 * _dt;
//		physics_camera.transform.position.y -= physics_camera_front.y * 10 * _dt;
//		physics_camera.transform.position.z -= physics_camera_front.z * 10 * _dt;
//	}
//
//	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
//		physics_camera.transform.position.y -= 10 * _dt;
//	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
//		physics_camera.transform.position.y += 10 * _dt;
//	}
//
//	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
//		rde_vec_3F _cp = rde_math_cross_product(physics_camera_front, physics_camera_up);
//		rde_math_normalize(&_cp);
//
//		physics_camera.transform.position.x -= _cp.x * 10 * _dt;
//		physics_camera.transform.position.y -= _cp.y * 10 * _dt;
//		physics_camera.transform.position.z -= _cp.z * 10 * _dt;
//	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
//		rde_vec_3F _cp = rde_math_cross_product(physics_camera_front, physics_camera_up);
//		rde_math_normalize(&_cp);
//
//		physics_camera.transform.position.x += _cp.x * 10 * _dt;
//		physics_camera.transform.position.y += _cp.y * 10 * _dt;
//		physics_camera.transform.position.z += _cp.z * 10 * _dt;
//	}
//}
//
//void physics_mouse_controller(float _dt) {
//	UNUSED(_dt)
//
//	if(rde_events_is_mouse_button_just_released(current_window, RDE_MOUSE_BUTTON_1)) { 
//		physics_first_mouse = true;
//	}
//
//	if(rde_events_is_mouse_button_pressed(current_window, RDE_MOUSE_BUTTON_1)) {
//		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
//		float _x_pos = (float)_mouse_pos.x;
//		float _y_pos = (float)_mouse_pos.y;
//
//		if(physics_last_x == _x_pos && physics_last_y == _y_pos) {
//			return;
//		}
//
//		if (physics_first_mouse) {
//			physics_last_x = _x_pos;
//			physics_last_y = _y_pos;
//			physics_first_mouse = false;
//		}
//
//		float _x_offset = _x_pos - physics_last_x;
//		float _y_offset = _y_pos - physics_last_y;
//		physics_last_x = _x_pos;
//		physics_last_y = _y_pos;
//
//		float _sensitivity = 0.1f;
//		_x_offset *= _sensitivity;
//		_y_offset *= _sensitivity;
//
//		physics_yaw -= _x_offset;
//		physics_pitch += _y_offset;
//
//		if (physics_pitch > 89.0f)
//		physics_pitch = 89.0f;
//		
//		if (physics_pitch < -89.0f)
//		physics_pitch = -89.0f;
//
//		rde_vec_3F _front;
//		_front.x = cos(rde_math_degrees_to_radians(physics_yaw)) * cos(rde_math_degrees_to_radians(physics_pitch));
//		_front.y = sin(rde_math_degrees_to_radians(physics_pitch));
//		_front.z = sin(rde_math_degrees_to_radians(physics_yaw)) * cos(rde_math_degrees_to_radians(physics_pitch));
//		rde_math_normalize(&_front);
//
//		physics_camera.direction = _front;
//		physics_camera_front = _front;
//
//
//	}
//}
//
//void physics_on_event(rde_event* _event, rde_window* _window) {
//	UNUSED(_window);
//	UNUSED(_event);
//}
//
//void physics_on_update(float _dt) {
//	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
//	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
//		physics_camera.transform.position.x += physics_camera_front.x * 10 * _dt * (_scrolled.y * 3);
//		physics_camera.transform.position.y += physics_camera_front.y * 10 * _dt * (_scrolled.y * 3);
//		physics_camera.transform.position.z += physics_camera_front.z * 10 * _dt * (_scrolled.y * 3);
//	}
//
//	physics_mouse_controller(_dt);
//	physics_keyboard_controller(_dt);
//}
//
//void physics_on_fixed_update(float _dt) {
//	UNUSED(_dt)
//}
//
//void physics_on_late_update(float _dt) {
//	UNUSED(_dt)
//}
//
//void physics_draw_3d(rde_window* _window, float _dt) {
//	UNUSED(_dt)
//	
//	rde_render_3d(_window, &physics_camera, false, {
//	              rde_rendering_3d_draw_model(&physics_cube_body_transform, physics_body_model, NULL);
//				  rde_rendering_3d_draw_mesh(&physics_floor_body_transform, physics_floor_mesh, NULL);
//	})
//}
//
//void physics_draw_imgui(float _dt, rde_window* _window) {
//	(void)_dt;
//	(void)_window;
//	
//	rde_imgui_begin("Physics Settings", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
//	if(rde_imgui_button_default("Play Simulation")) {
//		physics_simulation_running = !physics_simulation_running;
//		rde_physics_3d_body_enable(physics_cube_body, physics_simulation_running);
//		rde_physics_3d_body_enable(physics_floor_body, physics_simulation_running);
//	}
//
//	rde_imgui_separator();
//
//	rde_imgui_text("Transform");
//	float _position[3] = { INITIAL_TRANSFORM.position.x, INITIAL_TRANSFORM.position.y, INITIAL_TRANSFORM.position.z };
//	if(rde_imgui_drag_float_3("Position", _position, 1.f, 0, 0, "%.3f", 0)) {
//		INITIAL_TRANSFORM.position.x = _position[0];
//		INITIAL_TRANSFORM.position.y = _position[1];
//		INITIAL_TRANSFORM.position.z = _position[2];
//	}
//
//	rde_imgui_separator();
//	float _radians = rde_math_degrees_to_radians(INITIAL_TRANSFORM.rotation.x);
//
//	if(rde_imgui_slider_angle("Rotation X", &_radians, 0, 360, 0)) {
//		INITIAL_TRANSFORM.rotation.x = rde_math_radians_to_degrees(_radians);
//	}
//
//	_radians = rde_math_degrees_to_radians(INITIAL_TRANSFORM.rotation.y);
//	if(rde_imgui_slider_angle("Rotation Y", &_radians, 0, 360, 0)) {
//		INITIAL_TRANSFORM.rotation.y = rde_math_radians_to_degrees(_radians);
//	}
//
//	_radians = rde_math_degrees_to_radians(INITIAL_TRANSFORM.rotation.z);
//	if(rde_imgui_slider_angle("Rotation Z", &_radians, 0, 360, 0)) {
//		INITIAL_TRANSFORM.rotation.z = rde_math_radians_to_degrees(_radians);
//	}
//
//	if(rde_imgui_button_default("Restart Simulation")) {
//		physics_simulation_running = true;
//		rde_physics_3d_body_enable(physics_cube_body, physics_simulation_running);
//		rde_physics_3d_body_enable(physics_floor_body, physics_simulation_running);
//		rde_physics_3d_body_set_transform(physics_cube_body, INITIAL_TRANSFORM);
//	}
//
//	rde_imgui_end();
//}
//
//void physics_on_render(float _dt, rde_window* _window) {
//	UNUSED(_dt)
//	
//	draw_grid(&physics_camera, _window);
//	physics_draw_3d(_window, _dt);
//}
//
//void physics_unload() {
//	rde_physics_3d_body_unload(physics_floor_body);
//	rde_physics_3d_body_unload(physics_cube_body);
//
//	rde_rendering_mesh_destroy(physics_floor_mesh, false);
//	rde_rendering_model_unload(physics_body_model);
//
//	events_callback = NULL;
//	update_callback = NULL;
//	fixed_update_callback = NULL;
//	late_update_callback = NULL;
//	render_callback = NULL;
//	unload_callback = NULL;
//}
//
//void physics_init() {
//	physics_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_PERSPECTIVE);
//	physics_camera.transform.position = (rde_vec_3F) { -3.0, 8.0f, 14.0f };
//
//	physics_simulation_running = false;
//	INITIAL_TRANSFORM = rde_struct_create_transform();
//	INITIAL_TRANSFORM.position.y = 10;
//	INITIAL_TRANSFORM.rotation.x = 204;
//	INITIAL_TRANSFORM.rotation.y = 92;
//	INITIAL_TRANSFORM.rotation.z = 64;
//
//	events_callback = &physics_on_event;
//	update_callback = &physics_on_update;
//	fixed_update_callback = &physics_on_fixed_update;
//	late_update_callback = &physics_on_late_update;
//	render_callback = &physics_on_render;
//	render_imgui_callback = &physics_draw_imgui;
//	unload_callback = &physics_unload;
//
//	rde_physics_3d_shape_box_settings _settings = (rde_physics_3d_shape_box_settings) {
//		.common = (rde_physics_3d_common_shape_settings) {
//			.layer = 1,
//			.body_type = RDE_PHYSICS_3D_BODY_TYPE_DYNAMIC,
//			.active = false
//		},
//		.width = 1,
//		.height = 1,
//		.depth = 1
//	};
//
//	physics_cube_body_transform = INITIAL_TRANSFORM;
//	physics_cube_body = rde_physics_3d_body_load(RDE_PHYSICS_3D_SHAPE_TYPE_BOX, &physics_cube_body_transform, (void*)&_settings);
//	physics_body_model = rde_rendering_model_load("../../../../skate_or_dice/assets/dices/dice.obj");
//
//
//	rde_physics_3d_shape_box_settings _settings_floor = (rde_physics_3d_shape_box_settings) {
//		.common = (rde_physics_3d_common_shape_settings) {
//			.layer = 1,
//			.body_type = RDE_PHYSICS_3D_BODY_TYPE_STATIC,
//			.active = false
//		},
//		.width = 10,
//		.height = 1,
//		.depth = 10
//	};
//
//	physics_floor_body_transform = rde_struct_create_transform();
//	physics_floor_mesh = rde_rendering_mesh_create_prism((rde_vec_3F) { 10, 1, 10 }, NULL);
//	physics_floor_body = rde_physics_3d_body_load(RDE_PHYSICS_3D_SHAPE_TYPE_BOX, &physics_floor_body_transform, (void*)&_settings_floor);
//}