#ifndef RDE_PRIVATE_STRUCTS
#define RDE_PRIVATE_STRUCTS

	struct rde_window {
		SDL_Window* sdl_window;
		SDL_GLContext sdl_gl_context;
		RDE_INPUT_STATUS_ key_states[RDE_AMOUNT_OF_KEYS];
		RDE_INPUT_STATUS_ mouse_states[RDE_AMOUNT_OF_MOUSE_BUTTONS];
		rde_vec_2I mouse_position;
		rde_vec_2F mouse_scroll;
	};
	
	#ifdef RDE_RENDERING_MODULE
	#define RDE_SHADER_MAX_NAME 256
	struct rde_shader {
		GLuint vertex_program_id;
		GLuint fragment_program_id;
		int compiled_program_id;
		char name[RDE_SHADER_MAX_NAME];
	};
	
	struct rde_texture {
		int opengl_texture_id;
		rde_vec_2UI size;
		rde_vec_2UI position; // This is only used for atlases
		int channels;
		GLenum internal_format;
		GLenum data_format;
		const char* file_path;
		rde_texture* atlas_texture;
		unsigned char* pixels;
		bool pixels_changed;
	};
	
	typedef struct {
		rde_vec_2I position;
		rde_vec_2I size;
	} rde_atlas_sub_texture_data;
	
	typedef struct {
		char* key;
		rde_texture value;
	} rde_atlas_sub_textures;
	
	typedef struct {
		rde_vec_2I advance;
		rde_vec_2I bearing;
		rde_vec_2I size;
		rde_vec_2I offset;
		rde_vec_2I metrics;
		rde_texture texture;
	} rde_font_char_info;
	
	typedef struct {
		rde_vec_3F position;
		int color;
		rde_vec_2F texture_coordinates;
	} rde_vertex_2d;
	
	typedef struct {
		rde_shader* shader;
		rde_texture texture;
		rde_vertex_2d* vertices;
		size_t amount_of_vertices;
		GLuint vertex_buffer_object;
		GLuint index_buffer_object;
		GLuint vertex_array_object;
		mat4 mvp;
		bool is_hud;
	} rde_batch_2d;
	
	typedef struct {
		rde_vec_3F position;
		uint color;
	} rde_line_vertex;
	
	typedef struct {
		rde_line_vertex* vertices;
		size_t amount_of_vertices;
		GLuint vertex_buffer_object;
		GLuint vertex_array_object;
		unsigned short thickness;
		rde_shader* shader;
	} rde_line_batch;
	
	typedef struct {
		rde_mesh* mesh;
		rde_shader* shader;
		size_t amount_of_models_per_draw;
		rde_line_batch line_batch;
		bool draw_mesh_wireframe;
	} rde_batch_3d;
	
	struct rde_atlas {
		rde_texture* texture;
		rde_atlas_sub_textures* sub_textures;
	};
	
	struct rde_font {
		rde_texture* texture;
		rde_font_char_info* chars;
	};
	
	#define RDE_MESH_NAME_MAX 512
	struct rde_mesh {
		char name[RDE_MESH_NAME_MAX];
		size_t vertex_count;
	
		float* vertex_positions;
		float* vertex_normals;
		float* vertex_texcoords;
		mat4* transforms;
	
		uint vao;
		uint vbo[4]; // 0 -> positions (static), 
		// 1 -> normals (static), 
		// 2 -> texture coords (static)
		// 3 -> transforms to render (dynamic)
		rde_material material;
	};
	
	struct rde_model {
		rde_mesh* mesh_array;
		uint mesh_array_size;
	};
	
	typedef struct {
		uint vao;
		uint vbo;
		int opengl_texture_id;
	} rde_skybox;
	
	struct rde_render_texture {
		uint opengl_framebuffer_id;
		uint opengl_renderbuffer_id;
		uint opengl_texture_id;

		rde_vec_2UI size;

		uint vao;
		uint vbo;
	};

	#endif
	
	#ifdef RDE_AUDIO_MODULE
	struct rde_sound {
		bool used;
		
		bool playing;
		bool paused;
		bool looping;
	
		size_t played_frame;
	
		ma_decoder miniaudio_decoder;
	};
	#endif
	
	#define RDE_DEFAULT_SHADERS_AMOUNT 6
	struct rde_engine {
		float delta_time;
		float fixed_delta_time;
		float fixed_time_step_accumulator;
		
		RDE_PLATFORM_TYPE_ platform_type;
		
		bool running;
		bool use_rde_2d_physics_system;
		bool supress_engine_logs;
		
		rde_end_user_mandatory_callbacks mandatory_callbacks;
		rde_event_func user_event_callback;
		
		rde_window* windows;
		
		#if IS_ANDROID()
		ANativeWindow* android_native_window;
		JNIEnv* android_env;
		#endif

	#ifdef RDE_RENDERING_MODULE
	#define RDE_SHADERS_AMOUNT 7
		rde_shader* line_shader;
		rde_shader* color_shader_2d;
		rde_shader* texture_shader_2d;
		rde_shader* text_shader_2d;
		rde_shader* framebuffer_shader;
		rde_shader* mesh_shader;
		rde_shader* skybox_shader;
		rde_shader* shaders;
	
		size_t total_amount_of_textures;
	
		rde_texture* textures;
		rde_atlas* atlases;
		rde_font* fonts;
		rde_mesh* meshes;
		rde_model* models;
	
		rde_directional_light directional_light;
		rde_point_light* point_lights[RDE_MAX_POINT_LIGHTS];
		size_t amount_of_point_lights;
		rde_spot_light* spot_lights[RDE_MAX_SPOT_LIGHTS];
		size_t amount_of_spot_lights;
	
		rde_skybox skybox;
	#endif
	
	#ifdef RDE_AUDIO_MODULE
		rde_sound* sounds;
		ma_device miniaudio_device;
		rde_sound_config device_config;
	#endif
		
		rde_event_func window_events[RDE_WIN_EVENT_COUNT];
		rde_event_func display_events[RDE_DISPLAY_EVENT_COUNT];
		rde_event_func key_events[RDE_KEY_EVENT_COUNT];
		rde_event_func mouse_events[RDE_MOUSE_EVENT_COUNT];
		rde_event_func drag_and_drop_events[RDE_DRAG_AND_DROP_EVENT_COUNT];
	
		rde_engine_heap_allocs_config heap_allocs_config;
	
	#if IS_WINDOWS()
		HANDLE console_handle;
	#endif
	};
	
	struct rde_file_handle {
		char file_path[RDE_MAX_PATH];
		SDL_RWops* sdl_handle;
		RDE_FILE_MODE_ file_mode;
		char* text_allocated;
	};

	rde_probability rde_struct_create_probability() {
		rde_probability _p;
		_p.probability_rolled = 0.f;
		_p.happened = false;
		return _p;
	}

	rde_end_user_mandatory_callbacks rde_struct_create_end_user_mandatory_callbacks() {
		rde_end_user_mandatory_callbacks _e;
		_e.on_update = NULL;
		_e.on_fixed_update = NULL;
		_e.on_late_update = NULL;
		_e.on_render = NULL;
		return _e;
	}

	rde_display_info rde_struct_create_display_info() {
		rde_display_info _d;
		_d.index = -1;
		return _d;
	}
	
	rde_event_window rde_struct_create_event_window() {
		rde_event_window _e;
		_e.position.x = -1;
		_e.position.y = -1;
		_e.size.x = -1;
		_e.size.y = -1;
		_e.display_index = -1;
		_e.minimized = false;
		_e.maximized = false;
		return _e;
	}

	rde_event_display rde_struct_create_event_display() {
		rde_event_display _e;
		_e.orientation = -1;
		_e.display_index = -1;
		return _e;
	}

	rde_event_key rde_struct_create_event_key() {
		rde_event_key _e;
		_e.key = RDE_KEYBOARD_KEY_NONE;
		_e.typed_char = '\0';
		_e.amount_of_times_pressed = -1;
		return _e;
	}

	rde_event_mouse rde_struct_create_event_mouse() {
		rde_event_mouse _e;
		_e.button = RDE_MOUSE_BUTTON_NONE;
		_e.position.x = -1;
		_e.position.y = -1;
		_e.scrolled.x = -1.f;
		_e.scrolled.y = -1.f;
		return _e;
	}

	rde_event_controller rde_struct_create_event_controller() {
		rde_event_controller _e;
		_e.controller_id = -1;
		_e.left_joystick.x = -1.f;
		_e.left_joystick.y = -1.f;
		_e.right_joystick.x = -1.f;
		_e.right_joystick.y = -1.f;
		_e.back_buttons.x = -1.f;
		_e.back_buttons.y = -1.f;
		_e.button = RDE_CONTROLLER_BUTTON_NONE;
		_e.axis = RDE_CONTROLLER_AXIS_NONE;
		return _e;
	}

	rde_event_mobile rde_struct_create_event_mobile() {
		rde_event_mobile _e;
		_e.init_touch_position.x = -1;
		_e.init_touch_position.y = -1;
		_e.end_touch_position.x = -1;
		_e.end_touch_position.y = -1;
		_e.pressure = -1.f;
		_e.finger_id = -1;
		return _e;
	}

	rde_event_drag_and_drop rde_struct_create_event_drag_and_drop() {
		rde_event_drag_and_drop _e;
		_e.window_id = 0;
		_e.file_path = NULL;
		return _e;
	}

	rde_event_data rde_struct_create_event_data() {
		rde_event_data _e;
		_e.window_event_data = rde_struct_create_event_window();
		_e.key_event_data = rde_struct_create_event_key();
		_e.mouse_event_data = rde_struct_create_event_mouse();
		_e.controller_event_data = rde_struct_create_event_controller();
		_e.mobile_event_data = rde_struct_create_event_mobile();
		_e.display_event_data = rde_struct_create_event_display();
		_e.drag_and_drop_data = rde_struct_create_event_drag_and_drop();
		return _e;
	}

	rde_camera rde_struct_create_camera(RDE_CAMERA_TYPE_ _camera_type) {
		static size_t _camera_counter = 0;
		rde_camera _c;
		_c.id = _camera_counter++;
		_c.zoom = 1.f;
		_c.fov = 45.f;
		_c.transform = rde_struct_create_transform();
		_c.camera_type = _camera_type;
		_c.enabled = true;
		_c.direction = (rde_vec_3F) { 0.0f, 0.0f, -1.0f };
		_c.up = (rde_vec_3F) { 0.0f, 1.0f, 0.0f };
		_c.near_far = (rde_vec_2F) { 0.1f, 100.f };
		return _c;
	}

	rde_color rde_struct_create_color() {
		rde_color _c;
		_c.r = 255;
		_c.g = 255;
		_c.b = 255;
		_c.a = 255;
		return _c;
	}

	rde_event rde_struct_create_event() {
		rde_event _e;
		_e.type = RDE_EVENT_TYPE_NONE;
		_e.time_stamp = 0;
		_e.window_id = 0;
		_e.handled = false;
		_e.data = rde_struct_create_event_data();
		_e.sdl_native_event = NULL;
		return _e;
	}

	rde_transform rde_struct_create_transform() {
		rde_transform _t;
		_t.position.x = 0.f;
		_t.position.y = 0.f;
		_t.position.z = 0.f;
		_t.rotation.x = 0.f;
		_t.rotation.y = 0.f;
		_t.rotation.z = 0.f;
		_t.scale.x = 1.f;
		_t.scale.y = 1.f;
		_t.scale.z = 1.f;
		_t.parent = NULL;
		return _t;
	}

