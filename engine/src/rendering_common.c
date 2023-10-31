#ifdef RDE_RENDERING_MODULE

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

rde_render_texture* DEFAULT_RENDER_TEXTURE = NULL;
rde_render_texture* current_render_texture = NULL;

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
void rde_inner_rendering_flush_render_texture_3d();

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
	rde_util_check_opengl_error("getting profile mask");
	if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
		printf("Core profile\n");
	} else {
		printf("Compatibility profile\n");
	}
	rde_log_level(RDE_LOG_LEVEL_INFO, "OpenGL Version: %s, Vendor: %s, GPU: %s, GLSL: %s", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

#if !IS_MOBILE() 
	#if !IS_WASM()
		glEnable(GL_PROGRAM_POINT_SIZE);
		rde_util_check_opengl_error("Invalid Point size");
		glEnable(GL_LINE_SMOOTH);
		rde_util_check_opengl_error("Invalid Line Smooth");
		glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
		rde_util_check_opengl_error("Invalid Line Smooth Hint -> GL_NICEST");
	#endif
			
	#if !IS_MAC() && !IS_LINUX()
		#if !IS_WASM()
			//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			rde_util_check_opengl_error("Invalid Point Smooth Hint -> GL_NICEST");
		#endif
	#endif
#endif

#if !IS_MOBILE() && !IS_WASM()
	rde_file_handle* _shader_vertex_handle = rde_file_open("shaders/core/line_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _shader_fragment_handle = rde_file_open("shaders/core/line_frag.glsl", RDE_FILE_MODE_READ);
	char* _vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	char* _fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.line_shader = rde_rendering_shader_load(RDE_SHADER_LINE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/color_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/color_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/texture_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/texture_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/text_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/text_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/framebuffer_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/framebuffer_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_FRAMEBUFFER, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/mesh_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/mesh_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.mesh_shader = rde_rendering_shader_load(RDE_SHADER_MESH, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/skybox_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/skybox_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle);
	ENGINE.skybox_shader = rde_rendering_shader_load(RDE_SHADER_SKYBOX, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
#else
	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, RDE_COLOR_VERTEX_SHADER_2D_ES, RDE_COLOR_FRAGMENT_SHADER_2D_ES);
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, RDE_TEXTURE_VERTEX_SHADER_2D_ES, RDE_TEXTURE_FRAGMENT_SHADER_2D_ES);
	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, RDE_TEXT_VERTEX_SHADER_2D_ES, RDE_TEXT_FRAGMENT_SHADER_2D_ES);
	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_MESH, RDE_FRAME_BUFFER_VERTEX_SHADER_ES, RDE_FRAME_BUFFER_FRAGMENT_SHADER_ES);
#endif
	
	rde_vec_2I _window_size = rde_window_get_window_size(_window);

	GLuint _vao, _vbo;
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FRAMEBUFFER_QUAD_DATA), &FRAMEBUFFER_QUAD_DATA, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	DEFAULT_RENDER_TEXTURE = rde_rendering_render_texture_create(_window_size.x, _window_size.y);
	DEFAULT_RENDER_TEXTURE->vao = _vao;
	DEFAULT_RENDER_TEXTURE->vbo = _vbo;
}

