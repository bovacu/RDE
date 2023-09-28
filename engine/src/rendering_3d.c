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

void rde_rendering_generate_gl_vertex_config_for_mesh_3d(rde_mesh* _mesh) {
	glGenVertexArrays(1, &_mesh->vao);
	glBindVertexArray(_mesh->vao);
	
	glGenBuffers(1, &_mesh->vbo[0]); // vertices
	glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[0]);
	size_t _vertex_size = sizeof(float) * _mesh->vertex_count;
	glBufferData(GL_ARRAY_BUFFER, _vertex_size, _mesh->vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &_mesh->vbo[1]); // colors
	glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[1]);
	size_t _colors_size = sizeof(unsigned int) * _mesh->vertex_count;
	glBufferData(GL_ARRAY_BUFFER, _colors_size, _mesh->vertex_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &_mesh->ibo); // indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
	size_t _indices_size = sizeof(unsigned int) * _mesh->indices_count;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_size, _mesh->indices, GL_STATIC_DRAW);

	rde_util_check_opengl_error("ERROR: 0");
	rde_log_level(RDE_LOG_LEVEL_INFO, "Mesh initiated with data vao: %d, vbo_vertices: %d, vbo_colors: %d", _mesh->vao, _mesh->vbo[0], _mesh->vbo[1]);
}

rde_mesh rde_struct_create_mesh(size_t _vertex_count, float* _vertices, unsigned int* _colors, size_t _indices_count, unsigned int* _indices) {
	rde_mesh _mesh;

	_mesh.vertex_count = _vertex_count;
	_mesh.vertices = _vertices;
	_mesh.vertex_colors = _colors;
	_mesh.indices_count = _indices_count;
	_mesh.indices = _indices;

	rde_rendering_generate_gl_vertex_config_for_mesh_3d(&_mesh);

	return _mesh;
}

void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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

void rde_rendering_draw_mesh_3d(const rde_transform* _transform, const rde_mesh* _mesh, rde_shader* _shader) {
	(void)_transform;

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;
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

	glUniformMatrix4fv(glGetUniformLocation(_drawing_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_model_view_projection_matrix);
	rde_util_check_opengl_error("After UseProgram");

	glBindVertexArray(_mesh->vao);
	rde_util_check_opengl_error("After glBindVertexArray");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
	glDrawElements(GL_TRIANGLES, _mesh->indices_count, GL_UNSIGNED_INT, 0);
}

void rde_rendering_end_drawing_3d() {
	glDisable(GL_DEPTH_TEST);
	current_drawing_camera = NULL;
}