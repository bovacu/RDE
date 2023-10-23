#ifdef RDE_RENDERING_MODULE

#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR 1
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD 2

#define RDE_DEFAULT_TEXTURE_SIZE 64
#define RDE_DEFAULT_TEXTURE_CHANNELS 3
static rde_texture* DEFAULT_TEXTURE;

#define RDE_MAX_MODELS_PER_DRAW 1000
#define RDE_MAX_LINES_PER_DRAW 10000

#define RDE_OBJ_EXTENSION "obj"
#define RDE_FBX_EXTENSION "fbx"
#define RDE_GLTF_EXTENSION "gltf"

static rde_batch_3d current_batch_3d;

#include "rendering_3d_default_meshes.c"

typedef struct {
	float* positions;
	size_t positions_size;

	uint* indices;
	size_t indices_size; 
	
	float* texcoords;
	size_t texcoords_size;
	
	uint* colors;
	size_t colors_size;

	float* normals;
	size_t normals_size;

	rde_texture* map_ka;
	rde_texture* map_kd;
	rde_texture* map_ks;
	rde_texture* map_bump;
} rde_mesh_gen_data;

void rde_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats);
void rde_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats);
void rde_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh);
void rde_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness);
void rde_rendering_flush_batch_3d();
void rde_rendering_flush_line_batch();
void rde_rendering_reset_batch_3d();
void rde_rendering_reset_line_batch();
rde_mesh rde_struct_create_mesh(uint _vertex_count, rde_mesh_gen_data* _data);

#include "fbx_importer.c"
#include "obj_importer.c"

void rde_create_line_batch_buffers() {
	glGenVertexArrays(1, &current_batch_3d.line_batch.vertex_array_object);
	glBindVertexArray(current_batch_3d.line_batch.vertex_array_object);
	
	glGenBuffers(1, &current_batch_3d.line_batch.vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_line_vertex), (const void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_line_vertex), (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	rde_util_check_opengl_error("After creating line batch");
}

void rde_destroy_line_batch_buffers() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &current_batch_3d.line_batch.vertex_buffer_object);
	glDeleteVertexArrays(1, &current_batch_3d.line_batch.vertex_array_object);

	free(current_batch_3d.line_batch.vertices);
	current_batch_3d.line_batch.vertices = NULL;
}

void rde_rendering_init_3d() {
	DEFAULT_TEXTURE = rde_rendering_memory_texture_create(RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_CHANNELS);
	
	for(int _y = 0; _y < RDE_DEFAULT_TEXTURE_SIZE; _y++) {
		for(int _x = 0; _x < RDE_DEFAULT_TEXTURE_SIZE; _x++) {
			rde_rendering_memory_texture_set_pixel(DEFAULT_TEXTURE, (rde_vec_2I) {_x, _y}, RDE_COLOR_WHITE);
		}
	}

	rde_rendering_memory_texture_gpu_upload(DEFAULT_TEXTURE);
	rde_util_check_opengl_error("ERROR: Creating default texture");

	current_batch_3d = rde_struct_create_batch_3d();
	current_batch_3d.line_batch.vertices = (rde_line_vertex*)malloc(sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW);
	rde_create_line_batch_buffers();
}

