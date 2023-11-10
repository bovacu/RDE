#ifdef RDE_RENDERING_MODULE

#define RDE_SHADOW_MAP_SIZE 1024

typedef struct {
	size_t number_of_drawcalls;
} rde_rendering_statistics;

static float FRAMEBUFFER_QUAD_DATA[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

#ifdef RDE_RENDERING_MODULE
rde_render_texture* DEFAULT_RENDER_TEXTURE = NULL;
rde_render_texture* current_render_texture = NULL;
#endif

#define RDE_CHECK_SHADER_COMPILATION_STATUS(_program_id, _compiled, _path)												\
	if(!_compiled) {																									\
		char _infolog[1024];																							\
		glGetShaderInfoLog(_program_id, 1024, NULL, _infolog);															\
		glDeleteShader(_program_id);																					\
		rde_critical_error(true, "Shader(%d) '%s' compile failed with error: \n%s \n", _program_id, _path, _infolog);	\
		return NULL;																									\
	}

static rde_camera* current_drawing_camera = NULL;
static rde_window* current_drawing_window = NULL;
static mat4 projection_matrix;
rde_rendering_statistics statistics;

void rde_inner_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch);
rde_vec_2F rde_inner_rendering_get_aspect_ratio();
void rde_inner_rendering_set_rendering_configuration(rde_window* _window);
rde_texture_parameters rde_innner_rendering_validate_texture_parameters(rde_texture_parameters* _params);
void rde_inner_rendering_flush_render_texture_3d();
void rde_inner_rendering_destroy_current_antialiasing_config();
void rde_inner_rendering_create_shadows();
void rde_inner_rendering_draw_scene_shadows(rde_window* _window, rde_camera* _camera);
void rde_inner_rendering_destroy_shadows();
void rde_inner_rendering_flush_to_default_render_texture(rde_window* _window);
void rde_inner_engine_on_render(float _dt, rde_window* _window);

rde_vec_2F rde_inner_rendering_get_aspect_ratio() {
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	bool _is_horizontal = rde_window_orientation_is_horizontal(current_drawing_window);
	float _aspect_ratio = _window_size.x / (float)_window_size.y;
	rde_vec_2F _aspect_ratios;
	_aspect_ratios.x = _is_horizontal ? 1.f : _aspect_ratio;
	_aspect_ratios.y = _is_horizontal ? _aspect_ratio : 1.f;
	return _aspect_ratios;
}

void rde_inner_rendering_set_rendering_configuration(rde_window* _window) {
#if !IS_MOBILE()
	GLint profile;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
	if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
		rde_log_level(RDE_LOG_LEVEL_INFO, "Core profile");
	} else if(profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		rde_log_level(RDE_LOG_LEVEL_INFO, "Compatibility profile");
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "Unkown OpenGL context profile");
	}
#endif

	rde_log_level(RDE_LOG_LEVEL_INFO, "OpenGL Version: %s, Vendor: %s, GPU: %s, GLSL: %s", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));

	RDE_CHECK_GL(glEnable, GL_BLEND);
	RDE_CHECK_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//RDE_CHECK_GL(glBlendFuncSeparate, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

#if !IS_MOBILE() 
	#if !IS_WASM()
		RDE_CHECK_GL(glEnable, GL_PROGRAM_POINT_SIZE);
		RDE_CHECK_GL(glEnable, GL_LINE_SMOOTH);
		RDE_CHECK_GL(glHint, GL_LINE_SMOOTH_HINT,  GL_NICEST);
	#endif
			
	#if !IS_MAC() && !IS_LINUX()
		#if !IS_WASM()
			//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		#endif
	#endif
#endif

