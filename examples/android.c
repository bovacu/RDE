#include "rde.h"
rde_camera android_camera;
rde_model* android_model = NULL;
rde_transform* android_transform;

bool android_draw_wireframe = false;

float android_yaw = -90.0f;
bool android_first_mouse = true;
float android_pitch =  0.0f;
float android_last_x =  1280.f * 0.5f;
float android_last_y =  720.f * 0.5f;
rde_vec_3F android_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F android_camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F android_directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F android_directional_light_ambient_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F android_directional_light_diffuse_color = { 1.0f, 1.0f, 1.0f };
rde_vec_3F android_directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

rde_model_data android_model_data;
int android_max_meshes_to_render = 0;

rde_mesh* android_point_light_mesh = NULL;
rde_point_light android_point_light;

rde_mesh* android_spot_light_mesh = NULL;
rde_spot_light android_spot_light;

rde_mesh* android_second_screen_mesh = NULL;
rde_render_texture* android_second_screen_rt = NULL;
static bool android_render_model_to_plane = false;

rde_skybox_id android_skybox;

bool android_show_skybox = false;
int android_msaa_samples = -1;

rde_vec_2I android_mouse_pos;

void android_keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.x += android_camera_front.x * 10 * _dt;
		_position.y += android_camera_front.y * 10 * _dt;
		_position.z += android_camera_front.z * 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.x -= android_camera_front.x * 10 * _dt;
		_position.y -= android_camera_front.y * 10 * _dt;
		_position.z -= android_camera_front.z * 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.y -= 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.y += 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(android_camera_front, android_camera_up);
		rde_math_normalize(&_cp);
		
		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.x -= _cp.x * 10 * _dt;
		_position.y -= _cp.y * 10 * _dt;
		_position.z -= _cp.z * 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(android_camera_front, android_camera_up);
		rde_math_normalize(&_cp);

		rde_vec_3F _position = rde_transform_get_position(android_camera.transform);
		_position.x += _cp.x * 10 * _dt;
		_position.y += _cp.y * 10 * _dt;
		_position.z += _cp.z * 10 * _dt;
		rde_transform_set_position(android_camera.transform, _position);
	}
}

void android_mouse_controller(float _dt) {
	RDE_UNUSED(_dt)

	if(rde_events_is_mobile_touch_just_pressed(current_window, 0)) { 
		android_first_mouse = true;
	}

	if(rde_events_is_mobile_touch_pressed(current_window, 0)) {
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)-_mouse_pos.y;

		if(android_last_x == _x_pos && android_last_y == _y_pos) {
			return;
		}

		if (android_first_mouse) {
			android_last_x = _x_pos;
			android_last_y = _y_pos;
			android_first_mouse = false;
		}

		float _x_offset = _x_pos - android_last_x;
		float _y_offset = _y_pos - android_last_y;
		android_last_x = _x_pos;
		android_last_y = _y_pos;

		float _sensitivity = 0.1f;
		_x_offset *= _sensitivity;
		_y_offset *= _sensitivity;

		android_yaw -= _x_offset;
		android_pitch += _y_offset;

		if (android_pitch > 89.0f)
		android_pitch = 89.0f;
		
		if (android_pitch < -89.0f)
		android_pitch = -89.0f;

		rde_vec_3F _front;
		_front.x = cos(rde_math_degrees_to_radians(android_yaw)) * cos(rde_math_degrees_to_radians(android_pitch));
		_front.y = sin(rde_math_degrees_to_radians(android_pitch));
		_front.z = sin(rde_math_degrees_to_radians(android_yaw)) * cos(rde_math_degrees_to_radians(android_pitch));
		rde_math_normalize(&_front);

		android_camera.direction = _front;
		android_camera_front = _front;
	}
}

void android_on_event(rde_event* _event, rde_window* _window) {
	RDE_UNUSED(_window);
	RDE_UNUSED(_event);

	if(_event->type == RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED || _event->type == RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN) {
		android_mouse_pos = _event->data.mobile_event_data.moved_touch_position;
	}
}

