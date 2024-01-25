rde_camera model_viewer_camera;
rde_model* model_viewer_model = NULL;
rde_transform* model_viewer_transform;

bool model_viewer_draw_wireframe = false;

float model_viewer_yaw = -90.0f;
bool model_viewer_first_mouse = true;
float model_viewer_pitch =  0.0f;
float model_viewer_last_x =  1280.f * 0.5f;
float model_viewer_last_y =  720.f * 0.5f;
rde_vec_3F model_viewer_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F model_viewer_camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F model_viewer_directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F model_viewer_directional_light_position = { 0.0f, 0.0f, 0.0f };
rde_vec_3F model_viewer_directional_light_ambient_color = { 0.2f, 0.2f, 0.2f };
rde_vec_3F model_viewer_directional_light_diffuse_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F model_viewer_directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

rde_transform* model_viewer_point_light_transform;
rde_transform* model_viewer_spot_light_transform;
rde_transform* model_viewer_second_screen_texture_transform;

rde_model_data model_viewer_model_data;
int model_viewer_max_meshes_to_render = 0;

rde_mesh* model_viewer_point_light_mesh = NULL;
rde_point_light model_viewer_point_light;

rde_mesh* model_viewer_spot_light_mesh = NULL;
rde_spot_light model_viewer_spot_light;

rde_mesh* model_viewer_second_screen_mesh = NULL;
rde_render_texture* model_viewer_second_screen_rt = NULL;
static bool model_viewer_render_model_to_plane = false;

rde_skybox_id model_viewer_skybox;

bool model_viewer_show_skybox = false;
int model_viewer_msaa_samples = -1;

void model_viewer_keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.x += model_viewer_camera_front.x * 10 * _dt;
		_position.y += model_viewer_camera_front.y * 10 * _dt;
		_position.z += model_viewer_camera_front.z * 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.x -= model_viewer_camera_front.x * 10 * _dt;
		_position.y -= model_viewer_camera_front.y * 10 * _dt;
		_position.z -= model_viewer_camera_front.z * 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.y -= 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.y += 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(model_viewer_camera_front, model_viewer_camera_up);
		rde_math_normalize(&_cp);
		
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.x -= _cp.x * 10 * _dt;
		_position.y -= _cp.y * 10 * _dt;
		_position.z -= _cp.z * 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(model_viewer_camera_front, model_viewer_camera_up);
		rde_math_normalize(&_cp);

		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.x += _cp.x * 10 * _dt;
		_position.y += _cp.y * 10 * _dt;
		_position.z += _cp.z * 10 * _dt;
		rde_transform_set_position(model_viewer_camera.transform, _position);
	}
}

void model_viewer_mouse_controller(float _dt) {
	RDE_UNUSED(_dt)

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
	RDE_UNUSED(_window);

	if(_event->type == RDE_EVENT_TYPE_DRAG_AND_DROP_FILE) {
		if(strcmp(rde_util_file_get_file_extension(_event->data.drag_and_drop_data.file_path), "obj") != 0) {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Only obj can be loaded at the time");
			rde_engine_show_message_box(RDE_LOG_LEVEL_ERROR, 
			                         "Wrong Format", 
			                         "Model Viewer can only render OBJ files", 
			                         current_window
			);
		} else {
			if(model_viewer_model != NULL) {
				rde_rendering_model_unload(model_viewer_model);
				free(model_viewer_model_data.meshes);
			}
			model_viewer_model = rde_rendering_model_load(_event->data.drag_and_drop_data.file_path);
			model_viewer_model_data = rde_rendering_model_get_data(model_viewer_model);
			model_viewer_max_meshes_to_render = model_viewer_model_data.amount_of_meshes;
		}
	}
}

