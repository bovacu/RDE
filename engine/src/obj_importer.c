#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_3D_MODULE)

// TODO: remove this diagnostics, basically change strtok for strtok_s on Windows
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	unsigned int* indices;	// contains X pairs of 3 uints each, first uint is the index of the position in the array of positions, 
							// second for texcoords and third for normals.
	unsigned int vertices_count;
} obj_face;

const int NO_VERTEX_UVS = 0b00000001;
const int NO_VERTEX_NORMALS = 0b00000010;

void separate_into_floats(char* _buffer, float** _arr, size_t* _size, const char* _path, const char* _where, int _line) {
	char* _ptr = strtok(_buffer, " ");
	_ptr = strtok(NULL, " "); // remove 'v', 'vt' or 'vn'

	short _amount_read = 0;

	while(_ptr != NULL) {
		if(_amount_read >= 3) {
			rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_OBJ_VERTEX_INCLUDES_COLORS, _path);
			break;
		}

		_ptr[strcspn(_ptr, "\r\n")] = 0;

		char* _end_ptr;
		float _value = strtof(_ptr, &_end_ptr);
		
		if(*_end_ptr != '\0') {
			rde_critical_error(true, RDE_ERROR_OBJ_INVALID_DATA, _path, _where, _line + 1);
		}

		stbds_arrput(*_arr, _value);
		_ptr = strtok(NULL, " ");
		(*_size)++;
		_amount_read++;
	}
}

int separate_into_faces(char* _buffer, obj_face** _arr, size_t* _size, const char* _path, const char* _where, int _line) {
	char* _face_ptr = strtok(_buffer, " ");
	_face_ptr = strtok(NULL, " "); // remove 'f'

	obj_face _face = { NULL, 0 };

	int _lacking_face_data = 0;

	while(_face_ptr != NULL) {
		rde_util_string_trim(_face_ptr);
		_face_ptr[strcspn(_face_ptr, "\r\n")] = 0;

		if(_face_ptr != NULL && strlen(_face_ptr) > 0) {
			_face.vertices_count++;
		}

		int _index = 0;
		char* _end = "/";
		for (const char* _p = _face_ptr; *_end == '/'; _p = _end + 1) {
			long _value = strtol(_p, &_end, 10) - 1;
			
			if (_end == _p) {
				stbds_arrput(_face.indices, RDE_UINT_MAX);
				_lacking_face_data |= NO_VERTEX_UVS;
				_index++;
				continue;
			}

			if(_end == NULL) {
				rde_critical_error(true, RDE_ERROR_OBJ_INVALID_DATA, _path, _where, _line + 1);
			}

			stbds_arrput(_face.indices, _value);

			_index++;
		}

		if(_index == 1 || _index == 2) {
			_lacking_face_data |= NO_VERTEX_NORMALS;
		}

		for(short _i = _index; _i < 3; _i++) {
			stbds_arrput(_face.indices, RDE_UINT_MAX);
		}
		
		_face_ptr = strtok(NULL, " ");
	}

	stbds_arrput(*_arr, _face);
	(*_size)++;

	return _lacking_face_data;
}

