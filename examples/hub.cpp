#define RDE_AUDIO_MODULE
#define RDE_RENDERING_MODULE
#define RDE_FBX_MODULE
#define RDE_OBJ_MODULE

/**
 * NOTES:
 * 		- The examples suite is compiled as CPP just because of ImGui. I know there are bindings, but original lib
 * 		  is always more reliable.
 */

#include "rde.h"
#include <math.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

typedef void (*unload_func)();

rde_event_func events_callback = NULL;
rde_engine_user_side_loop_func update_callback = NULL;
rde_engine_user_side_loop_func fixed_update_callback = NULL;
rde_engine_user_side_loop_func late_update_callback = NULL;
rde_engine_user_side_loop_func_2 render_callback = NULL;
rde_engine_user_side_loop_func_2 render_imgui_callback = NULL;
unload_func unload_callback = NULL;

rde_window* current_window = NULL;

void model_viewer_draw_grid(rde_camera* _camera, rde_window* _window) {
	const size_t _line_thickness = 1;
	rde_rendering_3d_begin_drawing(_camera, _window, false);
	rde_rendering_3d_draw_line((rde_vec_3F) {-1000, 0, 0}, (rde_vec_3F) {1000, 0, 0}, RDE_COLOR_RED, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, -1000, 0}, (rde_vec_3F) {0, 1000, 0}, RDE_COLOR_GREEN, _line_thickness, NULL);
	rde_rendering_3d_draw_line((rde_vec_3F) {0, 0, -1000}, (rde_vec_3F) {0, 0, 1000}, RDE_COLOR_BLUE, _line_thickness, NULL);
	rde_rendering_3d_end_drawing();
}

#include "model_viewer.cpp"
#include "performance_test.cpp"

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

const rde_engine_heap_allocs_config _heap_allocs_config = {
	.max_number_of_windows = 5,
	.max_number_of_vertices_per_batch = 60000,
	.max_number_of_shaders = 0,
	.max_number_of_textures = 10,
	.max_number_of_atlases = 10,
	.max_number_of_fonts = 10,
	.max_number_of_models = 5,
	.max_number_of_models_textures = 340,
	.max_number_of_sounds = 5
};

void on_event(rde_event* _event, rde_window* _window) {
	(void)_window;
	ImGui_ImplSDL2_ProcessEvent((SDL_Event*)_event->native_event);

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

void on_imgui_hub_menu() {
	static ImGuiDockNodeFlags _dockspace_flags = ImGuiDockNodeFlags_None;
	_dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiID _dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(_dockspace_id, ImVec2(0.0f, 0.0f), _dockspace_flags);

	if(ImGui::Begin("Hub", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Welcome to the Hub!");
		ImGui::Text("Choose any demo from the list");
		static int _option = -1;
		ImGui::NewLine();
		if(ImGui::RadioButton("Model Viewer", &_option, 0)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			model_viewer_init();
		}
		
		if(ImGui::RadioButton("Performance Test 3D", &_option, 1)) {
			if(unload_callback != NULL) {
				unload_callback();
			}
			performance_test_3d_init();
		}
		ImGui::End();
	}

}

//void dock_space(float _dt, rde_window* _window) {
//	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
//	const ImGuiViewport* viewport = ImGui::GetMainViewport();
//	ImGui::SetNextWindowPos(ImVec2 { 0, 0 });
//	rde_vec_2I _window_size = rde_window_get_window_size(current_window);
//	ImGui::SetNextWindowSize(ImVec2 { (float)_window_size.x, (float)_window_size.y });
//	ImGui::SetNextWindowViewport(viewport->ID);
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
//	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
//	ImGui::PopStyleVar(3);
//	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
//	on_imgui_hub_menu();
//	if(render_imgui_callback != NULL) {
//		render_imgui_callback(_dt, _window);
//	}
//	ImGui::End();
//}

void on_render(float _dt, rde_window* _window) {
	rde_rendering_set_background_color(RDE_COLOR_BLACK);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame((SDL_Window*)rde_window_get_native_sdl_window_handle(_window));
	ImGui::NewFrame();
	
	if(render_callback != NULL) {
		render_callback(_dt, _window);
	}

	//dock_space(_dt, _window);
	on_imgui_hub_menu();
	if(render_imgui_callback != NULL) {
		render_imgui_callback(_dt, _window);
	}

	ImGui::Render();
	ImDrawData* _draw_data = ImGui::GetDrawData();
	if(_draw_data != NULL) {
		ImGui_ImplOpenGL3_RenderDrawData(_draw_data);
	}
}

void init_func(int _argc, char** _argv) {
	(void)_argc;
	(void)_argv;

	rde_engine_set_event_user_callback(on_event);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io = ImGui::GetIO(); (void)_io;
	_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)rde_window_get_native_sdl_window_handle(current_window), 
	                             rde_window_get_native_sdl_gl_context_handle(current_window));
	ImGui_ImplOpenGL3_Init();

	// rde_window_set_icon(current_window, "logo.ico");
}

void end_func() {
	if(unload_callback != NULL) {
		unload_callback();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

RDE_MAIN(current_window, _heap_allocs_config, _mandatory_callbacks, init_func, end_func);