#include <cglm/call.h>

static rde_camera* current_drawing_camera = NULL;
static rde_batch_2d current_batch_2d;
static mat4 projection_matrix;

typedef struct {
	size_t number_of_drawcalls;
} rde_rendering_statistics;

rde_rendering_statistics statistics;

rde_vec_2F rde_rendering_get_aspect_ratio() {
	rde_vec_2I _window_size = rde_window_get_window_size(current_batch_2d.window);
	bool _is_horizontal = rde_window_orientation_is_horizontal(current_batch_2d.window);
	float _aspect_ratio = _window_size.x / (float)_window_size.y;
	rde_vec_2F _aspect_ratios;
	_aspect_ratios.x = _is_horizontal ? 1.f : _aspect_ratio;
	_aspect_ratios.y = _is_horizontal ? _aspect_ratio : 1.f;
	return _aspect_ratios;
}

void rde_rendering_transform_to_glm_mat4(const rde_transform* _transform, mat4 _mat) {
	rde_vec_2I _window_size = rde_window_get_window_size(current_batch_2d.window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;
	rde_vec_2F _screen_pos;
	_screen_pos.x = _transform->position.x;
	_screen_pos.y = _transform->position.y / _aspect_ratio;
	rde_math_convert_world_position_to_screen_coordinates_2d(current_batch_2d.window, &_screen_pos);

	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _screen_pos.x, _screen_pos.y, _transform->position.z });
	glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y / _aspect_ratio, _transform->scale.z });
	glm_mat4_copy(_transformation_matrix, _mat);
}

void rde_util_assert(bool _assert, bool _crash, const char* _message) {
	if(!_assert) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, " '%s' %s \n", _message, _crash ? "exiting application" : "");
		if(_crash) {
			assert(false && _message);
		}
	}
}

