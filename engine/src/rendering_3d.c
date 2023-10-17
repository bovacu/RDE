#ifdef RDE_RENDERING_MODULE

#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR 1
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD 2

#define RDE_DEFAULT_TEXTURE_SIZE 64
#define RDE_DEFAULT_TEXTURE_CHANNELS 3
static rde_texture* DEFAULT_TEXTURE;

#define RDE_MAX_MODELS_PER_DRAW 1000

static rde_batch_3d current_batch_3d;

#include "rendering_3d_default_meshes.c"

void rde_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats);
void rde_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh);
void rde_rendering_flush_batch_3d();
void rde_rendering_reset_batch_3d();

void rde_rendering_init_3d() {
	DEFAULT_TEXTURE = rde_rendering_create_memory_texture(RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_CHANNELS);
	
	for(int _y = 0; _y < RDE_DEFAULT_TEXTURE_SIZE; _y++) {
		for(int _x = 0; _x < RDE_DEFAULT_TEXTURE_SIZE; _x++) {
			rde_rendering_memory_texture_set_pixel(DEFAULT_TEXTURE, (rde_vec_2I) {_x, _y}, RDE_COLOR_WHITE);
		}
	}

	rde_rendering_memory_texture_gpu_upload(DEFAULT_TEXTURE);
	rde_util_check_opengl_error("ERROR: Creating default texture");

	current_batch_3d.mesh = NULL;
	current_batch_3d.shader = NULL;
	current_batch_3d.amount_of_models_per_draw = 0;

}

void rde_rendering_end_3d() {
	rde_rendering_destroy_memory_texture(DEFAULT_TEXTURE);
}

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

float* rde_rendering_mesh_calculate_normals(float* _vertex_positions, size_t _indices_count, size_t _vertex_count, unsigned int* _indices) {
	size_t _normals_size = _vertex_count * 3;
	float* _normals = (float*)malloc(sizeof(float) * _normals_size);
	memset(_normals, 0, _normals_size);

	for(unsigned int _i = 0; _i < _indices_count; _i += 3) {
		vec3 _a = (vec3) { _vertex_positions[_indices[_i + 0] + 0], _vertex_positions[_indices[_i + 0] + 1], _vertex_positions[_indices[_i + 0] + 2] };
		vec3 _b = (vec3) { _vertex_positions[_indices[_i + 1] + 0], _vertex_positions[_indices[_i + 1] + 1], _vertex_positions[_indices[_i + 1] + 2] };
		vec3 _c = (vec3) { _vertex_positions[_indices[_i + 2] + 0], _vertex_positions[_indices[_i + 2] + 1], _vertex_positions[_indices[_i + 2] + 2] };

		vec3 _c_b = (vec3) { _c[0] - _b[0], _c[1] - _b[1], _c[2] - _b[2] };
		vec3 _a_b = (vec3) { _a[0] - _b[0], _a[1] - _b[1], _a[2] - _b[2] };

		vec3 _cross_product = GLM_VEC3_ZERO;
		glm_cross(_c_b, _a_b, _cross_product);
		glm_normalize(_cross_product);

		_normals[_indices[_i + 0] + 0] += _cross_product[0];
		_normals[_indices[_i + 0] + 1] += _cross_product[1];
		_normals[_indices[_i + 0] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 0], _normals[_indices[_i + 0] + 0], _normals[_indices[_i + 0] + 1], _normals[_indices[_i + 0] + 2]);

		_normals[_indices[_i + 1] + 0] += _cross_product[0];
		_normals[_indices[_i + 1] + 1] += _cross_product[1];
		_normals[_indices[_i + 1] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 1], _normals[_indices[_i + 1] + 0], _normals[_indices[_i + 1] + 1], _normals[_indices[_i + 1] + 2]);

		_normals[_indices[_i + 2] + 0] += _cross_product[0];
		_normals[_indices[_i + 2] + 1] += _cross_product[1];
		_normals[_indices[_i + 2] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 2], _normals[_indices[_i + 2] + 0], _normals[_indices[_i + 2] + 1], _normals[_indices[_i + 2] + 2]);

		// rde_log_level(RDE_LOG_LEVEL_INFO, "Doing triangle of indices (%d, %d, %d)", _indices[_i + 0], _indices[_i + 1], _indices[_i + 2]);
	}

	// rde_log_level(RDE_LOG_LEVEL_INFO, "Indices: %d", _indices_count);
	for(unsigned int _i = 0; _i < _indices_count; _i++) {
		vec3 _normal = (vec3) { _normals[_indices[_i] + 0], _normals[_indices[_i] + 1], _normals[_indices[_i] + 2] };
		glm_normalize(_normal);
		_normals[_indices[_i] + 0] = _normal[0];
		_normals[_indices[_i] + 1] = _normal[1];
		_normals[_indices[_i] + 2] = _normal[2];

		// rde_log_level(RDE_LOG_LEVEL_INFO, "vertex index: %d, normal(%f, %f, %f)", _i, _normal[0], _normal[1], _normal[2]);
	}

	return _normals;
}