#if !IS_MOBILE() && !IS_WASM()
	rde_file_handle* _shader_vertex_handle = rde_file_open("shaders/core/line_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _shader_fragment_handle = rde_file_open("shaders/core/line_frag.glsl", RDE_FILE_MODE_READ);
	char* _vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	char* _fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.line_shader = rde_rendering_shader_load(RDE_SHADER_LINE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/color_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/color_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/texture_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/texture_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/text_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/text_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/framebuffer_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/framebuffer_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_FRAMEBUFFER, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/mesh_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/mesh_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.mesh_shader = rde_rendering_shader_load(RDE_SHADER_MESH, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/skybox_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/skybox_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.skybox_shader = rde_rendering_shader_load(RDE_SHADER_SKYBOX, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
#else
	rde_file_handle* _shader_vertex_handle = rde_file_open("shaders/es/line_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _shader_fragment_handle = rde_file_open("shaders/es/line_frag.glsl", RDE_FILE_MODE_READ);
	char* _vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	char* _fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.line_shader = rde_rendering_shader_load(RDE_SHADER_LINE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/color_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/color_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/texture_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/texture_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/text_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/text_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/framebuffer_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/framebuffer_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_FRAMEBUFFER, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/mesh_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/mesh_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.mesh_shader = rde_rendering_shader_load(RDE_SHADER_MESH, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/skybox_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/skybox_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.skybox_shader = rde_rendering_shader_load(RDE_SHADER_SKYBOX, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
#endif
	
	rde_vec_2I _window_size = rde_window_get_window_size(_window);

	GLuint _vao, _vbo;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);
	RDE_CHECK_GL(glGenBuffers, 1, &_vbo);
	RDE_CHECK_GL(glBindVertexArray, _vao);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _vbo);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(FRAMEBUFFER_QUAD_DATA), &FRAMEBUFFER_QUAD_DATA, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	RDE_CHECK_GL(glEnableVertexAttribArray, 1);
	RDE_CHECK_GL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	DEFAULT_RENDER_TEXTURE = rde_rendering_render_texture_create(_window_size.x, _window_size.y);
	DEFAULT_RENDER_TEXTURE->vao = _vao;
	DEFAULT_RENDER_TEXTURE->vbo = _vbo;
	rde_rendering_set_antialiasing(_window, RDE_ANTIALIASING_X4);
}

void rde_inner_rendering_draw_to_framebuffer(rde_render_texture* _render_texture) {
	GLuint _framebuffer_id = _render_texture == DEFAULT_RENDER_TEXTURE ? 0 : _render_texture->opengl_framebuffer_id;
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _framebuffer_id);
	RDE_CHECK_GL(glDisable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT);

	RDE_CHECK_GL(glUseProgram, ENGINE.framebuffer_shader->compiled_program_id);

	RDE_CHECK_GL(glBindVertexArray, _render_texture->vao);
	RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glDrawArrays, GL_TRIANGLES, 0, 6);
}

void rde_inner_rendering_flush_to_default_render_texture(rde_window* _window) {
	if(ENGINE.antialiasing.samples > 0) {
		RDE_CHECK_GL(glBindFramebuffer, GL_READ_FRAMEBUFFER, ENGINE.antialiasing.frame_buffer_id);
		RDE_CHECK_GL(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, DEFAULT_RENDER_TEXTURE->opengl_framebuffer_id);
		rde_vec_2I _screen_size = rde_window_get_window_size(_window);
		RDE_CHECK_GL(glBlitFramebuffer, 0, 0, _screen_size.x, _screen_size.y, 0, 0, _screen_size.x, _screen_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	rde_inner_rendering_draw_to_framebuffer(DEFAULT_RENDER_TEXTURE);
}

void rde_inner_rendering_destroy_current_antialiasing_config() {
	if(ENGINE.antialiasing.opengl_texture_id == -1 || ENGINE.antialiasing.frame_buffer_id == RDE_UINT_MAX) {
		return;
	}

	uint _texture_id = ENGINE.antialiasing.opengl_texture_id;
	RDE_CHECK_GL(glDeleteTextures, 1, &_texture_id);
	RDE_CHECK_GL(glDeleteRenderbuffers, 1, &ENGINE.antialiasing.render_buffer_id);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &ENGINE.antialiasing.frame_buffer_id);

	ENGINE.antialiasing.opengl_texture_id = -1;
	ENGINE.antialiasing.frame_buffer_id = RDE_UINT_MAX;
	ENGINE.antialiasing.render_buffer_id = RDE_UINT_MAX;
}

void rde_inner_rendering_create_shadows() {
	glGenFramebuffers(1, &ENGINE.shadows.frame_buffer_id);
	
	uint _depth_texture;
	RDE_CHECK_GL(glGenTextures, 1, &_depth_texture);
	ENGINE.shadows.opengl_texture_id = _depth_texture;
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, ENGINE.shadows.opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RDE_SHADOW_MAP_SIZE, RDE_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.shadows.frame_buffer_id);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ENGINE.shadows.opengl_texture_id, 0);
	
#if IS_MOBILE()
	GLenum _a[1] = { GL_NONE };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#else
	RDE_CHECK_GL(glDrawBuffer, GL_NONE);
#endif

	RDE_CHECK_GL(glReadBuffer, GL_NONE);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_inner_rendering_destroy_shadows() {
	uint _depth_texture = ENGINE.shadows.opengl_texture_id;
	RDE_CHECK_GL(glDeleteTextures, 1, &_depth_texture);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &ENGINE.shadows.frame_buffer_id);

	ENGINE.shadows.opengl_texture_id = -1;
	ENGINE.shadows.frame_buffer_id = RDE_UINT_MAX;
}

void rde_inner_rendering_draw_scene_shadows(rde_window* _window, rde_camera* _camera) {
	(void)_window;
	RDE_CHECK_GL(glClearColor, 0.1f, 0.1f, 0.1f, 1.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 _light_projection;
	mat4 _light_view;
	mat4 _light_space_matrix;
	glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, _camera->near_far.x, _camera->near_far.x, _light_projection);
	glm_lookat((vec3) { ENGINE.directional_light.direction.x, ENGINE.directional_light.direction.x, ENGINE.directional_light.direction.z },
				(vec3) { 0.0f, 0.0f, 0.0f },
				(vec3) { 0.0f, 1.0f, 0.0f },
				_light_view
	);
	 glm_mul(_light_projection, _light_view, _light_space_matrix);
	// render scene from light's point of view
	//simpleDepthShader.use();
	//simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	RDE_CHECK_GL(glViewport, 0, 0, RDE_SHADOW_MAP_SIZE, RDE_SHADOW_MAP_SIZE);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.shadows.frame_buffer_id);
	RDE_CHECK_GL(glClear, GL_DEPTH_BUFFER_BIT);
	
	// Render scene

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_inner_engine_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	RDE_CHECK_GL(glViewport, 0, 0, _window_size.x, _window_size.y);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.samples > 0 ? ENGINE.antialiasing.frame_buffer_id : DEFAULT_RENDER_TEXTURE->opengl_framebuffer_id);

}


// ==============================================================================
// =									API										=
// ==============================================================================

rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code) {
	bool _error = false;

	GLuint _vertex_program_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint _fragment_program_id = glCreateShader(GL_FRAGMENT_SHADER);

	GLint _vertex_source_size = strlen(_vertex_code);
	GLint _fragment_source_size = strlen(_fragment_code);

	RDE_CHECK_GL(glShaderSource, _vertex_program_id, 1, &_vertex_code, &_vertex_source_size);
	RDE_CHECK_GL(glShaderSource, _fragment_program_id, 1, &_fragment_code, &_fragment_source_size);

	RDE_CHECK_GL(glCompileShader, _vertex_program_id);
	RDE_CHECK_GL(glCompileShader, _fragment_program_id);

	GLint _is_vertex_compiled, _is_fragment_compiled;
	RDE_CHECK_GL(glGetShaderiv, _vertex_program_id, GL_COMPILE_STATUS, &_is_vertex_compiled);
	RDE_CHECK_GL(glGetShaderiv, _fragment_program_id, GL_COMPILE_STATUS, &_is_fragment_compiled);

	RDE_CHECK_SHADER_COMPILATION_STATUS( _vertex_program_id, _is_vertex_compiled, _vertex_code)
	RDE_CHECK_SHADER_COMPILATION_STATUS(_fragment_program_id, _is_fragment_compiled, _fragment_code)

	GLuint _program_id = glCreateProgram();
	RDE_CHECK_GL(glAttachShader, _program_id, _vertex_program_id);
	RDE_CHECK_GL(glAttachShader, _program_id, _fragment_program_id);

	RDE_CHECK_GL(glLinkProgram, _program_id);

	if (_error) {
		printf("%s \n\n %s", _vertex_code, _fragment_code);
		return NULL;
	}

	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_shaders; _i++) {
		if (ENGINE.shaders[_i].compiled_program_id != -1) {
			continue;
		}

		rde_shader* _shader = &ENGINE.shaders[_i];
		_shader->vertex_program_id = _vertex_program_id;
		_shader->fragment_program_id = _fragment_program_id;
		_shader->compiled_program_id = _program_id;
		rde_strcat(_shader->name, RDE_SHADER_MAX_NAME, _name);

		rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded shader with index '%d' successfully", _shader->compiled_program_id);
		return _shader;
	}

	rde_critical_error(true, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "shaders", ENGINE.heap_allocs_config.max_number_of_shaders);
	return NULL;
}

