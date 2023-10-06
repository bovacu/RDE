#ifdef RDE_OBJ_MODULE
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
	char* ptr = strtok(_buffer, " ");
	ptr = strtok(NULL, " "); // remove 'v', 'vt' or 'vn'
	while(ptr != NULL) {
		ptr[strcspn(ptr, "\r\n")] = 0;
		stbds_arrput(*_arr, strtof(ptr, NULL));
		ptr = strtok(NULL, " ");
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
                             size_t* _faces_size, obj_face** _faces) {
	FILE* _fp;
	char buffer[512];
	size_t len = 512;

#if IS_WINDOWS()
	rde_critical_error(fopen_s(&_fp, _obj_path, "r") != 0, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#else
	_fp = fopen(_obj_path, "r");
	rde_critical_error(_fp == NULL, RDE_ERROR_OBJ_COULD_NOT_LOAD, _obj_path);
#endif

	while(fgets(buffer, len, _fp)) {
		if(buffer[0] == 'v') {
			if(buffer[1] == ' ') {
				separate_into_floats(buffer, _positions, _positions_size);
			} else {
				if(buffer[1] == 'n') {
					separate_into_floats(buffer, _normals, _normals_size);
				} else if(buffer[1] == 't') {
					separate_into_floats(buffer, _texcoords, _texcoords_size);
				}
			}
		} else if(buffer[0] == 'f') {
			separate_into_faces(buffer, _faces, _faces_size);
		}
	}

	fclose(_fp);
}



rde_model* rde_rendering_load_obj_model(const char* _obj_path) {
	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading OBJ '%s':", _obj_path);
	rde_model* _model = (rde_model*)malloc(sizeof(rde_model));
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

	read_file_and_fill_data(_obj_path, 
	                        &_positions_size, &_positions, 
	                        &_normals_size, &_normals, 
	                        &_texcoords_size, &_texcoords, 
	                        &_faces_size, &_faces);

#if 0
	printf("Positions: %zu \n", _positions_size);
	for(size_t _i = 0; _i < _positions_size; _i += 3) {
		printf("(%f, %f, %f) \n", _positions[_i + 0], _positions[_i + 1], _positions[_i + 2]);
	}

	printf("Normals: %zu \n", _normals_size);
	for(size_t _i = 0; _i < _normals_size; _i += 3) {
		printf("(%f, %f, %f) \n", _normals[_i + 0], _normals[_i + 1], _normals[_i + 2]);
	}

	printf("Texcoords: %zu \n", _texcoords_size);
	for(size_t _i = 0; _i < _texcoords_size; _i += 2) {
		printf("(%f, %f) \n", _texcoords[_i + 0], _texcoords[_i + 1]);
	}

	printf("Face vertices: %zu \n", _faces_size);
	for(size_t _i = 0; _i < _faces_size; _i++) {
		printf("%u vertices ", _faces[_i].vertices_count);
		for(size_t _j = 0; _j < _faces[_i].vertices_count; _j++) {
			printf("(v:%u, t:%u, n:%u) ", _faces[_i].indices[_j * 3 + 0], _faces[_i].indices[_j * 3 + 1], _faces[_i].indices[_j * 3 + 2]);
		}
		printf("\n");

		for(size_t _k = 0; _k < _faces[_i].vertices_count * 3; _k++) {
			printf("%u ", _faces[_i].indices[_k]);
		}
		printf("\n");
	}
#endif

	uint _mesh_indices_size = 0;
	size_t _mesh_positions_size = 0;
	unsigned int _mesh_texcoords_size = 0;
	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];
		if (_face->vertices_count == 3) {
			_mesh_indices_size += 3;
			_mesh_positions_size += 3;
			_mesh_texcoords_size += 3;
		} else if(_face->vertices_count == 4) {
			_mesh_indices_size += 6;
			_mesh_positions_size += 4;
			_mesh_texcoords_size += 4;
		} else {
			int _indices_in_face = _face->vertices_count;
			int _moving_pointer = 1;
					
			while(_moving_pointer != _indices_in_face - 1) {
				_mesh_indices_size += 3;
				_mesh_positions_size += 3;
				_mesh_texcoords_size += 3;
				_moving_pointer++;
			}
		}
	}

	unsigned int* _mesh_indices = (unsigned int*)malloc(sizeof(unsigned int) * _mesh_indices_size * 1);
	float* _mesh_positions = (float*)malloc(sizeof(float) * _mesh_positions_size * 3);
	float* _mesh_texcoords = (float*)malloc(sizeof(float) * _mesh_texcoords_size * 2);

	unsigned int _indices_pointer = 0;
	unsigned int _positions_pointer = 0;
	unsigned int _texcoords_pointer = 0;

	rde_texture* _texture = rde_rendering_load_texture("textures/test.png");

	for(size_t _i = 0; _i < _faces_size; _i++) {
		obj_face* _face = &_faces[_i];

		if (_face->vertices_count == 3) {
			_mesh_indices[_indices_pointer + 0] = 3 * _i + 0;
			_mesh_indices[_indices_pointer + 1] = 3 * _i + 1;
			_mesh_indices[_indices_pointer + 2] = 3 * _i + 2;

			_mesh_positions[_positions_pointer + 0] = _positions[_face->indices[0 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 1] = _positions[_face->indices[0 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 2] = _positions[_face->indices[0 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 0] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 1] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 1];

			_mesh_positions[_positions_pointer + 3] = _positions[_face->indices[1 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 4] = _positions[_face->indices[1 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 5] = _positions[_face->indices[1 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 2] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 3] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 1];
			
			_mesh_positions[_positions_pointer + 6] = _positions[_face->indices[2 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 7] = _positions[_face->indices[2 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 8] = _positions[_face->indices[2 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 4] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 5] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 1];

			_indices_pointer += 3;
			_positions_pointer += 9;
			_texcoords_pointer += 6;

		} else if(_face->vertices_count == 4) {
			// CCW    	CW
			// 0		0
			// 1		2
			// 3		1
			// 3		1
			// 2		2
			// 0		3
			_mesh_indices[_indices_pointer + 0] = 4 * _i + 0; // This gets the 1st element of each group of indices values 
			_mesh_indices[_indices_pointer + 1] = 4 * _i + 1;
			_mesh_indices[_indices_pointer + 2] = 4 * _i + 2;
			_mesh_indices[_indices_pointer + 3] = 4 * _i + 2;
			_mesh_indices[_indices_pointer + 4] = 4 * _i + 3;
			_mesh_indices[_indices_pointer + 5] = 4 * _i + 0;

			_mesh_positions[_positions_pointer + 0] = _positions[_face->indices[0 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 1] = _positions[_face->indices[0 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 2] = _positions[_face->indices[0 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 0] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 1] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 1];

			_mesh_positions[_positions_pointer + 3] = _positions[_face->indices[1 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 4] = _positions[_face->indices[1 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 5] = _positions[_face->indices[1 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 2] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 3] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 1];
			
			_mesh_positions[_positions_pointer + 6] = _positions[_face->indices[2 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 7] = _positions[_face->indices[2 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 8] = _positions[_face->indices[2 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 4] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 5] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 1];

			_mesh_positions[_positions_pointer + 9]  = _positions[_face->indices[3 * 3] * 3 + 0];
			_mesh_positions[_positions_pointer + 10] = _positions[_face->indices[3 * 3] * 3 + 1];
			_mesh_positions[_positions_pointer + 11] = _positions[_face->indices[3 * 3] * 3 + 2];
			_mesh_texcoords[_texcoords_pointer + 6]  = _texcoords[_face->indices[3 * 3 + 1] * 2 + 0];
			_mesh_texcoords[_texcoords_pointer + 7]  = _texcoords[_face->indices[3 * 3 + 1] * 2 + 1];

			_indices_pointer += 6;
			_positions_pointer += 12;
			_texcoords_pointer += 8;

		} else {
			int _indices_in_face = _face->vertices_count;
			int _moving_pointer = 1;
					
			while(_moving_pointer != _indices_in_face - 1) {
				_mesh_indices[_indices_pointer + 0] = 4 * _i + 0;
				_mesh_indices[_indices_pointer + 1] = 4 * _i + _moving_pointer * 3;
				_mesh_indices[_indices_pointer + 2] = 4 * _i + (_moving_pointer + 1) * 3;
				
				_mesh_positions[_positions_pointer + 0] = _positions[_face->indices[0 * 3] * 3 + 0];
				_mesh_positions[_positions_pointer + 1] = _positions[_face->indices[0 * 3] * 3 + 1];
				_mesh_positions[_positions_pointer + 2] = _positions[_face->indices[0 * 3] * 3 + 2];
				_mesh_texcoords[_texcoords_pointer + 0] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 0];
				_mesh_texcoords[_texcoords_pointer + 1] = _texcoords[_face->indices[0 * 3 + 1] * 2 + 1];

				_mesh_positions[_positions_pointer + 3] = _positions[_face->indices[1 * 3] * 3 + 0];
				_mesh_positions[_positions_pointer + 4] = _positions[_face->indices[1 * 3] * 3 + 1];
				_mesh_positions[_positions_pointer + 5] = _positions[_face->indices[1 * 3] * 3 + 2];
				_mesh_texcoords[_texcoords_pointer + 2] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 0];
				_mesh_texcoords[_texcoords_pointer + 3] = _texcoords[_face->indices[1 * 3 + 1] * 2 + 1];
				
				_mesh_positions[_positions_pointer + 6] = _positions[_face->indices[2 * 3] * 3 + 0];
				_mesh_positions[_positions_pointer + 7] = _positions[_face->indices[2 * 3] * 3 + 1];
				_mesh_positions[_positions_pointer + 8] = _positions[_face->indices[2 * 3] * 3 + 2];
				_mesh_texcoords[_texcoords_pointer + 4] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 0];
				_mesh_texcoords[_texcoords_pointer + 5] = _texcoords[_face->indices[2 * 3 + 1] * 2 + 1];

				_indices_pointer += 3;
				_positions_pointer += 9;
				_texcoords_pointer += 6;
				_moving_pointer++;
			}
		}
	}

	rde_log_color(RDE_LOG_COLOR_GREEN, "	- vertices: %u, indices: %u, texcoords: %u \n", _mesh_positions_size, _mesh_indices_size, _mesh_texcoords_size);

	rde_mesh* _mesh = rde_struct_create_mesh(_mesh_positions_size, _mesh_indices_size);
	rde_rendering_mesh_set_vertex_positions(_mesh, _mesh_positions, true);
	rde_rendering_mesh_set_indices(_mesh, _mesh_indices, true);
	rde_rendering_mesh_set_vertex_texture_data(_mesh, _mesh_texcoords_size, _mesh_texcoords, _texture, true);

	stbds_arrput(_model->mesh_array, _mesh);



	stbds_arrfree(_positions);
	stbds_arrfree(_normals);
	stbds_arrfree(_texcoords);
	stbds_arrfree(_faces->indices);
	stbds_arrfree(_faces);

	return _model;
}

void rde_rendering_unload_obj_model(rde_model* _model) {
	UNUSED(_model);
}

#pragma clang diagnostic pop
#endif