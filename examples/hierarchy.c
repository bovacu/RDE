rde_camera hierarchy_camera;
rde_mesh* cube_mesh;
rde_transform* hierarchy_transform_0;
rde_transform* hierarchy_transform_1;
rde_transform* hierarchy_transform_2;
rde_transform* hierarchy_transform_3;
rde_transform* hierarchy_transform_4;
rde_transform* hierarchy_transform_5;

int id_counter = 0;

float hierarchy_yaw = -90.0f;
bool hierarchy_first_mouse = true;
float hierarchy_pitch =  0.0f;
float hierarchy_last_x =  1280.f * 0.5f;
float hierarchy_last_y =  720.f * 0.5f;
rde_vec_3F hierarchy_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F hierarchy_camera_up = { 0.0, 1.0f, 0.0f };

void hierarchy_on_event(rde_event* _event, rde_window* _window) {
	RDE_UNUSED(_window);
	RDE_UNUSED(_event);
}

void hierarchy_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		rde_vec_3F _position = rde_transform_get_position(hierarchy_camera.transform);
		_position.x += hierarchy_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		_position.y += hierarchy_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		_position.z += hierarchy_camera_front.z * 10 * _dt * (_scrolled.y * 3);
		rde_transform_set_position(hierarchy_camera.transform, _position);
	}

	common_mouse_controller(&hierarchy_camera, _dt);
	common_keyboard_controller(&hierarchy_camera ,_dt);
}

void hierarchy_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void hierarchy_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void hierarchy_draw_3d(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)

	rde_render_3d(_window, &hierarchy_camera, false, {
		rde_rendering_3d_draw_mesh(hierarchy_transform_0, cube_mesh, NULL);
		rde_rendering_3d_draw_mesh(hierarchy_transform_1, cube_mesh, NULL);
		rde_rendering_3d_draw_mesh(hierarchy_transform_2, cube_mesh, NULL);

		rde_rendering_3d_draw_mesh(hierarchy_transform_3, cube_mesh, NULL);
		rde_rendering_3d_draw_mesh(hierarchy_transform_4, cube_mesh, NULL);
		rde_rendering_3d_draw_mesh(hierarchy_transform_5, cube_mesh, NULL);
	})
}

void hierarchy_imgui_transform(const char* _name, rde_transform* _transform) {
	rde_imgui_begin(_name, NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
	rde_imgui_push_id(id_counter++);
	rde_vec_3F _model_position = rde_transform_get_position(_transform);
	float _position[3] = { _model_position.x, _model_position.y, _model_position.z };
	if(rde_imgui_drag_float_3("Position", _position, 1.f, 0, 0, "%.3f", 0)) {
		rde_transform_set_position(_transform, (rde_vec_3F) { _position[0], _position[1], _position[2] });
	}

	rde_imgui_separator();
	rde_vec_3F _model_rotation = rde_transform_get_rotation_degs(_transform);
	float _radians = rde_math_degrees_to_radians(_model_rotation.x);

	if(rde_imgui_slider_angle("Rotation X", &_radians, -360, 360, 0)) {
		_model_rotation.x = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(_transform, _model_rotation);
	}

	_radians = rde_math_degrees_to_radians(_model_rotation.y);
	if(rde_imgui_slider_angle("Rotation Y", &_radians, -360, 360, 0)) {
		_model_rotation.y = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(_transform, _model_rotation);
	}

	_radians = rde_math_degrees_to_radians(_model_rotation.z);
	if(rde_imgui_slider_angle("Rotation Z", &_radians, -360, 360, 0)) {
		_model_rotation.z = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(_transform, _model_rotation);
	}

	rde_vec_3F _model_scale = rde_transform_get_scale(_transform);
	float _scale[3] = { _model_scale.x, _model_scale.y, _model_scale.z };
	if(rde_imgui_drag_float_3("Scale", _scale, 0.25f, 0, 0, "%.3f", 0)) {
		rde_transform_set_scale(_transform, (rde_vec_3F) { _scale[0], _scale[1], _scale[2] });
	}
	rde_imgui_pop_id();
	rde_imgui_end();
}

void hierarchy_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	
	hierarchy_imgui_transform("0", hierarchy_transform_0);
	hierarchy_imgui_transform("1", hierarchy_transform_1);
	hierarchy_imgui_transform("2", hierarchy_transform_2);
	hierarchy_imgui_transform("3", hierarchy_transform_3);
	hierarchy_imgui_transform("4", hierarchy_transform_4);
	hierarchy_imgui_transform("5", hierarchy_transform_5);

	id_counter = 0;
 }

void hierarchy_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	
	draw_grid(&hierarchy_camera, _window);
	hierarchy_draw_3d(_window, _dt);
}

void hierarchy_unload() {
	if(cube_mesh != NULL) {
		rde_rendering_mesh_destroy(cube_mesh, true);
	}

	rde_transform_unload(hierarchy_transform_0);
	rde_transform_unload(hierarchy_transform_1);
	rde_transform_unload(hierarchy_transform_2);
	rde_transform_unload(hierarchy_transform_3);
	rde_transform_unload(hierarchy_transform_4);
	rde_transform_unload(hierarchy_transform_5);

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void hierarchy_init() {
	hierarchy_camera = rde_struct_create_camera(current_window, RDE_CAMERA_TYPE_PERSPECTIVE);
	rde_transform_set_position(hierarchy_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

	hierarchy_transform_0 = rde_transform_load();
	hierarchy_transform_1 = rde_transform_load();
	hierarchy_transform_2 = rde_transform_load();
	hierarchy_transform_3 = rde_transform_load();
	hierarchy_transform_4 = rde_transform_load();
	hierarchy_transform_5 = rde_transform_load();

	rde_transform_set_parent(hierarchy_transform_1, hierarchy_transform_0);
	rde_transform_set_parent(hierarchy_transform_2, hierarchy_transform_1);

	rde_transform_set_parent(hierarchy_transform_4, hierarchy_transform_3);
	rde_transform_set_parent(hierarchy_transform_5, hierarchy_transform_4);

	rde_transform_set_position(hierarchy_transform_0, (rde_vec_3F) { -8,  8, 0 });
	rde_transform_set_position(hierarchy_transform_1, (rde_vec_3F) {  0, -8, 0 });
	rde_transform_set_position(hierarchy_transform_2, (rde_vec_3F) {  0,  0, 8 });

	rde_transform_set_position(hierarchy_transform_3, (rde_vec_3F) {  8,  8, 0 });
	rde_transform_set_position(hierarchy_transform_4, (rde_vec_3F) {  0, -8, 0 });
	rde_transform_set_position(hierarchy_transform_5, (rde_vec_3F) {  0,  0, 8 });

	events_callback = &hierarchy_on_event;
	update_callback = &hierarchy_on_update;
	fixed_update_callback = &hierarchy_on_fixed_update;
	late_update_callback = &hierarchy_on_late_update;
	render_callback = &hierarchy_on_render;
	render_imgui_callback = &hierarchy_draw_imgui;
	unload_callback = &hierarchy_unload;

	cube_mesh = rde_rendering_mesh_create_cube(1, NULL);
}
