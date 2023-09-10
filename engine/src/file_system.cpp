#include <fstream>

rde_atlas_sub_textures rde_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	std::ifstream _stream(_atlas_path);
	_stream.seekg(0, std::ios::end);
	size_t _size = _stream.tellg();
	std::string _buffer(_size, ' ');
	_stream.seekg(0);
	_stream.read(&_buffer[0], _size); 

	rde_atlas_sub_textures _atlas_sub_textures;
	cJSON* _atlas_json = cJSON_Parse(_buffer.c_str());

	if(_atlas_json == nullptr) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		printf("Error: Could not load JSON from file '%s' due to error '%s' \n", _atlas_path, _error_ptr);
		exit(-1);
	}

	cJSON* _sub_texture_json = nullptr;
	cJSON_ArrayForEach(_sub_texture_json, _atlas_json) {
		cJSON* _position = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "position");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "size");

		_atlas_sub_textures[std::string(_sub_texture_json->string)] = rde_texture {
			.opengl_texture_id = _atlas->opengl_texture_id,
			.size = rde_vec_2UI { (uint)cJSON_GetArrayItem(_size, 0)->valueint, (uint)cJSON_GetArrayItem(_size, 1)->valueint },
			.position = rde_vec_2UI { (uint)cJSON_GetArrayItem(_position, 0)->valueint, (uint)cJSON_GetArrayItem(_position, 1)->valueint },
			.channels = _atlas->channels,
			.internal_format = _atlas->internal_format,
			.data_format = _atlas->data_format,
			.file_path = _atlas->file_path,
			.atlas_texture = _atlas
		};

		cJSON_free(_position);
		cJSON_free(_size);
	}

	cJSON_free(_atlas_json);

	return _atlas_sub_textures;
}