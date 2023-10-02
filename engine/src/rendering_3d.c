#ifdef RDE_RENDERING_3D_MODULE

#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR 1
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD 2

void rde_rendering_transform_to_glm_mat4_3d(const rde_transform* _transform, mat4 _mat) {
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _transform->position.x, _transform->position.y, _transform->position.z });
	
	if(_transform->rotation.x != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.x), (vec3){ 1.0f, 0.0f, 0.0f });
	}

	if(_transform->rotation.y != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.y), (vec3){ 0.0f, 1.0f, 0.0f });
	}

	if(_transform->rotation.z != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	}

	glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y, _transform->scale.z });
	glm_mat4_copy(_transformation_matrix, _mat);
}

rde_mesh* rde_struct_create_mesh(size_t _vertex_count, size_t _index_count) {
	rde_critical_error(_vertex_count <= 3 || _index_count <= 3, -1, "Error while creating mesh, _vertex_count = %d and _index_count = %d. _vertex_count must be >= 3 and _index_count >= 3", _vertex_count, _index_count);

	rde_mesh* _mesh = (rde_mesh*)malloc(sizeof(rde_mesh));
	glGenVertexArrays(1, &_mesh->vao);
	rde_util_check_opengl_error("ERROR: Creating mesh");

	_mesh->vertex_count = _vertex_count;
	_mesh->vertex_positions = NULL;
	_mesh->vertex_colors = NULL;
	_mesh->index_count = _index_count;
	_mesh->vertex_normals = NULL;
	_mesh->vertex_texture_coordinates = NULL;
	_mesh->indices = NULL;
	_mesh->texture = NULL;

	_mesh->vbo[0] = RDE_UINT_MAX;
	_mesh->vbo[1] = RDE_UINT_MAX;
	_mesh->vbo[2] = RDE_UINT_MAX;
	_mesh->vbo[3] = RDE_UINT_MAX;

	_mesh->ibo = RDE_UINT_MAX;

	_mesh->free_vertex_positions_on_end = false;
	_mesh->free_vertex_colors_on_end = false;
	_mesh->free_vertex_normals_on_end = false;
	_mesh->free_vertex_texture_coordinates_on_end = false;
	_mesh->free_vertex_texture_on_end = false;
	_mesh->free_indices_on_end = false;

	return _mesh;
}

void rde_rendering_mesh_set_indices(rde_mesh* _mesh, unsigned int* _indices, bool _free_indices_on_destroy) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set indices data on a NULL mesh");
	rde_critical_error(_indices == NULL, -1, "Tried to set NULL indices data on a mesh");
	
	size_t _indices_size = sizeof(unsigned int) * _mesh->index_count;
	glBindVertexArray(_mesh->vao);
	if(_mesh->ibo == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_size, _indices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices_size, _indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	_mesh->indices = _indices;
	_mesh->free_indices_on_end = _free_indices_on_destroy;
}

void rde_rendering_mesh_set_vertex_positions(rde_mesh* _mesh, float* _positions, bool _free_positions_on_destroy) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set positions data on a NULL mesh");
	rde_critical_error(_positions == NULL, -1, "Tried to set NULL positions data on a mesh");

	size_t _positions_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[0] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, _positions_size, _positions, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _positions_size, _positions);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	_mesh->vertex_positions = _positions;
	_mesh->free_vertex_positions_on_end = _free_positions_on_destroy;
}

void rde_rendering_mesh_set_vertex_colors(rde_mesh* _mesh, unsigned int* _colors, bool _free_colors_on_destroy) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set colors data on a NULL mesh");
	rde_critical_error(_colors == NULL, -1, "Tried to set NULL colors data on a mesh");

	size_t _colors_size = sizeof(unsigned int) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[1] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, _colors_size, _colors, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _colors_size, _colors);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	_mesh->vertex_colors = _colors;
	_mesh->free_vertex_colors_on_end = _free_colors_on_destroy;
}

void rde_rendering_mesh_set_vertex_normals(rde_mesh* _mesh, float* _normals, bool _free_normals_on_destroy) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set normals data on a NULL mesh");
	rde_critical_error(_normals == NULL, -1, "Tried to set NULL normals data on a mesh");

	size_t _normals_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[2] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[2]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, _normals_size, _normals, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _normals_size, _normals);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	_mesh->vertex_normals = _normals;
	_mesh->free_vertex_normals_on_end = _free_normals_on_destroy;
}

