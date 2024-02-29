#include "rde.h"
rde_camera shadows_camera;
rde_mesh* cube_mesh;
rde_mesh* plain_mesh;
rde_model* shadows_model;
rde_transform* shadows_transform_0;
rde_transform* shadows_transform_1;

bool shadows_first_mouse = true;
float shadows_last_x =  1280.f * 0.5f;
float shadows_last_y =  720.f * 0.5f;
rde_vec_3F shadows_camera_front = { -0.31f, -0.24f, -0.91f };
rde_vec_3F shadows_camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F shadows_directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F shadows_directional_light_position = { 0.0f, 10.0f, 0.0f };
rde_vec_3F shadows_directional_light_ambient_color = { 0.2f, 0.2f, 0.2f };
rde_vec_3F shadows_directional_light_diffuse_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F shadows_directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

void shadows_on_event(rde_event* _event, rde_window* _window) {
	RDE_UNUSED(_window);
	RDE_UNUSED(_event);
}

void shadows_on_update(float _dt) {
	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(current_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		rde_vec_3F _position = rde_transform_get_position(shadows_camera.transform);
		_position.x += shadows_camera.v_front.x * 10 * _dt * (_scrolled.y * 3);
		_position.y += shadows_camera.v_front.y * 10 * _dt * (_scrolled.y * 3);
		_position.z += shadows_camera.v_front.z * 10 * _dt * (_scrolled.y * 3);
		rde_transform_set_position(shadows_camera.transform, _position);
	}

	common_mouse_controller(&shadows_camera, _dt);
	common_keyboard_controller(&shadows_camera ,_dt);
}

void shadows_on_fixed_update(float _dt) {
	RDE_UNUSED(_dt)
}

void shadows_on_late_update(float _dt) {
	RDE_UNUSED(_dt)
}

void shadows_draw_3d(rde_window* _window, float _dt) {
	RDE_UNUSED(_dt)

	rde_render_3d_with_shadows(_window, &shadows_camera, false, {
		rde_rendering_3d_draw_model_frustum_sub_meshes(_window, &shadows_camera, shadows_transform_0, shadows_model, rde_rendering_shader_get_by_name(RDE_SHADER_MESH_SHADOWS));
	})
}