void rde_rendering_shader_set_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_FV_1 		   : glUniform1fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_2 		   : glUniform2fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_3 		   : glUniform3fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_4 		   : glUniform4fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_MATRIX_2   : glUniformMatrix2fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3   : glUniformMatrix3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4   : glUniformMatrix4fv(_location, 1, _transpose, _data); break;
			
#if !IS_WASM()
			case RDE_UNIFORM_FV_MATRIX_2x3 : glUniformMatrix2x3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3x2 : glUniformMatrix3x2fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3x4 : glUniformMatrix3x4fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4x3 : glUniformMatrix4x3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_2x4 : glUniformMatrix2x4fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4x2 : glUniformMatrix4x2fv(_location, 1, _transpose, _data); break;
#else
			default: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s' of type '%d' in WASM and is not supported", _uniform_name, (int)_type);
			}
#endif
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

void rde_rendering_shader_set_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_IV_1 : glUniform1iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_2 : glUniform2iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_3 : glUniform3iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_4 : glUniform4iv(_location, 1, _data); break;
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

void rde_rendering_shader_set_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_UIV_1 : glUniform1uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_2 : glUniform2uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_3 : glUniform3uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_4 : glUniform4uiv(_location, 1, _data); break;
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

rde_shader_data rde_rendering_shader_get_data(rde_shader* _shader) {
	rde_critical_error(_shader == NULL, RDE_ERROR_NO_NULL_ALLOWED, "shader");
	return (rde_shader_data) {
		.vertex_program_id = _shader->vertex_program_id,
		.fragment_program_id = _shader->fragment_program_id,
		.compiled_program_id = _shader->compiled_program_id
	};
}