rde_mesh rde_struct_create_mesh(size_t _vertex_count, size_t _indices_count) {
	rde_mesh _mesh;
	_mesh.vao = 0;
	
	GLuint _vao = 0;
	glGenVertexArrays(1, &_vao);
	rde_util_check_opengl_error("ERROR: Creating mesh");

	_mesh.vao = _vao;
	_mesh.vertex_count = _vertex_count;
	_mesh.vertex_positions = NULL;
	_mesh.vertex_colors = NULL;
	_mesh.index_count = _indices_count;
	_mesh.vertex_normals = NULL;
	_mesh.vertex_texture_coordinates = NULL;
	_mesh.indices = NULL;
	_mesh.texture = NULL;
	_mesh.transforms = NULL;
	_mesh.transforms = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW );
	memset(_mesh.transforms, 0, RDE_MAX_MODELS_PER_DRAW);

	_mesh.vbo[0] = RDE_UINT_MAX;
	_mesh.vbo[1] = RDE_UINT_MAX;
	_mesh.vbo[2] = RDE_UINT_MAX;
	_mesh.vbo[3] = RDE_UINT_MAX;

	_mesh.ibo = RDE_UINT_MAX;

	_mesh.free_vertex_positions_on_end = false;
	_mesh.free_vertex_colors_on_end = false;
	_mesh.free_vertex_normals_on_end = false;
	_mesh.free_vertex_texture_coordinates_on_end = false;
	_mesh.free_vertex_texture_on_end = false;
	_mesh.free_indices_on_end = false;

	glBindVertexArray(_mesh.vao);
	glGenBuffers(1, &_mesh.vbo[4]);
	glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return _mesh;
}

rde_mesh* rde_struct_create_memory_mesh(size_t _vertex_count, size_t _index_count) {
	rde_critical_error(_vertex_count <= 3 || _index_count <= 3, RDE_ERROR_BAD_MESH_DATA, _vertex_count, _index_count);

	rde_mesh* _mesh = (rde_mesh*)malloc(sizeof(rde_mesh));
	_mesh->vao = 0;

	glGenVertexArrays(1, &_mesh->vao);
	rde_util_check_opengl_error("ERROR: Creating mesh Ptr");

	_mesh->vertex_count = _vertex_count;
	_mesh->vertex_positions = NULL;
	_mesh->vertex_colors = NULL;
	_mesh->index_count = _index_count;
	_mesh->vertex_normals = NULL;
	_mesh->vertex_texture_coordinates = NULL;
	_mesh->indices = NULL;
	_mesh->texture = NULL;
	_mesh->transforms = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW);
	memset(_mesh->transforms, 0, RDE_MAX_MODELS_PER_DRAW);

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
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	rde_critical_error(_indices == NULL, RDE_ERROR_NO_NULL_ALLOWED, "indices");
	
	size_t _indices_size = sizeof(unsigned int) * _mesh->index_count;
	glBindVertexArray(_mesh->vao);
	if(_mesh->ibo == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices_size, _indices, GL_STATIC_DRAW);
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
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	rde_critical_error(_positions == NULL, RDE_ERROR_NO_NULL_ALLOWED, "positions");

	size_t _positions_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[0] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, _positions_size, _positions, GL_STATIC_DRAW);
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
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	rde_critical_error(_colors == NULL, RDE_ERROR_NO_NULL_ALLOWED, "colors");

	size_t _colors_size = sizeof(unsigned int) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[1] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, _colors_size, _colors, GL_STATIC_DRAW);
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
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	rde_critical_error(_normals == NULL, RDE_ERROR_NO_NULL_ALLOWED, "normals");

	size_t _normals_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
	glBindVertexArray(_mesh->vao);
	rde_util_check_opengl_error("rde_rendering_mesh_set_vertex_normals: VAO");
	if(_mesh->vbo[2] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[2]);
		rde_util_check_opengl_error("rde_rendering_mesh_set_vertex_normals: VBO");
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[2]);
		rde_util_check_opengl_error("rde_rendering_mesh_set_vertex_normals: BIND VBO");
		glBufferData(GL_ARRAY_BUFFER, _normals_size, _normals, GL_STATIC_DRAW);
		rde_util_check_opengl_error("rde_rendering_mesh_set_vertex_normals: BUFFER DATA VBO");
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

