#ifdef RDE_RENDERING_MODULE

#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR 1
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD 2

#define RDE_DEFAULT_TEXTURE_SIZE 64
#define RDE_DEFAULT_TEXTURE_CHANNELS 3

#define RDE_MAX_MODELS_PER_DRAW 1000
#define RDE_MAX_LINES_PER_DRAW 10000

#define RDE_OBJ_EXTENSION "obj"
#define RDE_FBX_EXTENSION "fbx"
#define RDE_GLTF_EXTENSION "gltf"

static rde_batch_3d current_batch_3d;
static rde_texture* DEFAULT_TEXTURE;

rde_mesh rde_inner_struct_create_mesh(rde_mesh_gen_data* _data);
void rde_inner_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats);
void rde_inner_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats);
void rde_inner_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh);
void rde_inner_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness);
void rde_inner_rendering_flush_batch_3d();
void rde_inner_rendering_flush_line_batch();
void rde_inner_rendering_reset_batch_3d();
void rde_inner_rendering_reset_line_batch();
void rde_inner_create_line_batch_buffers();
void rde_inner_destroy_line_batch_buffers();
void rde_inner_rendering_init_3d();
void rde_inner_rendering_end_3d();
void rde_inner_rendering_transform_to_glm_mat4_3d(const rde_transform* _transform, mat4 _mat);
float* rde_inner_rendering_mesh_calculate_normals(float* _vertex_positions, size_t _indices_count, size_t _vertex_count, uint* _indices);
bool rde_inner_rendering_is_mesh_ok_to_render(rde_mesh* _mesh);

#include "rendering_3d_default_meshes.c"
#include "fbx_importer.c"
#include "obj_importer.c"

void rde_inner_create_line_batch_buffers() {
	RDE_CHECK_GL(glGenVertexArrays, 1, &current_batch_3d.line_batch.vertex_array_object);
	RDE_CHECK_GL(glBindVertexArray, current_batch_3d.line_batch.vertex_array_object);
	
	RDE_CHECK_GL(glGenBuffers, 1, &current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW, NULL, GL_DYNAMIC_DRAW);

	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_line_vertex), (const void*)0);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	
	RDE_CHECK_GL(glVertexAttribPointer, 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_line_vertex), (const void*)(sizeof(float) * 3));
	RDE_CHECK_GL(glEnableVertexAttribArray, 1);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
}

void rde_inner_destroy_line_batch_buffers() {
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glDeleteBuffers, 1, &current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glDeleteVertexArrays, 1, &current_batch_3d.line_batch.vertex_array_object);

	free(current_batch_3d.line_batch.vertices);
	current_batch_3d.line_batch.vertices = NULL;
}

void rde_inner_rendering_init_3d() {
	DEFAULT_TEXTURE = rde_rendering_memory_texture_create(RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_CHANNELS);
	
	for(int _y = 0; _y < RDE_DEFAULT_TEXTURE_SIZE; _y++) {
		for(int _x = 0; _x < RDE_DEFAULT_TEXTURE_SIZE; _x++) {
			rde_rendering_memory_texture_set_pixel(DEFAULT_TEXTURE, (rde_vec_2I) {_x, _y}, RDE_COLOR_WHITE);
		}
	}

	rde_rendering_memory_texture_gpu_upload(DEFAULT_TEXTURE);
	current_batch_3d = rde_struct_create_batch_3d();
	current_batch_3d.line_batch.vertices = (rde_line_vertex*)malloc(sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW);
	rde_inner_create_line_batch_buffers();
}

void rde_inner_rendering_end_3d() {
	if(DEFAULT_TEXTURE != NULL) {
		rde_rendering_memory_texture_destroy(DEFAULT_TEXTURE);
	}
	rde_inner_destroy_line_batch_buffers();
}

