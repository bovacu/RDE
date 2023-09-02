#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PNM
#define STBI_NO_TGA
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS (1 << 13)
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if IS_WINDOWS()
#define STBIW_WINDOWS_UTF8
#endif
#include "stb/stb_image_write.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#define RDE_GLM_VEC_MAT_TO_POINTER(_type, _glmVecMat) reinterpret_cast<_type*>(glm::value_ptr(_glmVecMat))

static rde_camera* current_drawing_camera = nullptr;
static rde_batch_2d current_batch_2d;

struct rde_rendering_statistics {
	size_t number_of_drawcalls = 0;
} statistics;

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(rde_vertex_2d) * RDE_MAX_VERTICES_PER_BATCH, nullptr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), reinterpret_cast<const void*>(0));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_vertex_2d), reinterpret_cast<const void*>(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), reinterpret_cast<const void*>(sizeof(float) * 3 + sizeof(unsigned char) * 4));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rde_util_check_opengl_error("ERROR: 0");
}

void rde_rendering_reset_batch_2d() {
	memset(current_batch_2d.vertices, 0, RDE_MAX_VERTICES_PER_BATCH);
	current_batch_2d.shader = nullptr;
	current_batch_2d.amount_of_vertices = 0;
	current_batch_2d.texture_id = -1;
}

void rde_rendering_try_create_batch_2d(rde_shader* _shader) {
	if(current_batch_2d.shader == nullptr) {
		current_batch_2d.shader = _shader;
	}
}

void rde_rendering_flush_batch_2d() {
	glUseProgram(current_batch_2d.shader->compiled_program_id);
	//glUniformMatrix4fv(glGetUniformLocation(ENGINE.color_shader_2d->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, RDE_GLM_VEC_MAT_TO_POINTER(GLfloat, _view_projection_matrix));
	
	glBindVertexArray(current_batch_2d.shader->vertex_array_object);

	if(current_batch_2d.texture_id >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current_batch_2d.texture_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, current_batch_2d.shader->vertex_buffer_object);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(sizeof(rde_vertex_2d) * current_batch_2d.amount_of_vertices), &current_batch_2d.vertices);

	glDrawArrays(GL_TRIANGLES, 0, current_batch_2d.amount_of_vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rde_rendering_reset_batch_2d();
	rde_util_check_opengl_error("ERROR: 1");

	statistics.number_of_drawcalls++;
}

void rde_rendering_try_flush_batch_2d(rde_shader* _shader, size_t _extra_vertices) {
	if(current_batch_2d.amount_of_vertices + _extra_vertices <= RDE_MAX_VERTICES_PER_BATCH &&
		current_batch_2d.shader == _shader) {
		return;
	}

	rde_rendering_flush_batch_2d();
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
		glGetShaderInfoLog(_program_id, 1024, nullptr, _infolog);					\
		printf("Shader compile failed with error: %s \n", _infolog);				\
		glDeleteShader(_program_id);												\
		return nullptr;																\
	}

#define RDE_COLOR_TO_HEX_COLOR(_color) (int(_color.a) << 24) + (int(_color.b) << 16) + (int(_color.g) << 8) + int(_color.r)

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
		return nullptr;
	}

	for (size_t _i = 0; _i < RDE_MAX_LOADABLE_SHADERS; _i++) {
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
	return nullptr;
}

void rde_rendering_unload_shader(rde_shader* _shader) {
	assert(_shader != nullptr && "Tried to unload a nullptr shader");

	glDeleteShader(_shader->vertex_program_id);
	glDeleteShader(_shader->fragment_program_id);
	glDeleteProgram(_shader->compiled_program_id);

	glDeleteBuffers(1, &_shader->vertex_buffer_object);
	glDeleteBuffers(1, &_shader->index_buffer_object);
	glDeleteVertexArrays(1, &_shader->vertex_array_object);

	_shader->compiled_program_id = -1;
}

rde_texture* rde_rendering_load_texture(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_texture")
	return nullptr;
}

void rde_rendering_unload_texture(rde_texture* _texture) {
	UNUSED(_texture);
	UNIMPLEMENTED("rde_rendering_unload_texture")
}

rde_texture* rde_rendering_load_atlas(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_atlas")
	return nullptr;
}