void rde_inner_rendering_draw_to_framebuffer(rde_render_texture* _render_texture) {
	GLuint _framebuffer_id = _render_texture == DEFAULT_RENDER_TEXTURE ? 0 : _render_texture->opengl_framebuffer_id;
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(ENGINE.framebuffer_shader->compiled_program_id);

	glBindVertexArray(_render_texture->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}





// ==============================================================================
// =									API										=
// ==============================================================================

rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code) {
	bool _error = false;

	GLuint _vertex_program_id = glCreateShader(GL_VERTEX_SHADER);
	_error |= rde_util_check_opengl_error("vertex program creation");
	GLuint _fragment_program_id = glCreateShader(GL_FRAGMENT_SHADER);
	_error |= rde_util_check_opengl_error("fragment program creation");

	GLint _vertex_source_size = strlen(_vertex_code);
	GLint _fragment_source_size = strlen(_fragment_code);

	glShaderSource(_vertex_program_id, 1, &_vertex_code, &_vertex_source_size);
	_error |= rde_util_check_opengl_error("vertex source attachment before compilation");
	glShaderSource(_fragment_program_id, 1, &_fragment_code, &_fragment_source_size);
	_error |= rde_util_check_opengl_error("fragment source attachment before compilation");

	glCompileShader(_vertex_program_id);
	_error |= rde_util_check_opengl_error("vertex compilation");
	glCompileShader(_fragment_program_id);
	_error |= rde_util_check_opengl_error("fragment compilation");

	GLint _is_vertex_compiled, _is_fragment_compiled;
	glGetShaderiv(_vertex_program_id, GL_COMPILE_STATUS, &_is_vertex_compiled);
	glGetShaderiv(_fragment_program_id, GL_COMPILE_STATUS, &_is_fragment_compiled);

	RDE_CHECK_SHADER_COMPILATION_STATUS( _vertex_program_id, _is_vertex_compiled, _vertex_code)
	RDE_CHECK_SHADER_COMPILATION_STATUS(_fragment_program_id, _is_fragment_compiled, _fragment_code)

	GLuint _program_id = glCreateProgram();
	_error |= rde_util_check_opengl_error("program creation");
	glAttachShader(_program_id, _vertex_program_id);
	_error |= rde_util_check_opengl_error("vertex attached to program");
	glAttachShader(_program_id, _fragment_program_id);
	_error |= rde_util_check_opengl_error("fragment attached to program");

	glLinkProgram(_program_id);
	_error |= rde_util_check_opengl_error("vertex and fragment linking");

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
#if IS_WINDOWS()
		strcat_s(_shader->name, RDE_SHADER_MAX_NAME, _name);
#else
		strcat(_shader->name, _name);
#endif

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

	glDeleteShader(_shader->vertex_program_id);
	glDeleteShader(_shader->fragment_program_id);
	glDeleteProgram(_shader->compiled_program_id);

	_shader->compiled_program_id = -1;
}

rde_texture* rde_rendering_texture_load(const char* _file_path) {
	const char* _extension = rde_util_file_get_name_extension(_file_path);
	char _extension_lower[10];
	memset(_extension_lower, 0, 10);
	rde_util_string_to_lower(_extension_lower, _extension);
	rde_critical_error(strcmp(_extension_lower, "jpeg") != 0 && strcmp(_extension_lower, "jpg") != 0 &&
	                   strcmp(_extension_lower, "png") != 0, RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT, _file_path, _extension);

	rde_texture* _texture = NULL;

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (ENGINE.textures[_i].file_path != NULL && strcmp(ENGINE.textures[_i].file_path, _file_path) == 0) {
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
	_data = stbi_load(_file_path, &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));

	if (_data == NULL) {
		printf("Error while loading texture at '%s' \n", _file_path);
		return NULL;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	GLenum _internal_format = 0;
	GLenum _data_format = 0;
	if (strcmp(_extension, "png") == 0) {
		_internal_format = GL_RGBA8;
		_data_format = GL_RGBA;
	} else {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		_internal_format = GL_RGB8;
		_data_format = GL_RGB;
	}
	
	GLuint _texture_id;
	glGenTextures(1, &_texture_id);
	rde_util_check_opengl_error("Generating texture");
	glBindTexture(GL_TEXTURE_2D, _texture_id);
	rde_util_check_opengl_error("Binding texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	rde_util_check_opengl_error("Param0 texture");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	rde_util_check_opengl_error("Param1 texture");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	rde_util_check_opengl_error("Param2 texture");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	rde_util_check_opengl_error("Param3 texture");

	glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);
	rde_util_check_opengl_error("TexImage2D texture");

	stbi_image_free(_data);

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	_texture->internal_format = _internal_format;
	_texture->data_format = _data_format;
	_texture->file_path = _file_path;

	rde_log_color(RDE_LOG_COLOR_GREEN, "Texture at '%s' loaded correctly: ", _file_path);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Size: %dx%d: ", _width, _height);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Channels: %d: ", _channels);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- OpenGL ID: %u: ", _texture_id);
	
	if(!rde_engine_logs_supressed()) {
		printf("\n");
	}

	return _texture;
}