void model_viewer_on_update(float _dt) {
	if(model_viewer_msaa_samples != -1) {
		rde_rendering_set_antialiasing(current_window, (RDE_ANTIALIASING_)model_viewer_msaa_samples);
		model_viewer_msaa_samples = -1;
	}

	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		rde_vec_3F _position = rde_transform_get_position(model_viewer_camera.transform);
		_position.x += model_viewer_camera_front.x * 10 * _dt * (_scrolled.y * 3);
		_position.y += model_viewer_camera_front.y * 10 * _dt * (_scrolled.y * 3);
		_position.z += model_viewer_camera_front.z * 10 * _dt * (_scrolled.y * 3);
		rde_transform_set_position(model_viewer_camera.transform, _position);
	}

	model_viewer_mouse_controller(_dt);
	model_viewer_keyboard_controller(_dt);
}

void model_viewer_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void model_viewer_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void model_viewer_draw_3d(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)

	if(model_viewer_model != NULL) {
		rde_render_3d(_window, &model_viewer_camera, false, {
			if(model_viewer_render_model_to_plane) rde_rendering_render_texture_enable(model_viewer_second_screen_rt);
			for(int _i = 0; _i < model_viewer_max_meshes_to_render; _i++) {
				rde_rendering_3d_draw_mesh(model_viewer_transform, model_viewer_model_data.meshes[_i], NULL);
			}

			if(model_viewer_point_light_mesh != NULL) {
				rde_vec_3F _position = model_viewer_point_light.position;
				rde_vec_3F _scale = rde_transform_get_scale(model_viewer_transform);
				_position.x *= _scale.x;
				_position.y *= _scale.y;
				_position.z *= _scale.z;
				rde_transform_set_position(model_viewer_point_light_transform, _position);
				rde_rendering_3d_draw_mesh(model_viewer_point_light_transform, model_viewer_point_light_mesh, NULL);
			}

			if(model_viewer_spot_light_mesh != NULL) {
				rde_vec_3F _position = model_viewer_spot_light.position;
				rde_vec_3F _scale = rde_transform_get_scale(model_viewer_transform);
				_position.x *= _scale.x;
				_position.y *= _scale.y;
				_position.z *= _scale.z;
				rde_transform_set_position(model_viewer_spot_light_transform, _position);
				rde_rendering_3d_draw_mesh(model_viewer_spot_light_transform, model_viewer_spot_light_mesh, NULL);
			}

			if(model_viewer_render_model_to_plane) rde_rendering_render_texture_disable();

			if(model_viewer_render_model_to_plane) {
				rde_vec_3F _position = rde_transform_get_position(model_viewer_second_screen_texture_transform);
				_position.y = 4;
				rde_transform_set_position(model_viewer_second_screen_texture_transform, _position);
				rde_rendering_3d_draw_mesh(model_viewer_second_screen_texture_transform ,model_viewer_second_screen_mesh, NULL);
			}
		})
	}
}

