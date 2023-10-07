#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_3D_MODULE)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned int* indices;	// contains X pairs of 3 uints each, first uint is the index of the position in the array of positions, 
							// second for texcoords and third for normals.
	unsigned int vertices_count;
} obj_face;

void separate_into_floats(char* _buffer, float** _arr, size_t* _size) {
	char* _ptr = strtok(_buffer, " ");
	_ptr = strtok(NULL, " "); // remove 'v', 'vt' or 'vn'
	while(_ptr != NULL) {
		_ptr[strcspn(_ptr, "\r\n")] = 0;
		stbds_arrput(*_arr, strtof(_ptr, NULL));
		_ptr = strtok(NULL, " ");
		(*_size)++;
	}
}

void separate_into_faces(char* _buffer, obj_face** _arr, size_t* _size) {

#if IS_WINDOWS()
	#define strtok_rde strtok_s
#else
	#define strtok_rde strtok_r
#endif

	char* _outer_saveptr = NULL;
	char* _inner_saveptr = NULL;

	char* _face_ptr = strtok_rde(_buffer, " ", &_outer_saveptr);
	_face_ptr = strtok_rde(NULL, " ", &_outer_saveptr); // remove 'f'

	obj_face _face = { NULL, 0 };

	while(_face_ptr != NULL) {
		_face_ptr[strcspn(_face_ptr, "\r\n")] = 0;
		char* _indices_ptr = strtok_rde(_face_ptr, "/", &_inner_saveptr);
		while(_indices_ptr != NULL) {
			_indices_ptr[strcspn(_indices_ptr, "\r\n")] = 0;
			stbds_arrput(_face.indices, strtol(_indices_ptr, NULL, 10) - 1);
			_indices_ptr = strtok_rde(NULL, "/", &_inner_saveptr);
		}
		
		_face_ptr = strtok_rde(NULL, " ", &_outer_saveptr);
		_face.vertices_count++;
	}

	stbds_arrput(*_arr, _face);
	(*_size)++;

#undef strtok_rde
}