#ifdef RDE_RENDERING_MODULE
	rde_material_light_data rde_struct_create_material_light_data() {
		rde_material_light_data _m;
		_m.shininess = 1.0f;
		_m.ka = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		_m.kd = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		_m.ks = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		return _m;
	}

	rde_material rde_struct_create_material() {
		rde_material _m;
		_m.map_ka = NULL;
		_m.map_kd = NULL;
		_m.map_ks = NULL;
		_m.map_bump = NULL;
		_m.render_texture = NULL;
		_m.material_light_data = rde_struct_create_material_light_data();
		return _m;
	}

	rde_directional_light rde_struct_create_directional_light() {
		rde_directional_light _d;
		_d.direction = (rde_vec_3F) { -0.2f, -1.0f, -0.3f };
		_d.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
		_d.diffuse_color = (rde_vec_3F) { 0.5f, 0.5f, 0.5f };
		_d.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		return _d;
	}

	rde_point_light rde_struct_create_point_light() {
		rde_point_light _p;
		_p.position = (rde_vec_3F) { 0.0, 0.0f, 0.0f };
		_p.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
		_p.diffuse_color = (rde_vec_3F) { 0.5f, 0.5f, 0.5f };
		_p.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		_p.constant = 1.0f;
		_p.linear = 0.09f;
		_p.quadratic = 0.032f;
		return _p;
	}

	rde_spot_light rde_struct_create_spot_light() {
		rde_spot_light _s;
		_s.position = (rde_vec_3F) { 0.0, 0.0f, 0.0f };
		_s.direction = (rde_vec_3F) { 0.0, -1.0f, 0.0f };
		_s.cut_off = 0.99999f;
		_s.outer_cut_off = 0.99999f;
		_s.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
		_s.diffuse_color = (rde_vec_3F) { 0.8f, 0.8f, 0.8f };
		_s.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
		_s.constant = 1.0f;
		_s.linear = 0.09f;
		_s.quadratic = 0.032f;
		return _s;
	}
#endif

	rde_polygon rde_struct_create_polygon() {
		rde_polygon _p;
		_p.vertices = NULL;
		_p.vertices_count = 0;
		return _p;
	}

#ifdef RDE_AUDIO_MODULE
	rde_sound_config rde_struct_create_audio_config() {
		rde_sound_config _s;
		_s.user_data = NULL;
		_s.channels = 2;
		_s.rate = 48000;
		return _s;
	}
#endif

#endif