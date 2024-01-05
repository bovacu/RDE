#include "rde_imgui.h"

#include "imgui.h"
#include <SDL3/SDL.h>

#include "backends/imgui_impl_sdl3.h"

#include "backends/imgui_impl_opengl3.h"

void rde_imgui_init(void* _sdl_window, void* _opengl_context) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io = ImGui::GetIO(); (void)_io;
	_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForOpenGL((SDL_Window*)_sdl_window, (SDL_GLContext*)_opengl_context);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void rde_imgui_new_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
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
	ImGui_ImplSDL3_ProcessEvent((SDL_Event*)_sdl_event);
}

rde_ImGuiID rde_imgui_get_id(const char* _str_id) {
	return (rde_ImGuiID)ImGui::GetID(_str_id);
}

void rde_imgui_push_id(int _id) {
	ImGui::PushID(_id);
}

void rde_imgui_pop_id() {
	ImGui::PopID();
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

void rde_imgui_new_line() {
	ImGui::NewLine();
}

void rde_imgui_separator() {
	ImGui::Separator();
}

void rde_imgui_same_line(float _offset_from_start_x, float _spacing) {
	ImGui::SameLine(_offset_from_start_x, _spacing);
}

void rde_imgui_text(const char* _fmt, ...) {
	va_list _args;
  	va_start(_args, _fmt);
	ImGui::TextV(_fmt, _args);
  	va_end(_args);
}

bool rde_imgui_radio_button(const char* _label, int* _v, int _v_button) {
	return ImGui::RadioButton(_label, _v, _v_button);
}

bool rde_imgui_drag_float(const char* _label, float* _v, float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragFloat(_label, _v, _v_speed, _v_min, _v_max, _format, _flags);
}

bool rde_imgui_drag_float_2(const char* _label, float _v[2], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragFloat2(_label, _v, _v_speed, _v_min, _v_max, _format, _flags);
}

bool rde_imgui_drag_float_3(const char* _label, float _v[3], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragFloat3(_label, _v, _v_speed, _v_min, _v_max, _format, _flags);
}

bool rde_imgui_drag_float_4(const char* _label, float _v[4], float _v_speed, float _v_min, float _v_max, const char* _format, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragFloat4(_label, _v, _v_speed, _v_min, _v_max, _format, _flags);
}

bool rde_imgui_drag_int(const char* _label, int* _v, float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragInt(_label, _v, _v_speed, _v_min, _v_max, "%d", _flags);
}

bool rde_imgui_drag_int_2(const char* _label, int _v[2], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragInt2(_label, _v, _v_speed, _v_min, _v_max, "%d", _flags);
}

bool rde_imgui_drag_int_3(const char* _label, int _v[3], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragInt3(_label, _v, _v_speed, _v_min, _v_max, "%d", _flags);
}

bool rde_imgui_drag_int_4(const char* _label, int _v[4], float _v_speed, float _v_min, float _v_max, rde_ImGuiSliderFlags _flags) {
	return ImGui::DragInt4(_label, _v, _v_speed, _v_min, _v_max, "%d", _flags);
}

bool rde_imgui_slider_angle(const char* _label, float* _v_rad, float _v_degrees_min, float _v_degrees_max, rde_ImGuiSliderFlags _flags) {
	return ImGui::SliderAngle(_label, _v_rad, _v_degrees_min, _v_degrees_max, "%.0f deg", _flags);
}

bool rde_imgui_checkbox(const char* _label, bool* _v) {
	return ImGui::Checkbox(_label, _v);
}

bool rde_imgui_button_default(const char* _label) {
	return rde_imgui_button(_label, (rde_ImVec2) { 0, 0 });
}

bool rde_imgui_button(const char* _label, rde_ImVec2 _size) {
	return ImGui::Button(_label, ImVec2(_size.x, _size.y));
}

void rde_imgui_progress_bar(float _progess) {
	ImGui::ProgressBar(_progess);
}


void rde_imgui_shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
