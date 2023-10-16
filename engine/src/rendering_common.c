static rde_camera* current_drawing_camera = NULL;
static rde_window* current_drawing_window = NULL;
static mat4 projection_matrix;

typedef struct {
	size_t number_of_drawcalls;
} rde_rendering_statistics;

rde_rendering_statistics statistics;

void rde_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch);
void rde_rendering_generate_gl_vertex_config_for_mesh_3d(rde_mesh* _mesh);

rde_vec_2F rde_rendering_get_aspect_ratio() {
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	bool _is_horizontal = rde_window_orientation_is_horizontal(current_drawing_window);
	float _aspect_ratio = _window_size.x / (float)_window_size.y;
	rde_vec_2F _aspect_ratios;
	_aspect_ratios.x = _is_horizontal ? 1.f : _aspect_ratio;
	_aspect_ratios.y = _is_horizontal ? _aspect_ratio : 1.f;
	return _aspect_ratios;
}

void rde_rendering_set_rendering_configuration() {
	// During init, enable debug output

#if !IS_MOBILE()
	printf("OpenGL Version: %s, Vendor: %s, GPU: %s \n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	rde_util_check_opengl_error("Invalid Point Smooth Hint -> GL_NICEST");
#endif
#endif
#endif

#if !IS_MOBILE() && !IS_WASM()
	ENGINE.color_shader_2d = rde_rendering_load_shader(RDE_COLOR_VERTEX_SHADER_2D, RDE_COLOR_FRAGMENT_SHADER_2D);
	ENGINE.texture_shader_2d = rde_rendering_load_shader(RDE_TEXTURE_VERTEX_SHADER_2D, RDE_TEXTURE_FRAGMENT_SHADER_2D);
	ENGINE.text_shader_2d = rde_rendering_load_shader(RDE_TEXT_VERTEX_SHADER_2D, RDE_TEXT_FRAGMENT_SHADER_2D);
	ENGINE.frame_buffer_shader = rde_rendering_load_shader(RDE_FRAME_BUFFER_VERTEX_SHADER, RDE_FRAME_BUFFER_FRAGMENT_SHADER);
	ENGINE.mesh_shader = rde_rendering_load_shader(RDE_MESH_VERTEX_SHADER, RDE_MESH_FRAGMENT_SHADER);
#else
	ENGINE.color_shader_2d = rde_rendering_load_shader(RDE_COLOR_VERTEX_SHADER_2D_ES, RDE_COLOR_FRAGMENT_SHADER_2D_ES);
	ENGINE.texture_shader_2d = rde_rendering_load_shader(RDE_TEXTURE_VERTEX_SHADER_2D_ES, RDE_TEXTURE_FRAGMENT_SHADER_2D_ES);
	ENGINE.text_shader_2d = rde_rendering_load_shader(RDE_TEXT_VERTEX_SHADER_2D_ES, RDE_TEXT_FRAGMENT_SHADER_2D_ES);
	ENGINE.frame_buffer_shader = rde_rendering_load_shader(RDE_FRAME_BUFFER_VERTEX_SHADER_ES, RDE_FRAME_BUFFER_FRAGMENT_SHADER_ES);
#endif
	
}

#define RDE_CHECK_SHADER_COMPILATION_STATUS(_program_id, _compiled)						\
	if(!_compiled) {																	\
		char _infolog[1024];															\
		glGetShaderInfoLog(_program_id, 1024, NULL, _infolog);							\
		glDeleteShader(_program_id);													\
		rde_critical_error(true, "Shader compile failed with error: %s \n", _infolog);	\
		return NULL;																	\
	}

	// ======================= API ===========================

rde_shader* rde_rendering_load_shader(const char* _vertex_code, const char* _fragment_code) {
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

	RDE_CHECK_SHADER_COMPILATION_STATUS(_vertex_program_id, _is_vertex_compiled)
	RDE_CHECK_SHADER_COMPILATION_STATUS(_fragment_program_id, _is_fragment_compiled)

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

		rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded shader with index '%d' successfully", _shader->compiled_program_id);
		return _shader;
	}

	rde_critical_error(true, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "shaders", ENGINE.heap_allocs_config.max_number_of_shaders);
	return NULL;
}

