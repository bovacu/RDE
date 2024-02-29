#include "rde.h"
#include <math.h>
#include <string.h>

#if RDE_IS_MAC()
#include <stdlib.h>
#endif

typedef void (*unload_func)();

rde_event_func events_callback = NULL;
rde_update_func update_callback = NULL;
rde_update_func fixed_update_callback = NULL;
rde_update_func late_update_callback = NULL;
rde_render_func render_callback = NULL;
rde_render_func render_imgui_callback = NULL;
unload_func unload_callback = NULL;

rde_window* current_window = NULL;
rde_camera hub_camera;
rde_texture* duck_texture = NULL;
rde_transform* duck_transform = NULL;

void draw_grid(rde_camera* _camera, rde_window* _window) {
	const size_t _line_thickness = 1;
	rde_rendering_3d_begin_drawing(_camera, _window, false);
	rde_rendering_3d_draw_line((rde_vec_3F) {-1000, 0, 0}, (rde_vec_3F) {1000, 0, 0}, RDE_COLOR_RED, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, -1000, 0}, (rde_vec_3F) {0, 1000, 0}, RDE_COLOR_GREEN, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, 0, -1000}, (rde_vec_3F) {0, 0, 1000}, RDE_COLOR_BLUE, _line_thickness, NULL);
	rde_rendering_3d_end_drawing();
}

void draw_imgui_transform(const char* _name, rde_transform* _transform);
void common_keyboard_controller(rde_camera* _camera, float _dt);
void common_mouse_controller(rde_camera* _camera, float _dt);

#if !RDE_IS_ANDROID()
#include "model_viewer.c"
#include "hierarchy.c"
#include "ui.c"
#include "performance_test.c"
#include "shadows.c"
// #include "physics.c"
#else
#include "android.c"
#endif

void on_event(rde_event* _event, rde_window* _window);
void on_update(float _dt);
void on_fixed_update(float _dt);
void on_late_update(float _dt);
void on_render(float _dt, rde_window* _window);

const rde_end_user_mandatory_callbacks _mandatory_callbacks = {
	&on_update,
	&on_fixed_update,
	&on_late_update,
	&on_render
};

void on_event(rde_event* _event, rde_window* _window) {
	(void)_window;
	rde_imgui_handle_events(_event->sdl_native_event);

	if(events_callback != NULL) {
		events_callback(_event, _window);
	}
}

void on_update(float _dt) {
	static float _second = 1.f;

	if(_second >= 1.f) {
		char _title[64];
		memset(_title, 0, 64);
#ifdef RDE_DEBUG
		snprintf(_title, 64, "Debug - FPS: %d", (int)(1.f / _dt));
#else
		snprintf(_title, 64, "Release - FPS: %d", (int)(1.f / _dt));
#endif
		rde_window_set_window_title(current_window, _title);
		_second = 0.f;
	}

	_second += _dt;

	if(update_callback != NULL) {
		update_callback(_dt);
	}
}

void on_fixed_update(float _dt) {
	if(fixed_update_callback != NULL) {
		fixed_update_callback(_dt);
	}
}

void on_late_update(float _dt) {
	if(late_update_callback != NULL) {
		late_update_callback(_dt);
	}
}

void on_imgui_hub_menu(float _dt) {
	static RDE_IMGUI_DOCK_NODE_FLAGS_ _dockspace_flags = RDE_IMGUI_DOCK_NODE_FLAGS_None;
	_dockspace_flags &= ~RDE_IMGUI_DOCK_NODE_FLAGS_PassthruCentralNode;
	rde_imgui_id _dockspace_id = rde_imgui_get_id("MyDockSpace");
 	rde_imgui_dockspace(_dockspace_id, (rde_vec_2F) {0, 0}, _dockspace_flags);

	if(rde_imgui_begin("Hub", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize)) {

		static int _fps = 0;
		static float _second = 0;

		_second += _dt;
		if(_second >= 1.0f) {
			_fps = (int)(1.f / _dt);
			_second = 0;
		}

		rde_imgui_text("Welcome to the Hub! FPS %d", _fps);
		rde_imgui_text("Choose any demo from the list");
		static int _option = -1;
		rde_imgui_new_line();

#if !RDE_IS_ANDROID()
		if(rde_imgui_radio_button("Model Viewer", &_option, 0)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			model_viewer_init();
		}
		
		if(rde_imgui_radio_button("Performance Test 3D", &_option, 1)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			performance_test_3d_init();
		}

		// if(rde_imgui_radio_button("Physics", &_option, 2)) {
		// 	if(unload_callback != NULL) {
		// 		unload_callback();
		// 	}
		// 	physics_init();
		// }

		if(rde_imgui_radio_button("Hierarchy", &_option, 3)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			hierarchy_init();
		}

		if(rde_imgui_radio_button("Shadows", &_option, 4)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			shadows_init();
		}

		if(rde_imgui_radio_button("UI", &_option, 5)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			ui_init();
		}
#else
		if(rde_imgui_radio_button("Android demo", &_option, 0)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			android_init();
		}
#endif
		rde_imgui_end();
	}

}

