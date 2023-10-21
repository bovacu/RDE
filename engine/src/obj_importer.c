#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_MODULE)

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

typedef struct {
	float* positions;
	size_t positions_size;

	float* texcoords;
	size_t texcoords_size;

	float* normals;
	size_t normals_size;

	unsigned int* indices;
	size_t indices_size;

	unsigned int* colors;
	size_t colors_size;

	obj_face* faces;
	size_t faces_size;

	float diffuse_color[3];

	size_t indices_pointer;
	size_t positions_pointer;
	size_t texcoords_pointer;
	size_t normals_pointer;
	size_t colors_pointer;

	unsigned int face_lacking_data;

	rde_material material;
} obj_mesh_group;
obj_mesh_group create_obj_mesh_group() {
	obj_mesh_group _m;
	_m.positions = NULL;
	_m.positions_size = 0;
	_m.texcoords = NULL;
	_m.texcoords_size = 0;
	_m.normals = NULL;
	_m.normals_size = 0;
	_m.colors = NULL;
	_m.colors_size = 0;
	_m.indices = NULL;
	_m.indices_size = 0;
	_m.faces = NULL;
	_m.faces_size = 0;
	_m.diffuse_color[0] = 1.f;
	_m.diffuse_color[1] = 1.f;
	_m.diffuse_color[2] = 1.f;
	_m.indices_pointer = 0;
	_m.positions_pointer = 0;
	_m.texcoords_pointer = 0;
	_m.normals_pointer = 0;
	_m.colors_pointer = 0;
	_m.face_lacking_data = 0;
	_m.material = rde_struct_create_material();
	return _m;
}

typedef struct {
	char* key;
	obj_mesh_group value;
} obj_mesh_group_map;

const int NO_VERTEX_UVS = 0b00000001;
const int NO_VERTEX_NORMALS = 0b00000010;

rde_model* rde_rendering_load_obj_model(const char* _obj_path);

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
		_face_ptr = rde_util_string_trim(_face_ptr);
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