bool rde_util_check_opengl_error(const char* _message) {
	GLenum _err;
	while((_err = glGetError()) != GL_NO_ERROR){
		switch(_err) {
			case GL_NO_ERROR:
				printf("GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_ENUM:
				printf("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_VALUE:
				printf("GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_OPERATION:
				printf("GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				printf("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. | %s -> %u \n",  _message, _err);
				return true;
			case GL_OUT_OF_MEMORY:
				printf("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. | %s -> %u \n",  _message, _err);
				return true;
			case GL_STACK_UNDERFLOW:
				printf("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow. | %s -> %u \n",  _message, _err);
				return true;
			case GL_STACK_OVERFLOW:
				printf("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. | %s -> %u \n",  _message, _err);
				return true;
			default:
				printf("No description. | %s -> %u \n",  _message, _err);
				return true;
		}
	}

	return false;
}


void rde_rendering_generate_gl_vertex_config_for_shader(rde_shader* _shader) {
	glGenVertexArrays(1, &_shader->vertex_array_object);
	glBindVertexArray(_shader->vertex_array_object);
	
	glGenBuffers(1, &_shader->vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, _shader->vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rde_vertex_2d) * RDE_MAX_VERTICES_PER_BATCH, NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), (const void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_vertex_2d), (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), (const void*)(sizeof(float) * 3 + sizeof(unsigned char) * 4));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rde_util_check_opengl_error("ERROR: 0");
}

void rde_rendering_reset_batch_2d() {
	current_batch_2d.shader = NULL;
	current_batch_2d.texture = rde_struct_create_texture();
	memset(current_batch_2d.vertices, 0, RDE_MAX_VERTICES_PER_BATCH);
	current_batch_2d.amount_of_vertices = 0;
}

void rde_rendering_try_create_batch_2d(rde_shader* _shader, const rde_texture* _texture) {
	if(current_batch_2d.shader == NULL) {
		current_batch_2d.shader = _shader;
	}

	if(current_batch_2d.texture.opengl_texture_id == -1 && _texture != NULL) {
		current_batch_2d.texture = *_texture;
	}
}

void rde_rendering_flush_batch_2d() {
	if(current_batch_2d.shader == NULL) {
		return;
	}

	rde_util_check_opengl_error("Before UseProgram");
	glUseProgram(current_batch_2d.shader->compiled_program_id);
	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;
	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	mat4 _view_matrix_inv = GLM_MAT4_IDENTITY_INIT;

	rde_rendering_transform_to_glm_mat4(&current_drawing_camera->transform, _view_matrix);
	glm_mat4_inv(_view_matrix, _view_matrix_inv);
	glm_mat4_mul(projection_matrix, _view_matrix_inv, _view_projection_matrix);

	glUniformMatrix4fv(glGetUniformLocation(current_batch_2d.shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);
	rde_util_check_opengl_error("After UseProgram");

	glBindVertexArray(current_batch_2d.shader->vertex_array_object);
	rde_util_check_opengl_error("After glBindVertexArray");

	if(current_batch_2d.texture.opengl_texture_id >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current_batch_2d.texture.opengl_texture_id);
		rde_util_check_opengl_error("After glBindTexture");
	}

	glBindBuffer(GL_ARRAY_BUFFER, current_batch_2d.shader->vertex_buffer_object);
	rde_util_check_opengl_error("After glBindBuffer");
	glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(sizeof(rde_vertex_2d) * current_batch_2d.amount_of_vertices), &current_batch_2d.vertices);
	rde_util_check_opengl_error("After glBufferSubData");

	glDrawArrays(GL_TRIANGLES, 0, current_batch_2d.amount_of_vertices);
	rde_util_check_opengl_error("After glDrawArrays");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rde_rendering_reset_batch_2d();
	rde_util_check_opengl_error("ERROR: 1");

	statistics.number_of_drawcalls++;
}

void rde_rendering_try_flush_batch_2d(rde_shader* _shader, const rde_texture* _texture, size_t _extra_vertices) {
	bool _vertex_ok = current_batch_2d.amount_of_vertices + _extra_vertices <= RDE_MAX_VERTICES_PER_BATCH;
	bool _shader_ok = current_batch_2d.shader == _shader;
	bool _texture_ok = _texture == NULL || current_batch_2d.texture.opengl_texture_id == _texture->opengl_texture_id;
	if(_vertex_ok && _shader_ok && _texture_ok) {
		return;
	}

	rde_rendering_flush_batch_2d();
	rde_rendering_reset_batch_2d();
	rde_rendering_try_create_batch_2d(_shader, _texture);
}

void rde_rendering_set_rendering_configuration() {


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
#else
	ENGINE.color_shader_2d = rde_rendering_load_shader(RDE_COLOR_VERTEX_SHADER_2D_ES, RDE_COLOR_FRAGMENT_SHADER_2D_ES);
	ENGINE.texture_shader_2d = rde_rendering_load_shader(RDE_TEXTURE_VERTEX_SHADER_2D_ES, RDE_TEXTURE_FRAGMENT_SHADER_2D_ES);
	ENGINE.text_shader_2d = rde_rendering_load_shader(RDE_TEXT_VERTEX_SHADER_2D_ES, RDE_TEXT_FRAGMENT_SHADER_2D_ES);
	ENGINE.frame_buffer_shader = rde_rendering_load_shader(RDE_FRAME_BUFFER_VERTEX_SHADER_ES, RDE_FRAME_BUFFER_FRAGMENT_SHADER_ES);
#endif
	
}

#define RDE_CHECK_SHADER_COMPILATION_STATUS(_program_id, _compiled)					\
	if(!_compiled) {																\
		char _infolog[1024];														\
		glGetShaderInfoLog(_program_id, 1024, NULL, _infolog);						\
		printf("Shader compile failed with error: %s \n", _infolog);				\
		glDeleteShader(_program_id);												\
		return NULL;																\
	}

#define RDE_COLOR_TO_HEX_COLOR(_color) (((int)_color.a) << 24) + (((int)_color.b) << 16) + (((int)_color.g) << 8) + ((int)_color.r)

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

		rde_rendering_generate_gl_vertex_config_for_shader(_shader);

		return _shader;
	}

	assert(false && "Maximum number of shaders loaded reached!");
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
	assert(_shader != NULL && "Tried to unload a NULL shader");

	glDeleteShader(_shader->vertex_program_id);
	glDeleteShader(_shader->fragment_program_id);
	glDeleteProgram(_shader->compiled_program_id);

	glDeleteBuffers(1, &_shader->vertex_buffer_object);
	glDeleteBuffers(1, &_shader->index_buffer_object);
	glDeleteVertexArrays(1, &_shader->vertex_array_object);

	_shader->compiled_program_id = -1;
}

