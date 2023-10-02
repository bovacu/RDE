#ifdef RDE_RENDERING_2D_MODULE

static rde_batch_2d current_batch_2d;

void rde_rendering_init_2d() {
	current_batch_2d = rde_struct_create_2d_batch();
	rde_rendering_generate_gl_vertex_config_for_quad_2d(&current_batch_2d);

	current_batch_2d.vertices = (rde_vertex_2d*)malloc(sizeof(rde_vertex_2d) * ENGINE.heap_allocs_config.max_number_of_vertices_per_batch);
	rde_critical_error(current_batch_2d.vertices == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_vertex_2d) * ENGINE.heap_allocs_config.max_number_of_vertices_per_batch, "2d batch vertices");
}

void rde_rendering_end_2d() {
	glDeleteBuffers(1, &current_batch_2d.vertex_buffer_object);
	glDeleteBuffers(1, &current_batch_2d.index_buffer_object);
	glDeleteVertexArrays(1, &current_batch_2d.vertex_array_object);
	free(current_batch_2d.vertices);
}

void rde_rendering_transform_to_glm_mat4_2d(const rde_transform* _transform, mat4 _mat) {
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;
	rde_vec_2F _screen_pos;
	_screen_pos.x = _transform->position.x;
	_screen_pos.y = _transform->position.y / _aspect_ratio;
	rde_math_convert_world_position_to_screen_coordinates_2d(current_drawing_window, &_screen_pos);

	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _screen_pos.x, _screen_pos.y, _transform->position.z });
	glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y / _aspect_ratio, _transform->scale.z });
	glm_mat4_copy(_transformation_matrix, _mat);
}

void rde_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch) {
	glGenVertexArrays(1, &_batch->vertex_array_object);
	glBindVertexArray(_batch->vertex_array_object);
	
	glGenBuffers(1, &_batch->vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, _batch->vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rde_vertex_2d) * ENGINE.heap_allocs_config.max_number_of_vertices_per_batch, NULL, GL_DYNAMIC_DRAW);

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
	//memset(current_batch_2d.vertices, 0, ENGINE.heap_allocs_config.max_number_of_vertices_per_batch);
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

	rde_rendering_transform_to_glm_mat4_2d(&current_drawing_camera->transform, _view_matrix);
	glm_mat4_inv(_view_matrix, _view_matrix_inv);
	glm_mat4_mul(projection_matrix, _view_matrix_inv, _view_projection_matrix);

	glUniformMatrix4fv(glGetUniformLocation(current_batch_2d.shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);
	rde_util_check_opengl_error("After UseProgram");

	glBindVertexArray(current_batch_2d.vertex_array_object);
	rde_util_check_opengl_error("After glBindVertexArray");

	if(current_batch_2d.texture.opengl_texture_id >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current_batch_2d.texture.opengl_texture_id);
		rde_util_check_opengl_error("After glBindTexture");
	}

	glBindBuffer(GL_ARRAY_BUFFER, current_batch_2d.vertex_buffer_object);
	rde_util_check_opengl_error("After glBindBuffer");
	glBufferSubData(GL_ARRAY_BUFFER, 0, (long)(sizeof(rde_vertex_2d) * current_batch_2d.amount_of_vertices), current_batch_2d.vertices);
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
	bool _vertex_ok = current_batch_2d.amount_of_vertices + _extra_vertices <= ENGINE.heap_allocs_config.max_number_of_vertices_per_batch;
	bool _shader_ok = current_batch_2d.shader == _shader;
	bool _texture_ok = _texture == NULL || current_batch_2d.texture.opengl_texture_id == _texture->opengl_texture_id;
	if(_vertex_ok && _shader_ok && _texture_ok) {
		return;
	}

	rde_rendering_flush_batch_2d();
	rde_rendering_reset_batch_2d();
	rde_rendering_try_create_batch_2d(_shader, _texture);
}

void rde_rendering_begin_drawing_2d(rde_camera* _camera, rde_window* _window) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);
	current_drawing_camera = _camera;
	current_drawing_window = _window;
	current_batch_2d.texture = rde_struct_create_texture();

	switch (_camera->camera_type) {
		case RDE_CAMERA_TYPE_PERSPECTIVE : {
			rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
			float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.x : _aspect_ratios.y;
			glm_perspective(glm_rad(45.f), _aspect_ratio, 0.1f, 100.f, projection_matrix);
		}; break;

		case RDE_CAMERA_TYPE_ORTHOGRAPHIC : {
			rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
			float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.x : _aspect_ratios.y;
			float _zoom = _camera->zoom;
			glm_ortho(-_aspect_ratio * _zoom, _aspect_ratio * _zoom, -_zoom, _zoom, -_zoom, _zoom, projection_matrix);
		}; break;
	}
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
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
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
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0;

	rde_vertex_2d _vertex_1;
	_vertex_1.position = (rde_vec_3F){ _vertex_b.x, _vertex_b.y / _aspect_ratio, 0.f };
	_vertex_1.color = _c;
	_vertex_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1;

	rde_vertex_2d _vertex_2;
	_vertex_2.position = (rde_vec_3F){ _vertex_c.x, _vertex_c.y / _aspect_ratio, 0.f };
	_vertex_2.color = _c;
	_vertex_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_2;
}

void rde_rendering_draw_rectangle_2d(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.color_shader_2d : _shader;
	rde_rendering_try_create_batch_2d(_drawing_shader, NULL);
	rde_rendering_try_flush_batch_2d(_drawing_shader, NULL, _triangle_vertex_count);

	int _c = RDE_COLOR_TO_HEX_COLOR(_color);
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
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
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

	rde_vertex_2d _vertex_0_1;
	_vertex_0_1.position = (rde_vec_3F){ _top_right.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_0_1.color = _c;
	_vertex_0_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

	rde_vertex_2d _vertex_0_2;
	_vertex_0_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_0_2.color = _c;
	_vertex_0_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_2.position);
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
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

	rde_vertex_2d _vertex_1_1;
	_vertex_1_1.position = (rde_vec_3F){ _top_right.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_1.color = _c;
	_vertex_1_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

	rde_vertex_2d _vertex_1_2;
	_vertex_1_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_2.color = _c;
	_vertex_1_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_2.position);
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
	rde_rendering_transform_to_glm_mat4_2d(_transform, _transformation_matrix);

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
	rde_math_convert_world_size_to_screen_size(current_drawing_window, &_texture_tile_size_on_screen);

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
	rde_log_level(RDE_LOG_LEVEL_WARNING, "THIS FUNCTION IS CORRPUTING 3D MESHES TEXTURE. To reproduce just draw a memory texture and then a 3d mesh");
	
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
		rde_rendering_transform_to_glm_mat4_2d(&_t, _transformation_matrix);

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
		rde_math_convert_world_size_to_screen_size(current_drawing_window, &_texture_tile_size_on_screen);

		rde_vec_2F _size_in_screen = (rde_vec_2F) { (float)(_char_info.advance.x >> 6) - _char_info.bearing.x, 0.f};
		rde_math_convert_world_size_to_screen_size(current_drawing_window, &_size_in_screen);

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

void rde_rendering_end_drawing_2d() {
	rde_rendering_flush_batch_2d();
	rde_rendering_reset_batch_2d();
	current_drawing_camera = NULL;
	current_drawing_window = NULL;
	statistics.number_of_drawcalls = 0;
}

#endif