void rde_inner_rendering_transform_to_glm_mat4_3d(const rde_transform* _transform, mat4 _mat) {
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

bool rde_inner_rendering_is_mesh_ok_to_render(rde_mesh* _mesh) {
	if(_mesh->vertex_positions == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to render a mesh without positions, skipping rendering for this mesh");
		return false;
	}

	return true;
}

float* rde_inner_rendering_mesh_calculate_normals(float* _vertex_positions, size_t _indices_count, size_t _vertex_count, uint* _indices) {
	size_t _normals_size = _vertex_count * 3;
	float* _normals = (float*)malloc(sizeof(float) * _normals_size);
	for(size_t _i = 0; _i < _normals_size; _i++) {
		_normals[_i] = 0;
	}

	for(uint _i = 0; _i < _indices_count; _i += 3) {
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
	for(uint _i = 0; _i < _indices_count; _i++) {
		vec3 _normal = (vec3) { _normals[_indices[_i] + 0], _normals[_indices[_i] + 1], _normals[_indices[_i] + 2] };
		glm_normalize(_normal);
		_normals[_indices[_i] + 0] = _normal[0];
		_normals[_indices[_i] + 1] = _normal[1];
		_normals[_indices[_i] + 2] = _normal[2];

		// rde_log_level(RDE_LOG_LEVEL_INFO, "vertex index: %d, normal(%f, %f, %f)", _i, _normal[0], _normal[1], _normal[2]);
	}

	return _normals;
}

rde_mesh rde_inner_struct_create_mesh(rde_mesh_gen_data* _data) {
	rde_mesh _mesh;
	_mesh.vao = 0;

	memset(_mesh.name, 0, RDE_MESH_NAME_MAX);
	rde_strcat(_mesh.name, RDE_MESH_NAME_MAX, _data->name);
	
	GLuint _vao = 0;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);

	_mesh.vao = _vao;
	_mesh.vertex_count = _data->vertex_count;
	_mesh.vertex_positions = _data->positions;
	_mesh.vertex_normals = _data->normals;
	_mesh.vertex_texcoords = _data->texcoords;
	
	_mesh.transformation_matrices = NULL;
	_mesh.transformation_matrices = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW );
	for(size_t _i = 0; _i < RDE_MAX_MODELS_PER_DRAW; _i++) {
		glm_mat4_zero(_mesh.transformation_matrices[_i]);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh.vao);
	
	size_t _positions_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[0]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[0]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _positions_size, _mesh.vertex_positions, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	if(_data->normals != NULL && _data->normals_size > 0) {
		size_t _normals_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[1]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[1]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _normals_size, _mesh.vertex_normals, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 1);
		RDE_CHECK_GL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	if(_data->texcoords != NULL && _data->texcoords_size > 0) {
		size_t _texcoords_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[2]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[2]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _texcoords_size, _mesh.vertex_texcoords, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 2);
		RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[3]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[3]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	
	RDE_CHECK_GL(glEnableVertexAttribArray, 3);
	RDE_CHECK_GL(glVertexAttribPointer, 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);

	RDE_CHECK_GL(glEnableVertexAttribArray, 4);
	RDE_CHECK_GL(glVertexAttribPointer, 4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));

	RDE_CHECK_GL(glEnableVertexAttribArray, 5);
	RDE_CHECK_GL(glVertexAttribPointer, 5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));

	RDE_CHECK_GL(glEnableVertexAttribArray, 6);
	RDE_CHECK_GL(glVertexAttribPointer, 6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));

	RDE_CHECK_GL(glVertexAttribDivisor, 3, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 4, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 5, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 6, 1);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

	_mesh.material = rde_struct_create_material();
	_mesh.material.map_ka = _data->material.map_ka;
	_mesh.material.map_kd = _data->material.map_kd;
	_mesh.material.map_ks = _data->material.map_ks;
	_mesh.material.map_bump = _data->material.map_bump;
	_mesh.material.material_light_data = _data->material.material_light_data;


	rde_log_level(RDE_LOG_LEVEL_INFO, "Created mesh with %zu vertices. VAO: %u\n", _mesh.vertex_count, _vao);

	return _mesh;
}

void rde_inner_rendering_reset_line_batch() {
	for(size_t _i = 0; _i < RDE_MAX_LINES_PER_DRAW; _i++) {
		current_batch_3d.line_batch.vertices[_i] = rde_struct_create_line_vertex();
	}

	current_batch_3d.line_batch.amount_of_vertices = 0;
	current_batch_3d.line_batch.shader = NULL;
	current_batch_3d.line_batch.thickness = 0;
}

void rde_inner_rendering_reset_batch_3d() {
	current_batch_3d.shader = NULL;

	if(current_batch_3d.mesh != NULL) {
		for(size_t _i = 0; _i < current_batch_3d.amount_of_models_per_draw; _i++) {
			glm_mat4_zero(current_batch_3d.mesh->transformation_matrices[_i]);
		}
	}

	current_batch_3d.mesh = NULL;
	current_batch_3d.amount_of_models_per_draw = 0;
}