rde_shader* rde_rendering_shader_get_by_name(const char* _name) {
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_shaders; _i++) {
		rde_shader* _shader = &ENGINE.shaders[_i];
		if(_shader != NULL && strcmp(_shader->name, _name) == 0) {
			return _shader;
		}
	}

	return NULL;
}

void rde_rendering_shader_unload(rde_shader* _shader) {
	rde_critical_error(_shader == NULL, RDE_ERROR_NO_NULL_ALLOWED, "shader");

	RDE_CHECK_GL(glDeleteShader, _shader->vertex_program_id);
	RDE_CHECK_GL(glDeleteShader, _shader->fragment_program_id);
	RDE_CHECK_GL(glDeleteProgram, _shader->compiled_program_id);

	_shader->compiled_program_id = -1;
}

rde_texture_parameters rde_innner_rendering_validate_texture_parameters(rde_texture_parameters* _params) {
	rde_texture_parameters _tex_params = _params == NULL ? RDE_DEFAULT_TEXTURE_PARAMETERS : *_params;
#define RDE_TEXTURE_PARAM_CHECKER(_field, _min, _max, _warn_text) 																								\
	if(_tex_params._field < _min || _tex_params._field > _max) {																								\
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_WRONG_TEXTURE_PARAM, _tex_params._field, _warn_text, RDE_DEFAULT_TEXTURE_PARAMETERS._field);	\
		_tex_params._field = RDE_DEFAULT_TEXTURE_PARAMETERS._field;																								\
	}

	RDE_TEXTURE_PARAM_CHECKER(min_filter, RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR, "MIN_FILTER")
	RDE_TEXTURE_PARAM_CHECKER(mag_filter, RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR, "MAG_FILTER")
	RDE_TEXTURE_PARAM_CHECKER(wrap_s, RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT, RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT, "WRAP_S")
	RDE_TEXTURE_PARAM_CHECKER(wrap_t, RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT, RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT, "WRAP_T")
	RDE_TEXTURE_PARAM_CHECKER(mipmap_min_filter, RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE, "MIPMAP_MIN_FILTER")

	return _tex_params;
}

rde_texture* rde_rendering_texture_load(const char* _file_path, rde_texture_parameters* _params) {
	const char* _extension = rde_util_file_get_name_extension(_file_path);
	char _extension_lower[10];
	memset(_extension_lower, 0, 10);
	rde_util_string_to_lower(_extension_lower, _extension);
	rde_critical_error(strcmp(_extension_lower, "jpeg") != 0 && strcmp(_extension_lower, "jpg") != 0 &&
	                   strcmp(_extension_lower, "png") != 0, RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT, _file_path, _extension);

	rde_texture* _texture = NULL;

	char _sanitized_path[RDE_MAX_PATH];
	memset(_sanitized_path, 0, RDE_MAX_PATH);
	rde_util_file_sanitaize_path(_file_path, _sanitized_path, RDE_MAX_PATH);

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (strlen(ENGINE.textures[_i].file_path) != 0 && strcmp(ENGINE.textures[_i].file_path, _sanitized_path) == 0) {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "Path '%s' already loaded in texture", _sanitized_path);
			return &ENGINE.textures[_i];
		}
	}

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	int _width, _height, _channels;
	stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
	stbi_convert_iphone_png_to_rgb(1);
	stbi_set_unpremultiply_on_load(1);
#endif

	stbi_uc* _data = NULL;
	
#if IS_MOBILE()
	while(rde_util_string_contains_substring(_sanitized_path, "../", false)) {
		size_t _sp_size = strlen(_sanitized_path);
		size_t _init_point = 0;
		size_t _end_point = 0;

		for(size_t _i = 0; _i < _sp_size; _i++) {
			if(_sanitized_path[_i] == '/' &&
			   _i + 1 < _sp_size && _sanitized_path[_i + 1] != '.' &&
			   _i + 2 < _sp_size && _sanitized_path[_i + 2] != '.' ) {
				_init_point = _i;
			}

			if(_sanitized_path[_i] == '.' &&
			   _i + 1 < _sp_size && _sanitized_path[_i + 1] == '.' &&
			   _i + 2 < _sp_size && _sanitized_path[_i + 2] == '/') {
				_end_point = _i + 2;
				break;
			}
		}

		rde_critical_error(_init_point == _end_point, "Error converting path");

		char _new_path[RDE_MAX_PATH];
		memset(_new_path, 0, RDE_MAX_PATH);
		char* _ptr = _sanitized_path;
		rde_strncat(_new_path, RDE_MAX_PATH, _ptr, _init_point);
		rde_strncat(_new_path, RDE_MAX_PATH, _ptr + _end_point, _sp_size - _end_point);
		memset(_sanitized_path, 0, RDE_MAX_PATH);
		rde_strcpy(_sanitized_path, RDE_MAX_PATH, _new_path);
		rde_log_level(RDE_LOG_LEVEL_INFO, "Removed a .. from path: %s", _sanitized_path);
	}

	rde_log_level(RDE_LOG_LEVEL_INFO, "This is the new path: %s", _sanitized_path);
	size_t _total_size = 0;
	rde_file_handle* _file_handle = rde_file_open(_sanitized_path, RDE_FILE_MODE_READ);
	const unsigned char* _texture_data = rde_file_read_full_file_bytes(_file_handle, &_total_size);
	_data = stbi_load_from_memory(_texture_data, _total_size, &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));