void read_file_and_fill_data(const char* _obj_path, 
                            obj_mesh_group_map** _obj_group_map, 
                            size_t* _obj_positions_size, float** _obj_positions,
                            size_t* _obj_normals_size, float** _obj_normals, 
                            size_t* _obj_texcoords_size, float** _obj_texcoords,
                            char* _obj_material_path) {
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
	char _current_key[256];
	memset(_current_key, 0, 256);
	obj_mesh_group* _current_group = NULL;

	while(fgets(_buffer, len, _fp)) {
		if(_buffer[0] == 'v') {
			if(_buffer[1] == ' ') {
				separate_into_floats(_buffer, _obj_positions, _obj_positions_size, _obj_path, _where_positions, _file_line);
			} else {
				if(_buffer[1] == 'n') {
					separate_into_floats(_buffer, _obj_normals, _obj_normals_size, _obj_path, _where_normals, _file_line);
				} else if(_buffer[1] == 't') {
					separate_into_floats(_buffer, _obj_texcoords, _obj_texcoords_size, _obj_path, _where_texcoords, _file_line);
				}
			}
		} else if(_buffer[0] == 'f') {
			rde_critical_error(_current_group == NULL, "Started to read faces before creating a group, this is because before the first 'f' a usemtl was not found");
			_current_group->face_lacking_data = separate_into_faces(_buffer, &_current_group->faces, &_current_group->faces_size, _obj_path, _where_faces, _file_line);
		} else if(strncmp(_buffer, "mtllib", strlen("mtllib")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'mtllib'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			
			char* _last = strrchr(_obj_path, '/');

			if (_last != NULL) {
				strncat(_obj_material_path, _obj_path, _last - _obj_path);
				strcat(_obj_material_path, "/");
			}

			strcat(_obj_material_path, _ptr);
		} else if(strncmp(_buffer, "usemtl", strlen("usemtl")) == 0) {
			memset(_current_key, 0, 256);
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'usemtl'
			_ptr[strcspn(_ptr, "\r\n")] = 0;

			strcat(_current_key, _ptr);
			char* _key = (char*)malloc(sizeof(char) * strlen(_current_key));
			memset(_key, 0, strlen(_current_key));
			strcat(_key, _current_key);
			stbds_shput(*_obj_group_map, _key, create_obj_mesh_group());
			int _index = stbds_shgeti(*_obj_group_map, _key);
			_current_group = &(*_obj_group_map)[_index].value;
		}

		_file_line++;
	}

	fclose(_fp);
}

void parse_3_vertices_face_obj(unsigned int _i, unsigned int _v, obj_mesh_group* _group,
							   size_t _mesh_indices_size, size_t _mesh_positions_size, size_t _mesh_texcoords_size, size_t _mesh_normals_size, size_t _mesh_colors_size,
                               unsigned int* _mesh_indices, float* _mesh_positions, float* _mesh_texcoords, float* _mesh_normals, unsigned int* _mesh_colors,
                               obj_face* _face, 
                               size_t* _indices_pointer, size_t* _positions_pointer, size_t* _texcoords_pointer, size_t* _normals_pointer, size_t* _colors_pointer,
							   size_t _obj_positions_size, size_t _obj_texcoords_size, size_t _obj_normals_size,
                               float* _obj_positions, float* _obj_texcoords, float* _obj_normals) {
	
	rde_arr_s_set_uint(*_indices_pointer + 0, 3 * _i + 0, _mesh_indices, _mesh_indices_size, "Indices");
	rde_arr_s_set_uint(*_indices_pointer + 1, 3 * _i + 1, _mesh_indices, _mesh_indices_size, "Indices");
	rde_arr_s_set_uint(*_indices_pointer + 2, 3 * _i + 2, _mesh_indices, _mesh_indices_size, "Indices");

	rde_color _rde_color = { (unsigned int)(_group->diffuse_color[0] * 255), (unsigned int)(_group->diffuse_color[1] * 255), (unsigned int)(_group->diffuse_color[2] * 255), 255 };
	unsigned int _color = RDE_COLOR_TO_HEX_COLOR(_rde_color);

	unsigned int _face_index = rde_arr_s_get_uint(0 * 3, _face->indices, _face->vertices_count * 3, "Face Index");
	rde_arr_s_set_float(*_positions_pointer + 0, rde_arr_s_get_float(_face_index * 3 + 0, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 1, rde_arr_s_get_float(_face_index * 3 + 1, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 2, rde_arr_s_get_float(_face_index * 3 + 2, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_uint(*_colors_pointer + 0, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");

	_face_index = rde_arr_s_get_uint((_v + 1) * 3, _face->indices, _face->vertices_count * 3, "Face Index");
	rde_arr_s_set_float(*_positions_pointer + 3, rde_arr_s_get_float(_face_index * 3 + 0, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 4, rde_arr_s_get_float(_face_index * 3 + 1, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 5, rde_arr_s_get_float(_face_index * 3 + 2, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_uint(*_colors_pointer + 1, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
	
	_face_index = rde_arr_s_get_uint((_v + 2) * 3, _face->indices, _face->vertices_count * 3, "Face Index");
	rde_arr_s_set_float(*_positions_pointer + 6, rde_arr_s_get_float(_face_index * 3 + 0, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 7, rde_arr_s_get_float(_face_index * 3 + 1, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_float(*_positions_pointer + 8, rde_arr_s_get_float(_face_index * 3 + 2, _obj_positions, _obj_positions_size, "Positions Obj"), _mesh_positions, _mesh_positions_size * 3, "Positions Mesh");
	rde_arr_s_set_uint(*_colors_pointer + 2, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");

	_face_index = rde_arr_s_get_uint(0 * 3 + 1, _face->indices, _face->vertices_count * 3, "Face Index");
	if((_group->face_lacking_data & NO_VERTEX_UVS) == 0 && _face_index != RDE_UINT_MAX) {
		rde_arr_s_set_float(*_texcoords_pointer + 0, rde_arr_s_get_float(_face_index * 2 + 0, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");
		rde_arr_s_set_float(*_texcoords_pointer + 1, rde_arr_s_get_float(_face_index * 2 + 1, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");

		_face_index = rde_arr_s_get_uint((_v + 1) * 3 + 1, _face->indices, _face->vertices_count * 3, "Face Index");
		rde_arr_s_set_float(*_texcoords_pointer + 2, rde_arr_s_get_float(_face_index * 2 + 0, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");
		rde_arr_s_set_float(*_texcoords_pointer + 3, rde_arr_s_get_float(_face_index * 2 + 1, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");

		_face_index = rde_arr_s_get_uint((_v + 2) * 3 + 1, _face->indices, _face->vertices_count * 3, "Face Index");
		rde_arr_s_set_float(*_texcoords_pointer + 4, rde_arr_s_get_float(_face_index * 2 + 0, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");
		rde_arr_s_set_float(*_texcoords_pointer + 5, rde_arr_s_get_float(_face_index * 2 + 1, _obj_texcoords, _obj_texcoords_size, "Texcoords Obj"), _mesh_texcoords, _mesh_texcoords_size * 2, "Texcoords Mesh");
	}

	_face_index = rde_arr_s_get_uint(0 * 3 + 2, _face->indices, _face->vertices_count * 3, "Face Index");
	if((_group->face_lacking_data & NO_VERTEX_NORMALS) == 0 && _face_index != RDE_UINT_MAX) {
		rde_arr_s_set_float(*_normals_pointer + 0, rde_arr_s_get_float(_face_index * 3 + 0, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 1, rde_arr_s_get_float(_face_index * 3 + 1, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 2, rde_arr_s_get_float(_face_index * 3 + 2, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");

		_face_index = rde_arr_s_get_uint((_v + 1) * 3 + 2, _face->indices, _face->vertices_count * 3, "Face Index");
		rde_arr_s_set_float(*_normals_pointer + 3, rde_arr_s_get_float(_face_index * 3 + 0, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 4, rde_arr_s_get_float(_face_index * 3 + 1, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 5, rde_arr_s_get_float(_face_index * 3 + 2, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		
		_face_index = rde_arr_s_get_uint((_v + 2) * 3 + 2, _face->indices, _face->vertices_count * 3, "Face Index");
		rde_arr_s_set_float(*_normals_pointer + 6, rde_arr_s_get_float(_face_index * 3 + 0, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 7, rde_arr_s_get_float(_face_index * 3 + 1, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
		rde_arr_s_set_float(*_normals_pointer + 8, rde_arr_s_get_float(_face_index * 3 + 2, _obj_normals, _obj_normals_size, "Normals Obj"), _mesh_normals, _mesh_normals_size * 3, "Normals Mesh");
	}

	*_indices_pointer += 3;
	*_texcoords_pointer += 6;
	*_positions_pointer += 9;
	*_colors_pointer += 3;
	*_normals_pointer += 9;
}

void load_obj_material(const char* _obj_path, obj_mesh_group_map** _obj_group_map) {

	if(_obj_path == NULL || strlen(_obj_path) == 0) {
		return;
	}

	FILE* _fp;
	char _buffer[RDE_MAX_PATH];
	memset(_buffer, 0, RDE_MAX_PATH);
	size_t len = RDE_MAX_PATH;

#if IS_WINDOWS()
	rde_critical_error(fopen_s(&_fp, _obj_path, "r") != 0, RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD, _obj_path);
#else
	_fp = fopen(_obj_path, "r");
	rde_critical_error(_fp == NULL, RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD, _obj_path);
#endif

	obj_mesh_group* _current_group = NULL;
	rde_material* _material = NULL;

	size_t _line = 0;
	
	while(fgets(_buffer, len, _fp)) {
		if(strncmp(_buffer, "newmtl", strlen("newmtl")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr[strcspn(_ptr, "\r\n")] = 0;

			_ptr = strtok(NULL, " ");  // remove 'newmtl'
			_ptr[strcspn(_ptr, "\r\n")] = 0;

			_current_group = &(*_obj_group_map)[stbds_shgeti(*_obj_group_map, _ptr)].value;
			_material = &_current_group->material;
		} else if(strncmp(_buffer, "map_Kd", strlen("map_Kd")) == 0) {
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

			_material->texture = rde_rendering_texture_load(_full_path);
		} else if(strncmp(_buffer, "Ka", strlen("Ka")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'Ka'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			char* _end_ptr;
			float _value = strtof(_ptr, &_end_ptr);
			if(*_end_ptr != '\0') {
				rde_critical_error(true, RDE_ERROR_OBJ_INVALID_MATERIAL_DATA, "Ka", _ptr, _obj_path, "Ka", _line + 1);
			}
			_material->material_light_data.ka = _value;
		} else if(strncmp(_buffer, "Kd", strlen("Kd")) == 0) {
			int _index = 0;
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'Kd'
			while(_ptr != NULL) {
				_ptr[strcspn(_ptr, "\r\n")] = 0;
				char* _end_ptr;
				float _value = strtof(_ptr, &_end_ptr);
				if(*_end_ptr != '\0') {
					rde_critical_error(true, RDE_ERROR_OBJ_INVALID_MATERIAL_DATA, "Kd", _ptr, _obj_path, "Kd", _line + 1);
				}
				_ptr = strtok(NULL, " ");
				_current_group->diffuse_color[_index] = _value;
				_index++;
			}

			_material->material_light_data.kd = 1.f;
		} else if(strncmp(_buffer, "Ks", strlen("Ks")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'Ks'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			char* _end_ptr;
			float _value = strtof(_ptr, &_end_ptr);
			if(*_end_ptr != '\0') {
				rde_critical_error(true, RDE_ERROR_OBJ_INVALID_MATERIAL_DATA, "Ks", _ptr, _obj_path, "Ks", _line + 1);
			}
			_material->material_light_data.ks = _value;
		} else if(strncmp(_buffer, "Ke", strlen("Ke")) == 0) {
			char* _ptr = strtok(_buffer, " ");
			_ptr = strtok(NULL, " "); // remove 'Ke'
			_ptr[strcspn(_ptr, "\r\n")] = 0;
			char* _end_ptr;
			float _value = strtof(_ptr, &_end_ptr);
			if(*_end_ptr != '\0') {
				rde_critical_error(true, RDE_ERROR_OBJ_INVALID_MATERIAL_DATA, "Ke", _ptr, _obj_path, "Ke", _line + 1);
			}
			_material->material_light_data.shininess = _value;
		}
		_line++;
	}

	fclose(_fp);
}

rde_model* rde_rendering_load_obj_model(const char* _obj_path) {
	clock_t _t_start, _t_end;
	_t_start = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading OBJ '%s':", _obj_path);

	rde_model* _model = NULL;
	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if (_m->mesh_array != NULL) {
			continue;
		}

		printf("Loading model at index %zu\n", _i);
		_model = _m;
		break;
	}

	rde_critical_error(_model == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "models", ENGINE.heap_allocs_config.max_number_of_models);

	_model->mesh_array = NULL;
	_model->mesh_array_size = 1;

	float* _obj_positions = NULL;
	size_t _obj_positions_size = 0;

	float* _obj_normals = NULL;
	size_t _obj_normals_size = 0;

	float* _obj_texcoords = NULL;
	size_t _obj_texcoords_size = 0;

	char _obj_material_path[RDE_MAX_PATH];
	memset(_obj_material_path, 0, RDE_MAX_PATH);

	char _obj_path_s[RDE_MAX_PATH];
	rde_util_sanitaize_path(_obj_path, _obj_path_s, RDE_MAX_PATH);

	obj_mesh_group_map* _obj_group_map = NULL;
	read_file_and_fill_data(_obj_path_s,
							&_obj_group_map,
							&_obj_positions_size, &_obj_positions,
							&_obj_normals_size, &_obj_normals,
							&_obj_texcoords_size, &_obj_texcoords,
							_obj_material_path);

	printf("OBJ FILE: , P: %zu, T: %zu, N: %zu \n", _obj_positions_size, _obj_texcoords_size, _obj_normals_size);

	_model->mesh_array = NULL;
	_model->mesh_array_size = stbds_shlenu(_obj_group_map);

	char _material_path_s[RDE_MAX_PATH];
	rde_util_sanitaize_path(_obj_material_path, _material_path_s, RDE_MAX_PATH);

	for(size_t _g = 0; _g < _model->mesh_array_size; _g++) {
		obj_mesh_group* _group = &_obj_group_map[_g].value;
		for (size_t _i = 0; _i < _group->faces_size; _i++) {
			obj_face* _face = &(_group->faces)[_i];
			if (_face->vertices_count == 3) {
				_group->indices_size += 3;
				_group->positions_size += 3;
				_group->colors_size += 3;
				_group->texcoords_size += 3;
				_group->normals_size += 3;
			} else {
				int _indices_in_face = _face->vertices_count;
				int _moving_pointer = 1;
					
				while (_moving_pointer != _indices_in_face - 1) {
					_group->indices_size += 3;
					_group->positions_size += 3;
					_group->colors_size += 3;
					_group->texcoords_size += 3;
					_group->normals_size += 3;
					_moving_pointer++;
				}
			}
		}
		printf("MESH(%zu): I: %zu, P: %zu, C: %zu, T: %zu, N: %zu \n", _g, _group->indices_size, _group->positions_size, _group->colors_size, _group->texcoords_size, _group->normals_size);
	}

	rde_engine_supress_logs(true);
	load_obj_material(_material_path_s, &_obj_group_map);
	rde_engine_supress_logs(false);

	for(size_t _g = 0; _g < _model->mesh_array_size; _g++) {
		obj_mesh_group* _group = &_obj_group_map[_g].value;

		_group->indices = (unsigned int*)malloc(sizeof(unsigned int) * _group->indices_size * 1);
		_group->positions = (float*)malloc(sizeof(float) * _group->positions_size * 3);
		_group->colors = (unsigned int*)malloc(sizeof(unsigned int) * _group->colors_size * 1);
		_group->texcoords = NULL;
		_group->normals = NULL;

		if ((_group->face_lacking_data & NO_VERTEX_UVS) == 0) {
			_group->texcoords = (float*)malloc(sizeof(float) * _group->texcoords_size * 2);
		} else {
			_group->texcoords_size = 0;
		}

		if ((_group->face_lacking_data & NO_VERTEX_NORMALS) == 0) {
			_group->normals = (float*)malloc(sizeof(float) * _group->normals_size * 3);
		} else {
			_group->normals_size = 0;
		}

		for (size_t _i = 0; _i < _group->faces_size; _i++) {
			obj_face* _face = &_group->faces[_i];
			if (_face->vertices_count == 3) {
				parse_3_vertices_face_obj(_group->indices_pointer / 3, 0, _group,
										  _group->indices_size, _group->positions_size, _group->texcoords_size, _group->normals_size, _group->colors_size,
				                          _group->indices, _group->positions, _group->texcoords, _group->normals, _group->colors,
				                          _face,
				                          &_group->indices_pointer, &_group->positions_pointer, &_group->texcoords_pointer, &_group->normals_pointer, &_group->colors_pointer,
										  _obj_positions_size, _obj_texcoords_size, _obj_normals_size,
				                          _obj_positions, _obj_texcoords, _obj_normals);
			} else {
				for (size_t _v = 0; _v < _face->vertices_count - 2; _v++) {
					parse_3_vertices_face_obj(_group->indices_pointer / 3, _v, _group,
											  _group->indices_size, _group->positions_size, _group->texcoords_size, _group->normals_size, _group->colors_size,
					                          _group->indices, _group->positions, _group->texcoords, _group->normals, _group->colors,
					                          _face,
					                          &_group->indices_pointer, &_group->positions_pointer, &_group->texcoords_pointer, &_group->normals_pointer, &_group->colors_pointer,
											  _obj_positions_size, _obj_texcoords_size, _obj_normals_size,
					                          _obj_positions, _obj_texcoords, _obj_normals);
				}
			}
		}

		rde_mesh_gen_data _data = {
			.positions = _group->positions,
			.positions_size = _group->positions_size,
			.indices = _group->indices,
			.indices_size = _group->indices_size,
			.texcoords = _group->texcoords,
			.texcoords_size = _group->texcoords_size,
			.colors = _group->colors,
			.colors_size = _group->colors_size,
			.normals = _group->normals,
			.normals_size = _group->normals_size,
			.texture = _group->material.texture
		};
		rde_mesh _mesh = rde_struct_create_mesh(&_data);
		stbds_arrput(_model->mesh_array, _mesh);
	}

	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		rde_log_color(RDE_LOG_COLOR_GREEN, "	- Mesh (%lu):", _i);
		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Vertices: %lu", _model->mesh_array[_i].vertex_count);
		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texture: %s", (_model->mesh_array[_i].material.texture != NULL ? _model->mesh_array[_i].material.texture->file_path : "none"));
	}
	//if(_material.texture != NULL && (_lacking_face_data & NO_VERTEX_UVS) == 0) {
	//	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Texture: %s", _material.texture->file_path);
	//	rde_log_color(RDE_LOG_COLOR_GREEN, "		- Size: %ux%u", _material.texture->size.x, _material.texture->size.y);
	//	rde_log_color(RDE_LOG_COLOR_GREEN, "		- Channels: %d", _material.texture->channels);
	//	rde_log_color(RDE_LOG_COLOR_GREEN, "		- OpenGL ID: %d", _material.texture->opengl_texture_id);
	//}

	stbds_arrfree(_obj_positions);
	_obj_positions = NULL;

	if (_obj_normals != NULL) {
		stbds_arrfree(_obj_normals);
		_obj_normals = NULL;
	}
	
	if (_obj_texcoords != NULL) {
		stbds_arrfree(_obj_texcoords);
		_obj_texcoords = NULL;
	}

	size_t _total_groups = stbds_shlenu(_obj_group_map);
	for(size_t _i = 0; _i < _total_groups; _i++) {
		obj_mesh_group* _group = &(_obj_group_map[_i]).value;

		for(size_t _j = 0; _j < _group->faces_size; _j++) {
			stbds_arrfree(_group->faces[_j].indices);
			_group->faces[_j].indices = NULL;
			_group->faces[_j].vertices_count = 0;
		}
		stbds_arrfree(_group->faces);
		_group->faces = NULL;
	}
	stbds_shfree(_obj_group_map);
	_obj_group_map = NULL;

	_t_end = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "	- took %.3fms", (double)(_t_end - _t_start) / CLOCKS_PER_SEC);
	printf("\n");

	return _model;
}

#pragma clang diagnostic pop
#endif