void rde_inner_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness) {
	if(current_batch_3d.line_batch.shader == NULL) {
		current_batch_3d.line_batch.shader = _shader;
	}

	if(current_batch_3d.line_batch.thickness == 0 ) {
		current_batch_3d.line_batch.thickness = _thickness;
	}
}

void rde_inner_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh) {
	if(current_batch_3d.shader == NULL) {
		current_batch_3d.shader = _shader;
	}

	if(current_batch_3d.mesh == NULL && _mesh != NULL) {
		current_batch_3d.mesh = _mesh;
	}
}

void rde_inner_rendering_flush_line_batch() {
	if(current_batch_3d.line_batch.shader == NULL || current_batch_3d.line_batch.amount_of_vertices == 0) {
		return;
	}

	if(current_batch_3d.line_batch.thickness <= 0) {
		current_batch_3d.line_batch.thickness = 1;
	}
	RDE_CHECK_GL(glLineWidth, current_batch_3d.line_batch.thickness);

	rde_shader* _shader = current_batch_3d.line_batch.shader;
	RDE_CHECK_GL(glUseProgram, _shader->compiled_program_id);

	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform.position;
	rde_vec_3F _cam_direction = current_drawing_camera->direction;
	rde_vec_3F _cam_up = current_drawing_camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);
	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);

	RDE_CHECK_GL(glBindVertexArray, current_batch_3d.line_batch.vertex_array_object);
	
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBufferSubData, GL_ARRAY_BUFFER, 0, current_batch_3d.line_batch.amount_of_vertices * sizeof(rde_line_vertex), current_batch_3d.line_batch.vertices);

	RDE_CHECK_GL(glDrawArrays, GL_LINES, 0, current_batch_3d.line_batch.amount_of_vertices);
}

