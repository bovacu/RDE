#ifndef __ANDROID__
//#define RDE_AUDIO_MODULE
#define RDE_RENDERING_MODULE
#define RDE_FBX_MODULE
#define RDE_OBJ_MODULE
#define RDE_PHYSICS_MODULE
#define RDE_IMGUI_MODULE
#endif

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

#if !RDE_IS_ANDROID()
#include "model_viewer.c"
#include "hierarchy.c"
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
	static rde_ImGuiDockNodeFlags _dockspace_flags = rde_ImGuiDockNodeFlags_None;
	_dockspace_flags &= ~rde_ImGuiDockNodeFlags_PassthruCentralNode;
	rde_ImGuiID _dockspace_id = rde_imgui_get_id("MyDockSpace");
 	rde_imgui_dockspace(_dockspace_id, (rde_ImVec2) {0, 0}, _dockspace_flags);

	if(rde_imgui_begin("Hub", NULL, rde_ImGuiWindowFlags_AlwaysAutoResize)) {

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

	rde_render_2d(_window, &hub_camera, false, {
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
	rde_imgui_begin(_name, NULL, rde_ImGuiWindowFlags_AlwaysAutoResize);
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

	hub_camera = rde_struct_create_camera(RDE_CAMERA_TYPE_ORTHOGRAPHIC);
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
