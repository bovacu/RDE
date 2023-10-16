#define RDE_AUDIO_MODULE
#define RDE_RENDERING_MODULE
#define RDE_FBX_MODULE
#define RDE_OBJ_MODULE

#include "rde.h"
#include <stdio.h>
#include <math.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

#define ENGINE_DEBUG

const size_t _amount = 30000;
rde_window* _window = NULL;
rde_atlas* _duck_atlas = NULL;
rde_font* _font = NULL;
rde_texture* _duck_texture = NULL;
rde_texture* _duck_yellow = NULL;
rde_texture* _duck_red = NULL;
rde_texture* _duck_green = NULL;
rde_texture* _duck_blue = NULL;
rde_texture* _center = NULL;
rde_camera camera;
rde_transform* _transforms = NULL;
rde_texture* _memory_texture = NULL;
rde_texture* _cube_texture = NULL;
rde_mesh* _mesh;
rde_model* _test_model = NULL;

float _yaw = -90.0f;
bool _first_mouse = true;
float _pitch =  0.0f;
float _last_x =  1280.f * 0.5f;
float _last_y =  720.f * 0.5f;
rde_vec_3F _camera_front = { 0.2f, 0.7f, 0.5f };
rde_vec_3F _camera_up = { 0.0, 1.0f, 0.0f };

rde_vec_3F directional_light_direction = { -0.2f, -1.0f, -0.3f };
rde_vec_3F directional_light_ambient_color = { 0.2f, 0.2f, 0.2f };
rde_vec_3F directional_light_diffuse_color = { 0.5f, 0.5f, 0.5f };
rde_vec_3F directional_light_specular_color = { 1.0f, 1.0f, 1.0f };

void on_event(rde_event* _event, rde_window* _window);
void on_update(float _dt);
void on_fixed_update(float _dt);
void on_late_update(float _dt);
void on_render(float _dt, rde_window* _window);

float g_vertex_buffer_data[] = {
	// front
	-1.0, -1.0,  1.0,
	1.0, -1.0,  1.0,
	1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// top
	-1.0,  1.0,  1.0,
	1.0,  1.0,  1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
	// back
	1.0, -1.0, -1.0,
	-1.0, -1.0, -1.0,
	-1.0,  1.0, -1.0,
	1.0,  1.0, -1.0,
	// bottom
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, -1.0,  1.0,
	-1.0, -1.0,  1.0,
	// left
	-1.0, -1.0, -1.0,
	-1.0, -1.0,  1.0,
	-1.0,  1.0,  1.0,
	-1.0,  1.0, -1.0,
	// right
	1.0, -1.0,  1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	1.0,  1.0,  1.0,
};


unsigned int g_color_buffer_data[24];

unsigned int indices[] = {
	// front
	0,  1,  2,
	2,  3,  0,
	// top
	4,  5,  6,
	6,  7,  4,
	// back
	8,  9, 10,
	10, 11,  8,
	// bottom
	12, 13, 14,
	14, 15, 12,
	// left
	16, 17, 18,
	18, 19, 16,
	// right
	20, 21, 22,
	22, 23, 20,
};

float cube_texcoords[2*4*6] = {
	// front
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
};


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
	.max_number_of_models_textures = 5,
	.max_number_of_sounds = 5
};

void keyboard_controller(float _dt) {
	if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_W)) {
		camera.transform.position.x += _camera_front.x * 10 * _dt;
		camera.transform.position.y += _camera_front.y * 10 * _dt;
		camera.transform.position.z += _camera_front.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_S)) {
		camera.transform.position.x -= _camera_front.x * 10 * _dt;
		camera.transform.position.y -= _camera_front.y * 10 * _dt;
		camera.transform.position.z -= _camera_front.z * 10 * _dt;
	}

	if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_DOWN)) {
		camera.transform.position.y -= 10 * _dt;
	} else if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_UP)) {
		camera.transform.position.y += 10 * _dt;
	}

	if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_A)) {
		rde_vec_3F _cp = rde_math_cross_product(_camera_front, _camera_up);
		rde_math_normalize(&_cp);

		camera.transform.position.x -= _cp.x * 10 * _dt;
		camera.transform.position.y -= _cp.y * 10 * _dt;
		camera.transform.position.z -= _cp.z * 10 * _dt;
	} else if(rde_events_is_key_pressed(_window, RDE_KEYBOARD_KEY_D)) {
		rde_vec_3F _cp = rde_math_cross_product(_camera_front, _camera_up);
		rde_math_normalize(&_cp);

		camera.transform.position.x += _cp.x * 10 * _dt;
		camera.transform.position.y += _cp.y * 10 * _dt;
		camera.transform.position.z += _cp.z * 10 * _dt;
	}
}