void model_viewer_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	
	{
		rde_imgui_begin("Directional Lighting", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
		rde_imgui_push_id(1);
		float _vec[3] = { model_viewer_directional_light_direction.x, model_viewer_directional_light_direction.y, model_viewer_directional_light_direction.z };
		if (rde_imgui_drag_float_3("Direction", _vec, 0.25f, 0, 0, "%.3f", 0)) {
			model_viewer_directional_light_direction.x = _vec[0];
			model_viewer_directional_light_direction.y = _vec[1];
			model_viewer_directional_light_direction.z = _vec[2];
			rde_rendering_lighting_set_directional_light_direction(model_viewer_directional_light_direction);
		}

		float _vec_0[3] = { model_viewer_directional_light_position.x, model_viewer_directional_light_position.y, model_viewer_directional_light_position.z };
		if (rde_imgui_drag_float_3("Position", _vec_0, 0.25f, 0, 0, "%.3f", 0)) {
			model_viewer_directional_light_position.x = _vec_0[0];
			model_viewer_directional_light_position.y = _vec_0[1];
			model_viewer_directional_light_position.z = _vec_0[2];
			rde_rendering_lighting_set_directional_light_position(model_viewer_directional_light_position);
		}
	
		float _vec_1[3] = { model_viewer_directional_light_ambient_color.x, model_viewer_directional_light_ambient_color.y, model_viewer_directional_light_ambient_color.z };
		if (rde_imgui_drag_float_3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_directional_light_ambient_color.x = _vec_1[0];
			model_viewer_directional_light_ambient_color.y = _vec_1[1];
			model_viewer_directional_light_ambient_color.z = _vec_1[2];
			rde_rendering_lighting_set_directional_light_ambient_color_f(model_viewer_directional_light_ambient_color);
		}
	
		float _vec_2[3] = { model_viewer_directional_light_diffuse_color.x, model_viewer_directional_light_diffuse_color.y, model_viewer_directional_light_diffuse_color.z };
		if (rde_imgui_drag_float_3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_directional_light_diffuse_color.x = _vec_2[0];
			model_viewer_directional_light_diffuse_color.y = _vec_2[1];
			model_viewer_directional_light_diffuse_color.z = _vec_2[2];
			rde_rendering_lighting_set_directional_light_diffuse_color_f(model_viewer_directional_light_diffuse_color);
		}
	
		float _vec_3[3] = { model_viewer_directional_light_specular_color.x, model_viewer_directional_light_specular_color.y, model_viewer_directional_light_specular_color.z };
		if(rde_imgui_drag_float_3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_directional_light_specular_color.x = _vec_3[0];
			model_viewer_directional_light_specular_color.y = _vec_3[1];
			model_viewer_directional_light_specular_color.z = _vec_3[2];
			rde_rendering_lighting_set_directional_light_specular_color_f(model_viewer_directional_light_specular_color);
		}
		rde_imgui_pop_id();
		rde_imgui_end();
	}

	{
		rde_imgui_begin("Point Lighting", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
		rde_imgui_push_id(2);
		float _vec[3] = { model_viewer_point_light.position.x, model_viewer_point_light.position.y, model_viewer_point_light.position.z };
		if(rde_imgui_drag_float_3("Position", _vec, 0.25f, 0, 0, "%.3f", 0)) {
			model_viewer_point_light.position.x = _vec[0];
			model_viewer_point_light.position.y = _vec[1];
			model_viewer_point_light.position.z = _vec[2];
		}

		float _vec_1[3] = { model_viewer_point_light.ambient_color.x, model_viewer_point_light.ambient_color.y, model_viewer_point_light.ambient_color.z };
		if(rde_imgui_drag_float_3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_point_light.ambient_color.x = _vec_1[0];
			model_viewer_point_light.ambient_color.y = _vec_1[1];
			model_viewer_point_light.ambient_color.z = _vec_1[2];
		}

		float _vec_2[3] = { model_viewer_point_light.diffuse_color.x, model_viewer_point_light.diffuse_color.y, model_viewer_point_light.diffuse_color.z };
		if(rde_imgui_drag_float_3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_point_light.diffuse_color.x = _vec_2[0];
			model_viewer_point_light.diffuse_color.y = _vec_2[1];
			model_viewer_point_light.diffuse_color.z = _vec_2[2];
		}

		float _vec_3[3] = { model_viewer_point_light.specular_color.x, model_viewer_point_light.specular_color.y, model_viewer_point_light.specular_color.z };
		if(rde_imgui_drag_float_3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_point_light.specular_color.x = _vec_3[0];
			model_viewer_point_light.specular_color.y = _vec_3[1];
			model_viewer_point_light.specular_color.z = _vec_3[2];
		}

		rde_imgui_drag_float("Constan", &model_viewer_point_light.constant, 0.0001f, 0.001f, 1.0f, "%.3f", 0);
		rde_imgui_drag_float("Linear", &model_viewer_point_light.linear, 0.0001f, 0.0f, 1.0f, "%.3f", 0);
		rde_imgui_drag_float("Quadratic", &model_viewer_point_light.quadratic, 0.0001f, 0.0f, 2.0f, "%.3f", 0);
		rde_imgui_pop_id();
		rde_imgui_end();
	}

	{
		rde_imgui_begin("Spot Lighting", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
		rde_imgui_push_id(3);
		float _vec[3] = { model_viewer_spot_light.position.x, model_viewer_spot_light.position.y, model_viewer_spot_light.position.z };
		if(rde_imgui_drag_float_3("Position", _vec, 0.25f, 0, 0, "%.3f", 0)) {
			model_viewer_spot_light.position.x = _vec[0];
			model_viewer_spot_light.position.y = _vec[1];
			model_viewer_spot_light.position.z = _vec[2];
		}

		float _vec_0[3] = { model_viewer_spot_light.direction.x, model_viewer_spot_light.direction.y, model_viewer_spot_light.direction.z };
		if(rde_imgui_drag_float_3("Direction", _vec_0, 0.01f, 0, 0, "%.3f", 0)) {
			model_viewer_spot_light.direction.x = _vec_0[0];
			model_viewer_spot_light.direction.y = _vec_0[1];
			model_viewer_spot_light.direction.z = _vec_0[2];
		}

		float _vec_1[3] = { model_viewer_spot_light.ambient_color.x, model_viewer_spot_light.ambient_color.y, model_viewer_spot_light.ambient_color.z };
		if(rde_imgui_drag_float_3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_spot_light.ambient_color.x = _vec_1[0];
			model_viewer_spot_light.ambient_color.y = _vec_1[1];
			model_viewer_spot_light.ambient_color.z = _vec_1[2];
		}

		float _vec_2[3] = { model_viewer_spot_light.diffuse_color.x, model_viewer_spot_light.diffuse_color.y, model_viewer_spot_light.diffuse_color.z };
		if(rde_imgui_drag_float_3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_spot_light.diffuse_color.x = _vec_2[0];
			model_viewer_spot_light.diffuse_color.y = _vec_2[1];
			model_viewer_spot_light.diffuse_color.z = _vec_2[2];
		}

		float _vec_3[3] = { model_viewer_spot_light.specular_color.x, model_viewer_spot_light.specular_color.y, model_viewer_spot_light.specular_color.z };
		if(rde_imgui_drag_float_3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			model_viewer_spot_light.specular_color.x = _vec_3[0];
			model_viewer_spot_light.specular_color.y = _vec_3[1];
			model_viewer_spot_light.specular_color.z = _vec_3[2];
		}

		rde_imgui_drag_float("Cut Off", &model_viewer_spot_light.cut_off, 1.f, 0.0f, 100.0f, "%.3f", 0);
		rde_imgui_drag_float("Outer Cut Off", &model_viewer_spot_light.outer_cut_off, 1.f, 0.0f, 100.0f, "%.3f", 0);
		rde_imgui_drag_float("Constan", &model_viewer_spot_light.constant, 0.0001f, 0.001f, 1.0f, "%.3f", 0);
		rde_imgui_drag_float("Linear", &model_viewer_spot_light.linear, 0.0001f, 0.0f, 1.0f, "%.3f", 0);
		rde_imgui_drag_float("Quadratic", &model_viewer_spot_light.quadratic, 0.0001f, 0.0f, 2.0f, "%.3f", 0);
		rde_imgui_pop_id();
		rde_imgui_end();
	}

	rde_imgui_begin("Model", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);

	rde_imgui_text("Transform");
	
	rde_vec_3F _model_position = rde_transform_get_position(model_viewer_transform);
	float _position[3] = { _model_position.x, _model_position.y, _model_position.z };
	if(rde_imgui_drag_float_3("Position", _position, 1.f, 0, 0, "%.3f", 0)) {
		rde_transform_set_position(model_viewer_transform, (rde_vec_3F) { _position[0], _position[1], _position[2] });
	}

	rde_imgui_separator();
	rde_vec_3F _model_rotation = rde_transform_get_rotation_degs(model_viewer_transform);
	float _radians = rde_math_degrees_to_radians(_model_rotation.x);

	if(rde_imgui_slider_angle("Rotation X", &_radians, 0, 360, 0)) {
		_model_rotation.x = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(model_viewer_transform, _model_rotation);
	}

	_radians = rde_math_degrees_to_radians(_model_rotation.y);
	if(rde_imgui_slider_angle("Rotation Y", &_radians, 0, 360, 0)) {
		_model_rotation.y = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(model_viewer_transform, _model_rotation);
	}

	_radians = rde_math_degrees_to_radians(_model_rotation.z);
	if(rde_imgui_slider_angle("Rotation Z", &_radians, 0, 360, 0)) {
		_model_rotation.z = rde_math_radians_to_degrees(_radians);
		rde_transform_set_rotation(model_viewer_transform, _model_rotation);
	}

	rde_imgui_separator();
	static bool _proportional_scale = true;

	if(!_proportional_scale) {
		rde_vec_3F _model_scale = rde_transform_get_scale(model_viewer_transform);
		float _scale[3] = { _model_scale.x, _model_scale.y, _model_scale.z };
		if(rde_imgui_drag_float_3("Scale", _scale, 0.25f, 0, 0, "%.3f", 0)) {
			rde_transform_set_scale(model_viewer_transform, (rde_vec_3F) { _scale[0], _scale[1], _scale[2] });
		}
	} else {
		rde_vec_3F _model_scale = rde_transform_get_scale(model_viewer_transform);
		if(rde_imgui_drag_float("Scale", &_model_scale.x, 0.25f, 0, 0, "%.3f", 0)) {
			rde_transform_set_scale(model_viewer_transform, (rde_vec_3F) { _model_scale.x, _model_scale.x, _model_scale.x });
		}
	}

	rde_imgui_checkbox("Proportional Scale", &_proportional_scale);
	rde_imgui_end();

	rde_imgui_begin("Camera Options", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
	rde_imgui_drag_float("FOV", &model_viewer_camera.fov, 1, 0, 180, "%.3f", 0);

	float _near_far[2] = { model_viewer_camera.near_far.x, model_viewer_camera.near_far.y };
	if(rde_imgui_drag_float_2("Near/Far", _near_far, 1, 0, 10000, "%.3f", 0)) {
 		model_viewer_camera.near_far.x = _near_far[0];
 		model_viewer_camera.near_far.y = _near_far[1];
	}

	rde_imgui_end();

	rde_imgui_begin("Rendering Options", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
	rde_imgui_text("Total Meshes: %zu", model_viewer_model_data.amount_of_meshes);
	unsigned int _total_vertices = model_viewer_model != NULL ? rde_rendering_model_get_vertices_count(model_viewer_model) : 0;
	rde_imgui_text("Total Triangles: %zu", _total_vertices);
	rde_imgui_drag_int("Meshes", &model_viewer_max_meshes_to_render, 1, 0, model_viewer_model_data.amount_of_meshes, 0);
	rde_imgui_checkbox("Wireframe", &model_viewer_draw_wireframe);
	rde_imgui_checkbox("Skybox", &model_viewer_show_skybox);
	rde_imgui_checkbox("Render Model To Plane", &model_viewer_render_model_to_plane);
	rde_imgui_text("MSAA");
	
	int _current_mssa_selected = (int)rde_rendering_get_current_antialiasing();
	if(rde_imgui_radio_button("None", &_current_mssa_selected, (int)RDE_ANTIALIASING_NONE)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_NONE;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X2", &_current_mssa_selected, (int)RDE_ANTIALIASING_X2)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_X2;
	}

	rde_imgui_same_line(0, -1);

	if(rde_imgui_radio_button("X4", &_current_mssa_selected, (int)RDE_ANTIALIASING_X4)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_X4;
	}

	rde_imgui_same_line(0, -1);

	if(rde_imgui_radio_button("X8", &_current_mssa_selected, (int)RDE_ANTIALIASING_X8)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_X8;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X16", &_current_mssa_selected, (int)RDE_ANTIALIASING_X16)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_X16;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X32", &_current_mssa_selected, (int)RDE_ANTIALIASING_X32)) {
		model_viewer_msaa_samples = (int)RDE_ANTIALIASING_X32;
	}
	rde_imgui_end();
 }

void model_viewer_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	
	draw_grid(&model_viewer_camera, _window);
	model_viewer_draw_3d(_window, _dt);

	if(model_viewer_show_skybox) {
		rde_rendering_3d_draw_skybox(&model_viewer_camera);
	}
}

void model_viewer_unload() {
	if(model_viewer_model != NULL) {
		rde_rendering_model_unload(model_viewer_model);
		free(model_viewer_model_data.meshes);
	}

	if(model_viewer_point_light_mesh != NULL) {
		rde_rendering_mesh_destroy(model_viewer_point_light_mesh, true);
	}

	if(model_viewer_spot_light_mesh != NULL) {
		rde_rendering_mesh_destroy(model_viewer_spot_light_mesh, true);
	}

	rde_rendering_skybox_unload(model_viewer_skybox);

	rde_transform_unload(model_viewer_transform);

	model_viewer_model = NULL;

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void model_viewer_init() {
	model_viewer_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_PERSPECTIVE);
	rde_transform_set_position(model_viewer_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

	model_viewer_transform = rde_transform_load();
	model_viewer_point_light_transform = rde_transform_load();
	model_viewer_spot_light_transform = rde_transform_load();
	model_viewer_second_screen_texture_transform = rde_transform_load();

	events_callback = &model_viewer_on_event;
	update_callback = &model_viewer_on_update;
	fixed_update_callback = &model_viewer_on_fixed_update;
	late_update_callback = &model_viewer_on_late_update;
	render_callback = &model_viewer_on_render;
	render_imgui_callback = &model_viewer_draw_imgui;
	unload_callback = &model_viewer_unload;

	model_viewer_point_light = rde_struct_create_point_light();
	rde_rendering_lighting_add_point_light(&model_viewer_point_light);

	model_viewer_spot_light = rde_struct_create_spot_light();
	rde_rendering_lighting_add_spot_light(&model_viewer_spot_light);

	rde_material _light_ball_material = rde_struct_create_material();
	_light_ball_material.material_light_data.ka = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	_light_ball_material.material_light_data.kd = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	_light_ball_material.material_light_data.ks = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	model_viewer_point_light_mesh = rde_rendering_mesh_create_sphere(0.25f, &_light_ball_material);

	rde_material _light_cube_material = rde_struct_create_material();
	_light_cube_material.material_light_data.ka = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	_light_cube_material.material_light_data.kd = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	_light_cube_material.material_light_data.ks = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	model_viewer_spot_light_mesh = rde_rendering_mesh_create_cube(0.25f, &_light_cube_material);

	model_viewer_skybox = rde_rendering_skybox_load((const char*[6]) {
		"hub_assets/skyboxes/nebulosa/right.png",
		"hub_assets/skyboxes/nebulosa/left.png",
		"hub_assets/skyboxes/nebulosa/bottom.png",
		"hub_assets/skyboxes/nebulosa/top.png",
		"hub_assets/skyboxes/nebulosa/front.png",
		"hub_assets/skyboxes/nebulosa/back.png",
	});
	rde_rendering_skybox_use(model_viewer_skybox);

	float _screen_positions[] = {
		// positions
		-1.777f,  1.0f, 5,
		-1.777f, -1.0f, 5, 
		1.777f, -1.0f,  5, 
		-1.777f,  1.0f, 5, 
		1.777f, -1.0f,  5, 
		1.777f,  1.0f,  5, 
	};

	float _screen_coords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	model_viewer_second_screen_rt = rde_rendering_render_texture_create(1280, 720);

	rde_mesh_gen_data _data = {
		.vertex_count = 6,
		.positions = _screen_positions,
		.positions_size = 6 * 3,
		.texcoords = _screen_coords,
		.texcoords_size = 6 * 2,
		.normals = NULL,
		.normals_size = 0,
		.material = rde_struct_create_material()
	};

	_data.material.render_texture = model_viewer_second_screen_rt;

	model_viewer_second_screen_mesh = rde_struct_memory_mesh_create(&_data);

	rde_engine_show_message_box(RDE_LOG_LEVEL_INFO, 
	                         "Instructions", 
	                         "Drag and Drop a .obj file", 
	                         current_window
	);
}