void rde_rendering_end_3d() {
	if(DEFAULT_TEXTURE != NULL) {
		rde_rendering_memory_texture_destroy(DEFAULT_TEXTURE);
	}
	rde_destroy_line_batch_buffers();
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

rde_mesh rde_struct_create_mesh(uint _vertex_count, rde_mesh_gen_data* _data) {
	rde_mesh _mesh;
	_mesh.vao = 0;
	
	rde_util_check_opengl_error("ERROR: MESH - Start");

	GLuint _vao = 0;
	printf("Start mesh generation\n");
	glGenVertexArrays(1, &_vao);
	printf("Mesh VAO: %u\n", _vao);
	rde_util_check_opengl_error("ERROR: MESH - VAO");

	_mesh.vao = _vao;
	_mesh.vertex_count = _vertex_count;
	_mesh.vertex_positions = _data->positions;
	_mesh.vertex_colors = _data->colors;
	_mesh.vertex_normals = _data->normals;
	_mesh.vertex_texcoords = _data->texcoords;
	
	_mesh.material.map_ka = _data->map_ka;
	_mesh.material.map_kd = _data->map_kd;
	_mesh.material.map_ks = _data->map_ks;
	_mesh.material.map_bump = _data->map_bump;

	_mesh.transforms = NULL;
	_mesh.transforms = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW );
	memset(_mesh.transforms, 0, RDE_MAX_MODELS_PER_DRAW);

	glBindVertexArray(_mesh.vao);
	rde_util_check_opengl_error("ERROR: MESH - Bind VAO");
	
	size_t _positions_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	glGenBuffers(1, &_mesh.vbo[0]);
	rde_util_check_opengl_error("ERROR: MESH - Gen VBO Positions");
	glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[0]);
	rde_util_check_opengl_error("ERROR: MESH - Bind VBO Positions");
	glBufferData(GL_ARRAY_BUFFER, _positions_size, _mesh.vertex_positions, GL_STATIC_DRAW);
	rde_util_check_opengl_error("ERROR: MESH - Positions VBO Set Data");
	glEnableVertexAttribArray(0);
	rde_util_check_opengl_error("ERROR: MESH - Positions VBO Enable Attrib");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	rde_util_check_opengl_error("ERROR: MESH - Positions VBO Set Attrib Data");
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	size_t _colors_size = sizeof(unsigned int) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR;
	glGenBuffers(1, &_mesh.vbo[1]);
	rde_util_check_opengl_error("ERROR: MESH - Gen VBO Color");
	glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[1]);
	rde_util_check_opengl_error("ERROR: MESH - Bind VBO Color");
	glBufferData(GL_ARRAY_BUFFER, _colors_size, _mesh.vertex_colors, GL_STATIC_DRAW);
	rde_util_check_opengl_error("ERROR: MESH - Color VBO Set Data");
	glEnableVertexAttribArray(1);
	rde_util_check_opengl_error("ERROR: MESH - Color VBO Enable Attrib");
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	rde_util_check_opengl_error("ERROR: MESH - Color VBO Set Attrib Data");
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if(_data->normals != NULL && _data->normals_size > 0) {
		size_t _normals_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		glGenBuffers(1, &_mesh.vbo[2]);
		rde_util_check_opengl_error("ERROR: MESH - Gen VBO Normal");
		glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[2]);
		rde_util_check_opengl_error("ERROR: MESH - Bind VBO Normal");
		glBufferData(GL_ARRAY_BUFFER, _normals_size, _mesh.vertex_normals, GL_STATIC_DRAW);
		rde_util_check_opengl_error("ERROR: MESH - Normal VBO Set Data");
		glEnableVertexAttribArray(2);
		rde_util_check_opengl_error("ERROR: MESH - Normal VBO Enable Attrib");
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		rde_util_check_opengl_error("ERROR: MESH - Normal VBO Set Attrib Data");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if(_data->texcoords != NULL && _data->texcoords_size > 0) {
		size_t _texcoords_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		glGenBuffers(1, &_mesh.vbo[3]);
		rde_util_check_opengl_error("ERROR: MESH - Gen VBO Texcoords");
		glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[3]);
		rde_util_check_opengl_error("ERROR: MESH - Bind VBO Texcoords");
		glBufferData(GL_ARRAY_BUFFER, _texcoords_size, _mesh.vertex_texcoords, GL_STATIC_DRAW);
		rde_util_check_opengl_error("ERROR: MESH - Texcoords VBO Set Data");
		glEnableVertexAttribArray(3);
		rde_util_check_opengl_error("ERROR: MESH - Texcoords VBO Enable Attrib");
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		rde_util_check_opengl_error("ERROR: MESH - Texcoords VBO Set Attrib Data");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glGenBuffers(1, &_mesh.vbo[4]);
	rde_util_check_opengl_error("ERROR: MESH - Gen VBO Transform");
	glBindBuffer(GL_ARRAY_BUFFER, _mesh.vbo[4]);
	rde_util_check_opengl_error("ERROR: MESH - Bind VBO Transform");
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	rde_util_check_opengl_error("ERROR: MESH - VBO Transform Set Data");
	
	glEnableVertexAttribArray(4);
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Enable Attrib 4");
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Set Attrib Data 4");

	glEnableVertexAttribArray(5);
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Enable Attrib 5");
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Set Attrib Data 5");

	glEnableVertexAttribArray(6);
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Enable Attrib 6");
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Set Attrib Data 6");

	glEnableVertexAttribArray(7);
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Enable Attrib 7");
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));
	rde_util_check_opengl_error("ERROR: MESH - Transform VBO Set Attrib Data 7");

	glVertexAttribDivisor(4, 1);
	rde_util_check_opengl_error("ERROR: MESH - Transform Attrib divisor 4");
    glVertexAttribDivisor(5, 1);
	rde_util_check_opengl_error("ERROR: MESH - Transform Attrib divisor 5");
    glVertexAttribDivisor(6, 1);
	rde_util_check_opengl_error("ERROR: MESH - Transform Attrib divisor 6");
    glVertexAttribDivisor(7, 1);
	rde_util_check_opengl_error("ERROR: MESH - Transform Attrib divisor 7");
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_mesh.material = rde_struct_create_material();

	rde_log_level(RDE_LOG_LEVEL_INFO, "Created mesh with %zu vertices. VAO: %u\n", _mesh.vertex_count, _vao);

	return _mesh;
}

