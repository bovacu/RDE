
#ifndef __ANDROID__
//#define RDE_AUDIO_MODULE
#define RDE_RENDERING_MODULE
#define RDE_FBX_MODULE
#define RDE_OBJ_MODULE
#define RDE_PHYSICS_3D_MODULE
#define RDE_IMGUI_MODULE
#endif

#include "rde.h"
#include <math.h>
#include <string.h>

#if IS_MAC()
#include <stdlib.h>
#endif

typedef void (*unload_func)();

rde_event_func events_callback = NULL;
rde_engine_user_side_loop_func update_callback = NULL;
rde_engine_user_side_loop_func fixed_update_callback = NULL;
rde_engine_user_side_loop_func late_update_callback = NULL;
rde_engine_user_side_loop_func_2 render_callback = NULL;
rde_engine_user_side_loop_func_2 render_imgui_callback = NULL;
unload_func unload_callback = NULL;

rde_window* current_window = NULL;

void draw_grid(rde_camera* _camera, rde_window* _window) {
	const size_t _line_thickness = 1;
	rde_rendering_3d_begin_drawing(_camera, _window, false);
	rde_rendering_3d_draw_line((rde_vec_3F) {-1000, 0, 0}, (rde_vec_3F) {1000, 0, 0}, RDE_COLOR_RED, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, -1000, 0}, (rde_vec_3F) {0, 1000, 0}, RDE_COLOR_GREEN, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, 0, -1000}, (rde_vec_3F) {0, 0, 1000}, RDE_COLOR_BLUE, _line_thickness, NULL);
	rde_rendering_3d_end_drawing();
}

#if !IS_ANDROID()
#include "model_viewer.c"
#include "performance_test.c"
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

#if IS_MOBILE()
const rde_engine_init_info ENGINE_INFO = {
	.heap_allocs_config = (rde_engine_heap_allocs_config) {
		.max_number_of_windows = 5,
		.max_number_of_vertices_per_batch = 60000,
		.max_number_of_shaders = 0,
		.max_number_of_textures = 10,
		.max_number_of_atlases = 10,
		.max_number_of_fonts = 10,
		.max_number_of_models = 5,
		.max_number_of_models_textures = 340
	},
	.illumination_config = (rde_illumination_config) {
		.amount_of_point_lights = 0,
		.amount_of_spot_lights = 0
	}
};
#else
const rde_engine_init_info ENGINE_INFO = {
	.heap_allocs_config = (rde_engine_heap_allocs_config) {
		.max_number_of_windows = 5,
		.max_number_of_vertices_per_batch = 60000,
		.max_number_of_shaders = 0,
		.max_number_of_textures = 10,
		.max_number_of_atlases = 10,
		.max_number_of_fonts = 10,
		.max_number_of_models = 5,
		.max_number_of_models_textures = 340
	},
	.illumination_config = (rde_illumination_config) {
		.amount_of_point_lights = 10,
		.amount_of_spot_lights = 10
	},
	.physics_3d_config = (rde_physics_3d_config) {
		.temp_allocator_bytes = 10 * 1024 * 1024,
		.max_amout_of_allowed_jobs = 2048,
		.max_amount_of_physics_barriers = 8,
		.max_amount_of_threads = 1,
		.max_amount_of_bodies = 65536,
		.max_amount_of_mutexes = 0,
		.max_amount_of_contact_constraints = 10240,
	}
};
#endif

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
		rde_window_set_title(current_window, _title);
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

#if !IS_ANDROID()
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
#else
		if(rde_imgui_radio_button("Android demo", &_option, 2)) {
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

void init_func(int _argc, char** _argv) {
	(void)_argc;
	(void)_argv;

	rde_engine_set_event_user_callback(on_event);

	rde_imgui_init(rde_window_get_native_sdl_window_handle(current_window), rde_window_get_native_sdl_gl_context_handle(current_window));
	// rde_window_set_icon(current_window, "logo.ico");
}

void end_func() {
	if(unload_callback != NULL) {
		unload_callback();
	}

	rde_imgui_shutdown();
}

RDE_MAIN(current_window, ENGINE_INFO, _mandatory_callbacks, init_func, end_func);