void rde_rendering_mesh_set_vertex_texture_data(rde_mesh* _mesh, unsigned int _texture_coords_size, float* _texture_coords, rde_texture* _texture, bool _free_texture_coords_on_destroy) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	rde_critical_error(_texture_coords == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture coordinates");
	rde_critical_error(_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture");

	size_t _texture_coords_array_size = sizeof(unsigned int) * _texture_coords_size * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
	glBindVertexArray(_mesh->vao);
	if(_mesh->vbo[3] == RDE_UINT_MAX) {
		glGenBuffers(1, &_mesh->vbo[3]);
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, _texture_coords_array_size, _texture_coords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _texture_coords_array_size, _texture_coords);
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

	// if(_mesh->vertex_normals == NULL) {
	// 	float* _normals = rde_rendering_mesh_calculate_normals(_mesh->vertex_positions, _mesh->index_count, _mesh->vertex_count, _mesh->indices);
	// 	rde_rendering_mesh_set_vertex_normals(_mesh, _normals, true);
	// }

	return true;
}

void rde_rendering_destroy_mesh(rde_mesh* _mesh) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");

	if(_mesh->vbo[0] != RDE_UINT_MAX) {
		glDeleteBuffers(1, &_mesh->vbo[0]);
		_mesh->vbo[0] = RDE_UINT_MAX;
	}
	
	if(_mesh->vbo[1] != RDE_UINT_MAX) {
		glDeleteBuffers(1, &_mesh->vbo[1]);
		_mesh->vbo[1] = RDE_UINT_MAX;
	}

	if(_mesh->vbo[2] != RDE_UINT_MAX) {
		glDeleteBuffers(1, &_mesh->vbo[2]);
		_mesh->vbo[2] = RDE_UINT_MAX;
	}
	
	if(_mesh->vbo[3] != RDE_UINT_MAX) {
		glDeleteBuffers(1, &_mesh->vbo[3]);
		_mesh->vbo[3] = RDE_UINT_MAX;
	}

	if(_mesh->vbo[4] != RDE_UINT_MAX) {
		glDeleteBuffers(1, &_mesh->vbo[4]);
		_mesh->vbo[4] = RDE_UINT_MAX;
	}

	glDeleteBuffers(1, &_mesh->ibo);

	glDeleteVertexArrays(1, &_mesh->vao);
	
	if(_mesh->free_vertex_positions_on_end) {
		free(_mesh->vertex_positions);
		_mesh->vertex_positions = NULL;
	}

	if(_mesh->free_vertex_colors_on_end) {
		free(_mesh->vertex_colors);
		_mesh->vertex_colors = NULL;
	}

	if(_mesh->free_vertex_normals_on_end) {
		free(_mesh->vertex_normals);
		_mesh->vertex_normals = NULL;
	}

	if(_mesh->free_vertex_texture_coordinates_on_end) {
		free(_mesh->vertex_texture_coordinates);
		_mesh->vertex_texture_coordinates = NULL;
	}

	if(_mesh->free_indices_on_end) {
		free(_mesh->indices);
		_mesh->indices = NULL;
	}

	if(_mesh->texture != NULL && _mesh->texture != DEFAULT_TEXTURE) {
		rde_rendering_unload_texture(_mesh->texture);
		_mesh->texture = NULL;
	}

	free(_mesh->transforms);
	_mesh->transforms = NULL;
}