rde_mesh* rde_struct_memory_mesh_create(size_t _vertex_count, size_t _index_count) {
	rde_critical_error(_vertex_count <= 3 || _index_count <= 3, RDE_ERROR_BAD_MESH_DATA, _vertex_count, _index_count);

	rde_mesh* _mesh = (rde_mesh*)malloc(sizeof(rde_mesh));
	_mesh->vao = 0;

	glGenVertexArrays(1, &_mesh->vao);
	rde_util_check_opengl_error("ERROR: Creating mesh Ptr");

	_mesh->vertex_count = _vertex_count;
	_mesh->vertex_positions = NULL;
	_mesh->vertex_colors = NULL;
	_mesh->vertex_normals = NULL;
	_mesh->vertex_texcoords = NULL;
	_mesh->transforms = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW);
	memset(_mesh->transforms, 0, RDE_MAX_MODELS_PER_DRAW);

	_mesh->vbo[0] = RDE_UINT_MAX;
	_mesh->vbo[1] = RDE_UINT_MAX;
	_mesh->vbo[2] = RDE_UINT_MAX;
	_mesh->vbo[3] = RDE_UINT_MAX;

	_mesh->free_vertex_positions_on_end = false;
	_mesh->free_vertex_colors_on_end = false;
	_mesh->free_vertex_normals_on_end = false;
	_mesh->free_vertex_texcoords_on_end = false;
	_mesh->free_vertex_texture_on_end = false;
	_mesh->free_indices_on_end = false;

	_mesh->material = rde_struct_create_material();

	return _mesh;
}

rde_model* rde_rendering_model_load(const char* _model_path) {
	const char* _extension = rde_util_file_get_name_extension(_model_path);

	if(strcmp(_extension, RDE_FBX_EXTENSION) == 0) {
		rde_critical_error(true, RDE_ERROR_FEATURE_NOT_SUPPORTED_YET, "rde_rendering_model_load for GLTF");
	} else if(strcmp(_extension, RDE_OBJ_EXTENSION) == 0) {
#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_MODULE)
		return rde_rendering_load_obj_model(_model_path);
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

bool rde_rendering_is_mesh_ok_to_render(rde_mesh* _mesh) {
	if(_mesh->vertex_positions == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "%s", "Tried to render a mesh without indices, skipping rendering for this mesh");
		return false;
	}

	return true;
}

void rde_rendering_mesh_destroy(rde_mesh* _mesh) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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

	glDeleteVertexArrays(1, &_mesh->vao);
	_mesh->vao = RDE_UINT_MAX;
	
	free(_mesh->vertex_positions);
	_mesh->vertex_positions = NULL;

	free(_mesh->vertex_colors);
	_mesh->vertex_colors = NULL;

	free(_mesh->vertex_normals);
	_mesh->vertex_normals = NULL;

	free(_mesh->vertex_texcoords);
	_mesh->vertex_texcoords = NULL;

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

	free(_mesh->transforms);
	_mesh->transforms = NULL;

	_mesh->material = rde_struct_create_material();
}