rde_texture* rde_rendering_load_texture(const char* _file_path) {
	rde_texture* _texture = NULL;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_textures; _i++) {
		if(ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	rde_util_assert(_texture != NULL, true, "Max number of loaded textures reached");

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
	
	return _texture;
}

rde_texture* rde_rendering_load_text_texture(const char* _file_path) {
	rde_texture* _texture = NULL;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_textures; _i++) {
		if(ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	assert(_texture != NULL && "Max number of loaded textures reached");

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
	
	return _texture;
}

rde_texture* rde_rendering_create_memory_texture(size_t _width, size_t _height, int _channels) {
	rde_texture* _texture = NULL;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_textures; _i++) {
		if(ENGINE.textures[_i].opengl_texture_id != -1) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	assert(_texture != NULL && "Tried to get a texture, but all are already used");

	_texture->pixels = (unsigned char*)malloc(_width * _height * _channels);
	memset(_texture->pixels, 0, _width * _height * _channels);
	_texture->size = (rde_vec_2UI) { _width, _height };
	_texture->channels = _channels;

	return _texture;
}

void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color) {
	assert(_memory_texture != NULL && _memory_texture->pixels != NULL && "Tried to set a pixel on a NULL memory texture or NULL pixel array");

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
	assert(_memory_texture != NULL && _memory_texture->pixels && "Tried to get a pixel on a NULL memory texture");

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

unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture) {
	assert(_memory_texture != NULL && _memory_texture->pixels && "Tried to get a pixel on a NULL memory texture");
	return _memory_texture->pixels;
}

void rde_rendering_unload_texture(rde_texture* _texture) {
	assert(_texture != NULL && "Error: Tried to unload a NULL texture");
	GLuint _id = (GLuint)_texture->opengl_texture_id;
	glDeleteTextures(1, &_id);
	_texture->opengl_texture_id = -1;
	_texture->size = (rde_vec_2UI){ 0, 0 };
	_texture->channels = 0;
	_texture->internal_format = 0;
	_texture->data_format = 0;
	_texture->file_path = NULL;
}

rde_atlas* rde_rendering_load_atlas(const char* _texture_path, const char* _config_path) {
	rde_texture* _texture = rde_rendering_load_texture(_texture_path);
	rde_atlas_sub_textures* _atlas_sub_textures = rde_file_system_read_atlas_config(_config_path, _texture);

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_atlases; _i++) {
		rde_atlas* _atlas = &ENGINE.atlases[_i];
		if(_atlas->texture != NULL) {
			continue;
		}

		_atlas->texture = _texture;
		_atlas->sub_textures = _atlas_sub_textures;
		return _atlas;
	}

	assert(_texture != NULL && "Max number of loaded atlases reached");
	return NULL;
}

rde_texture* rde_rendering_get_atlas_sub_texture(rde_atlas* _atlas, const char* _texture_name) {
	bool _exists = stbds_shgeti(_atlas->sub_textures, _texture_name) != -1;
	if(!_exists) {
		printf("Error: could not load sub texture %s for atlas at %s \n", _texture_name, _atlas->texture->file_path);
		assert(false && "Tried to load an inexsitent sub texture in an atlas, check logs");
	}

	return &stbds_shget(_atlas->sub_textures, _texture_name);
}

