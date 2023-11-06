#include "rde_imgui.h"

#include "imgui.h"
#include "SDL.h"

#include "backends/imgui_impl_sdl2.h"

#include "backends/imgui_impl_opengl3.h"

void rde_imgui_init(void* _sdl_window, void* _opengl_context) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io = ImGui::GetIO(); (void)_io;
	_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)_sdl_window, (SDL_GLContext*)_opengl_context);
	ImGui_ImplOpenGL3_Init();
}

void rde_imgui_new_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void rde_imgui_draw() {
	ImGui::Render();
	ImDrawData* _draw_data = ImGui::GetDrawData();
	if(_draw_data != NULL) {
		ImGui_ImplOpenGL3_RenderDrawData(_draw_data);
	}
}

void rde_imgui_handle_events(void* _sdl_event) {
	ImGui_ImplSDL2_ProcessEvent((SDL_Event*)_sdl_event);
}

rde_ImGuiID rde_imgui_get_id(const char* _str_id) {
	return (rde_ImGuiID)ImGui::GetID(_str_id);
}

void rde_imgui_dockspace(rde_ImGuiID _id, rde_ImVec2 _size, rde_ImGuiDockNodeFlags _flags) {
	ImGui::DockSpace((ImGuiID)_id, ImVec2(_size.x, _size.y), (ImGuiDockNodeFlags)_flags, NULL);
}

bool rde_imgui_begin(const char* _name, bool* _open, rde_ImGuiWindowFlags _flags) {
	return ImGui::Begin(_name, _open, (ImGuiButtonFlags)_flags);
}

void rde_imgui_end() {
	ImGui::End();
}

void rde_imgui_text(const char* _fmt, ...) {
	va_list _args;
  	va_start(_args, _fmt);
  	ImGui::Text(_fmt, _args);
  	va_end(_args);
}

bool rde_imgui_radio_button(const char* _label, int* _v, int _v_button) {
	return ImGui::RadioButton(_label, _v, _v_button);
}

void rde_imgui_new_line() {
	ImGui::NewLine();
}


void rde_imgui_shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}