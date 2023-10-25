#if defined(RDE_OBJ_MODULE) && defined(RDE_RENDERING_MODULE)

// TODO: remove this diagnostics, basically change strtok for strtok_s on Windows
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fast_obj/fast_obj.c"

typedef struct {
	size_t vertex_count;

	float* positions;
	size_t positions_size;
	size_t positions_pointer;

	float* texcoords;
	size_t texcoords_size;
	size_t texcoords_pointer;

	float* normals;
	size_t normals_size;
	size_t normals_pointer;

	uint* colors;
	size_t colors_size;
	size_t colors_pointer;

	fastObjMaterial* material;
	rde_texture* map_ka;
	rde_texture* map_kd;
	rde_texture* map_ks;
	rde_texture* map_bump;
} rde_obj_mesh_data;
rde_obj_mesh_data rde_struct_create_obj_mesh_data() {
	rde_obj_mesh_data _o;
	_o.vertex_count = 0;
	_o.material = NULL;

	_o.positions_size = 0;
	_o.positions = NULL;
	_o.positions_pointer = 0;

	_o.texcoords = NULL;
	_o.texcoords_size = 0;
	_o.texcoords_pointer = 0;

	_o.normals = NULL;
	_o.normals_size = 0;
	_o.normals_pointer = 0;

	_o.colors = NULL;
	_o.colors_size = 0;
	_o.colors_pointer = 0;

	_o.map_ka = NULL;
	_o.map_kd = NULL;
	_o.map_ks = NULL;
	_o.map_bump = NULL;

	return _o;
}
void rde_fill_obj_mesh_data(rde_obj_mesh_data* _data, fastObjMaterial* _material, bool _has_t, bool _has_n) {
	_data->material = _material;

	_data->positions_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	_data->positions = (float*)malloc(sizeof(float) * _data->positions_size);

	if(_has_t) {
		_data->texcoords_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		_data->texcoords = (float*)malloc(sizeof(float) * _data->texcoords_size);
	}

	if(_has_n) {
		_data->normals_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		_data->normals = (float*)malloc(sizeof(float) * _data->normals_size);
	}

	_data->material = _material;

	if(_material != NULL) {
		
	}

	if(_material != NULL && _material->map_Ka.path != NULL && strlen(_material->map_Ka.path) > 0) {
		_data->map_ka = rde_rendering_texture_load(_material->map_Ka.path);
	}

	if(_material != NULL && _material->map_Kd.path != NULL && strlen(_material->map_Kd.path) > 0) {
		_data->map_kd = rde_rendering_texture_load(_material->map_Kd.path);
	}

	if(_material != NULL && _material->map_Ks.path != NULL && strlen(_material->map_Ks.path) > 0) {
		_data->map_ks = rde_rendering_texture_load(_material->map_Ks.path);
	}

	if(_material != NULL && _material->map_bump.path != NULL && strlen(_material->map_bump.path) > 0) {
		_data->map_bump = rde_rendering_texture_load(_material->map_bump.path);
	}
}

RDE_IMPLEMENT_SAFE_ARR_ACCESS(fastObjIndex)

void parse_3_vertices_face_obj(unsigned int _v, uint _offset, fastObjMesh* _mesh, rde_obj_mesh_data* _obj_mesh_data) {
	
//	rde_color _rde_color = { (unsigned int)(_group->diffuse_color[0] * 255), (unsigned int)(_group->diffuse_color[1] * 255), (unsigned int)(_group->diffuse_color[2] * 255), 255 };
//	unsigned int _color = RDE_COLOR_TO_HEX_COLOR(_rde_color);
	fastObjIndex _face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 0, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 1, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 2, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	//rde_arr_s_set_uint(*_colors_pointer + 0, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
	//printf("%u -> (%f, %f, %f) ", _face_index.p,_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 0],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 1],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 2]);

	_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 3, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 4, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 5, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	//rde_arr_s_set_uint(*_colors_pointer + 1, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
	//printf("%u -> (%f, %f, %f) ", _face_index.p,_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 3],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 4],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 5]);

	_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 6, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 7, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 8, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	//rde_arr_s_set_uint(*_colors_pointer + 2, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
	//printf("%u -> (%f, %f, %f) ", _face_index.p,_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 6],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 7],_obj_mesh_data->positions[_obj_mesh_data->positions_pointer + 8]);
	//printf(" | ");

	_face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	if(_obj_mesh_data->texcoords != NULL && _obj_mesh_data->texcoords_size > 0) {
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 0, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 1, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		//printf("%u -> (%f, %f) ", 	_face_index.t,_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 0],_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 1]);

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 2, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 3, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		//printf("%u -> (%f, %f) ", 	_face_index.t,_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 2],_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 3]);

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 4, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 5, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		//printf("%u -> (%f, %f) ", 	_face_index.t,_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 4],_obj_mesh_data->texcoords[_obj_mesh_data->texcoords_pointer + 5]);
	}
	//printf(" | ");
//
	_face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	if(_obj_mesh_data->normals != NULL && _obj_mesh_data->normals_size > 0) {
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 0, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 1, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 2, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		//rde_arr_s_set_uint(*_colors_pointer + 0, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
		//printf("%u -> (%f, %f, %f) ", _face_index.p,_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 0],_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 1],_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 2]);

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 3, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 4, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 5, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		//rde_arr_s_set_uint(*_colors_pointer + 1, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
		//printf("%u -> (%f, %f, %f) ", _face_index.p,_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 3],_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 4],_obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 5]);

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 6, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 7, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 8, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		//rde_arr_s_set_uint(*_colors_pointer + 2, _color, _mesh_colors, _mesh_colors_size, "Colors Mesh");
		//printf("%u -> (%f, %f, %f) ", _face_index.p, _obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 6], _obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 7], _obj_mesh_data->normals[_obj_mesh_data->normals_pointer + 8]);
	}

	_obj_mesh_data->positions_pointer += 9;
	_obj_mesh_data->texcoords_pointer += 6;
	_obj_mesh_data->normals_pointer += 9;
	//_obj_mesh_data->colors_pointer += 3;
}