#else
	_data = stbi_load(_sanitized_path, &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));
#endif

	rde_critical_error(_data == NULL, "Could not load texture at '%s'", _sanitized_path);

	RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 4);
	GLenum _internal_format = 0;
	GLenum _data_format = 0;
	if (strcmp(_extension, "png") == 0) {
		_internal_format = GL_RGBA8;
		_data_format = GL_RGBA;
	} else {
		RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
		_internal_format = GL_RGB8;
		_data_format = GL_RGB;
	}

	rde_texture_parameters _tex_params = rde_innner_rendering_validate_texture_parameters(_params);
	
	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _texture_id);

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _tex_params.min_filter);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _tex_params.mag_filter);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _tex_params.wrap_s);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _tex_params.wrap_t);

	if(_tex_params.mipmap_min_filter != RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE) {
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _tex_params.mipmap_min_filter);
	}

	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);

	if(_tex_params.mipmap_min_filter != RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE) {
		RDE_CHECK_GL(glGenerateMipmap, GL_TEXTURE_2D);
	}
	
	stbi_image_free(_data);

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	_texture->internal_format = _internal_format;
	_texture->data_format = _data_format;
	rde_strcpy(_texture->file_path, RDE_MAX_PATH, _sanitized_path);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Texture at '%s' loaded correctly: ", _sanitized_path);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Size: %dx%d: ", _width, _height);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Channels: %d: ", _channels);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- OpenGL ID: %u: ", _texture_id);
	
	if(!rde_engine_logs_supressed()) {
		printf("\n");
	}

#if IS_MOBILE()
	rde_file_close(_file_handle);
#endif

	return _texture;
}

rde_texture* rde_rendering_texture_text_load(const char* _file_path) {
	rde_texture* _texture = NULL;

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (strlen(ENGINE.textures[_i].file_path) != 0 && strcmp(ENGINE.textures[_i].file_path, _file_path) == 0) {
			return &ENGINE.textures[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if(ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	int _width, _height, _channels;
	stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
	stbi_convert_iphone_png_to_rgb(1); 
	stbi_set_unpremultiply_on_load(1);
#endif

	stbi_uc* _data = NULL;
	_data = stbi_load(_file_path, &_width, &_height, &_channels, 0);

	if(_data == NULL) {
		printf("Error while loading texture at '%s' \n", _file_path);
		return NULL;
	}

	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _texture_id);
#if IS_MOBILE() || defined(__EMSCRIPTEN__)
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_ALPHA;
	_texture->data_format = GL_ALPHA;
#else
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_RED;
	_texture->data_format = GL_RED;
#endif

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(_data);

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	rde_strcpy(_texture->file_path, RDE_MAX_PATH, _file_path);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Texture at '%s' loaded correctly: ", _file_path);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Size: %dx%d: ", _width, _height);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Channels: %d: ", _channels);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- OpenGL ID: %u: ", _texture_id);
	if(!rde_engine_logs_supressed()) {
		printf("\n");
	}

	return _texture;
}

rde_texture_data rde_rendering_texture_get_data(rde_texture* _texture) {
	rde_critical_error(_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture");
	return (rde_texture_data) {
		.opengl_texture_id = _texture->opengl_texture_id,
		.size = _texture->size,
		.position = _texture->position,
		.channels = _texture->channels,
		.file_path = _texture->file_path,
		.atlas_texture_id = _texture->atlas_texture->opengl_texture_id
	};
}

rde_texture* rde_rendering_memory_texture_create(size_t _width, size_t _height, int _channels) {
	rde_texture* _texture = (rde_texture*)malloc(sizeof(rde_texture));
	rde_struct_init_alloc_ptr_texture(_texture);

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	_texture->pixels = (unsigned char*)malloc(_width * _height * _channels);
	rde_critical_error(_texture->pixels == NULL, RDE_ERROR_NO_MEMORY, _width * _height * _channels, "texture pixels array");
	memset(_texture->pixels, 0, _width * _height * _channels);
	_texture->size = (rde_vec_2UI) { _width, _height };
	_texture->channels = _channels;

	return _texture;
}

void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");

	rde_vec_2UI _t_size = _memory_texture->size;
	int _channels = _memory_texture->channels;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 0] = _color.r;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 1] = _color.g;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 2] = _color.b;
	
	if(_channels == 4) {
		_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 3] = _color.a;
	}

	_memory_texture->pixels_changed = true;
}

rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");

	rde_vec_2UI _t_size = _memory_texture->size;
	int _channels = _memory_texture->channels;
	rde_color _color;
	_color.r = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 0];
	_color.g = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 1];
	_color.b = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 2];
	
	if(_channels == 4) {
		_color.a = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 3];
	}

	return _color;
}

void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture) {
	if(_memory_texture->opengl_texture_id == -1) {
		GLenum _internal_format = 0, _data_format = 0;
		if (_memory_texture->channels == 4) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else if (_memory_texture->channels == 3) {
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		_memory_texture->internal_format = _internal_format;
		_memory_texture->data_format = _data_format;
		
		GLuint _id;
		RDE_CHECK_GL(glGenTextures, 1, &_id);

		_memory_texture->opengl_texture_id = _id;
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _memory_texture->opengl_texture_id);

		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, (int)_memory_texture->internal_format, _memory_texture->size.x, _memory_texture->size.y, 0, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
	} else {
		if(_memory_texture->pixels_changed) {
			RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _memory_texture->opengl_texture_id);
			RDE_CHECK_GL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, _memory_texture->size.x, _memory_texture->size.y, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
			_memory_texture->pixels_changed = false;
		}
	}
}

unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");
	return _memory_texture->pixels;
}

void rde_rendering_texture_unload(rde_texture* _texture) {
	rde_critical_error(_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture");
	GLuint _id = (GLuint)_texture->opengl_texture_id;

	if(_id < 0) {
		return;
	}

	glDeleteTextures(1, &_id);
	_texture->opengl_texture_id = -1;
	_texture->size = (rde_vec_2UI){ 0, 0 };
	_texture->channels = 0;
	_texture->internal_format = 0;
	_texture->data_format = 0;
	memset(_texture->file_path, 0, RDE_MAX_PATH);
}

rde_atlas* rde_rendering_atlas_load(const char* _texture_path, const char* _config_path) {
	rde_texture* _texture = rde_rendering_texture_load(_texture_path, NULL);
	rde_atlas_sub_textures* _atlas_sub_textures = rde_inner_file_system_read_atlas_config(_config_path, _texture);

	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_atlases; _i++) {
		if (ENGINE.atlases[_i].texture != NULL && strcmp(ENGINE.atlases[_i].texture->file_path, _texture_path) == 0) {
			return &ENGINE.atlases[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_atlases; _i++) {
		rde_atlas* _atlas = &ENGINE.atlases[_i];
		if(_atlas->texture != NULL) {
			continue;
		}

		_atlas->texture = _texture;
		_atlas->sub_textures = _atlas_sub_textures;
		return _atlas;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "atlases", ENGINE.total_amount_of_textures);
	return NULL;
}

rde_texture* rde_rendering_atlas_get_subtexture(rde_atlas* _atlas, const char* _texture_name) {
	bool _exists = stbds_shgeti(_atlas->sub_textures, _texture_name) != -1;
	if(!_exists) {
		rde_critical_error(true, RDE_ERROR_ATLAS_SUB_TEXTURE, _texture_name, _atlas->texture->file_path);
	}

	return &stbds_shget(_atlas->sub_textures, _texture_name);
}

rde_atlas_data rde_rendering_atlas_get_data(rde_atlas* _atlas) {
	rde_critical_error(_atlas == NULL, RDE_ERROR_NO_NULL_ALLOWED, "atlas");
	return (rde_atlas_data) {
		.opengl_texture_id = _atlas->texture->opengl_texture_id,
		.amount_of_subtextures = stbds_arrlenu(_atlas->sub_textures)
	};
}

void rde_rendering_atlas_unload(rde_atlas* _atlas) {
	rde_critical_error(_atlas == NULL, RDE_ERROR_NO_NULL_ALLOWED, "atlas");
	stbds_shfree(_atlas->sub_textures);
	_atlas->sub_textures = NULL;
	rde_rendering_texture_unload(_atlas->texture);
	_atlas->texture = NULL;
}

void rde_rendering_memory_texture_destroy(rde_texture* _memory_texture) {
	rde_critical_error(_memory_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture");
	free(_memory_texture->pixels);
	_memory_texture->pixels = NULL;
	_memory_texture->pixels_changed = false;
	if(_memory_texture->opengl_texture_id != -1) {
		rde_rendering_texture_unload(_memory_texture);
	}
}

rde_font* rde_rendering_font_load(const char* _font_path, const char* _font_config_path) {
	rde_texture* _texture = rde_rendering_texture_text_load(_font_path);
	rde_font_char_info* _chars = rde_inner_file_system_read_font_config(_font_config_path, _texture);

	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_fonts; _i++) {
		if (ENGINE.fonts[_i].texture != NULL && strcmp(ENGINE.fonts[_i].texture->file_path, _font_path) == 0) {
			return &ENGINE.fonts[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_fonts; _i++) {
		rde_font* _font = &ENGINE.fonts[_i];
		if(_font->texture != NULL) {
			continue;
		}

		_font->texture = _texture;
		_font->chars = _chars;
		return _font;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "fonts", ENGINE.heap_allocs_config.max_number_of_fonts);
	return NULL;
}

rde_font_data rde_rendering_font_get_data(rde_font* _font) {
	rde_critical_error(_font == NULL, RDE_ERROR_NO_NULL_ALLOWED, "font");
	return (rde_font_data) {
		.opengl_texture_id = _font->texture->opengl_texture_id,
		.amount_of_chars = stbds_arrlenu(_font->chars)
	};
}

void rde_rendering_font_unload(rde_font* _font) {
	rde_critical_error(_font == NULL, RDE_ERROR_NO_NULL_ALLOWED, "font");
	stbds_arrfree(_font->chars);
	_font->chars = NULL;
	rde_rendering_texture_unload(_font->texture);
	_font->texture = NULL;
}

void rde_rendering_set_background_color(rde_color _color) {
	glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

rde_skybox_id rde_rendering_skybox_load(const char* _texture_paths[6]) {
	if(ENGINE.skybox.vao == RDE_UINT_MAX) {
		float _vertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		RDE_CHECK_GL(glGenVertexArrays, 1, &ENGINE.skybox.vao);
		RDE_CHECK_GL(glGenBuffers, 1, &ENGINE.skybox.vbo);
		RDE_CHECK_GL(glBindVertexArray, ENGINE.skybox.vao);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, ENGINE.skybox.vbo);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 0);
		RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_CUBE_MAP, _texture_id);

	for(size_t _i = 0; _i < 6; _i++) {
		const char* _extension = rde_util_file_get_name_extension(_texture_paths[_i]);
		char _extension_lower[10];
		memset(_extension_lower, 0, 10);
		rde_util_string_to_lower(_extension_lower, _extension);
		rde_critical_error(strcmp(_extension_lower, "jpeg") != 0 && strcmp(_extension_lower, "jpg") != 0 &&
		                   strcmp(_extension_lower, "png") != 0, RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT, _texture_paths[_i], _extension);

		int _width, _height, _channels;
		//stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
		stbi_convert_iphone_png_to_rgb(1);
		stbi_set_unpremultiply_on_load(1);
#endif

		stbi_uc* _data = NULL;
		_data = stbi_load(_texture_paths[_i], &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));

		if (_data == NULL) {
			rde_critical_error(true, RDE_ERROR_FILE_NOT_FOUND, _texture_paths[_i]);
			return -1;
		}

		RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 4);
		GLenum _internal_format = 0;
		GLenum _data_format = 0;
		if (strcmp(_extension, "png") == 0) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else {
			RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + _i, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);

		stbi_image_free(_data);
	}

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Loaded correctly skybox with textures:");
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Left: %s", _texture_paths[0]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Right: %s", _texture_paths[1]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Top: %s", _texture_paths[2]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Bottom: %s", _texture_paths[3]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Front: %s", _texture_paths[4]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Back: %s", _texture_paths[5]);

	return _texture_id;
}

