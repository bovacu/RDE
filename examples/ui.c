#include "rde.h"
rde_camera ui_camera;

rde_camera ui_hud_camera;
rde_ui_container* ui_root_container;

rde_vec_3F ui_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F ui_camera_up = { 0.0, 1.0f, 0.0f };

void ui_on_event(rde_event* _event, rde_window* _window) {
	if(ui_root_container != NULL) {
		rde_events_ui_poll(_window, _event, ui_root_container);
	}
}

void ui_on_update(float _dt) {
	RDE_UNUSED(_dt)
}

void ui_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void ui_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void ui_draw_ui(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)

	rde_render_ui_2d(_window, &ui_hud_camera, {
		rde_rendering_draw_ui(ui_root_container);
	})
}

void ui_draw_imgui(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	RDE_UNUSED(_window)
}

void ui_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	ui_draw_ui(_window, _dt);
}

void ui_unload() {
	rde_ui_container_unload_root(ui_root_container);

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void button_0_callback(rde_ui_container* _container, int _button_down) {
	RDE_UNUSED(_container)
	RDE_UNUSED(_button_down)
	rde_log_level(RDE_LOG_LEVEL_INFO, "Clicked on button 0");
}

void button_1_callback(rde_ui_container* _container, int _button_down) {
	RDE_UNUSED(_container)
	RDE_UNUSED(_button_down)
	rde_log_level(RDE_LOG_LEVEL_INFO, "Clicked on button 1");
}

void ui_init() {
	ui_camera = rde_struct_create_camera(current_window, RDE_CAMERA_TYPE_PERSPECTIVE);
	ui_hud_camera = rde_struct_create_camera(current_window, RDE_CAMERA_TYPE_ORTHOGRAPHIC);
	rde_transform_set_position(ui_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });
	
	rde_vec_2I _screen_size = rde_window_get_window_size(current_window);
	ui_root_container = rde_ui_container_load_root((rde_vec_2UI) { _screen_size.x, _screen_size.y });

	rde_ui_container* _button_0 = rde_ui_add_button_default(ui_root_container, (rde_vec_2UI) { 170, 64 }, "Button");
	// _button_0->dynamic_layout.offset_position = (rde_vec_2F) { -128, 70.0f };
	rde_ui_container_set_percentual_size(_button_0, (rde_vec_2F) { 0.5f, 0.5f });
	rde_ui_container_set_anchor_and_strecth(_button_0, RDE_UI_ANCHOR_MIDDLE, RDE_UI_STRETCH_HORIZONTAL_STRETCH | RDE_UI_STRETCH_VERTICAL_STRETCH);
	rde_ui_container_callbacks _callbacks = rde_struct_create_ui_container_callbacks();
	_callbacks.on_button_up = button_0_callback;
	rde_ui_container_set_callbacks(_button_0, _callbacks);

	// rde_ui_container* _button_1 = rde_ui_add_button_default(ui_root_container, (rde_vec_2UI) { 170, 64 }, "Button");
	// _button_1->callbacks.on_button_down = button_1_callback;

	events_callback = &ui_on_event;
	update_callback = &ui_on_update;
	fixed_update_callback = &ui_on_fixed_update;
	late_update_callback = &ui_on_late_update;
	render_callback = &ui_on_render;
	render_imgui_callback = &ui_draw_imgui;
	unload_callback = &ui_unload;
}
