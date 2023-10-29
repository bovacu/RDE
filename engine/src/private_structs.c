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

struct rde_render_texture {
	UNIMPLEMENTED_STRUCT()
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
	
#ifdef RDE_RENDERING_MODULE
#define RDE_SHADERS_AMOUNT 7
	rde_shader* line_shader;
	rde_shader* color_shader_2d;
	rde_shader* texture_shader_2d;
	rde_shader* text_shader_2d;
	rde_shader* frame_buffer_shader;
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

#endif

#endif