void rde_rendering_mesh_set_vertex_texture_data(rde_mesh* _mesh, float* _texture_coords, rde_texture* _texture, bool _free_texture_coords_on_destroy) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set texture data on a NULL mesh");
	rde_critical_error(_texture_coords == NULL, -1, "Tried to set NULL texture coordinates data on a mesh");
	rde_critical_error(_texture == NULL, -1, "Tried to set NULL texture data on a mesh");

	size_t _texture_coords_size = sizeof(unsigned int) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[3] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[3]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, _texture_coords_size, _texture_coords, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _texture_coords_size, _texture_coords);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	_mesh->vertex_texture_coordinates = _texture_coords;
	_mesh->texture = _texture;

	_mesh->free_vertex_texture_coordinates_on_end = _free_texture_coords_on_destroy;
}

bool rde_rendering_is_mesh_ok_to_render(rde_mesh* _mesh) {
	if(_mesh->indices == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to render a mesh without vertex positions, skipping rendering for this mesh");
		return false;
	}

	if(_mesh->vertex_positions == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to render a mesh without indices, skipping rendering for this mesh");
		return false;
	}

	if(_mesh->vertex_colors == NULL) {
		size_t _colors_size = sizeof(unsigned int) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR;
		unsigned int* _colors = (unsigned int*)malloc(_colors_size);
		for(size_t _i = 0; _i < _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR; _i++) {
			_colors[_i] = RDE_COLOR_TO_HEX_COLOR(RDE_COLOR_WHITE);
		}
		rde_rendering_mesh_set_vertex_colors(_mesh, _colors, true);
	}

	return true;
}

void rde_rendering_destroy_mesh(rde_mesh* _mesh) {
	rde_critical_error(_mesh == NULL, -1, "Tried to set texture data on a NULL mesh");

	glDeleteBuffers(1, &_mesh->vbo[0]);
	glDeleteBuffers(1, &_mesh->vbo[1]);
	glDeleteBuffers(1, &_mesh->vbo[2]);
	glDeleteBuffers(1, &_mesh->vbo[3]);

	glDeleteBuffers(1, &_mesh->ibo);

	glDeleteVertexArrays(1, &_mesh->vao);
	
	if(_mesh->free_vertex_positions_on_end) {
		free(_mesh->vertex_positions);
	}

	if(_mesh->free_vertex_colors_on_end) {
		free(_mesh->vertex_colors);
	}

	if(_mesh->free_vertex_normals_on_end) {
		free(_mesh->vertex_normals);
	}

	if(_mesh->free_vertex_texture_coordinates_on_end) {
		free(_mesh->vertex_texture_coordinates);
	}

	if(_mesh->free_indices_on_end) {
		free(_mesh->indices);
	}
}


void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	current_drawing_camera = _camera;
	current_drawing_window = _window;

	rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.y : _aspect_ratios.x;
	glm_perspective(45.f, _aspect_ratio, 0.1f, 10.f, projection_matrix);
}

void rde_rendering_draw_line_3d(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_3d")
}

void rde_rendering_draw_mesh_3d(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader) {
	(void)_transform;

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;

	if(!rde_rendering_is_mesh_ok_to_render(_mesh)) {
		return;
	}

	glUseProgram(_drawing_shader->compiled_program_id);

	mat4 _model_view_projection_matrix = GLM_MAT4_IDENTITY_INIT;
	mat4 _model_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_rendering_transform_to_glm_mat4_3d(_transform, _model_matrix);

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform.position;
	glm_lookat((vec3) {_cam_pos.x, _cam_pos.y + 2, _cam_pos.z}, 
			   (vec3) {_cam_pos.x + 0, _cam_pos.y + 0, _cam_pos.z - 4}, (vec3) {0, 1, 0}, _view_matrix);

	glm_mat4_mul(projection_matrix, _view_matrix, _model_view_projection_matrix);
	glm_mat4_mul(_model_view_projection_matrix, _model_matrix, _model_view_projection_matrix);

	if(_mesh->texture != NULL && _mesh->texture->opengl_texture_id >= 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _mesh->texture->opengl_texture_id);
		rde_util_check_opengl_error("After glBindTexture");
	}

	glUniformMatrix4fv(glGetUniformLocation(_drawing_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_model_view_projection_matrix);
	rde_util_check_opengl_error("After UseProgram");

	glBindVertexArray(_mesh->vao);
	rde_util_check_opengl_error("After glBindVertexArray");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
	glDrawElements(GL_TRIANGLES, _mesh->index_count, GL_UNSIGNED_INT, 0);
}

void rde_rendering_end_drawing_3d() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	current_drawing_camera = NULL;
}

#endif