void shadows_draw_imgui(float _dt, rde_window* _window) {
	(void)_dt;
	(void)_window;
	
	rde_imgui_push_id(1);
	draw_imgui_transform("0", shadows_transform_0);
	rde_imgui_pop_id();

	rde_imgui_push_id(2);
	draw_imgui_transform("1", shadows_transform_1);
	rde_imgui_pop_id();

	{
		rde_imgui_begin("Directional Lighting", NULL, RDE_IMGUI_WINDOW_FLAGS_AlwaysAutoResize);
		rde_imgui_push_id(3);
		float _vec[3] = { shadows_directional_light_position.x, shadows_directional_light_position.y, shadows_directional_light_position.z };
		if (rde_imgui_drag_float_3("Position", _vec, 0.25f, 0, 0, "%.3f", 0)) {
			shadows_directional_light_position.x = _vec[0];
			shadows_directional_light_position.y = _vec[1];
			shadows_directional_light_position.z = _vec[2];
		}
		
		float _vec_0[3] = { shadows_directional_light_direction.x, shadows_directional_light_direction.y, shadows_directional_light_direction.z };
		if (rde_imgui_drag_float_3("Direction", _vec_0, 0.25f, 0, 0, "%.3f", 0)) {
			shadows_directional_light_direction.x = _vec_0[0];
			shadows_directional_light_direction.y = _vec_0[1];
			shadows_directional_light_direction.z = _vec_0[2];
		}
	
		float _vec_1[3] = { shadows_directional_light_ambient_color.x, shadows_directional_light_ambient_color.y, shadows_directional_light_ambient_color.z };
		if (rde_imgui_drag_float_3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			shadows_directional_light_ambient_color.x = _vec_1[0];
			shadows_directional_light_ambient_color.y = _vec_1[1];
			shadows_directional_light_ambient_color.z = _vec_1[2];
		}
	
		float _vec_2[3] = { shadows_directional_light_diffuse_color.x, shadows_directional_light_diffuse_color.y, shadows_directional_light_diffuse_color.z };
		if (rde_imgui_drag_float_3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			shadows_directional_light_diffuse_color.x = _vec_2[0];
			shadows_directional_light_diffuse_color.y = _vec_2[1];
			shadows_directional_light_diffuse_color.z = _vec_2[2];
		}
	
		float _vec_3[3] = { shadows_directional_light_specular_color.x, shadows_directional_light_specular_color.y, shadows_directional_light_specular_color.z };
		if(rde_imgui_drag_float_3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f, "%.3f", 0)) {
			shadows_directional_light_specular_color.x = _vec_3[0];
			shadows_directional_light_specular_color.y = _vec_3[1];
			shadows_directional_light_specular_color.z = _vec_3[2];
		}
		
		rde_directional_light _directional_light = (rde_directional_light) {
			.direction = shadows_directional_light_direction,
			.position = shadows_directional_light_position,
			.ambient_color = shadows_directional_light_ambient_color,
			.diffuse_color = shadows_directional_light_diffuse_color,
			.specular_color = shadows_directional_light_specular_color
		};
		rde_rendering_lighting_set_directional_light(_directional_light);
		rde_imgui_pop_id();

		float _near_far[2] = { shadows_camera.near_far.x, shadows_camera.near_far.y };
		if(rde_imgui_drag_float_2("Near/Far", _near_far, 1, 0, 10000, "%.3f", 0)) {
			shadows_camera.near_far.x = _near_far[0];
			shadows_camera.near_far.y = _near_far[1];
		}

		rde_imgui_end();
	}
 }

void shadows_on_render(float _dt, rde_window* _window) {
	RDE_UNUSED(_dt)
	
	draw_grid(&shadows_camera, _window);
	shadows_draw_3d(_window, _dt);
}

void shadows_unload() {
	// if(cube_mesh != NULL) {
	// 	rde_rendering_mesh_destroy(cube_mesh, true);
	// }

	rde_rendering_model_unload(shadows_model);

	rde_transform_unload(shadows_transform_0);
	rde_transform_unload(shadows_transform_1);

	events_callback = NULL;
	update_callback = NULL;
	fixed_update_callback = NULL;
	late_update_callback = NULL;
	render_callback = NULL;
	unload_callback = NULL;
}

void shadows_init() {
	shadows_camera = rde_struct_create_camera(current_window, RDE_CAMERA_TYPE_PERSPECTIVE);
	rde_transform_set_position(shadows_camera.transform, (rde_vec_3F) { -3.0, 8.0f, 14.0f });

	shadows_transform_0 = rde_transform_load();
	shadows_transform_1 = rde_transform_load();

	rde_transform_set_position(shadows_transform_1, (rde_vec_3F) { 0, 5, 0 });

	events_callback = &shadows_on_event;
	update_callback = &shadows_on_update;
	fixed_update_callback = &shadows_on_fixed_update;
	late_update_callback = &shadows_on_late_update;
	render_callback = &shadows_on_render;
	render_imgui_callback = &shadows_draw_imgui;
	unload_callback = &shadows_unload;

	// cube_mesh = rde_rendering_mesh_create_cube(1, NULL);
	// plain_mesh = rde_rendering_mesh_create_prism((rde_vec_3F) { 15, 1, 15 }, NULL);

	shadows_model = rde_rendering_model_load_threaded("../../../../../skate_or_dice/assets/skatepark/park.obj", 12);

	// rde_rendering_lighting_set_directional_light_position((rde_vec_3F) { 0.0f, 112.0f, 38.0f });
	// shadows_directional_light_direction = rde_rendering_lighting_get_directional_light().position;
}