void rde_rendering_unload_atlas(rde_atlas* _atlas) {
	assert(_atlas != NULL && "Tried to unload a null atlas");
	stbds_shfree(_atlas->sub_textures);
	_atlas->sub_textures = NULL;
	rde_rendering_unload_texture(_atlas->texture);
	_atlas->texture = NULL;
}

void rde_rendering_destroy_memory_texture(rde_texture* _memory_texture) {
	assert(_memory_texture != NULL && "Tried to free a NULL memory texture");
	free(_memory_texture->pixels);
	_memory_texture->pixels = NULL;
	_memory_texture->pixels_changed = false;
	if(_memory_texture->opengl_texture_id != -1) {
		rde_rendering_unload_texture(_memory_texture);
	}
}

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

	assert(_texture != NULL && "Max number of loaded fonts reached");
	return NULL;
}

void rde_rendering_unload_font(rde_font* _font) {
	assert(_font != NULL && "Tried to unload a null font");
	stbds_arrfree(_font->chars);
	_font->chars = NULL;
	rde_rendering_unload_texture(_font->texture);
	_font->texture = NULL;
}

void rde_rendering_set_background_color(rde_color _color) {
	glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void rde_rendering_begin_drawing_2d(rde_camera* _camera, rde_window* _window) {
	assert(current_drawing_camera == NULL || _window == NULL && "Tried to begin drawing again before ending the previous one or provided _camera or _window = NULL");
	current_drawing_camera = _camera;
	current_batch_2d.window = _window;
	current_batch_2d.texture = rde_struct_create_texture();

	switch(_camera->camera_type) {
		case RDE_CAMERA_TYPE_PERSPECTIVE : {
			rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
			float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.x : _aspect_ratios.y;
			glm_perspective(glm_rad(45.f), _aspect_ratio, 0.1f, 100.f, projection_matrix);
		};break;

		case RDE_CAMERA_TYPE_ORTHOGRAPHIC : {
			rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
			float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.x : _aspect_ratios.y;
			float _zoom = _camera->zoom;
			glm_ortho(-_aspect_ratio * _zoom, _aspect_ratio * _zoom, -_zoom, _zoom, -_zoom, _zoom, projection_matrix);
		} break;
	}
}

void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window) {
	glEnable(GL_DEPTH_TEST);
	UNUSED(_camera);
	UNUSED(_window);
	UNIMPLEMENTED("rde_rendering_start_drawing_3d")
}

void rde_rendering_draw_point_2d(rde_vec_2F _position, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_point_2d")
}

void rde_rendering_draw_point_3d(rde_vec_3F _position, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_point_3d")
}

void rde_rendering_draw_line_2d(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_2d")
}

void rde_rendering_draw_triangle_2d(rde_vec_2F _vertex_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 3;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.color_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader, NULL);
	rde_rendering_try_flush_batch_2d(_drawing_shader, NULL, _triangle_vertex_count);

	int _c = RDE_COLOR_TO_HEX_COLOR(_color);
	rde_vec_2I _window_size = rde_window_get_window_size(current_batch_2d.window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0;
	_vertex_0.position = (rde_vec_3F){ _vertex_a.x, _vertex_a.y / _aspect_ratio, 0.f };
	_vertex_0.color = _c;
	_vertex_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0;

	rde_vertex_2d _vertex_1;
	_vertex_1.position = (rde_vec_3F){ _vertex_b.x, _vertex_b.y / _aspect_ratio, 0.f };
	_vertex_1.color = _c;
	_vertex_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1;

	rde_vertex_2d _vertex_2;
	_vertex_2.position = (rde_vec_3F){ _vertex_c.x, _vertex_c.y / _aspect_ratio, 0.f };
	_vertex_2.color = _c;
	_vertex_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_2;
}