void rde_inner_rendering_flush_batch_3d() {
	if(current_batch_3d.shader == NULL || current_batch_3d.mesh == NULL || current_batch_3d.amount_of_models_per_draw == 0) {
		return;
	}

	rde_mesh* _mesh = current_batch_3d.mesh;
	rde_shader* _shader = current_batch_3d.shader;

	if(!rde_inner_rendering_is_mesh_ok_to_render(_mesh)) {
		return;
	}

	RDE_CHECK_GL(glUseProgram, current_batch_3d.shader->compiled_program_id);

	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform.position;
	rde_vec_3F _cam_direction = current_drawing_camera->direction;
	rde_vec_3F _cam_up = current_drawing_camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);

	GLint _using_render_texture_location = glGetUniformLocation(_shader->compiled_program_id, "material.using_render_texture");
	if(_mesh->material.render_texture != NULL) {
		RDE_CHECK_GL(glUseProgram, current_batch_3d.shader->compiled_program_id);

		GLint _rt_location = glGetUniformLocation(_shader->compiled_program_id, "render_texture");
		RDE_CHECK_GL(glUniform1i, _rt_location, 0);

		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _mesh->material.render_texture->opengl_texture_id);

		RDE_CHECK_GL(glUniform1i, _using_render_texture_location, 1);
	} else {
		rde_texture* _ka_texture = _mesh->material.map_ka != NULL ? _mesh->material.map_ka : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _ka_texture->opengl_texture_id);

		rde_texture* _kd_texture = _mesh->material.map_kd != NULL ? _mesh->material.map_kd : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE1);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _kd_texture->opengl_texture_id);

		rde_texture* _ks_texture = _mesh->material.map_ks != NULL ? _mesh->material.map_ks : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE2);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _ks_texture->opengl_texture_id);

		rde_texture* _bump_texture = _mesh->material.map_bump != NULL ? _mesh->material.map_bump : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE3);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _bump_texture->opengl_texture_id);

		GLint _ka_location = glGetUniformLocation(_shader->compiled_program_id, "tex_ka");
		RDE_CHECK_GL(glUniform1i, _ka_location, 0);
		GLint _kd_location = glGetUniformLocation(_shader->compiled_program_id, "tex_kd");
		RDE_CHECK_GL(glUniform1i, _kd_location, 1);
		GLint _ks_location = glGetUniformLocation(_shader->compiled_program_id, "tex_ks");
		RDE_CHECK_GL(glUniform1i, _ks_location, 2);
		GLint _bump_location = glGetUniformLocation(_shader->compiled_program_id, "tex_bump");
		RDE_CHECK_GL(glUniform1i, _bump_location, 3);
		RDE_CHECK_GL(glUniform1i, _using_render_texture_location, 0);
	}

	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);

	rde_vec_3F _camera_pos = current_drawing_camera->transform.position;
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "camera_pos"), _camera_pos.x, _camera_pos.y, _camera_pos.z);
	
	rde_vec_3F _dl_direction = ENGINE.illumination.directional_light.direction;
	rde_vec_3F _dl_ambient = ENGINE.illumination.directional_light.ambient_color;
	rde_vec_3F _dl_diffuse = ENGINE.illumination.directional_light.diffuse_color;
	rde_vec_3F _dl_specular = ENGINE.illumination.directional_light.specular_color;

	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.direction"), _dl_direction.x, _dl_direction.y, _dl_direction.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.ambient_color"), _dl_ambient.x, _dl_ambient.y, _dl_ambient.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.diffuse_color"), _dl_diffuse.x, _dl_diffuse.y, _dl_diffuse.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.specular_color"), _dl_specular.x, _dl_specular.y, _dl_specular.z);

	rde_material_light_data _material = _mesh->material.material_light_data;
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, "material.shininess"), _material.shininess);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Ka"), _material.ka.x, _material.ka.y, _material.ka.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Kd"), _material.kd.x, _material.kd.y, _material.kd.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Ks"), _material.ks.x, _material.ks.y, _material.ks.z);

	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.amount_of_point_lights; _i++) {
		rde_point_light* _p = ENGINE.illumination.point_lights[_i];
		char _point_light_var[256];
		memset(_point_light_var, 0, 256);

		if(_p != NULL) {
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].position", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->position.x, _p->position.y, _p->position.z);
			memset(_point_light_var, 0, 256);
		
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].ambient_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->ambient_color.x, _p->ambient_color.y, _p->ambient_color.z);
			memset(_point_light_var, 0, 256);
		
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].diffuse_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->diffuse_color.x, _p->diffuse_color.y, _p->diffuse_color.z);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].specular_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->specular_color.x, _p->specular_color.y, _p->specular_color.z);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].constant", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->constant);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].linear", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->linear);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].quadratic", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->quadratic);
			memset(_point_light_var, 0, 256);
		}

		rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].used", _i);
		RDE_CHECK_GL(glUniform1i, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _i < ENGINE.init_info.illumination_config.amount_of_point_lights && _p != NULL ? 1 : -1);

		memset(_point_light_var, 0, 256);
	}

	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.amount_of_spot_lights; _i++) {
		rde_spot_light* _s = ENGINE.illumination.spot_lights[_i];
		char _spot_light_var[256];
		memset(_spot_light_var, 0, 256);

		if(_s != NULL) {
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].position", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->position.x, _s->position.y, _s->position.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].direction", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->direction.x, _s->direction.y, _s->direction.z);
			memset(_spot_light_var, 0, 256);
		
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].ambient_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->ambient_color.x, _s->ambient_color.y, _s->ambient_color.z);
			memset(_spot_light_var, 0, 256);
		
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].diffuse_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->diffuse_color.x, _s->diffuse_color.y, _s->diffuse_color.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].specular_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->specular_color.x, _s->specular_color.y, _s->specular_color.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].constant", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->constant);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].linear", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->linear);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].quadratic", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->quadratic);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].cut_off", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), cos(glm_rad(_s->cut_off)));
			memset(_spot_light_var, 0, 256);

			
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].outer_cut_off", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), cos(glm_rad(_s->outer_cut_off)));
			memset(_spot_light_var, 0, 256);
		}

		rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].used", _i);
		RDE_CHECK_GL(glUniform1i, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _i < ENGINE.init_info.illumination_config.amount_of_spot_lights && _s != NULL ? 1 : -1);

		memset(_spot_light_var, 0, 256);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh->vao);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[3]);
	RDE_CHECK_GL(glBufferSubData, GL_ARRAY_BUFFER, 0, current_batch_3d.amount_of_models_per_draw * (sizeof(float) * 16), _mesh->transformation_matrices);

	RDE_CHECK_GL(glDrawArraysInstanced, GL_TRIANGLES, 0, _mesh->vertex_count, current_batch_3d.amount_of_models_per_draw);
}