void rde_rendering_3d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _draw_wireframe_over_mesh) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	current_drawing_camera = _camera;
	current_drawing_window = _window;

	rde_vec_2F _aspect_ratios = rde_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.y : _aspect_ratios.x;
	glm_perspective(_camera->fov, _aspect_ratio, _camera->near_far.x, _camera->near_far.y, projection_matrix);

	current_batch_3d.draw_mesh_wireframe = _draw_wireframe_over_mesh;
}

void rde_rendering_3d_draw_line(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, unsigned short _thickness, rde_shader* _shader) {
	const size_t _line_vertex_count = 2;

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.line_shader : _shader;
	rde_rendering_try_create_line_batch(_drawing_shader, _thickness);
	rde_rendering_try_flush_line_batch(_drawing_shader, _thickness, _line_vertex_count);
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

void rde_rendering_reset_line_batch() {
	memset(current_batch_3d.line_batch.vertices, 0, RDE_MAX_LINES_PER_DRAW);
	current_batch_3d.line_batch.amount_of_vertices = 0;
	current_batch_3d.line_batch.shader = NULL;
	current_batch_3d.line_batch.thickness = 0;
}

void rde_rendering_reset_batch_3d() {
	current_batch_3d.shader = NULL;

	if(current_batch_3d.mesh != NULL) {
		memset(current_batch_3d.mesh->transforms, 0, current_batch_3d.amount_of_models_per_draw);
	}

	current_batch_3d.mesh = NULL;
	current_batch_3d.amount_of_models_per_draw = 0;
}

void rde_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness) {
	if(current_batch_3d.line_batch.shader == NULL) {
		current_batch_3d.line_batch.shader = _shader;
	}

	if(current_batch_3d.line_batch.thickness == 0 ) {
		current_batch_3d.line_batch.thickness = _thickness;
	}
}

void rde_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh) {
	if(current_batch_3d.shader == NULL) {
		current_batch_3d.shader = _shader;
	}

	if(current_batch_3d.mesh == NULL && _mesh != NULL) {
		current_batch_3d.mesh = _mesh;
	}
}

void rde_rendering_flush_line_batch() {
	if(current_batch_3d.line_batch.shader == NULL || current_batch_3d.line_batch.amount_of_vertices == 0) {
		return;
	}

	if(current_batch_3d.line_batch.thickness <= 0) {
		current_batch_3d.line_batch.thickness = 1;
	}
	glLineWidth(current_batch_3d.line_batch.thickness);

	rde_util_check_opengl_error("GL Error Before LineFlush3D");
	rde_shader* _shader = current_batch_3d.line_batch.shader;

	glUseProgram(_shader->compiled_program_id);
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
	glUniformMatrix4fv(glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_model_view_projection_matrix);

	glBindVertexArray(current_batch_3d.line_batch.vertex_array_object);
	rde_util_check_opengl_error("After glBindVertexArray");
	
	glBindBuffer(GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	glBufferSubData(GL_ARRAY_BUFFER, 0, current_batch_3d.line_batch.amount_of_vertices * sizeof(rde_line_vertex), current_batch_3d.line_batch.vertices);

	glDrawArrays(GL_LINES, 0, current_batch_3d.line_batch.amount_of_vertices);
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

	rde_texture* _ka_texture = _mesh->material.map_ka != NULL ? _mesh->material.map_ka : DEFAULT_TEXTURE;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ka_texture->opengl_texture_id);

	rde_texture* _kd_texture = _mesh->material.map_kd != NULL ? _mesh->material.map_kd : DEFAULT_TEXTURE;
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _kd_texture->opengl_texture_id);

	rde_texture* _ks_texture = _mesh->material.map_ks != NULL ? _mesh->material.map_ks : DEFAULT_TEXTURE;
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _ks_texture->opengl_texture_id);

	rde_texture* _bump_texture = _mesh->material.map_bump != NULL ? _mesh->material.map_bump : DEFAULT_TEXTURE;
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _bump_texture->opengl_texture_id);

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

	rde_material_light_data _material = _mesh->material.material_light_data;
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.shininess"), _material.shininess);
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.Ka"), _material.ka);
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.Kd"), _material.kd);
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.Ks"), _material.ks);
	glUniform1f(glGetUniformLocation(_shader->compiled_program_id, "material.Ke"), _material.shininess);

	glBindVertexArray(_mesh->vao);
	rde_util_check_opengl_error("After glBindVertexArray");

	glBindBuffer(GL_ARRAY_BUFFER, _mesh->vbo[4]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, current_batch_3d.amount_of_models_per_draw * (sizeof(float) * 16), _mesh->transforms);

	glDrawArraysInstanced(GL_TRIANGLES, 0, _mesh->vertex_count, current_batch_3d.amount_of_models_per_draw);
}