void read_file_and_fill_data(const char* _obj_path, 
                             size_t* _positions_size, float** _positions,
                             size_t* _normals_size, float** _normals, 
                             size_t* _texcoords_size, float** _texcoords, 
                             size_t* _faces_size, obj_face** _faces,
                             char* _material_path) {
	FILE* _fp;
	char _buffer[512];
	size_t len = 512;

#if IS_WINDOWS()
	rde_critical_error(fopen_s(&_fp, _obj_path, "r") != 0, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#else
	_fp = fopen(_obj_path, "r");
	rde_critical_error(_fp == NULL, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#endif

	while(fgets(_buffer, len, _fp)) {
		if(_buffer[0] == 'v') {
			if(_buffer[1] == ' ') {
				separate_into_floats(_buffer, _positions, _positions_size);
			} else {
				if(_buffer[1] == 'n') {
					separate_into_floats(_buffer, _normals, _normals_size);
				} else if(_buffer[1] == 't') {
					separate_into_floats(_buffer, _texcoords, _texcoords_size);
				}
			}
		} else if(_buffer[0] == 'f') {
			separate_into_faces(_buffer, _faces, _faces_size);
		} else if(strncmp(_buffer, "mtllib", strlen("mtllib")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'mtllib'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			strcat(_material_path, _ptr);
			printf("%s \n", _material_path);
		}
	}

	fclose(_fp);
}

void parse_3_vertices_face(unsigned int _i, unsigned int _v, unsigned int* _mesh_indices, float* _mesh_positions, float* _mesh_texcoords, obj_face* _face, 
							unsigned int* _indices_pointer, unsigned int* _positions_pointer, unsigned int* _texcoords_pointer,
							float* _positions, float* _texcoords) {
	_mesh_indices[*_indices_pointer + 0] = 3 * _i + 0;
	_mesh_indices[*_indices_pointer + 1] = 3 * _i + 1;
	_mesh_indices[*_indices_pointer + 2] = 3 * _i + 2;

	_mesh_positions[*_positions_pointer + 0] = _positions[_face->indices[0 * 3] * 3 + 0];
	_mesh_positions[*_positions_pointer + 1] = _positions[_face->indices[0 * 3] * 3 + 1];
	_mesh_positions[*_positions_pointer + 2] = _positions[_face->indices[0 * 3] * 3 + 2];

	_mesh_positions[*_positions_pointer + 3] = _positions[_face->indices[(_v + 1) * 3] * 3 + 0];
	_mesh_positions[*_positions_pointer + 4] = _positions[_face->indices[(_v + 1) * 3] * 3 + 1];
	_mesh_positions[*_positions_pointer + 5] = _positions[_face->indices[(_v + 1) * 3] * 3 + 2];
	
	_mesh_positions[*_positions_pointer + 6] = _positions[_face->indices[(_v + 2) * 3] * 3 + 0];
	_mesh_positions[*_positions_pointer + 7] = _positions[_face->indices[(_v + 2) * 3] * 3 + 1];
	_mesh_positions[*_positions_pointer + 8] = _positions[_face->indices[(_v + 2) * 3] * 3 + 2];

	if(_mesh_texcoords != NULL) {
		_mesh_texcoords[*_texcoords_pointer + 0] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 1] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 1];

		_mesh_texcoords[*_texcoords_pointer + 2] = _texcoords[_face->indices[(_v + 1) * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 3] = _texcoords[_face->indices[(_v + 1) * 3 + 1] * 2 + 1];

		_mesh_texcoords[*_texcoords_pointer + 4] = _texcoords[_face->indices[(_v + 2) * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 5] = _texcoords[_face->indices[(_v + 2) * 3 + 1] * 2 + 1];
	}

	*_indices_pointer += 3;
	*_positions_pointer += 9;
	*_texcoords_pointer += 6;
}

rde_model_material load_obj_material(const char* _obj_path) {

	if(_obj_path == NULL || strlen(_obj_path) == 0) {
		return rde_struct_create_model_material();
	}

	FILE* _fp;
	char _buffer[512];
	size_t len = 512;

#if IS_WINDOWS()
	rde_critical_error(fopen_s(&_fp, _obj_path, "r") != 0, RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD, _obj_path);
#else
	_fp = fopen(_obj_path, "r");
	rde_critical_error(_fp == NULL, RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD, _obj_path);
#endif

	rde_model_material _material = rde_struct_create_model_material();
	while(fgets(_buffer, len, _fp)) {
		if(strncmp(_buffer, "map_Kd", strlen("map_Kd")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'map_Kd'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			_material.texture = rde_rendering_load_texture(_ptr);
		}
	}

	fclose(_fp);

	return _material;
}

rde_model* rde_rendering_load_obj_model(const char* _obj_path) {
	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading OBJ '%s':", _obj_path);

	rde_model* _model = NULL;
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if(_m->mesh_array != NULL) {
			continue;
		}

		_model = _m;
		break;
	}

	rde_critical_error(_model == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "models", ENGINE.heap_allocs_config.max_number_of_models);

	_model->mesh_array = NULL;
	_model->mesh_array_size = 1;

	float* _positions = NULL;
	size_t _positions_size = 0;

	float* _normals = NULL;
	size_t _normals_size = 0;

	float* _texcoords = NULL;
	size_t _texcoords_size = 0;

	obj_face* _faces = NULL; 
	size_t _faces_size = 0;

	char _material_path[512];
	memset(_material_path, 0, 512);

	read_file_and_fill_data(_obj_path, 
	                        &_positions_size, &_positions, 
	                        &_normals_size, &_normals, 
	                        &_texcoords_size, &_texcoords, 
	                        &_faces_size, &_faces,
	                        _material_path);

	uint _mesh_indices_size = 0;
	size_t _mesh_positions_size = 0;
	unsigned int _mesh_texcoords_size = 0;

	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];
		if (_face->vertices_count == 3) {
			_mesh_indices_size += 3;
			_mesh_positions_size += 3;
			_mesh_texcoords_size += 3;
		} else {
			int _indices_in_face = _face->vertices_count;
			int _moving_pointer = 1;
					
			while(_moving_pointer != _indices_in_face) {
				_mesh_indices_size += 6;
				_mesh_positions_size += 6;
				_mesh_texcoords_size += 6;
				_moving_pointer++;
			}
		}
	}

	rde_model_material _model_material = load_obj_material(_material_path);

	unsigned int* _mesh_indices = (unsigned int*)malloc(sizeof(unsigned int) * _mesh_indices_size * 1);
	float* _mesh_positions = (float*)malloc(sizeof(float) * _mesh_positions_size * 3);
	float* _mesh_texcoords = NULL;

	if(_model_material.texture != NULL) {
		_mesh_texcoords = (float*)malloc(sizeof(float) * _mesh_texcoords_size * 2);
	}

	unsigned int _indices_pointer = 0;
	unsigned int _positions_pointer = 0;
	unsigned int _texcoords_pointer = 0;

	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];

		if (_face->vertices_count == 3) {
			parse_3_vertices_face(_indices_pointer / 3, 0,
								  _mesh_indices, _mesh_positions, _mesh_texcoords, 
								  _face, 
								  &_indices_pointer, &_positions_pointer, &_texcoords_pointer, 
								  _positions, _texcoords);
		} else {
			for(size_t _v = 0; _v < _face->vertices_count - 2; _v++) {
				parse_3_vertices_face(_indices_pointer / 3, _v,
								  _mesh_indices, _mesh_positions, _mesh_texcoords, 
								  _face, 
								  &_indices_pointer, &_positions_pointer, &_texcoords_pointer, 
								  _positions, _texcoords);
			}
		}
	}

	rde_log_color(RDE_LOG_COLOR_GREEN, "	- vertices: %u, indices: %u, texcoords: %u \n", _mesh_positions_size, _mesh_indices_size, _mesh_texcoords_size);

	rde_mesh* _mesh = rde_struct_create_mesh(_mesh_positions_size, _mesh_indices_size);
	rde_rendering_mesh_set_vertex_positions(_mesh, _mesh_positions, true);
	rde_rendering_mesh_set_indices(_mesh, _mesh_indices, true);
	
	if(_model_material.texture != NULL) {
		rde_rendering_mesh_set_vertex_texture_data(_mesh, _mesh_texcoords_size, _mesh_texcoords, _model_material.texture, true);
	}

	stbds_arrput(_model->mesh_array, _mesh);
	stbds_arrput(_model->material_array, _model_material);

	stbds_arrfree(_positions);
	stbds_arrfree(_normals);
	stbds_arrfree(_texcoords);
	stbds_arrfree(_faces->indices);
	stbds_arrfree(_faces);

	return _model;
}

#pragma clang diagnostic pop
#endif