void mouse_controller(float _dt) {
	(void)_dt;

	if(rde_events_is_mouse_button_just_released(_window, RDE_MOUSE_BUTTON_1)) { 
		_first_mouse = true;
	}

	if(rde_events_is_mouse_button_pressed(_window, RDE_MOUSE_BUTTON_1)) {
		rde_vec_2I _mouse_pos = rde_events_mouse_get_position(_window);
		float _x_pos = (float)_mouse_pos.x;
		float _y_pos = (float)_mouse_pos.y;

		if(_last_x == _x_pos && _last_y == _y_pos) {
			return;
		}

		if (_first_mouse) {
			_last_x = _x_pos;
			_last_y = _y_pos;
			_first_mouse = false;
		}

		float _x_offset = _x_pos - _last_x;
		float _y_offset = _y_pos - _last_y;
		_last_x = _x_pos;
		_last_y = _y_pos;

		float sensitivity = 0.1f;
		_x_offset *= sensitivity;
		_y_offset *= sensitivity;

		_yaw -= _x_offset;
		_pitch += _y_offset;

		if (_pitch > 89.0f)
		_pitch = 89.0f;
		if (_pitch < -89.0f)
		_pitch = -89.0f;

		rde_vec_3F _front;
		_front.x = cos(rde_math_degrees_to_radians(_yaw)) * cos(rde_math_degrees_to_radians(_pitch));
		_front.y = sin(rde_math_degrees_to_radians(_pitch));
		_front.z = sin(rde_math_degrees_to_radians(_yaw)) * cos(rde_math_degrees_to_radians(_pitch));
		rde_math_normalize(&_front);

		camera.direction = _front;
		_camera_front = _front;


	}
}

void on_event(rde_event* _event, rde_window* _window) {
	(void)_window;

	ImGui_ImplSDL2_ProcessEvent((SDL_Event*)_event->native_event);

	if(_event->type == RDE_EVENT_TYPE_DRAG_AND_DROP_FILE) {
		if(strcmp(rde_util_file_get_name_extension(_event->data.drag_and_drop_data.file_path), "obj") != 0) {
			rde_log_level(RDE_LOG_LEVEL_ERROR, "Only obj can be loaded at the time");
		} else {
			rde_rendering_unload_model(_test_model);
			_test_model = rde_rendering_load_obj_model(_event->data.drag_and_drop_data.file_path);
		}
	}
}

void on_update(float _dt) {
	static float _second = 1.f;

	rde_vec_2F _scrolled = rde_events_mouse_get_scrolled(_window);
	if(_scrolled.x != 0.f || _scrolled.y != 0.f) {
		camera.transform.position.x += _camera_front.x * 10 * _dt * (_scrolled.y * 3);
		camera.transform.position.y += _camera_front.y * 10 * _dt * (_scrolled.y * 3);
		camera.transform.position.z += _camera_front.z * 10 * _dt * (_scrolled.y * 3);
	}

	if(_second >= 1.f) {
		char _title[16];
		memset(_title, 0, 16);
		snprintf(_title, 16, "FPS: %d", (int)(1.f / _dt));
		rde_window_set_title(_window, _title);
		_second = 0.f;
	}

	_second += _dt;
	UNUSED(_dt)

	mouse_controller(_dt);
	keyboard_controller(_dt);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame((SDL_Window*)rde_window_get_native_sdl_window_handle(_window));
	ImGui::NewFrame();
}