void rde_inner_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats) {
	bool _lines_ok = current_batch_3d.line_batch.amount_of_vertices + _extra_floats <= RDE_MAX_LINES_PER_DRAW;
	bool _shader_ok = current_batch_3d.line_batch.shader == _shader;
	bool _thickness_ok = current_batch_3d.line_batch.thickness == _thickness;
	if(_shader_ok && _lines_ok && _thickness_ok) {
		return;
	}

	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
	rde_inner_rendering_try_create_line_batch(_shader, _thickness);
}

void rde_inner_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats) {
	bool _models_ok = current_batch_3d.amount_of_models_per_draw + 1 <= RDE_MAX_MODELS_PER_DRAW;
	bool _shader_ok = current_batch_3d.shader == _shader;
	bool _mesh_ok = current_batch_3d.mesh == _mesh;
	bool _transforms_ok = current_batch_3d.amount_of_models_per_draw * _extra_floats + _extra_floats <= RDE_MAX_MODELS_PER_DRAW * _extra_floats;
	if(_models_ok && _shader_ok && _transforms_ok && _mesh_ok) {
		return;
	}

#if !IS_ANDROID()
    if(current_batch_3d.draw_mesh_wireframe) {
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE);
		rde_inner_rendering_flush_batch_3d();
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	rde_inner_rendering_flush_batch_3d();
	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_try_create_batch_3d(_shader, _mesh);
}

void rde_inner_rendering_flush_render_texture_3d() {
	rde_inner_rendering_flush_batch_3d();
	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
}




// ==============================================================================
// =									API										=
// ==============================================================================

rde_mesh* rde_struct_memory_mesh_create(rde_mesh_gen_data* _data) {
	rde_mesh* _mesh = (rde_mesh*)malloc(sizeof(rde_mesh));
	rde_critical_error(_mesh == NULL, "Could not create a memory mesh");
	_mesh->vao = 0;

	memset(_mesh->name, 0, RDE_MESH_NAME_MAX);
	
	if(_data->name != NULL) {
		rde_strcat(_mesh->name, RDE_MESH_NAME_MAX, _data->name);
	} else {
		rde_strcat(_mesh->name, RDE_MESH_NAME_MAX, "mesh");
	}
	
	GLuint _vao = 0;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);
	_mesh->vao = _vao;
	_mesh->vertex_count = _data->vertex_count;
	_mesh->vertex_positions = _data->positions;
	_mesh->vertex_normals = _data->normals;
	_mesh->vertex_texcoords = _data->texcoords;
	
	_mesh->transformation_matrices = NULL;
	_mesh->transformation_matrices = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW);
	for(size_t _i = 0; _i < RDE_MAX_MODELS_PER_DRAW; _i++) {
		glm_mat4_zero(_mesh->transformation_matrices[_i]);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh->vao);
	
	size_t _positions_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[0]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[0]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _positions_size, _mesh->vertex_positions, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	if(_data->normals != NULL && _data->normals_size > 0) {
		size_t _normals_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[1]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[1]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _normals_size, _mesh->vertex_normals, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 1);
		RDE_CHECK_GL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	if(_data->texcoords != NULL && _data->texcoords_size > 0) {
		size_t _texcoords_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[2]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[2]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _texcoords_size, _mesh->vertex_texcoords, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 2);
		RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[3]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[3]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	
	RDE_CHECK_GL(glEnableVertexAttribArray, 3);
	RDE_CHECK_GL(glVertexAttribPointer, 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);

	RDE_CHECK_GL(glEnableVertexAttribArray, 4);
	RDE_CHECK_GL(glVertexAttribPointer, 4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));

	RDE_CHECK_GL(glEnableVertexAttribArray, 5);
	RDE_CHECK_GL(glVertexAttribPointer, 5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));

	RDE_CHECK_GL(glEnableVertexAttribArray, 6);
	RDE_CHECK_GL(glVertexAttribPointer, 6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));

	RDE_CHECK_GL(glVertexAttribDivisor, 3, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 4, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 5, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 6, 1);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

	_mesh->material = rde_struct_create_material();
	_mesh->material.map_ka = _data->material.map_ka;
	_mesh->material.map_kd = _data->material.map_kd;
	_mesh->material.map_ks = _data->material.map_ks;
	_mesh->material.map_bump = _data->material.map_bump;
	_mesh->material.render_texture = _data->material.render_texture;
	_mesh->material.material_light_data = _data->material.material_light_data;


	rde_log_level(RDE_LOG_LEVEL_INFO, "Created mesh with %zu vertices. VAO: %u\n", _mesh->vertex_count, _vao);

	return _mesh;
}

