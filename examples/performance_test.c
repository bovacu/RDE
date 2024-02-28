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

rde_transform** performance_test_3d_transforms = NULL;
int performance_test_3d_amount_to_show = 1;

void performance_test_3d_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		rde_vec_3F _position = rde_transform_get_position(performance_test_3d_camera.transform);
		_position.x += performance_test_3d_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		_position.y += performance_test_3d_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		_position.z += performance_test_3d_camera_front.z * 10 * _dt * (_scrolled.y * 3);
		rde_transform_set_position(performance_test_3d_camera.transform, _position);
	}

	common_mouse_controller(&performance_test_3d_camera, _dt);
	common_keyboard_controller(&performance_test_3d_camera ,_dt);
}

void performance_test_3d_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void performance_test_3d_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void performance_test_3d_draw_3d(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)
	if(performance_test_3d_model != NULL) {
		rde_rendering_3d_begin_drawing(&performance_test_3d_camera, _window, false);
		
		for(int _i = 0; _i < performance_test_3d_amount_to_show; _i++) {
			rde_rendering_3d_draw_model(performance_test_3d_transforms[_i], performance_test_3d_model, NULL);
		}

		rde_rendering_3d_end_drawing();
	}
}

void performance_test_3d_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	rde_imgui_begin("Performance Control", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
	rde_imgui_drag_int("Amount To Render", &performance_test_3d_amount_to_show, 1, 0, performance_test_3d_MAX_3D, 0);
	performance_test_3d_amount_to_show = rde_math_clamp_int(performance_test_3d_amount_to_show, 0, performance_test_3d_MAX_3D);
	unsigned int _total_vertices = rde_rendering_model_get_vertices_count(performance_test_3d_model);
	rde_imgui_text("Total Vertices: %u", _total_vertices * performance_test_3d_amount_to_show);
	rde_imgui_text("Total Triangles: %u", (_total_vertices / 3) * performance_test_3d_amount_to_show);
	rde_imgui_end();
}

void performance_test_3d_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	performance_test_3d_draw_3d(_window, _dt);
}

void performance_test_3d_unload() {
	if(performance_test_3d_model != NULL) {
		rde_rendering_model_unload(performance_test_3d_model);
	}

	performance_test_3d_model = NULL;

	for(size_t _i = 0; _i < performance_test_3d_MAX_3D; _i++) {
		rde_transform_unload(performance_test_3d_transforms[_i]);
	}

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
	rde_transform_set_position(performance_test_3d_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

	update_callback = &performance_test_3d_on_update;
	fixed_update_callback = &performance_test_3d_on_fixed_update;
	late_update_callback = &performance_test_3d_on_late_update;
	render_callback = &performance_test_3d_on_render;
	render_imgui_callback = &performance_test_3d_draw_imgui;
	unload_callback = &performance_test_3d_unload;

	performance_test_3d_transforms = (rde_transform**)malloc(sizeof(rde_transform*) * performance_test_3d_MAX_3D);
	for(size_t _i = 0; _i < performance_test_3d_MAX_3D; _i++) {
		performance_test_3d_transforms[_i] = rde_transform_load();
		rde_transform_set_position(performance_test_3d_transforms[_i], (rde_vec_3F) { (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-50, 0) });
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