void rde_rendering_skybox_use(rde_skybox_id _skybox_id) {
	ENGINE.skybox.opengl_texture_id = _skybox_id;
}

void rde_rendering_skybox_unload(rde_skybox_id _skybox_id) {
	rde_critical_error(!glIsTexture(_skybox_id), "Tried to delete a texture with negative id");
	RDE_CHECK_GL(glDeleteTextures, 1, &_skybox_id);

	if(ENGINE.skybox.opengl_texture_id == (int)_skybox_id) {
		ENGINE.skybox.opengl_texture_id = -1;
		ENGINE.skybox.vao = RDE_UINT_MAX;
	}
}

rde_render_texture* rde_rendering_render_texture_create(size_t _width, size_t _height) {
	rde_render_texture* _render_texture = (rde_render_texture*)malloc(sizeof(rde_render_texture));
	_render_texture->size = (rde_vec_2UI) { _width, _height };

	RDE_CHECK_GL(glGenFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	RDE_CHECK_GL(glGenTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, 0);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);

#if !IS_MOBILE()
	RDE_CHECK_GL(glDrawBuffer, GL_COLOR_ATTACHMENT0);
#else
	GLenum _a[1] = { GL_COLOR_ATTACHMENT0 };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#endif

	RDE_CHECK_GL(glGenRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	RDE_CHECK_GL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
	RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

	return _render_texture;
}

void rde_rendering_render_texture_enable(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	current_render_texture = _render_texture;
	if(current_render_texture != NULL) {
		RDE_CHECK_GL(glViewport, 0, 0, current_render_texture->size.x, current_render_texture->size.y);
		RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, current_render_texture->opengl_framebuffer_id);
	}

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rde_rendering_render_texture_disable() {
	rde_inner_rendering_flush_render_texture_3d();

	current_render_texture = DEFAULT_RENDER_TEXTURE;
	if(current_render_texture != NULL) {
		RDE_CHECK_GL(glViewport, 0, 0, current_render_texture->size.x, current_render_texture->size.y);
		RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.samples > 0 ? ENGINE.antialiasing.frame_buffer_id : current_render_texture->opengl_framebuffer_id);
	}

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
}