int read_file_and_fill_data(const char* _obj_path, 
                             size_t* _positions_size, float** _positions,
                             size_t* _normals_size, float** _normals, 
                             size_t* _texcoords_size, float** _texcoords, 
                             size_t* _faces_size, obj_face** _faces,
                             char* _material_path) {
	FILE* _fp;
	char _buffer[RDE_MAX_PATH];
	size_t len = RDE_MAX_PATH;

#if IS_WINDOWS()
	rde_critical_error(fopen_s(&_fp, _obj_path, "r") != 0, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#else
	_fp = fopen(_obj_path, "r");
	rde_critical_error(_fp == NULL, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#endif

	const char* _where_faces = "faces";
	const char* _where_texcoords = "texture coordinates";
	const char* _where_positions = "positions";
	const char* _where_normals = "normals";
	size_t _file_line = 0;

	int _lacking_face_data = 0;

	while(fgets(_buffer, len, _fp)) {
		if(_buffer[0] == 'v') {
			if(_buffer[1] == ' ') {
				separate_into_floats(_buffer, _positions, _positions_size, _obj_path, _where_positions, _file_line);
			} else {
				if(_buffer[1] == 'n') {
					separate_into_floats(_buffer, _normals, _normals_size, _obj_path, _where_normals, _file_line);
				} else if(_buffer[1] == 't') {
					separate_into_floats(_buffer, _texcoords, _texcoords_size, _obj_path, _where_texcoords, _file_line);
				}
			}
		} else if(_buffer[0] == 'f') {
			_lacking_face_data = separate_into_faces(_buffer, _faces, _faces_size, _obj_path, _where_faces, _file_line);
		} else if(strncmp(_buffer, "mtllib", strlen("mtllib")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'mtllib'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			
			char* _last = strrchr(_obj_path, '/');

			if (_last != NULL) {
				strncat(_material_path, _obj_path, _last - _obj_path);
				strcat(_material_path, "/");
			}

			strcat(_material_path, _ptr);
		}

		_file_line++;
	}

	fclose(_fp);

	return _lacking_face_data;
}

void parse_3_vertices_face_obj(unsigned int _i, unsigned int _v, 
                               unsigned int* _mesh_indices, float* _mesh_positions, float* _mesh_texcoords, float* _mesh_normals,
                               obj_face* _face, 
                               size_t* _indices_pointer, size_t* _positions_pointer, size_t* _texcoords_pointer, size_t* _normals_pointer,
                               float* _positions, float* _texcoords, float* _normals) {
	_mesh_indices[*_indices_pointer + 0] = 3 * _i + 0;
	_mesh_indices[*_indices_pointer + 1] = 3 * _i + 1;
	_mesh_indices[*_indices_pointer + 2] = 3 * _i + 2;

	unsigned int _face_index = _face->indices[0 * 3];
	_mesh_positions[*_positions_pointer + 0] = _positions[_face_index * 3 + 0];
	_mesh_positions[*_positions_pointer + 1] = _positions[_face_index * 3 + 1];
	_mesh_positions[*_positions_pointer + 2] = _positions[_face_index * 3 + 2];

	_face_index = _face->indices[(_v + 1) * 3];
	_mesh_positions[*_positions_pointer + 3] = _positions[_face_index * 3 + 0];
	_mesh_positions[*_positions_pointer + 4] = _positions[_face_index * 3 + 1];
	_mesh_positions[*_positions_pointer + 5] = _positions[_face_index * 3 + 2];
	
	_face_index = _face->indices[(_v + 2) * 3];
	_mesh_positions[*_positions_pointer + 6] = _positions[_face_index * 3 + 0];
	_mesh_positions[*_positions_pointer + 7] = _positions[_face_index * 3 + 1];
	_mesh_positions[*_positions_pointer + 8] = _positions[_face_index * 3 + 2];

	_face_index = _face->indices[0 * 3 + 1];
	if(_mesh_texcoords != NULL && _face_index != RDE_UINT_MAX) {
		_mesh_texcoords[*_texcoords_pointer + 0] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 1] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 1];

		_face_index = _face->indices[(_v + 1) * 3 + 1];
		_mesh_texcoords[*_texcoords_pointer + 2] = _texcoords[_face->indices[(_v + 1) * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 3] = _texcoords[_face->indices[(_v + 1) * 3 + 1] * 2 + 1];

		_face_index = _face->indices[(_v + 2) * 3 + 1];
		_mesh_texcoords[*_texcoords_pointer + 4] = _texcoords[_face->indices[(_v + 2) * 3 + 1] * 2 + 0];
		_mesh_texcoords[*_texcoords_pointer + 5] = _texcoords[_face->indices[(_v + 2) * 3 + 1] * 2 + 1];
	}

	_face_index = _face->indices[0 * 3 + 2];
	if(_mesh_normals != NULL && _face_index != RDE_UINT_MAX) {
		_mesh_normals[*_normals_pointer + 0] = _normals[_face->indices[0 * 3 + 2] * 3 + 0];
		_mesh_normals[*_normals_pointer + 1] = _normals[_face->indices[0 * 3 + 2] * 3 + 1];
		_mesh_normals[*_normals_pointer + 2] = _normals[_face->indices[0 * 3 + 2] * 3 + 2];

		_face_index = _face->indices[(_v + 1) * 3 + 2];
		_mesh_normals[*_normals_pointer + 3] = _normals[_face->indices[(_v + 1) * 3 + 2] * 3 + 0];
		_mesh_normals[*_normals_pointer + 4] = _normals[_face->indices[(_v + 1) * 3 + 2] * 3 + 1];
		_mesh_normals[*_normals_pointer + 5] = _normals[_face->indices[(_v + 1) * 3 + 2] * 3 + 2];
		
		_face_index = _face->indices[(_v + 2) * 3 + 2];
		_mesh_normals[*_normals_pointer + 6] = _normals[_face->indices[(_v + 2) * 3 + 2] * 3 + 0];
		_mesh_normals[*_normals_pointer + 7] = _normals[_face->indices[(_v + 2) * 3 + 2] * 3 + 1];
		_mesh_normals[*_normals_pointer + 8] = _normals[_face->indices[(_v + 2) * 3 + 2] * 3 + 2];
	}

	*_indices_pointer += 3;
	*_texcoords_pointer += 6;
	*_positions_pointer += 9;
	*_normals_pointer += 9;
}

rde_model_material load_obj_material(const char* _obj_path) {

	if(_obj_path == NULL || strlen(_obj_path) == 0) {
		return rde_struct_create_model_material();
	}

	FILE* _fp;
	char _buffer[RDE_MAX_PATH];
	size_t len = RDE_MAX_PATH;

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

			char* _last = strrchr(_obj_path, '/');
			char _full_path[RDE_MAX_PATH];
			memset(_full_path, 0, RDE_MAX_PATH);

			if (_last != NULL) {
				strncat(_full_path, _obj_path, _last - _obj_path);
				strcat(_full_path, "/");
			}
			strcat(_full_path, _ptr);

			_material.texture = rde_rendering_load_texture(_full_path);
		}
	}

	fclose(_fp);

	return _material;
}