rde_mesh* rde_rendering_mesh_create_cube(float _size, rde_material* _material) {
	CUBE_MESH_DATA(_size)

	rde_mesh_gen_data _data = {
		.name = "Cube",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_prism(rde_vec_3F _size, rde_material* _material) {
	PRISM_MESH_DATA(_size.x, _size.y, _size.z)

	rde_mesh_gen_data _data = {
		.name = "Cube",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_sphere(float _radius, rde_material* _material) {
	SPHERE_MESH_DATA(_radius)

	rde_mesh_gen_data _data = {
		.name = "Sphere",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_model* rde_rendering_model_load(const char* _model_path) {
	const char* _extension = rde_util_file_get_name_extension(_model_path);

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_number_of_models; _i++) {
		if (strlen(ENGINE.models[_i].file_path) != 0 && strcmp(ENGINE.models[_i].file_path, _model_path) == 0) {
			return &ENGINE.models[_i];
		}
	}

	if(strcmp(_extension, RDE_FBX_EXTENSION) == 0) {
		rde_critical_error(true, RDE_ERROR_FEATURE_NOT_SUPPORTED_YET, "rde_rendering_model_load for FBX");
	} else if(strcmp(_extension, RDE_OBJ_EXTENSION) == 0) {
#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_MODULE)
		return rde_inner_obj_load_model(_model_path);
#else
		rde_critical_error(true, RDE_ERROR_RENDERING_MODEL_MODULE_FORMAT_NOT_COMPILED, _model_path, _extension);
#endif
	} else if(strcmp(_extension, RDE_GLTF_EXTENSION) == 0) {
		rde_critical_error(true, RDE_ERROR_FEATURE_NOT_SUPPORTED_YET, "rde_rendering_model_load for GLTF");
	} else {
		rde_critical_error(true, RDE_ERROR_RENDERING_MODEL_UNSUPPORTED_FORMAT, _model_path, _extension);
	}

	return NULL;
}

void rde_rendering_mesh_destroy(rde_mesh* _mesh, bool _delete_allocated_buffers) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);

	// positions
	if(_mesh->vbo[0] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[0]);
		_mesh->vbo[0] = RDE_UINT_MAX;
	}
	
	// normals
	if(_mesh->vbo[1] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[1]);
		_mesh->vbo[1] = RDE_UINT_MAX;
	}

	// texcoords
	if(_mesh->vbo[2] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[2]);
		_mesh->vbo[2] = RDE_UINT_MAX;
	}
	
	// transforms
	if(_mesh->vbo[3] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[3]);
		_mesh->vbo[3] = RDE_UINT_MAX;
	}

	RDE_CHECK_GL(glDeleteVertexArrays, 1, &_mesh->vao);
	_mesh->vao = RDE_UINT_MAX;
	
	if(_delete_allocated_buffers) {
		free(_mesh->vertex_positions);
		_mesh->vertex_positions = NULL;

		free(_mesh->vertex_normals);
		_mesh->vertex_normals = NULL;

		free(_mesh->vertex_texcoords);
		_mesh->vertex_texcoords = NULL;
	}

	if(_mesh->material.map_ka != NULL && _mesh->material.map_ka != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_ka);
		_mesh->material.map_ka = NULL;
	}
	if(_mesh->material.map_kd != NULL && _mesh->material.map_kd != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_kd);
		_mesh->material.map_kd = NULL;
	}
	if(_mesh->material.map_ks != NULL && _mesh->material.map_ks != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_ks);
		_mesh->material.map_ks = NULL;
	}
	if(_mesh->material.map_bump != NULL && _mesh->material.map_bump != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_bump);
		_mesh->material.map_bump = NULL;
	}

	free(_mesh->transformation_matrices);
	_mesh->transformation_matrices = NULL;

	_mesh->material = rde_struct_create_material();
}