void rde_rendering_render_texture_update(rde_render_texture* _render_texture, size_t _width, size_t _height) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	rde_rendering_render_texture_destroy(_render_texture);

	_render_texture->size = (rde_vec_2UI) { _width, _height };

	RDE_CHECK_GL(glGenFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	RDE_CHECK_GL(glGenTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, 0);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);

#if !IS_MOBILE()
	RDE_CHECK_GL(glDrawBuffer, GL_COLOR_ATTACHMENT0);
#else
	GLenum _a[1] = { GL_COLOR_ATTACHMENT0 };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#endif

	RDE_CHECK_GL(glGenRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	RDE_CHECK_GL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
	RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");

	RDE_CHECK_GL(glDeleteTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glDeleteRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
}

void rde_rendering_set_antialiasing(rde_window* _window, RDE_ANTIALIASING_ _antialiasing) {

#if IS_MOBILE()
	rde_log_level(RDE_LOG_LEVEL_WARNING, "Antialiasing is not yet supported in mobile platforms, not applying");
	return;
#endif

	if((int)_antialiasing == ENGINE.antialiasing.samples && ENGINE.antialiasing.frame_buffer_id != RDE_UINT_MAX) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_ALREADY_SET, ENGINE.antialiasing.samples);
		return;
	}

	switch(_antialiasing) {
		case RDE_ANTIALIASING_NONE:
		case RDE_ANTIALIASING_X2:
		case RDE_ANTIALIASING_X4:
		case RDE_ANTIALIASING_X8:
		case RDE_ANTIALIASING_X16:
		case RDE_ANTIALIASING_X32: break;
		default: rde_critical_error(true, RDE_ERROR_RENDERING_WRONG_ANTIALIASING_VALUE, (int)_antialiasing);
	}

	int _max_opengl_supported_samples = 0;
	RDE_CHECK_GL(glGetIntegerv, GL_MAX_SAMPLES, &_max_opengl_supported_samples);
	int _antialiasing_samples = (int)_antialiasing;

	rde_inner_rendering_destroy_current_antialiasing_config();

	while(_antialiasing_samples > _max_opengl_supported_samples) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_NOT_SUPPORTED, _antialiasing_samples, _antialiasing_samples >> 1);
		_antialiasing_samples = _antialiasing_samples >> 1;
	}

	ENGINE.antialiasing.samples = _antialiasing_samples;
	if(ENGINE.antialiasing.samples == 0) {
		return;
	}

    RDE_CHECK_GL(glGenFramebuffers, 1, &ENGINE.antialiasing.frame_buffer_id);
    RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.frame_buffer_id);
    
    unsigned int _texture_multisampled_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_multisampled_id);
	ENGINE.antialiasing.opengl_texture_id = _texture_multisampled_id;
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D_MULTISAMPLE, ENGINE.antialiasing.opengl_texture_id);
	rde_vec_2I _screen_size = rde_window_get_window_size(_window);

	#if IS_MOBILE()
	RDE_CHECK_GL(glTexStorage2DMultisample, GL_TEXTURE_2D_MULTISAMPLE, _antialiasing_samples, GL_RGB, _screen_size.x, _screen_size.y, GL_TRUE);
	#else
	RDE_CHECK_GL(glTexImage2DMultisample, GL_TEXTURE_2D_MULTISAMPLE, _antialiasing_samples, GL_RGB, _screen_size.x, _screen_size.y, GL_TRUE);
	#endif
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D_MULTISAMPLE, 0);
    RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ENGINE.antialiasing.opengl_texture_id, 0);
    
    RDE_CHECK_GL(glGenRenderbuffers, 1, &ENGINE.antialiasing.render_buffer_id);
    RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);
    RDE_CHECK_GL(glRenderbufferStorageMultisample, GL_RENDERBUFFER, _antialiasing_samples, GL_DEPTH24_STENCIL8, _screen_size.x, _screen_size.y);
    RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);

    rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER_MSAA);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

RDE_ANTIALIASING_ rde_rendering_get_current_antialiasing() {
	return (RDE_ANTIALIASING_)ENGINE.antialiasing.samples;
}

#endif