void on_render(float _dt, rde_window* _window) {
	rde_rendering_set_background_color(RDE_COLOR_BLACK);

	rde_render_2d(_window, &hub_camera, {
		rde_rendering_2d_draw_texture(duck_transform, duck_texture, RDE_COLOR_WHITE, NULL);
	})

	rde_imgui_new_frame();
	
	if(render_callback != NULL) {
		render_callback(_dt, _window);
	}

	on_imgui_hub_menu(_dt);
	if(render_imgui_callback != NULL) {
		render_imgui_callback(_dt, _window);
	}

	rde_imgui_draw();
}

void draw_imgui_transform(const char* _name, rde_transform* _transform) {
	static int id_counter = 0;
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
	
	id_counter = 0;
}

void common_keyboard_controller(rde_camera* _camera, float _dt) {
	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_W)) {
		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.x += _camera->v_front.x * 10 * _dt;
		_position.y += _camera->v_front.y * 10 * _dt;
		_position.z += _camera->v_front.z * 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_S)) {
		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.x -= _camera->v_front.x * 10 * _dt;
		_position.y -= _camera->v_front.y * 10 * _dt;
		_position.z -= _camera->v_front.z * 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_DOWN)) {
		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.y -= 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_UP)) {
		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.y += 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	}

	if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(_camera->v_front, shadows_camera_up);
		rde_math_normalize(&_cp);
		
		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.x -= _cp.x * 10 * _dt;
		_position.y -= _cp.y * 10 * _dt;
		_position.z -= _cp.z * 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	} else if(rde_events_is_key_pressed(current_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(_camera->v_front, shadows_camera_up);
		rde_math_normalize(&_cp);

		rde_vec_3F _position = rde_transform_get_position(_camera->transform);
		_position.x += _cp.x * 10 * _dt;
		_position.y += _cp.y * 10 * _dt;
		_position.z += _cp.z * 10 * _dt;
		rde_transform_set_position(_camera->transform, _position);
	}
}

void common_mouse_controller(rde_camera* _camera, float _dt) {
	RDE_UNUSED(_dt)

	if(rde_events_is_mouse_button_just_released(current_window, RDE_MOUSE_BUTTON_1)) { 
		shadows_first_mouse = true;
	}

#if !RDE_IS_MOBILE()
	if(rde_events_is_mouse_button_pressed(current_window, RDE_MOUSE_BUTTON_1)) {
#else
	if(rde_events_is_mobile_touch_pressed(current_window, 0)) {
#endif
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(current_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)-_mouse_pos.y;

		if(shadows_last_x == _x_pos && shadows_last_y == _y_pos) {
			return;
		}

		if (shadows_first_mouse) {
			shadows_last_x = _x_pos;
			shadows_last_y = _y_pos;
			shadows_first_mouse = false;
		}

		float _x_offset = _x_pos - shadows_last_x;
		float _y_offset = _y_pos - shadows_last_y;
		shadows_last_x = _x_pos;
		shadows_last_y = _y_pos;

		float _sensitivity = 0.1f;
		_x_offset *= _sensitivity;
		_y_offset *= _sensitivity;

		rde_vec_3F _shadows_camera_rotation = rde_transform_get_rotation_degs(_camera->transform);
		_shadows_camera_rotation.x += _y_offset;
		_shadows_camera_rotation.y -= _x_offset;

		if (_shadows_camera_rotation.x > 89.0f)
			_shadows_camera_rotation.x = 89.0f;
		
		if (_shadows_camera_rotation.x < -89.0f)
			_shadows_camera_rotation.x = -89.0f;

		rde_transform_set_rotation(_camera->transform, _shadows_camera_rotation);
	}
}

void init_func(int _argc, char** _argv) {
	(void)_argc;
	(void)_argv;

	rde_engine_set_event_user_callback(on_event);

#if RDE_IS_MOBILE()
	rde_imgui_init(rde_window_get_native_sdl_window_handle(current_window), rde_window_get_native_sdl_gl_context_handle(current_window), "#version 300 es");
#else
	rde_imgui_init(rde_window_get_native_sdl_window_handle(current_window), rde_window_get_native_sdl_gl_context_handle(current_window), "#version 330");
#endif

	// rde_window_set_window_icon(current_window, "logo.ico");

	hub_camera = rde_struct_create_camera(current_window, RDE_CAMERA_TYPE_ORTHOGRAPHIC);
	rde_transform_set_position(hub_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

#if RDE_IS_MOBILE()
	duck_texture = rde_rendering_texture_load("assets/duck_yellow.png", NULL);
#else
	duck_texture = rde_rendering_texture_load("hub_assets/duck_yellow.png", NULL);
#endif
	duck_transform = rde_transform_load();
}

void end_func() {
	if(unload_callback != NULL) {
		unload_callback();
	}

	rde_imgui_shutdown();
}

#if RDE_IS_ANDROID()
RDE_MAIN(current_window, "assets/config.rde_config", _mandatory_callbacks, init_func, end_func);
#else
RDE_MAIN(current_window, "hub_assets/config.rde_config", _mandatory_callbacks, init_func, end_func);
#endif