void android_on_update(float _dt) {
	if(android_msaa_samples != -1) {
		rde_rendering_set_antialiasing(current_window, (RDE_ANTIALIASING_)android_msaa_samples);
		android_msaa_samples = -1;
	}

	// rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	// if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
	// 	android_camera.transform->position.x += android_camera_front.x * 10 * _dt * (_scrolled.y * 3);
	// 	android_camera.transform->position.y += android_camera_front.y * 10 * _dt * (_scrolled.y * 3);
	// 	android_camera.transform->position.z += android_camera_front.z * 10 * _dt * (_scrolled.y * 3);
	// }

	android_mouse_controller(_dt);
	android_keyboard_controller(_dt);

	// if(android_model != NULL) {
	// 	android_transform->rotation.y += _dt * 25.f;
	// 	if(android_transform->rotation.y > 360.f) {
	// 		android_transform->rotation.y = 0;
	// 	}
	// }
}

void android_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void android_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void android_draw_3d(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)

	if(android_model != NULL) {
		rde_render_3d(_window, &android_camera, false, {
			for(int _i = 0; _i < android_max_meshes_to_render; _i++) {
				rde_rendering_3d_draw_mesh(android_transform, android_model_data.meshes[_i], NULL);
			}
		})
	}
}

void android_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	
	{
		rde_imgui_begin("Directional Lighting", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
		rde_imgui_push_id(1);
		float _vec[3] = { android_directional_light_direction.x, android_directional_light_direction.y, android_directional_light_direction.z };
		if (rde_imgui_drag_float_3("Direction", _vec, 0.25f, 0, 0, "%.3f", 0)) {
			android_directional_light_direction.x = _vec[0];
			android_directional_light_direction.y = _vec[1];
			android_directional_light_direction.z = _vec[2];
		}
	
		float _vec_1[3] = { android_directional_light_ambient_color.x, android_directional_light_ambient_color.y, android_directional_light_ambient_color.z };
		if (rde_imgui_drag_float_3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			android_directional_light_ambient_color.x = _vec_1[0];
			android_directional_light_ambient_color.y = _vec_1[1];
			android_directional_light_ambient_color.z = _vec_1[2];
		}
	
		float _vec_2[3] = { android_directional_light_diffuse_color.x, android_directional_light_diffuse_color.y, android_directional_light_diffuse_color.z };
		if (rde_imgui_drag_float_3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			android_directional_light_diffuse_color.x = _vec_2[0];
			android_directional_light_diffuse_color.y = _vec_2[1];
			android_directional_light_diffuse_color.z = _vec_2[2];
		}
	
		float _vec_3[3] = { android_directional_light_specular_color.x, android_directional_light_specular_color.y, android_directional_light_specular_color.z };
		if(rde_imgui_drag_float_3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			android_directional_light_specular_color.x = _vec_3[0];
			android_directional_light_specular_color.y = _vec_3[1];
			android_directional_light_specular_color.z = _vec_3[2];
		}
		
		rde_directional_light _directional_light = (rde_directional_light) {
			.direction = android_directional_light_direction,
			.position = (rde_vec_3F) {0, 0, 0},
			.ambient_color = android_directional_light_ambient_color,
			.diffuse_color =android_directional_light_diffuse_color,
			.specular_color = android_directional_light_specular_color
		};
		rde_rendering_lighting_set_directional_light(_directional_light);
		rde_imgui_pop_id();
		rde_imgui_end();
	}

	rde_imgui_begin("Model", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);

	rde_imgui_text("Transform");
	rde_vec_3F _pos = rde_transform_get_position(android_transform);
	float _position[3] = { _pos.x, _pos.y, _pos.z };
	if(rde_imgui_drag_float_3("Position", _position, 1.f, 0, 0, "%.3f", 0)) {
		rde_transform_set_position(android_transform, (rde_vec_3F) { _position[0], _position[1], _position[2] });
	}

	rde_imgui_separator();
	rde_vec_3F _rotation = rde_transform_get_rotation_degs(android_transform);
	float _radians = rde_math_degrees_to_radians(_rotation.x);

	float _rot_x = _radians;
	if(rde_imgui_slider_angle("Rotation X", &_radians, 0, 360, 0)) {
		_rot_x = rde_math_radians_to_degrees(_radians);
	}

	_radians = rde_math_degrees_to_radians(_rotation.y);
	float _rot_y = _radians;
	if(rde_imgui_slider_angle("Rotation Y", &_radians, 0, 360, 0)) {
		_rot_y = rde_math_radians_to_degrees(_radians);
	}

	_radians = rde_math_degrees_to_radians(_rotation.z);
	float _rot_z = _radians;
	if(rde_imgui_slider_angle("Rotation Z", &_radians, 0, 360, 0)) {
		_rot_z = rde_math_radians_to_degrees(_radians);
	}

	rde_transform_set_rotation(android_transform, (rde_vec_3F) { _rot_x, _rot_y, _rot_z });
	
	rde_imgui_separator();
	static bool _proportional_scale = true;

	rde_vec_3F _scl = rde_transform_get_scale(android_transform);
	float _scale[3] = { _scl.x, _scl.y, _scl.z };
	if(!_proportional_scale) {
		if(rde_imgui_drag_float_3("Scale", &_scale[0], 0.25f, 0, 0, "%.3f", 0)) {
			rde_transform_set_scale(android_transform, (rde_vec_3F) { _scale[0], _scale[1], _scale[2] });
		}
	} else {
		if(rde_imgui_drag_float("Scale", &_scale[0], 0.25f, 0, 0, "%.3f", 0)) {
			rde_transform_set_scale(android_transform, (rde_vec_3F) { _scale[0], _scale[0], _scale[0] });
		}
	}

	rde_imgui_checkbox("Proportional Scale", &_proportional_scale);
	rde_imgui_end();

	rde_imgui_begin("Camera Options", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
	rde_imgui_drag_float("FOV", &android_camera.fov, 1, 0, 180, "%.3f", 0);

	float _near_far[2] = { android_camera.near_far.x, android_camera.near_far.y };
	if(rde_imgui_drag_float_2("Near/Far", _near_far, 1, 0, 10000, "%.3f", 0)) {
		android_camera.near_far.x = _near_far[0];
		android_camera.near_far.y = _near_far[1];
	}

	rde_imgui_end();

	rde_imgui_begin("Rendering Options", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
	rde_imgui_text("Total Meshes: %zu", android_model_data.amount_of_meshes);
	unsigned int _total_vertices = android_model != NULL ? rde_rendering_model_get_vertices_count(android_model) : 0;
	rde_imgui_text("Total Triangles: %zu", _total_vertices);
	rde_imgui_drag_int("Meshes", &android_max_meshes_to_render, 1, 0, android_model_data.amount_of_meshes, 0);
	rde_imgui_checkbox("Wireframe", &android_draw_wireframe);
	rde_imgui_checkbox("Skybox", &android_show_skybox);
	rde_imgui_checkbox("Render Model To Plane", &android_render_model_to_plane);
	rde_imgui_text("MSAA");
	
	int _current_mssa_selected = (int)rde_rendering_get_current_antialiasing();
	if(rde_imgui_radio_button("None", &_current_mssa_selected, (int)RDE_ANTIALIASING_NONE)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_NONE;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X2", &_current_mssa_selected, (int)RDE_ANTIALIASING_X2)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_X2;
	}

	rde_imgui_same_line(0, -1);

	if(rde_imgui_radio_button("X4", &_current_mssa_selected, (int)RDE_ANTIALIASING_X4)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_X4;
	}

	rde_imgui_same_line(0, -1);

	if(rde_imgui_radio_button("X8", &_current_mssa_selected, (int)RDE_ANTIALIASING_X8)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_X8;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X16", &_current_mssa_selected, (int)RDE_ANTIALIASING_X16)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_X16;
	}

	rde_imgui_same_line(0, -1);
	
	if(rde_imgui_radio_button("X32", &_current_mssa_selected, (int)RDE_ANTIALIASING_X32)) {
		android_msaa_samples = (int)RDE_ANTIALIASING_X32;
	}
	rde_imgui_end();
}

void android_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	
	draw_grid(&android_camera, _window);
	android_draw_3d(_window, _dt);
}

void android_unload() {
	if(android_model != NULL) {
		rde_rendering_model_unload(android_model);
		free(android_model_data.meshes);
	}

	android_model = NULL;

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void android_init() {
	android_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_PERSPECTIVE);
	rde_transform_set_position(android_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

	android_model = rde_rendering_model_load("assets/skatepark/park.obj");
	android_model_data = rde_rendering_model_get_data(android_model);
	android_max_meshes_to_render = android_model_data.amount_of_meshes;

	android_transform = rde_transform_load();

	events_callback = &android_on_event;
	update_callback = &android_on_update;
	fixed_update_callback = &android_on_fixed_update;
	late_update_callback = &android_on_late_update;
	render_callback = &android_on_render;
	render_imgui_callback = &android_draw_imgui;
	unload_callback = &android_unload;
}