void rde_rendering_set_shader_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose) {
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

void rde_rendering_set_shader_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data) {
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

void rde_rendering_set_shader_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data) {
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

void rde_rendering_unload_shader(rde_shader* _shader) {
	rde_critical_error(_shader == NULL, RDE_ERROR_NO_NULL_ALLOWED, "shader");

	glDeleteShader(_shader->vertex_program_id);
	glDeleteShader(_shader->fragment_program_id);
	glDeleteProgram(_shader->compiled_program_id);

	_shader->compiled_program_id = -1;
}

rde_texture* rde_rendering_load_texture(const char* _file_path) {
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

	GLenum _internal_format = 0;
	GLenum _data_format = 0;
	if (_channels == 4) {
		_internal_format = GL_RGBA8;
		_data_format = GL_RGBA;
	}
	else if (_channels == 3) {
		_internal_format = GL_RGB8;
		_data_format = GL_RGB;
	}

	GLuint _texture_id;
	glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);
	glBindTexture(GL_TEXTURE_2D, _texture_id);
	glTextureStorage2D(_texture_id, 1, _internal_format, _width, _height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(_texture_id, 0, 0, 0, _width, _height, _data_format, GL_UNSIGNED_BYTE, _data);

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
	printf("\n");

	return _texture;
}

rde_texture* rde_rendering_load_text_texture(const char* _file_path) {
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
	glCreateTextures(GL_TEXTURE_2D, 1, &_texture_id);
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
	printf("\n");

	return _texture;
}

rde_texture* rde_rendering_create_memory_texture(size_t _width, size_t _height, int _channels) {
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

void rde_rendering_unload_texture(rde_texture* _texture) {
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

#ifdef RDE_RENDERING_MODULE
rde_atlas* rde_rendering_load_atlas(const char* _texture_path, const char* _config_path) {
	rde_texture* _texture = rde_rendering_load_texture(_texture_path);
	rde_atlas_sub_textures* _atlas_sub_textures = rde_file_system_read_atlas_config(_config_path, _texture);

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

rde_texture* rde_rendering_get_atlas_sub_texture(rde_atlas* _atlas, const char* _texture_name) {
	bool _exists = stbds_shgeti(_atlas->sub_textures, _texture_name) != -1;
	if(!_exists) {
		rde_critical_error(true, RDE_ERROR_ATLAS_SUB_TEXTURE, _texture_name, _atlas->texture->file_path);
	}

	return &stbds_shget(_atlas->sub_textures, _texture_name);
}

void rde_rendering_unload_atlas(rde_atlas* _atlas) {
	rde_critical_error(_atlas == NULL, RDE_ERROR_NO_NULL_ALLOWED, "atlas");
	stbds_shfree(_atlas->sub_textures);
	_atlas->sub_textures = NULL;
	rde_rendering_unload_texture(_atlas->texture);
	_atlas->texture = NULL;
}
#endif

void rde_rendering_destroy_memory_texture(rde_texture* _memory_texture) {
	rde_critical_error(_memory_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture");
	free(_memory_texture->pixels);
	_memory_texture->pixels = NULL;
	_memory_texture->pixels_changed = false;
	if(_memory_texture->opengl_texture_id != -1) {
		rde_rendering_unload_texture(_memory_texture);
	}
}

#ifdef RDE_RENDERING_MODULE
rde_font* rde_rendering_load_font(const char* _font_path, const char* _font_config_path) {
	rde_texture* _texture = rde_rendering_load_text_texture(_font_path);
	rde_font_char_info* _chars = rde_file_system_read_font_config(_font_config_path, _texture);

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

void rde_rendering_unload_font(rde_font* _font) {
	rde_critical_error(_font == NULL, RDE_ERROR_NO_NULL_ALLOWED, "font");
	stbds_arrfree(_font->chars);
	_font->chars = NULL;
	rde_rendering_unload_texture(_font->texture);
	_font->texture = NULL;
}
#endif

void rde_rendering_set_background_color(rde_color _color) {
	glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}