void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	current_drawing_camera = _camera;
	current_drawing_window = _window;

	rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.y : _aspect_ratios.x;
	glm_perspective(_camera->fov, _aspect_ratio, _camera->near_far.x, _camera->near_far.y, projection_matrix);
}

void rde_rendering_draw_line_3d(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED("rde_rendering_draw_line_3d")
}

void rde_rendering_reset_batch_3d() {
	current_batch_3d.shader = NULL;

	memset(current_batch_3d.mesh->transforms, 0, current_batch_3d.amount_of_models_per_draw);

	current_batch_3d.mesh = NULL;
	current_batch_3d.amount_of_models_per_draw = 0;
}

void rde_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh) {
	if(current_batch_3d.shader == NULL) {
		current_batch_3d.shader = _shader;
	}

	if(current_batch_3d.mesh == NULL && _mesh != NULL) {
		current_batch_3d.mesh = _mesh;
	}
}

void rde_rendering_flush_batch_3d() {
	if(current_batch_3d.shader == NULL || current_batch_3d.mesh == NULL || current_batch_3d.amount_of_models_per_draw == 0) {
		return;
	}

	rde_util_check_opengl_error("GL Error Before Flush3D");

	rde_mesh* _mesh = current_batch_3d.mesh;
	rde_shader* _shader = current_batch_3d.shader;

	if(!rde_rendering_is_mesh_ok_to_render(_mesh)) {
		return;
	}

	glUseProgram(current_batch_3d.shader->compiled_program_id);
	rde_util_check_opengl_error("After glUseProgram");

	mat4 _model_view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform.position;
	rde_vec_3F _cam_direction = current_drawing_camera->direction;
	rde_vec_3F _cam_up = current_drawing_camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);

	glm_mat4_mul(projection_matrix, _view_matrix, _model_view_projection_matrix);

	rde_texture* _texture_to_draw = _mesh->texture != NULL ? _mesh->texture : DEFAULT_TEXTURE;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture_to_draw->opengl_texture_id);
	rde_util_check_opengl_error("After glBindTexture");

	glUniformMatrix4fv(glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_model_view_projection_matrix);
	rde_util_check_opengl_error("After Set Model Matrix Unform");

	rde_vec_3F _camera_pos = current_drawing_camera->transform.position;
	glUniform3f(glGetUniformLocation(_shader->compiled_program_id, "camera_pos"), _camera_pos.x, _camera_pos.y, _camera_pos.z);
	
	rde_vec_3F _dl_direction = ENGINE.directional_light.direction;
	rde_vec_3F _dl_ambient = ENGINE.directional_light.ambient_color;
	rde_vec_3F _dl_diffuse = ENGINE.directional_light.diffuse_color;
	rde_vec_3F _dl_specular = ENGINE.directional_light.specular_color;

	glUniform3f(glGetUniformLocation(_shader->compiled_program_id, "directional_light.direction"), _dl_direction.x, _dl_direction.y, _dl_direction.z);
	glUniform3f(glGetUniformLocation(_shader->compiled_program_id, "directional_light.ambient_color"), _dl_ambient.x, _dl_ambient.y, _dl_ambient.z);
	glUniform3f(glGetUniformLocation(_shader->compiled_program_id, "directional_light.diffuse_color"), _dl_diffuse.x, _dl_diffuse.y, _dl_diffuse.z);
	glUniform3f(glGetUniformLocation(_shader->compiled_program_id, "directional_light.specular_color"), _dl_specular.x, _dl_specular.y, _dl_specular.z);

	rde_material _material = rde_struct_create_material();
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.shininess"), _material.shininess);

	glBindVertexArray(_mesh->vao);
	rde_util_check_opengl_error("After glBindVertexArray");

	glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[4]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, current_batch_3d.amount_of_models_per_draw * (sizeof(float) * 16), _mesh->transforms);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->ibo);

	glDrawElementsInstanced(GL_TRIANGLES, _mesh->index_count, GL_UNSIGNED_INT, 0, current_batch_3d.amount_of_models_per_draw);
}