rde_model* rde_rendering_load_obj_model(const char* _obj_path) {
	clock_t _t_start, _t_end;
    _t_start = clock();

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

	char _material_path[RDE_MAX_PATH];
	memset(_material_path, 0, RDE_MAX_PATH);

	char _obj_path_s[RDE_MAX_PATH]; 
	rde_util_sanitaize_path(_obj_path, _obj_path_s, RDE_MAX_PATH);

	int _lacking_face_data = read_file_and_fill_data(_obj_path_s, 
	                        &_positions_size, &_positions, 
	                        &_normals_size, &_normals, 
	                        &_texcoords_size, &_texcoords, 
	                        &_faces_size, &_faces,
	                        _material_path);

	char _material_path_s[RDE_MAX_PATH]; 
	rde_util_sanitaize_path(_material_path, _material_path_s, RDE_MAX_PATH);

	size_t _mesh_indices_size = 0;
	size_t _mesh_positions_size = 0;
	size_t _mesh_texcoords_size = 0;
	size_t _mesh_normals_size = 0;

	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];
		if (_face->vertices_count == 3) {
			_mesh_indices_size += 3;
			_mesh_positions_size += 3;
			_mesh_texcoords_size += 3;
			_mesh_normals_size += 3;
		} else {
			int _indices_in_face = _face->vertices_count;
			int _moving_pointer = 1;
					
			while(_moving_pointer != _indices_in_face - 1) {
				_mesh_indices_size += 3;
				_mesh_positions_size += 3;
				_mesh_texcoords_size += 3;
				_mesh_normals_size += 3;
				_moving_pointer++;
			}
		}
	}

	rde_model_material _model_material = load_obj_material(_material_path_s);

	unsigned int* _mesh_indices = (unsigned int*)malloc(sizeof(unsigned int) * _mesh_indices_size * 1);
	float* _mesh_positions = (float*)malloc(sizeof(float) * _mesh_positions_size * 3);
	float* _mesh_texcoords = NULL;
	float* _mesh_normals = NULL;

	if(_model_material.texture != NULL && (_lacking_face_data & NO_VERTEX_UVS) == 0) {
		_mesh_texcoords = (float*)malloc(sizeof(float) * _mesh_texcoords_size * 2);
	} else {
		_mesh_texcoords_size = 0;
	}

	if((_lacking_face_data & NO_VERTEX_NORMALS) == 0) {
		_mesh_normals = (float*)malloc(sizeof(float) * _mesh_normals_size * 3);
	} else {
		_mesh_normals_size = 0;
	}

	size_t _indices_pointer = 0;
	size_t _positions_pointer = 0;
	size_t _texcoords_pointer = 0;
	size_t _normals_pointer = 0;

	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];

		if (_face->vertices_count == 3) {
			parse_3_vertices_face_obj(_indices_pointer / 3, 0,
								  _mesh_indices, _mesh_positions, _mesh_texcoords, _mesh_normals,
								  _face, 
								  &_indices_pointer, &_positions_pointer, &_texcoords_pointer, &_normals_pointer,
								  _positions, _texcoords, _normals);
		} else {
			for(size_t _v = 0; _v < _face->vertices_count - 2; _v++) {
				parse_3_vertices_face_obj(_indices_pointer / 3, _v,
								  _mesh_indices, _mesh_positions, _mesh_texcoords, _mesh_normals,
								  _face, 
								  &_indices_pointer, &_positions_pointer, &_texcoords_pointer, &_normals_pointer,
								  _positions, _texcoords, _normals);
			}
		}
	}

	rde_mesh _mesh = rde_struct_create_mesh(_mesh_positions_size, _mesh_positions_size);
	rde_rendering_mesh_set_vertex_positions(&_mesh, _mesh_positions, true);
	rde_rendering_mesh_set_indices(&_mesh, _mesh_indices, true);
	
	if(_model_material.texture != NULL && (_lacking_face_data & NO_VERTEX_UVS) == 0) {
		rde_rendering_mesh_set_vertex_texture_data(&_mesh, _mesh_texcoords_size, _mesh_texcoords, _model_material.texture, true);
	}
	
	if((_lacking_face_data & NO_VERTEX_NORMALS) == 0) {
		rde_rendering_mesh_set_vertex_normals(&_mesh, _mesh_normals, true);
	}

	stbds_arrput(_model->mesh_array, _mesh);
	stbds_arrput(_model->material_array, _model_material);

	stbds_arrfree(_positions);
	stbds_arrfree(_normals);
	stbds_arrfree(_texcoords);
	stbds_arrfree(_faces->indices);
	stbds_arrfree(_faces);

	_t_end = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "	- vertices: %u, indices: %u, texcoords: %u, normals: %u \n", _mesh_positions_size, _mesh_indices_size, _mesh_texcoords_size, _mesh_normals_size);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- took %.3fms \n", (double)(_t_end - _t_start) / CLOCKS_PER_SEC);

	return _model;
}

#pragma clang diagnostic pop
#endif