void rde_rendering_unload_atlas(rde_atlas* _atlas) {
	UNUSED(_atlas);
	UNIMPLEMENTED("rde_rendering_unload_atlas")
}

rde_texture* rde_rendering_create_cpu_texture(const rde_vec_2UI _texture_size) {
	UNUSED(_texture_size);
	UNIMPLEMENTED("rde_rendering_create_cpu_texture")
	return nullptr;
}

void rde_rendering_destroy_cpu_texture(rde_cpu_texture* _cpu_texture) {
	UNUSED(_cpu_texture);
	UNIMPLEMENTED("rde_rendering_destroy_cpu_texture")
}

void rde_rendering_upload_cpu_texture_to_gpu(rde_cpu_texture* _cpu_texture) {
	UNUSED(_cpu_texture);
	UNIMPLEMENTED("rde_rendering_upload_cpu_texture_to_gpu")
}

rde_font* rde_rendering_load_font(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_font")
	return nullptr;
}

void rde_rendering_unload_font(rde_font* _font) {
	UNUSED(_font);
	UNIMPLEMENTED("rde_rendering_unload_font")
}

rde_texture* rde_rendering_get_atlas_sub_texture(const char* _texture_name){
	UNUSED(_texture_name);
	UNIMPLEMENTED("rde_rendering_get_atlas_sub_texture")
	return nullptr;
}


void rde_rendering_set_background_color(const rde_color _color) {
	glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void rde_rendering_begin_drawing_2d(rde_camera* _camera) {
	//assert(current_drawing_camera == nullptr && "Tried to begin drawing again before ending the previous one");
	current_drawing_camera = _camera;
}

void rde_rendering_begin_drawing_3d(rde_camera* _camera) {
	UNUSED(_camera);
	UNIMPLEMENTED("rde_rendering_start_drawing_3d")
}

void rde_rendering_draw_point_2d(const rde_vec_2F _position, const rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_point_2d")
}

void rde_rendering_draw_point_3d(const rde_vec_3F _position, const rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_point_3d")
}

void rde_rendering_draw_line_2d(const rde_vec_2F _init, const rde_vec_2F _end, const rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_2d")
}

void rde_rendering_draw_triangle_2d(const rde_vec_2F _vertex_a, const rde_vec_2F _vertex_b, const rde_vec_2F _vertex_c, const rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 3;
	
	rde_shader* _drawing_shader = _shader == nullptr ? ENGINE.color_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader);
	rde_rendering_try_flush_batch_2d(_drawing_shader, _triangle_vertex_count);

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _vertex_a.x, _vertex_a.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _vertex_b.x, _vertex_b.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _vertex_c.x, _vertex_c.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};
}

void rde_rendering_draw_rectangle_2d(const rde_vec_2F _bottom_left, const rde_vec_2F _top_right, const rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;
	
	rde_shader* _drawing_shader = _shader == nullptr ? ENGINE.color_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader);
	rde_rendering_try_flush_batch_2d(_drawing_shader, _triangle_vertex_count);

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _bottom_left.x, _bottom_left.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _top_right.x, _bottom_left.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _bottom_left.x, _top_right.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	// 2   1
	// *--*
	//  \ |
	//   \|
	//    *
	//    0
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _top_right.x, _bottom_left.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _top_right.x, _top_right.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};

	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = rde_vertex_2d {
		.position = { _bottom_left.x, _top_right.y, 0.f },
		.color = RDE_COLOR_TO_HEX_COLOR(_color),
		.texture_coordinates = { 0.f, 0.f }
	};
}

void rde_rendering_draw_circle_2d(const rde_vec_2F _position, float _radius, const rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_radius);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_circle_2d")
}

void rde_rendering_draw_line_3d(const rde_vec_3F _init, const rde_vec_3F _end, const rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_3d")
}

void rde_rendering_end_drawing_2d() {
	rde_rendering_flush_batch_2d();
	rde_rendering_reset_batch_2d();
	current_drawing_camera = nullptr;
	printf("DrawCalls: %zu \n", statistics.number_of_drawcalls);

	statistics.number_of_drawcalls = 0;
}

void rde_rendering_end_drawing_3d() {
	UNIMPLEMENTED("rde_rendering_end_drawing_3d")
}