void rde_rendering_draw_rectangle_2d(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.color_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader, NULL);
	rde_rendering_try_flush_batch_2d(_drawing_shader, NULL, _triangle_vertex_count);

	int _c = RDE_COLOR_TO_HEX_COLOR(_color);
	rde_vec_2I _window_size = rde_window_get_window_size(current_batch_2d.window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0_0;
	_vertex_0_0.position = (rde_vec_3F){ _bottom_left.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_0_0.color = _c;
	_vertex_0_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_0_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

	rde_vertex_2d _vertex_0_1;
	_vertex_0_1.position = (rde_vec_3F){ _top_right.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_0_1.color = _c;
	_vertex_0_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_0_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

	rde_vertex_2d _vertex_0_2;
	_vertex_0_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_0_2.color = _c;
	_vertex_0_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_0_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

	// 2   1
	// *--*
	//  \ |
	//   \|
	//    *
	//    0

	rde_vertex_2d _vertex_1_0;
	_vertex_1_0.position = (rde_vec_3F){ _top_right.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_1_0.color = _c;
	_vertex_1_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_1_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

	rde_vertex_2d _vertex_1_1;
	_vertex_1_1.position = (rde_vec_3F){ _top_right.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_1.color = _c;
	_vertex_1_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_1_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

	rde_vertex_2d _vertex_1_2;
	_vertex_1_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_2.color = _c;
	_vertex_1_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_batch_2d.window, &_vertex_1_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;
}

void rde_rendering_draw_circle_2d(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_radius);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_circle_2d")
}

void rde_rendering_draw_polygon_2d(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader) {
	UNUSED(_transform)
	UNUSED(_polygon)
	UNUSED(_color)
	UNUSED(_color)
	UNUSED(_shader)
	UNIMPLEMENTED("rde_rendering_draw_polygon_2d")
}

void rde_rendering_draw_texture_2d(const rde_transform* _transform, const rde_texture* _texture, rde_color _tint_color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;

	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_rendering_transform_to_glm_mat4(_transform, _transformation_matrix);

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.texture_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader, _texture);
	rde_rendering_try_flush_batch_2d(_drawing_shader, _texture, _triangle_vertex_count);

	rde_vec_2F _texture_origin_norm = (rde_vec_2F){ 0.f, 0.f };
	rde_vec_2F _texture_tile_size_norm = (rde_vec_2F){ 1.f, 1.f };
	rde_vec_2F _texture_tile_size = (rde_vec_2F){ (float)_texture->size.x, (float)_texture->size.y };

	if(_texture->atlas_texture != NULL) {
		_texture_origin_norm.x = _texture->position.x / (float)_texture->atlas_texture->size.x;
		_texture_origin_norm.y = _texture->position.y / (float)_texture->atlas_texture->size.y;
	
		_texture_tile_size_norm.x = _texture_tile_size.x / (float)_texture->atlas_texture->size.x;
		_texture_tile_size_norm.y = _texture_tile_size.y / (float)_texture->atlas_texture->size.y;
	}

	rde_vec_2F _texture_tile_size_on_screen = _texture_tile_size;
	rde_math_convert_world_size_to_screen_size(current_batch_2d.window, &_texture_tile_size_on_screen);

	vec4 _bottom_left_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _bottom_right_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _top_right_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _top_left_texture_position = GLM_VEC4_ONE_INIT;

	glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_left_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_right_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_right_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_left_texture_position);

	// The (1.f - _texture_origin_norm.y) is needed as we need to invert the coordinates on the Y axis, the exported atlas (by RDE tool) assumes (0, 0)
	// on top-left, but OpenGL coords for UV origin is bottom-left.
	vec2 _top_left_texture_uv_coord   	 = (vec2){ _texture_origin_norm.x                            , 1.f - _texture_origin_norm.y };
	vec2 _top_right_texture_uv_coord  	 = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - _texture_origin_norm.y };
	vec2 _bottom_right_texture_uv_coord  = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };
	vec2 _bottom_left_texture_uv_coord   = (vec2){ _texture_origin_norm.x                            , 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };

	int _color = RDE_COLOR_TO_HEX_COLOR(_tint_color);

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0_0;
	_vertex_0_0.position = (rde_vec_3F){ _bottom_left_texture_position[0], _bottom_left_texture_position[1], 0.f };
	_vertex_0_0.color = _color;
	_vertex_0_0.texture_coordinates = (rde_vec_2F){ _bottom_left_texture_uv_coord[0], _bottom_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

	rde_vertex_2d _vertex_0_1;
	_vertex_0_1.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
	_vertex_0_1.color = _color;
	_vertex_0_1.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

	rde_vertex_2d _vertex_0_2;
	_vertex_0_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
	_vertex_0_2.color = _color;
	_vertex_0_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

	// 2   1
	// *--*
	//  \ |
	//   \|
	//    *
	//    0

	rde_vertex_2d _vertex_1_0;
	_vertex_1_0.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
	_vertex_1_0.color = _color;
	_vertex_1_0.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

	rde_vertex_2d _vertex_1_1;
	_vertex_1_1.position = (rde_vec_3F){ _top_right_texture_position[0], _top_right_texture_position[1], 0.f };
	_vertex_1_1.color = _color;
	_vertex_1_1.texture_coordinates = (rde_vec_2F){ _top_right_texture_uv_coord[0], _top_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

	rde_vertex_2d _vertex_1_2;
	_vertex_1_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
	_vertex_1_2.color = _color;
	_vertex_1_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;
}

void rde_rendering_draw_memory_texture_2d(const rde_transform* _transform, rde_texture* _texture, rde_color _tint_color, rde_shader* _shader) {
	if(_texture->opengl_texture_id == -1) {
		GLenum _internal_format = 0, _data_format = 0;
		if (_texture->channels == 4) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else if (_texture->channels == 3) {
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		_texture->internal_format = _internal_format;
		_texture->data_format = _data_format;
		
		GLuint _id;
		glGenTextures(1, &_id);

		_texture->opengl_texture_id = _id;
		glBindTexture(GL_TEXTURE_2D, _texture->opengl_texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, (int)_texture->internal_format, _texture->size.x, _texture->size.y, 0, _texture->data_format, GL_UNSIGNED_BYTE, _texture->pixels);
	} else {
		if(_texture->pixels_changed) {
			glBindTexture(GL_TEXTURE_2D, _texture->opengl_texture_id);
			glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, _texture->size.x, _texture->size.y, _texture->data_format, GL_UNSIGNED_BYTE, _texture->pixels);
			_texture->pixels_changed = false;
		}
	}

	rde_rendering_draw_texture_2d(_transform, _texture, _tint_color, _shader);
}

void rde_rendering_draw_text_2d(const rde_transform* _transform, const rde_font* _font, const char* _text, rde_color _tint_color, rde_shader* _shader) {
	int _text_size = strlen(_text);

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.text_shader_2d : _shader;

	int _next_pos_x = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		rde_transform _t = *_transform;
		int _key = (int)_text[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_t.position.x += _next_pos_x + _char_info.bearing.x * _t.scale.x;
		_t.position.y -= (_char_info.size.y * 0.5f - _char_info.bearing.y) * _t.scale.y;

		const size_t _triangle_vertex_count = 6;

		mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;
		rde_rendering_transform_to_glm_mat4(&_t, _transformation_matrix);

		rde_rendering_try_create_batch_2d(_drawing_shader, _char_info.texture.atlas_texture);
		rde_rendering_try_flush_batch_2d(_drawing_shader, _char_info.texture.atlas_texture, _triangle_vertex_count);

		rde_vec_2F _texture_origin_norm = (rde_vec_2F){ 0.f, 0.f };
		rde_vec_2F _texture_tile_size_norm = (rde_vec_2F){ 1.f, 1.f };
		rde_vec_2F _texture_tile_size = (rde_vec_2F){ (float)_char_info.size.x, (float)_char_info.size.y };

		if(_char_info.texture.atlas_texture != NULL) {
			_texture_origin_norm.x = _char_info.offset.x / (float)_char_info.texture.atlas_texture->size.x;
			_texture_origin_norm.y = _char_info.offset.y / (float)_char_info.texture.atlas_texture->size.y;
	
			_texture_tile_size_norm.x = _char_info.size.x / (float)_char_info.texture.atlas_texture->size.x;
			_texture_tile_size_norm.y = _char_info.size.y / (float)_char_info.texture.atlas_texture->size.y;
		}

		rde_vec_2F _texture_tile_size_on_screen = _texture_tile_size;
		rde_math_convert_world_size_to_screen_size(current_batch_2d.window, &_texture_tile_size_on_screen);

		rde_vec_2F _size_in_screen = (rde_vec_2F) { (float)(_char_info.advance.x >> 6) - _char_info.bearing.x, 0.f};
		rde_math_convert_world_size_to_screen_size(current_batch_2d.window, &_size_in_screen);

		vec4 _bottom_left_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _bottom_right_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _top_right_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _top_left_texture_position = GLM_VEC4_ONE_INIT;

		glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x + _size_in_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_left_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x  + _size_in_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_right_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x  + _size_in_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_right_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x + _size_in_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_left_texture_position);

		// The (1.f - _texture_origin_norm.y) is needed as we need to invert the coordinates on the Y axis, the exported atlas (by RDE tool) assumes (0, 0)
		// on top-left, but OpenGL coords for UV origin is bottom-left.
		vec2 _top_left_texture_uv_coord   	 = (vec2){ _texture_origin_norm.x                            , 1.f - _texture_origin_norm.y };
		vec2 _top_right_texture_uv_coord  	 = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - _texture_origin_norm.y };
		vec2 _bottom_right_texture_uv_coord  = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };
		vec2 _bottom_left_texture_uv_coord   = (vec2){ _texture_origin_norm.x                            , 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };

		int _color = RDE_COLOR_TO_HEX_COLOR(_tint_color);

		// * 2
		// |\
		// | \
		// *--*
		// 0   1
		rde_vertex_2d _vertex_0_0;
		_vertex_0_0.position = (rde_vec_3F){ _bottom_left_texture_position[0], _bottom_left_texture_position[1], 0.f };
		_vertex_0_0.color = _color;
		_vertex_0_0.texture_coordinates = (rde_vec_2F){ _bottom_left_texture_uv_coord[0], _bottom_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

		rde_vertex_2d _vertex_0_1;
		_vertex_0_1.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
		_vertex_0_1.color = _color;
		_vertex_0_1.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

		rde_vertex_2d _vertex_0_2;
		_vertex_0_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
		_vertex_0_2.color = _color;
		_vertex_0_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

		// 2   1
		// *--*
		//  \ |
		//   \|
		//    *
		//    0

		rde_vertex_2d _vertex_1_0;
		_vertex_1_0.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
		_vertex_1_0.color = _color;
		_vertex_1_0.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

		rde_vertex_2d _vertex_1_1;
		_vertex_1_1.position = (rde_vec_3F){ _top_right_texture_position[0], _top_right_texture_position[1], 0.f };
		_vertex_1_1.color = _color;
		_vertex_1_1.texture_coordinates = (rde_vec_2F){ _top_right_texture_uv_coord[0], _top_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

		rde_vertex_2d _vertex_1_2;
		_vertex_1_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
		_vertex_1_2.color = _color;
		_vertex_1_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;

		_next_pos_x += (_char_info.advance.x >> 6) * _t.scale.x; // /64.f
	}
}

void rde_rendering_draw_line_3d(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_3d")
}

void rde_rendering_end_drawing_2d() {
	rde_rendering_flush_batch_2d();
	rde_rendering_reset_batch_2d();
	current_drawing_camera = NULL;
	current_batch_2d.window = NULL;
	statistics.number_of_drawcalls = 0;
}

void rde_rendering_end_drawing_3d() {
	UNIMPLEMENTED("rde_rendering_end_drawing_3d")
	glDisable(GL_DEPTH_TEST);
}