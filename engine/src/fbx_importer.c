#if defined(RDE_FBX_MODULE) && defined(RDE_RENDERING_MODULE)

#include "ufbx/ufbx.c"
rde_model* rde_rendering_load_fbx_model(const char* _fbx_path, const char* _texture_path);

void parse_3_vertices_face_fbx(uint _i, uint _v, uint* _mesh_indices, 
                               float* _mesh_positions, float* _mesh_texcoords, ufbx_face* _face, ufbx_mesh* _mesh,
                           	   uint* _indices_pointer, uint* _positions_pointer, 
                           	   uint* _texcoords_pointer) {
	_mesh_indices[*_indices_pointer + 0] = 3 * _i + 0;
	_mesh_indices[*_indices_pointer + 1] = 3 * _i + 1;
	_mesh_indices[*_indices_pointer + 2] = 3 * _i + 2;

	_mesh_positions[*_positions_pointer + 0] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].x;
	_mesh_positions[*_positions_pointer + 1] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].y;
	_mesh_positions[*_positions_pointer + 2] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].z;

	_mesh_positions[*_positions_pointer + 3] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].x;
	_mesh_positions[*_positions_pointer + 4] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].y;
	_mesh_positions[*_positions_pointer + 5] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].z;
	
	_mesh_positions[*_positions_pointer + 6] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].x;
	_mesh_positions[*_positions_pointer + 7] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].y;
	_mesh_positions[*_positions_pointer + 8] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].z;
	
	if(_mesh_texcoords != NULL) {
		_mesh_texcoords[*_texcoords_pointer + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + 0]].x;
		_mesh_texcoords[*_texcoords_pointer + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + 0]].y;

		_mesh_texcoords[*_texcoords_pointer + 2] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 1)]].x;
		_mesh_texcoords[*_texcoords_pointer + 3] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 1)]].y;

		_mesh_texcoords[*_texcoords_pointer + 4] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 2)]].x;
		_mesh_texcoords[*_texcoords_pointer + 5] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 2)]].y;
	}

	*_indices_pointer += 3;
	*_positions_pointer += 9;
	*_texcoords_pointer += 6;
}

rde_model* rde_rendering_model_fbx_load(const char* _fbx_path, const char* _texture_path) {
	rde_model* _model = NULL;
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if(_m->mesh_array != NULL) {
			continue;
		}

		_model = _m;
		break;
	}

    ufbx_load_opts _opts = { 0 };
    ufbx_error _error;
    ufbx_scene* _scene = ufbx_load_file(_fbx_path, &_opts, &_error);
    rde_critical_error(_scene == NULL, RDE_ERROR_FBX_COULD_NOT_LOAD, _fbx_path, _error.description.data);

	uint _nodes_size = _scene->nodes.count;

	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;
	
	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading FBX '%s' with %u nodes:", _fbx_path, _nodes_size);

    for (size_t i = 0; i < _scene->nodes.count; i++) {
        ufbx_node *node = _scene->nodes.data[i];
		if (node->is_root || node->mesh == NULL) {
			continue;
		}

        if (node->mesh) {
			ufbx_mesh* _mesh = node->mesh;
			size_t _faces_size = _mesh->faces.count;
			
			uint _mesh_indices_size = 0;
			size_t _mesh_positions_size = 0;
			uint _mesh_texcoords_size = 0;

			for(size_t _i = 0; _i < _faces_size; _i++) {
				ufbx_face* _face = &_mesh->faces.data[_i];

				if (_face->num_indices == 3) {
					_mesh_indices_size += 3;
					_mesh_positions_size += 3;
					_mesh_texcoords_size += 3;
				} else {
					int _indices_in_face = _face->num_indices;
					int _moving_pointer = 1;
					
					while(_moving_pointer != _indices_in_face - 1) {
						_mesh_indices_size += 3;
						_mesh_positions_size += 3;
						_mesh_texcoords_size += 3;
						_moving_pointer++;
					}
				}
			}

			if(_mesh_indices_size == 0 || _mesh->vertex_position.values.count == 0) {
				continue;
			}

			uint* _mesh_indices = (uint*)malloc(sizeof(uint) * _mesh_indices_size * 1);
			float* _mesh_positions = (float*)malloc(sizeof(float) * _mesh_positions_size * 3);
			float* _mesh_texcoords = (float*)malloc(sizeof(float) * _mesh_texcoords_size * 2);;

			uint _indices_pointer = 0;
			uint _positions_pointer = 0;
			uint _texcoords_pointer = 0;

			for(size_t _i = 0; _i < _faces_size; _i++) {
				ufbx_face* _face = &_mesh->faces.data[_i];

				if (_face->num_indices == 3) {
					parse_3_vertices_face_fbx(_indices_pointer / 3, 0,
					                          _mesh_indices, _mesh_positions, _mesh_texcoords, 
					                          _face, _mesh,
					                          &_indices_pointer, &_positions_pointer, &_texcoords_pointer);
				} else {
					for(size_t _v = 0; _v < _face->num_indices - 2; _v++) {
						parse_3_vertices_face_fbx(_indices_pointer / 3, _v,
						                          _mesh_indices, _mesh_positions, _mesh_texcoords, 
						                          _face, _mesh,
						                          &_indices_pointer, &_positions_pointer, &_texcoords_pointer);
					}
				}
			}
			
			rde_mesh _m = rde_inner_struct_create_mesh(_mesh_positions_size, _mesh_indices_size);
			rde_rendering_mesh_set_vertex_positions(&_m, _mesh_positions, true);
			rde_rendering_mesh_set_indices(&_m, _mesh_indices, true);

			rde_texture* _texture = rde_rendering_texture_load(_texture_path, NULL);
			rde_rendering_mesh_set_vertex_texture_data(&_m, _mesh_texcoords_size, _mesh_texcoords, _texture, true);

			rde_log_color(RDE_LOG_COLOR_GREEN, "	- vertices: %u, indices: %u, texcoords: %u \n", _mesh_positions_size, _mesh_indices_size, _mesh_texcoords_size);

			stbds_arrput(_model->mesh_array, _m);
			_model->mesh_array_size++;
		}
    }

    ufbx_free_scene(_scene);
	return _model;
}

#endif