void rde_rendering_3d_draw_mesh(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader) {
	const size_t _floats_per_matrix = 1;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;
	rde_rendering_try_create_batch_3d(_drawing_shader, _mesh);
	rde_rendering_try_flush_batch_3d(_drawing_shader, _mesh, _floats_per_matrix);
	
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	rde_rendering_transform_to_glm_mat4_3d(_transform, _transformation_matrix);

	glm_mat4_copy(_transformation_matrix, _mesh->transforms[current_batch_3d.amount_of_models_per_draw]);
	current_batch_3d.amount_of_models_per_draw++;
}

void rde_rendering_3d_draw_model(const rde_transform* _transform, rde_model* _model, rde_shader* _shader) {
	for(uint _i = 0; _i < _model->mesh_array_size; _i++) {
		rde_mesh* _mesh = &_model->mesh_array[_i];
		rde_rendering_3d_draw_mesh(_transform, _mesh, _shader);
	}
}

void rde_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats) {
	bool _lines_ok = current_batch_3d.line_batch.amount_of_vertices + _extra_floats <= RDE_MAX_LINES_PER_DRAW;
	bool _shader_ok = current_batch_3d.line_batch.shader == _shader;
	bool _thickness_ok = current_batch_3d.line_batch.thickness == _thickness;
	if(_shader_ok && _lines_ok && _thickness_ok) {
		return;
	}

	rde_rendering_flush_line_batch();
	rde_rendering_reset_line_batch();
	rde_rendering_try_create_line_batch(_shader, _thickness);
}

void rde_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats) {
	bool _models_ok = current_batch_3d.amount_of_models_per_draw + 1 <= RDE_MAX_MODELS_PER_DRAW;
	bool _shader_ok = current_batch_3d.shader == _shader;
	bool _mesh_ok = current_batch_3d.mesh == _mesh;
	bool _transforms_ok = current_batch_3d.amount_of_models_per_draw * _extra_floats + _extra_floats <= RDE_MAX_MODELS_PER_DRAW * _extra_floats;
	if(_models_ok && _shader_ok && _transforms_ok && _mesh_ok) {
		return;
	}

	if(current_batch_3d.draw_mesh_wireframe) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		rde_rendering_flush_batch_3d();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	rde_rendering_flush_batch_3d();
	rde_rendering_reset_batch_3d();
	rde_rendering_try_create_batch_3d(_shader, _mesh);
}

void rde_rendering_3d_end_drawing() {
	
	if(current_batch_3d.draw_mesh_wireframe) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		rde_rendering_flush_batch_3d();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	rde_rendering_flush_batch_3d();

	rde_rendering_reset_batch_3d();
	rde_rendering_flush_line_batch();
	rde_rendering_reset_line_batch();
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

rde_directional_light rde_rendering_lighting_get_directional_light() {
	return ENGINE.directional_light;
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



void rde_rendering_model_unload(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "obj model");

	for(size_t _c = 0; _c < stbds_arrlenu(_model->mesh_array); _c++) {
		rde_mesh* _mesh = &_model->mesh_array[_c];
		rde_rendering_mesh_destroy(_mesh);
	}

	stbds_arrfree(_model->mesh_array);
	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;
}

#endif