void rde_rendering_draw_mesh_3d(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader) {
	const size_t _floats_per_matrix = 1;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;
	rde_rendering_try_create_batch_3d(_drawing_shader, _mesh);
	rde_rendering_try_flush_batch_3d(_drawing_shader, _mesh, _floats_per_matrix);
	
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	rde_rendering_transform_to_glm_mat4_3d(_transform, _transformation_matrix);

	glm_mat4_copy(_transformation_matrix, _mesh->transforms[current_batch_3d.amount_of_models_per_draw]);
	current_batch_3d.amount_of_models_per_draw++;
}

void rde_rendering_draw_model_3d(const rde_transform* _transform, rde_model* _model, rde_shader* _shader) {
	for(uint _i = 0; _i < _model->mesh_array_size; _i++) {
		rde_mesh* _mesh = &_model->mesh_array[_i];
		rde_rendering_draw_mesh_3d(_transform, _mesh, _shader);
	}
}

void rde_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats) {
	bool _models_ok = current_batch_3d.amount_of_models_per_draw + 1 <= RDE_MAX_MODELS_PER_DRAW;
	bool _shader_ok = current_batch_3d.shader == _shader;
	bool _transforms_ok = current_batch_3d.amount_of_models_per_draw * _extra_floats + _extra_floats <= RDE_MAX_MODELS_PER_DRAW * _extra_floats;
	if(_models_ok && _shader_ok && _transforms_ok) {
		return;
	}

	rde_rendering_flush_batch_3d();
	rde_rendering_reset_batch_3d();
	rde_rendering_try_create_batch_3d(_shader, _mesh);
}

void rde_rendering_end_drawing_3d() {
	rde_rendering_flush_batch_3d();
	rde_rendering_reset_batch_3d();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	current_drawing_camera = NULL;
}

void rde_rendering_lighting_set_directional_light_direction(rde_vec_3F _direction) {
	ENGINE.directional_light.direction = _direction;
}

void rde_rendering_lighting_set_directional_light_ambient_color(rde_color _ambient_color) {
	ENGINE.directional_light.ambient_color = (rde_vec_3F) { _ambient_color.r / 255.f,
															_ambient_color.g / 255.f,
															_ambient_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_ambient_color_f(rde_vec_3F _ambient_color) {
	ENGINE.directional_light.ambient_color = _ambient_color;
}

void rde_rendering_lighting_set_directional_light_diffuse_color(rde_color _diffuse_color) {
	ENGINE.directional_light.diffuse_color = (rde_vec_3F) { _diffuse_color.r / 255.f,
															_diffuse_color.g / 255.f,
															_diffuse_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_diffuse_color_f(rde_vec_3F _diffuse_color) {
	ENGINE.directional_light.diffuse_color = _diffuse_color;
}


void rde_rendering_lighting_set_directional_light_specular_color(rde_color _specular_color) {
	ENGINE.directional_light.specular_color = (rde_vec_3F) { _specular_color.r / 255.f,
															 _specular_color.g / 255.f,
															 _specular_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_specular_color_f(rde_vec_3F _specular_color) {
	ENGINE.directional_light.specular_color = _specular_color;
}

size_t rde_rendering_get_mesh_vertices_count(rde_mesh* _mesh) {
	return _mesh->vertex_count;
}

size_t rde_rendering_get_model_vertices_count(rde_model* _model) {
	size_t _total_vertices = 0;
	
	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		_total_vertices += rde_rendering_get_mesh_vertices_count(&_model->mesh_array[_i]);
	}

	return _total_vertices;
}

void rde_rendering_unload_model(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "obj model");

	for(size_t _c = 0; _c < stbds_arrlenu(_model->mesh_array); _c++) {
		rde_mesh* _mesh = &_model->mesh_array[_c];
		rde_rendering_destroy_mesh(_mesh);
	}

	stbds_arrfree(_model->mesh_array);
	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;

	if(_model->material_array != NULL) {
		stbds_arrfree(_model->material_array);
		_model->material_array = NULL;
		_model->material_array_size = 0;
	}
}

#endif