void rde_rendering_3d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _draw_wireframe_over_mesh) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDepthFunc, GL_LESS);

	RDE_CHECK_GL(glEnable, GL_CULL_FACE);
	RDE_CHECK_GL(glCullFace, GL_BACK);
	RDE_CHECK_GL(glFrontFace, GL_CCW);

	current_drawing_camera = _camera;
	current_drawing_window = _window;

	rde_vec_2F _aspect_ratios = rde_inner_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.y : _aspect_ratios.x;
	glm_perspective(_camera->fov, _aspect_ratio, _camera->near_far.x, _camera->near_far.y, projection_matrix);

	current_batch_3d.draw_mesh_wireframe = _draw_wireframe_over_mesh;
}

void rde_rendering_3d_draw_line(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, unsigned short _thickness, rde_shader* _shader) {
	const size_t _line_vertex_count = 2;

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.line_shader : _shader;
	rde_inner_rendering_try_create_line_batch(_drawing_shader, _thickness);
	rde_inner_rendering_try_flush_line_batch(_drawing_shader, _thickness, _line_vertex_count);
	int _c = RDE_COLOR_TO_HEX_COLOR(_color);

	rde_line_vertex _vertex_0;
	_vertex_0.position = (rde_vec_3F){ _init.x, _init.y, _init.z };
	_vertex_0.color = _c;
	current_batch_3d.line_batch.vertices[current_batch_3d.line_batch.amount_of_vertices++] = _vertex_0;

	rde_line_vertex _vertex_1;
	_vertex_1.position = (rde_vec_3F){ _end.x, _end.y, _end.z };
	_vertex_1.color = _c;
	current_batch_3d.line_batch.vertices[current_batch_3d.line_batch.amount_of_vertices++] = _vertex_1;
}

void rde_rendering_3d_draw_mesh(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader) {
	const size_t _floats_per_matrix = 1;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;
	rde_inner_rendering_try_create_batch_3d(_drawing_shader, _mesh);
	rde_inner_rendering_try_flush_batch_3d(_drawing_shader, _mesh, _floats_per_matrix);
	
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	rde_inner_rendering_transform_to_glm_mat4_3d(_transform, _transformation_matrix);

	glm_mat4_copy(_transformation_matrix, _mesh->transformation_matrices[current_batch_3d.amount_of_models_per_draw]);
	current_batch_3d.amount_of_models_per_draw++;
}

void rde_rendering_3d_draw_model(const rde_transform* _transform, rde_model* _model, rde_shader* _shader) {
	for(uint _i = 0; _i < _model->mesh_array_size; _i++) {
		rde_mesh* _mesh = &_model->mesh_array[_i];
		rde_rendering_3d_draw_mesh(_transform, _mesh, _shader);
	}
}

void rde_rendering_3d_end_drawing() {

#if !IS_ANDROID()
	if(current_batch_3d.draw_mesh_wireframe) {
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE);
		rde_inner_rendering_flush_batch_3d();
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	rde_inner_rendering_flush_batch_3d();

	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
	RDE_CHECK_GL(glDisable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDisable, GL_CULL_FACE);
	current_drawing_camera = NULL;
}

void rde_rendering_lighting_set_directional_light_direction(rde_vec_3F _direction) {
	ENGINE.illumination.directional_light.direction = _direction;
}