rde_texture* rde_rendering_texture_text_load(const char* _file_path) {
	rde_texture* _texture = NULL;

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
	rde_util_check_opengl_error("1");

#if IS_IOS()
	stbi_convert_iphone_png_to_rgb(1); 
	stbi_set_unpremultiply_on_load(1);
#endif

	stbi_uc* _data = NULL;
	_data = stbi_load(_file_path, &_width, &_height, &_channels, 0);
	rde_util_check_opengl_error("2");

	if(_data == NULL) {
		printf("Error while loading texture at '%s' \n", _file_path);
		return NULL;
	}

	GLuint _texture_id;
	glGenTextures(1, &_texture_id);
	rde_util_check_opengl_error("3");
	glBindTexture(GL_TEXTURE_2D, _texture_id);
	rde_util_check_opengl_error("4");
#if IS_MOBILE() || defined(__EMSCRIPTEN__)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_ALPHA;
	_texture->data_format = GL_ALPHA;
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_RED;
	_texture->data_format = GL_RED;
#endif
	rde_util_check_opengl_error("5");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	rde_util_check_opengl_error("6");

	stbi_image_free(_data);
	rde_util_check_opengl_error("8");

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	_texture->file_path = _file_path;

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
		glGenTextures(1, &_id);

		_memory_texture->opengl_texture_id = _id;
		glBindTexture(GL_TEXTURE_2D, _memory_texture->opengl_texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, (int)_memory_texture->internal_format, _memory_texture->size.x, _memory_texture->size.y, 0, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
	} else {
		if(_memory_texture->pixels_changed) {
			glBindTexture(GL_TEXTURE_2D, _memory_texture->opengl_texture_id);
			glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, _memory_texture->size.x, _memory_texture->size.y, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
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
	_texture->file_path = NULL;
}

rde_atlas* rde_rendering_atlas_load(const char* _texture_path, const char* _config_path) {
	rde_texture* _texture = rde_rendering_texture_load(_texture_path);
	rde_atlas_sub_textures* _atlas_sub_textures = rde_inner_file_system_read_atlas_config(_config_path, _texture);

	for(size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
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

		glGenVertexArrays(1, &ENGINE.skybox.vao);
		glGenBuffers(1, &ENGINE.skybox.vbo);
		glBindVertexArray(ENGINE.skybox.vao);
		glBindBuffer(GL_ARRAY_BUFFER, ENGINE.skybox.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	GLuint _texture_id;
	glGenTextures(1, &_texture_id);
	rde_util_check_opengl_error("Generating texture");
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_id);
	rde_util_check_opengl_error("Binding texture");

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

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		GLenum _internal_format = 0;
		GLenum _data_format = 0;
		if (strcmp(_extension, "png") == 0) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _i, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);
		rde_util_check_opengl_error("TexImage2D texture");

		stbi_image_free(_data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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
	glDeleteTextures(1, &_skybox_id);
}

rde_render_texture* rde_rendering_render_texture_create(size_t _width, size_t _height) {
	rde_render_texture* _render_texture = (rde_render_texture*)malloc(sizeof(rde_render_texture));
	_render_texture->size = (rde_vec_2UI) { _width, _height };

	glGenFramebuffers(1, &_render_texture->opengl_framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	glGenTextures(1, &_render_texture->opengl_texture_id);
	glBindTexture(GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenRenderbuffers(1, &_render_texture->opengl_renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return _render_texture;
}

void rde_rendering_render_texture_enable(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	current_render_texture = _render_texture;
	if(current_render_texture != NULL) {
		glViewport(0, 0, current_render_texture->size.x, current_render_texture->size.y);
		glBindFramebuffer(GL_FRAMEBUFFER, current_render_texture->opengl_framebuffer_id);
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rde_rendering_render_texture_disable() {
	rde_inner_rendering_flush_render_texture_3d();

	current_render_texture = DEFAULT_RENDER_TEXTURE;
	if(current_render_texture != NULL) {
		glViewport(0, 0, current_render_texture->size.x, current_render_texture->size.y);
		glBindFramebuffer(GL_FRAMEBUFFER, current_render_texture->opengl_framebuffer_id);
	}

	glEnable(GL_DEPTH_TEST);
}


void rde_rendering_render_texture_update(rde_render_texture* _render_texture, size_t _width, size_t _height) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	rde_rendering_render_texture_destroy(_render_texture);

	_render_texture->size = (rde_vec_2UI) { _width, _height };

	glGenFramebuffers(1, &_render_texture->opengl_framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	glGenTextures(1, &_render_texture->opengl_texture_id);
	glBindTexture(GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenRenderbuffers(1, &_render_texture->opengl_renderbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");

	glDeleteTextures(1, &_render_texture->opengl_texture_id);
	glDeleteRenderbuffers(1, &_render_texture->opengl_renderbuffer_id);
	glDeleteFramebuffers(1, &_render_texture->opengl_framebuffer_id);
}


#endif