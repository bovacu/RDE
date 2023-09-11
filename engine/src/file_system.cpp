rde_atlas_sub_textures rde_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	FILE* _file = nullptr;
	char* _text = nullptr;

	#if IS_WINDOWS()
		errno_t _err = fopen_s(&_file, _atlas_path, "r");
		assert(_err == 0 && "File could not be opened");
	#else
		_file = fopen(_atlas_path, "r");
		assert(_file != nullptr && "File could not be opened");
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	assert(_text != nullptr && "Could not allocate memory for reading the config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_atlas_sub_textures _atlas_sub_textures;
	cJSON* _atlas_json = cJSON_Parse(_text);

	if(_atlas_json == nullptr) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		printf("Error: Could not load JSON from file '%s' due to error '%s' \n", _atlas_path, _error_ptr);
		exit(-1);
	}

	cJSON* _sub_texture_json = nullptr;
	cJSON_ArrayForEach(_sub_texture_json, _atlas_json) {
		cJSON* _position = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "position");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "size");

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size = rde_vec_2UI { (uint)cJSON_GetArrayItem(_size, 0)->valueint, (uint)cJSON_GetArrayItem(_size, 1)->valueint };
		_texture.position = rde_vec_2UI { (uint)cJSON_GetArrayItem(_position, 0)->valueint, (uint)cJSON_GetArrayItem(_position, 1)->valueint };
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		_texture.file_path = _atlas->file_path;
		_texture.atlas_texture = _atlas;
		_atlas_sub_textures[std::string(_sub_texture_json->string)] = _texture;

		cJSON_free(_position);
		cJSON_free(_size);
	}

	cJSON_free(_atlas_json);

	fclose(_file);
	free(_text);

	return _atlas_sub_textures;
}