typedef struct {
	rde_obj_mesh_data value;
	int key;
} rde_obj_map_entry;

rde_model* rde_rendering_load_obj_model(const char* _obj_path) {
//	clock_t _t_start, _t_end;
//	_t_start = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading OBJ '%s':", _obj_path);

	rde_model* _model = NULL;
	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if (_m->mesh_array != NULL) {
			continue;
		}

		_model = _m;
		break;
	}

	rde_critical_error(_model == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "models", ENGINE.heap_allocs_config.max_number_of_models);

	fastObjMesh* _mesh = fast_obj_read(_obj_path);
//	printf("# of object    = %d\n", (int)_mesh->object_count);
//	printf("# of groups    = %d\n", (int)_mesh->group_count);
//	printf("# of materials = %d\n", (int)_mesh->material_count);
//	printf("# of positions = %d\n", (int)_mesh->position_count);
//	printf("# of normals = %d\n", (int)_mesh->normal_count);
//	printf("# of colors = %d\n", (int)_mesh->color_count);
//	printf("# of texcoords = %d\n", (int)_mesh->texcoord_count);
//	printf("# of faces = %d\n", (int)_mesh->face_count);
//	printf("# of indices = %d\n", (int)_mesh->index_count);

	size_t _material_count = _mesh->material_count == 0 ? 1 : _mesh->material_count;
	rde_obj_mesh_data* _obj = (rde_obj_mesh_data*)malloc(sizeof(rde_obj_mesh_data) * _material_count);
	for(size_t _i = 0; _i < _material_count; _i++) {
		_obj[_i] = rde_struct_create_obj_mesh_data();
	}

	fastObjGroup* _group_or_obj = _mesh->objects;
	uint _group_or_object_count = _mesh->object_count;
	if(_group_or_obj[0].name == NULL && _mesh->groups[0].name != NULL) {
		_group_or_obj = _mesh->groups;
		_group_or_object_count = _mesh->group_count;
	}

	//rde_critical_error(_mesh->objects[0].name != NULL && _mesh->groups[0].name != NULL, ".obj with both objects and groups.\n");

	for(size_t _i = 0; _i < _group_or_object_count; _i++) {
		rde_obj_mesh_data* _obj_mesh_data = NULL;
		fastObjGroup* _o_or_g_inner = &_group_or_obj[_i];
		for (size_t _j = 0; _j < _o_or_g_inner->face_count; _j++) {
			uint _material_key = _mesh->face_materials[_o_or_g_inner->face_offset + _j];
			_obj_mesh_data = &_obj[_material_key];
			
			uint _amount_of_elements_on_face_vertex = _mesh->face_vertices[_o_or_g_inner->face_offset + _j];
			if (_amount_of_elements_on_face_vertex == 3) {
				_obj_mesh_data->vertex_count += 3;
			} else {
				for (size_t _v = 0; _v < _amount_of_elements_on_face_vertex - 2; _v++) {
					_obj_mesh_data->vertex_count += 3;
				}
			}
		}
	}

	uint _offset = 0;
	for(size_t _i = 0; _i < _group_or_object_count; _i++) {
		fastObjGroup* _o_or_g = &_group_or_obj[_i];

		for (size_t _j = 0; _j < _o_or_g->face_count; _j++) {
			uint _material_key = _mesh->face_materials[_o_or_g->face_offset + _j];
			rde_obj_mesh_data* _obj_mesh_data = NULL;

			_obj_mesh_data = &_obj[_material_key];
			if(_obj_mesh_data->positions == NULL) {
				fastObjIndex _index = _mesh->indices[_o_or_g->index_offset + _j];
				rde_fill_obj_mesh_data(_obj_mesh_data, 
				                       &_mesh->materials[_material_key], 
				                       _index.t != 0,
				                       _index.n != 0);
			}

			uint _amount_of_elements_on_face_vertex = _mesh->face_vertices[_o_or_g->face_offset + _j];
			if (_amount_of_elements_on_face_vertex == 3) {
				parse_3_vertices_face_obj(0, _offset, _mesh, &_obj[_material_key]);
			} else {
				for (size_t _v = 0; _v < _amount_of_elements_on_face_vertex - 2; _v++) {
					parse_3_vertices_face_obj(_v, _offset, _mesh, &_obj[_material_key]);
				}
			}
			_offset += _amount_of_elements_on_face_vertex;
		}
	}

	for(size_t _i = 0; _i < _material_count; _i++) {
		rde_obj_mesh_data* _obj_mesh_data = &_obj[_i];

		if(_obj_mesh_data->vertex_count <= 0) {
			continue;
		}

		rde_mesh_gen_data _data = {
			.vertex_count = _obj_mesh_data->vertex_count,
			.positions = _obj_mesh_data->positions,
			.positions_size = _obj_mesh_data->positions_size,
			.texcoords = _obj_mesh_data->texcoords,
			.texcoords_size = _obj_mesh_data->texcoords_size,
			.colors = _obj_mesh_data->colors,
			.colors_size = _obj_mesh_data->colors_size,
			.normals = _obj_mesh_data->normals,
			.normals_size = _obj_mesh_data->normals_size,
			.material = {
				.map_ka = _obj_mesh_data->map_ka,
				.map_kd = _obj_mesh_data->map_kd,
				.map_ks = _obj_mesh_data->map_ks,
				.map_bump = _obj_mesh_data->map_bump,
			}
		};

		if(_obj_mesh_data->material != NULL) {
			_data.material.material_light_data = (rde_material_light_data) {
				.shininess = _obj_mesh_data->material->Ns,
				.ka = { _obj_mesh_data->material->Ka[0], _obj_mesh_data->material->Ka[1], _obj_mesh_data->material->Ka[2] },
				.kd = { _obj_mesh_data->material->Kd[0], _obj_mesh_data->material->Kd[1], _obj_mesh_data->material->Kd[2] },
				.ks = { _obj_mesh_data->material->Ks[0], _obj_mesh_data->material->Ks[1], _obj_mesh_data->material->Ks[2] }
			};
		} else {
			_data.material.material_light_data = rde_struct_create_material_light_data();
		}
		
		rde_engine_supress_logs(true);
		rde_mesh _mesh = rde_struct_create_mesh(&_data);
		rde_engine_supress_logs(false);
		stbds_arrput(_model->mesh_array, _mesh);
		_model->mesh_array_size++;
	}


	fast_obj_destroy(_mesh);
	free(_obj);
//
//	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
//		rde_log_color(RDE_LOG_COLOR_GREEN, "	- Mesh (%lu):", _i);
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Vertices: %lu", _model->mesh_array[_i].vertex_count);
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Positions (x, y, z): %lu", _model->mesh_array[_i].vertex_count * 3);
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texcoords (u, vertex_count): %lu", _model->mesh_array[_i].vertex_count * 2);
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Normals (x, y, z): %lu", _model->mesh_array[_i].vertex_count * 3);
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texture (Ka): %s", (_model->mesh_array[_i].material.map_ka != NULL ? _model->mesh_array[_i].material.map_ka->file_path : "none"));
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texture (Kd): %s", (_model->mesh_array[_i].material.map_kd != NULL ? _model->mesh_array[_i].material.map_kd->file_path : "none"));
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texture (Ks): %s", (_model->mesh_array[_i].material.map_ks != NULL ? _model->mesh_array[_i].material.map_ks->file_path : "none"));
//		rde_log_color(RDE_LOG_COLOR_GREEN, "		- Texture (Bump): %s", (_model->mesh_array[_i].material.map_bump != NULL ? _model->mesh_array[_i].material.map_bump->file_path : "none"));
//	}
	return _model;
}

#pragma clang diagnostic pop
#endif