void rde_rendering_lighting_set_directional_light_ambient_color(rde_color _ambient_color) {
	ENGINE.illumination.directional_light.ambient_color = (rde_vec_3F) { _ambient_color.r / 255.f,
															_ambient_color.g / 255.f,
															_ambient_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_ambient_color_f(rde_vec_3F _ambient_color) {
	ENGINE.illumination.directional_light.ambient_color = _ambient_color;
}

void rde_rendering_lighting_set_directional_light_diffuse_color(rde_color _diffuse_color) {
	ENGINE.illumination.directional_light.diffuse_color = (rde_vec_3F) { _diffuse_color.r / 255.f,
															_diffuse_color.g / 255.f,
															_diffuse_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_diffuse_color_f(rde_vec_3F _diffuse_color) {
	ENGINE.illumination.directional_light.diffuse_color = _diffuse_color;
}


void rde_rendering_lighting_set_directional_light_specular_color(rde_color _specular_color) {
	ENGINE.illumination.directional_light.specular_color = (rde_vec_3F) { _specular_color.r / 255.f,
															 _specular_color.g / 255.f,
															 _specular_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_specular_color_f(rde_vec_3F _specular_color) {
	ENGINE.illumination.directional_light.specular_color = _specular_color;
}

rde_directional_light rde_rendering_lighting_get_directional_light() {
	return ENGINE.illumination.directional_light;
}

void rde_rendering_light_add_add_point_light(rde_point_light* _point_light) {
	bool _found = false;
	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.amount_of_point_lights; _i++) {
		if(ENGINE.illumination.point_lights[_i] == NULL) {
			ENGINE.illumination.point_lights[_i] = _point_light;
			_found = true;
			break;
		}
	}

	rde_critical_error(!_found, "Max point light amount '%d' reached", ENGINE.init_info.illumination_config.amount_of_point_lights);
}

void rde_rendering_light_add_add_spot_light(rde_spot_light* _spot_light) {
	bool _found = false;
	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.amount_of_spot_lights; _i++) {
		if(ENGINE.illumination.spot_lights[_i] == NULL) {
			ENGINE.illumination.spot_lights[_i] = _spot_light;
			_found = true;
			break;
		}
	}

	rde_critical_error(!_found, "Max spot light amount '%d' reached", ENGINE.init_info.illumination_config.amount_of_spot_lights);
}

rde_mesh_data rde_rendering_mesh_get_data(rde_mesh* _mesh) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	return (rde_mesh_data) {
		.amount_of_vertices = _mesh->vertex_count,
		.vertex_buffer_objects_ids = { _mesh->vbo[0],_mesh->vbo[1], _mesh->vbo[2], _mesh->vbo[3] },
	};
}

size_t rde_rendering_model_get_vertices_count(rde_model* _model) {
	size_t _total_vertices = 0;
	
	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		_total_vertices += rde_rendering_mesh_get_data(&_model->mesh_array[_i]).amount_of_vertices;
	}

	return _total_vertices;
}

void rde_rendering_model_set_light_data(rde_model* _model, rde_material_light_data _light_data) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "model");
	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		_model->mesh_array[_i].material.material_light_data = _light_data;
	}
}
rde_material_light_data rde_rendering_model_get_light_data(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "model");
	return _model->mesh_array[0].material.material_light_data;
}

rde_model_data rde_rendering_model_get_data(rde_model* _model) {
	rde_model_data _m;
	_m.amount_of_meshes = _model->mesh_array_size;
	_m.meshes = (rde_mesh**)malloc(sizeof(rde_mesh*) * _m.amount_of_meshes);

	for(size_t _i = 0; _i < _m.amount_of_meshes; _i++) {
		_m.meshes[_i] = &_model->mesh_array[_i];
	}

	return _m;
}

void rde_rendering_model_unload(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "obj model");

	for(size_t _c = 0; _c < stbds_arrlenu(_model->mesh_array); _c++) {
		rde_mesh* _mesh = &_model->mesh_array[_c];
		rde_rendering_mesh_destroy(_mesh, true);
	}

	stbds_arrfree(_model->mesh_array);
	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;
	memset(_model->file_path, 0, RDE_MAX_PATH);
}

void rde_rendering_3d_draw_skybox(rde_camera* _camera) {
	if(ENGINE.skybox.opengl_texture_id < 0 || _camera == NULL) {
		return;
	}
	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDepthFunc, GL_LEQUAL);
	RDE_CHECK_GL(glUseProgram, ENGINE.skybox_shader->compiled_program_id);
	
	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = _camera->transform.position;
	rde_vec_3F _cam_direction = _camera->direction;
	rde_vec_3F _cam_up = _camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);
	_view_matrix[3][0] = 0.0f;
	_view_matrix[3][1] = 0.0f;
	_view_matrix[3][2] = 0.0f;
	_view_matrix[3][3] = 0.0f;

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);

	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(ENGINE.skybox_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);

	RDE_CHECK_GL(glBindVertexArray, ENGINE.skybox.vao);
	RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_CUBE_MAP, ENGINE.skybox.opengl_texture_id);
	RDE_CHECK_GL(glDrawArrays, GL_TRIANGLES, 0, 36);
	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glDepthFunc, GL_LESS);
}

#endif