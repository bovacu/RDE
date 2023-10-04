#ifdef RDE_FBX_MODULE

#include "ufbx/ufbx.c"

rde_model* rde_rendering_load_fbx_model(const char* _fbx_path, const char* _texture_path) {
    ufbx_load_opts _opts = { 0 };
    ufbx_error _error;
    ufbx_scene* _scene = ufbx_load_file(_fbx_path, &_opts, &_error);
    rde_critical_error(_scene == NULL, RDE_ERROR_FBX_COULD_NOT_LOAD, _fbx_path, _error.description.data);
    
	rde_model* _model = (rde_model*)malloc(sizeof(rde_model));
	rde_texture* _texture = rde_rendering_load_texture(_texture_path);
	(void)_texture;

	uint _nodes_size = _scene->nodes.count;

	_model->mesh_array = (rde_mesh**)malloc(sizeof(rde_mesh*) * _nodes_size);

	for(uint _i = 0; _i < _nodes_size; _i++) {
		_model->mesh_array[_i] = NULL;
	}

	_model->mesh_array_size = _nodes_size;
	
	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading FBX '%s' with %u nodes:", _fbx_path, _nodes_size);
    for (size_t i = 0; i < _scene->nodes.count; i++) {
        ufbx_node *node = _scene->nodes.data[i];
		if (node->is_root || node->mesh == NULL) {
			continue;
		}

        if (node->mesh) {
			ufbx_mesh* _mesh = node->mesh;
			
			float* _positions = (float*)malloc(sizeof(ufbx_vec3) * _mesh->vertex_position.values.count);
			
			uint _indices_size = 0;
			for(size_t _v = 0; _v < _mesh->faces.count; _v++) {
				ufbx_face* _face = &_mesh->faces.data[_v];
				if (_face->num_indices == 3) {
					_indices_size += 3;
				} else if(_face->num_indices == 4) {
					_indices_size += 6;
				} else {
					int _indices_in_face = _face->num_indices;
					int _moving_pointer = 1;
					
					while(_moving_pointer != _indices_in_face - 1) {
						_indices_size += 3;
						_moving_pointer++;
					}
				}
			}

			if(_indices_size == 0 || _mesh->vertex_position.values.count == 0) {
				continue;
			}

			rde_log_color(RDE_LOG_COLOR_GREEN, "	- Object: %s, vertices: %u, indices: %u", node->name.data, _mesh->vertex_position.values.count, _indices_size);
			unsigned int* _indices = (unsigned int*)malloc(sizeof(unsigned int) * _indices_size);

			for(size_t _v = 0; _v < _mesh->vertex_position.values.count; _v++) {
				_positions[_v * 3 + 0] = _mesh->vertex_position.values.data[_v].x;
				_positions[_v * 3 + 1] = _mesh->vertex_position.values.data[_v].y;
				_positions[_v * 3 + 2] = _mesh->vertex_position.values.data[_v].z;
			}

			printf("UVs: %zu, Indices: %zu \n", _mesh->vertex_uv.values.count, _mesh->vertex_uv.indices.count);
			float* _uvs = (float*)malloc(sizeof(ufbx_vec2) * _indices_size);

			uint _indices_pointer = 0;
			// uint _uv_pointer = 0;
			for(size_t _v = 0; _v < _mesh->faces.count; _v++) {
				ufbx_face* _face = &_mesh->faces.data[_v];

				if (_face->num_indices == 3) {
					_indices[_indices_pointer + 0] = _mesh->vertex_position.indices.data[_face->index_begin + 0];
					_indices[_indices_pointer + 1] = _mesh->vertex_position.indices.data[_face->index_begin + 1];
					_indices[_indices_pointer + 2] = _mesh->vertex_position.indices.data[_face->index_begin + 2];
					_indices_pointer += 3;
				} else if(_face->num_indices == 4) {
					_indices[_indices_pointer + 0] = _mesh->vertex_position.indices.data[_face->index_begin + 0];
					_indices[_indices_pointer + 1] = _mesh->vertex_position.indices.data[_face->index_begin + 1];
					_indices[_indices_pointer + 2] = _mesh->vertex_position.indices.data[_face->index_begin + 2];
					_indices[_indices_pointer + 3] = _mesh->vertex_position.indices.data[_face->index_begin + 2];
					_indices[_indices_pointer + 4] = _mesh->vertex_position.indices.data[_face->index_begin + 3];
					_indices[_indices_pointer + 5] = _mesh->vertex_position.indices.data[_face->index_begin + 0];
					_indices_pointer += 6;
				} else {
					int _indices_in_face = _face->num_indices;
					int _moving_pointer = 1;
					
					while(_moving_pointer != _indices_in_face - 1) {
						_indices[_indices_pointer + 0] = _mesh->vertex_position.indices.data[_face->index_begin + 0];
						_indices[_indices_pointer + 1] = _mesh->vertex_position.indices.data[_face->index_begin + _moving_pointer];
						_indices[_indices_pointer + 2] = _mesh->vertex_position.indices.data[_face->index_begin + _moving_pointer + 1];
						_indices_pointer += 3;
						_moving_pointer++;
					}
				}
			}

			printf("\n");
			for(size_t _v = 0; _v < _indices_size; _v++) { 
				printf("%u \n", _indices[_v]);
			}
			printf("\n");

			#define set_uv(_index) \
				_uvs[(_v + _index) * 2 + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + _index]].x; \
				_uvs[(_v + _index) * 2 + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + _index]].y; \
				printf("uv index %u (%f, %f) \n", _mesh->vertex_uv.indices.data[_v + _index], _uvs[(_v + _index) * 2 + 0], _uvs[(_v + _index) * 2 + 1]);

			printf("FBX UVs: %zu, _uvs: %llu \n", _mesh->vertex_uv.indices.count, _mesh->vertex_uv.values.count * 2);
			for(size_t _v = 0; _v < _mesh->vertex_uv.values.count; _v += 4) {

				uint _index = (_mesh->vertex_uv.values.count - 1 - (_v + 3)) * 2;
				_uvs[_index + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 0]].x;
				_uvs[_index + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 0]].y;
				printf("uv index %u (%f, %f) \n", _mesh->vertex_uv.indices.data[_v + 3], _uvs[_index + 0], _uvs[_index + 1]);

				_index = (_mesh->vertex_uv.values.count - 1 - (_v + 2)) * 2;
				_uvs[_index + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 1]].x;
				_uvs[_index + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 1]].y;
				printf("uv index %u (%f, %f) \n", _mesh->vertex_uv.indices.data[_v + 2], _uvs[_index + 0], _uvs[_index + 1]);

				_index = (_mesh->vertex_uv.values.count - 1 - (_v + 0)) * 2;
				_uvs[_index + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 2]].x;
				_uvs[_index + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 2]].y;
				printf("uv index %u (%f, %f) \n", _mesh->vertex_uv.indices.data[_v + 0], _uvs[_index + 0], _uvs[_index + 1]);

				_index = (_mesh->vertex_uv.values.count - 1 - (_v + 1)) * 2;
				_uvs[_index + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 3]].x;
				_uvs[_index + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v + 3]].y;
				printf("uv index %u (%f, %f) \n", _mesh->vertex_uv.indices.data[_v + 1], _uvs[_index + 0], _uvs[_index + 1]);
			}
			printf("\n");
			for(size_t _v = 0; _v < _mesh->vertex_uv.values.count; _v++) { 
				printf("%u - (%f, %f) \n", _mesh->vertex_position.indices.data[_v], _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v]].x, _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_v]].y);
			}

			printf("\n");
			for(size_t _v = 0; _v < _mesh->vertex_uv.values.count; _v++) { 
				printf("%u - (%f, %f) \n", _mesh->vertex_position.indices.data[_v], _uvs[_v * 2 + 0], _uvs[_v * 2 + 1]);
			}
			
			rde_mesh* _m = rde_struct_create_mesh(_mesh->vertex_position.values.count, _indices_size);
			rde_rendering_mesh_set_vertex_positions(_m, _positions, true);
			rde_rendering_mesh_set_indices(_m, _indices, true);
			rde_rendering_mesh_set_vertex_texture_data(_m, _mesh->vertex_uv.values.count, _uvs, _texture, true);

			_model->mesh_array[i] = _m;
		}
    }

	printf("\n");
    ufbx_free_scene(_scene);
	return _model;
}

void rde_rendering_unload_fbx_model(rde_model* _model) {
    (void)_model;
}

#endif