void on_fixed_update(float _dt) {
	UNUSED(_dt)
}

void on_late_update(float _dt) {
	UNUSED(_dt)
}

void draw_2d(rde_window* _window, float _dt) {
	(void)_dt;

	rde_transform _t = rde_struct_create_transform();
	_t.position.x = 0;
	_t.position.y = 0;

	rde_rendering_begin_drawing_2d(&camera, _window);
	
	for (size_t _i = 0; _i < _amount; _i++) {
		// _transforms[_i].rotation.z = _angle;
		rde_rendering_draw_texture_2d(&_transforms[_i], _duck_yellow, RDE_COLOR_WHITE, NULL);
	}

	rde_transform _t_1 = rde_struct_create_transform();
	_t_1.position.x = -500;
	_t_1.position.y = 0;
	rde_rendering_draw_text_2d(&_t_1, _font, "Esta es una frase a ver como queda", RDE_COLOR_BLACK, NULL);

	rde_rendering_draw_rectangle_2d((rde_vec_2F) { -0.5f, -0.5f }, (rde_vec_2F) { 0.5f, 0.5f }, RDE_COLOR_BLUE, NULL);

	rde_rendering_draw_texture_2d(&_t, _center, RDE_COLOR_WHITE, NULL);

	//	 _t.position.x = 256;
	//	 _t.position.y = 256;
	//	 rde_rendering_draw_memory_texture_2d(&_t, _memory_texture, RDE_COLOR_WHITE, NULL);

	rde_rendering_end_drawing_2d();
}

void draw_3d(rde_window* _window, float _dt) {
	(void)_dt;

	rde_transform _t = rde_struct_create_transform();
	_t.position.x = -10;
	_t.position.y = 0;

	rde_transform _t2 = rde_struct_create_transform();
	_t2.position.x = 5;
	_t2.position.y = 0;
	_t2.position.z = -9;

	if(_test_model != NULL) {
		rde_rendering_begin_drawing_3d(&camera, _window);
		_t.position.z = -5;
		// _t.rotation.x = -90;
		//_t.position.y = sin(_angle * 0.15f);
		//	_t.rotation.y = _angle;
		//_t.rotation.z = -_angle;
		//	_t2.rotation.y = -_angle;
		//_t.rotation.y = _angle;
		// rde_rendering_draw_mesh_3d(&_t, _mesh, NULL);
		rde_rendering_draw_model_3d(&_t, _test_model, NULL);
		rde_rendering_end_drawing_3d();
	}
}

void draw_imgui() {
	ImGui::Begin("Directional Light");

	float _vec[3] = { directional_light_direction.x, directional_light_direction.y, directional_light_direction.z };
	if(ImGui::DragFloat3("Direction", _vec, 0.25f)) {
		directional_light_direction.x = _vec[0];
		directional_light_direction.y = _vec[1];
		directional_light_direction.z = _vec[2];
		rde_rendering_lighting_set_directional_light_direction(directional_light_direction);
	}

	float _vec_1[3] = { directional_light_ambient_color.x, directional_light_ambient_color.y, directional_light_ambient_color.z };
	if(ImGui::DragFloat3("Ambient Color", _vec_1, 0.005f, 0.0f, 1.0f)) {
		directional_light_ambient_color.x = _vec_1[0];
		directional_light_ambient_color.y = _vec_1[1];
		directional_light_ambient_color.z = _vec_1[2];
		rde_rendering_lighting_set_directional_light_ambient_color_f(directional_light_ambient_color);
	}

	float _vec_2[3] = { directional_light_diffuse_color.x, directional_light_diffuse_color.y, directional_light_diffuse_color.z };
	if(ImGui::DragFloat3("Diffuse Color", _vec_2, 0.005f, 0.0f, 1.0f)) {
		directional_light_diffuse_color.x = _vec_2[0];
		directional_light_diffuse_color.y = _vec_2[1];
		directional_light_diffuse_color.z = _vec_2[2];
		rde_rendering_lighting_set_directional_light_diffuse_color_f(directional_light_diffuse_color);
	}

	float _vec_3[3] = { directional_light_specular_color.x, directional_light_specular_color.y, directional_light_specular_color.z };
	if(ImGui::DragFloat3("Specular color", _vec_3, 0.005f, 0.0f, 1.0f)) {
		directional_light_specular_color.x = _vec_3[0];
		directional_light_specular_color.y = _vec_3[1];
		directional_light_specular_color.z = _vec_3[2];
		rde_rendering_lighting_set_directional_light_specular_color_f(directional_light_specular_color);
	}
	ImGui::End();
}

void on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	rde_rendering_set_background_color(RDE_COLOR_RED);

	static float _angle = 0.f;
	
	// draw_2d(_window, _dt);
	draw_3d(_window, _dt);
	

	_angle -= _dt * 15;

	if(_angle >= 360.f) { 
		_angle = 0.f;
	}

	draw_imgui();

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

	//_duck_texture = rde_rendering_load_texture("logo512.png");

	_duck_atlas = rde_rendering_load_atlas("duck_atlas/duck_atlas.png", "duck_atlas/duck_atlas.rde_atlas");

	_duck_red = rde_rendering_get_atlas_sub_texture(_duck_atlas, "duck_red");
	_duck_blue = rde_rendering_get_atlas_sub_texture(_duck_atlas, "duck_blue");
	_duck_yellow = rde_rendering_get_atlas_sub_texture(_duck_atlas, "duck_yellow");
	_duck_green = rde_rendering_get_atlas_sub_texture(_duck_atlas, "duck_green");
	_center = rde_rendering_get_atlas_sub_texture(_duck_atlas, "center");

	_font = rde_rendering_load_font("fonts/arial.png", "fonts/arial.rde_font");

	camera = rde_struct_create_camera();

	_transforms = (rde_transform*)malloc(sizeof(rde_transform) * _amount);
	for(size_t _i = 0; _i < _amount; _i++) {
		_transforms[_i] = rde_struct_create_transform();
		_transforms[_i].position = (rde_vec_3F) { (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-10, 10), (float)rde_math_get_random_int(-50, 0) };
	}

	_memory_texture = rde_rendering_create_memory_texture(64, 64, 4);
	for(int _i = 0; _i < 64; _i++) {
		for(int _j = 0; _j < 64; _j++) {
			rde_rendering_memory_texture_set_pixel(_memory_texture, (rde_vec_2I) { _j, _i }, RDE_COLOR_GREEN);
		}
	}

	rde_window_set_icon(_window, "logo.ico");

	_cube_texture = rde_rendering_load_texture("textures/cube_texture.png");

	for (int _i = 1; _i < 6; _i++)
	memcpy(&cube_texcoords[_i * 4 * 2], &cube_texcoords[0], 2 * 4 * sizeof(float));

	_mesh = rde_struct_create_memory_mesh(24, 36);
	rde_rendering_mesh_set_vertex_positions(_mesh, g_vertex_buffer_data, false);
	rde_rendering_mesh_set_vertex_texture_data(_mesh, 24, cube_texcoords, _cube_texture, false);
	rde_rendering_mesh_set_indices(_mesh, indices, false);

	 //_test_model = rde_rendering_load_obj_model("models/common-3d-test-models/data/xyzrgb_dragon.obj");
	_test_model = rde_rendering_load_obj_model("models/halloween/Assets/obj/crypt.obj");
	//_test_model = rde_rendering_load_obj_model("models/test/test_texture.obj");

	//rde_audio_init(rde_struct_create_audio_config());
	//	rde_audio_load_sound("sounds/test.wav");
	//	rde_audio_play_sound(NULL);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io = ImGui::GetIO(); (void)_io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)rde_window_get_native_sdl_window_handle(_window), rde_window_get_native_sdl_gl_context_handle(_window));
	ImGui_ImplOpenGL3_Init();
}

void end_func() {
	//rde_rendering_destroy_mesh(_mesh);
	//rde_rendering_unload_model(_test_model);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

RDE_MAIN(_window, _heap_allocs_config, _mandatory_callbacks, init_func, end_func);