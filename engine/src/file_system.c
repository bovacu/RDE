rde_atlas_sub_textures* rde_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
		errno_t _err = fopen_s(&_file, _atlas_path, "r");
		assert(_err == 0 && "File could not be opened");
	#else
		_file = fopen(_atlas_path, "r");
		assert(_file != NULL && "File could not be opened");
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	assert(_text != NULL && "Could not allocate memory for reading the config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_atlas_sub_textures* hash = NULL;
	cJSON* _atlas_json = cJSON_Parse(_text);

	if(_atlas_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		printf("Error: Could not load JSON from file '%s' due to error '%s' \n", _atlas_path, _error_ptr);
		exit(-1);
	}

	cJSON* _sub_texture_json = NULL;
	cJSON_ArrayForEach(_sub_texture_json, _atlas_json) {
		cJSON* _position = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "position");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "size");

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = (uint)cJSON_GetArrayItem(_size, 0)->valueint;
		_texture.size.y = (uint)cJSON_GetArrayItem(_size, 1)->valueint;
		_texture.position.x = (uint)cJSON_GetArrayItem(_position, 0)->valueint;
		_texture.position.y = (uint)cJSON_GetArrayItem(_position, 1)->valueint;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		_texture.file_path = _atlas->file_path;
		_texture.atlas_texture = _atlas;
		stbds_shput(hash, _sub_texture_json->string, _texture);

		cJSON_free(_position);
		cJSON_free(_size);
	}

	cJSON_free(_atlas_json);

	fclose(_file);
	free(_text);

	return hash;
}

rde_font_char_info_map* rde_file_system_read_font_config(const char* _font_path) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
	errno_t _err = fopen_s(&_file, _font_path, "r");
	assert(_err == 0 && "File could not be opened");
	#else
	_file = fopen(_font_path, "r");
	assert(_file != NULL && "File could not be opened");
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	assert(_text != NULL && "Could not allocate memory for reading the config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_font_char_info_map* hash = NULL;
	cJSON* _font_json = cJSON_Parse(_text);

	if(_font_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		printf("Error: Could not load JSON from file '%s' due to error '%s' \n", _font_path, _error_ptr);
		exit(-1);
	}

	cJSON* _char_info_json = NULL;
	cJSON_ArrayForEach(_char_info_json, _font_json) {
		cJSON* _advance = cJSON_GetObjectItemCaseSensitive(_char_info_json, "advance");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_char_info_json, "size");
		cJSON* _bearing = cJSON_GetObjectItemCaseSensitive(_char_info_json, "bearing");
		cJSON* _offset = cJSON_GetObjectItemCaseSensitive(_char_info_json, "offset");

		rde_font_char_info _char_info = rde_struct_create_font_char_info();
		_char_info.advance = (rde_vec_2I) { (int)cJSON_GetArrayItem(_advance, 0)->valueint, (int)cJSON_GetArrayItem(_advance, 1)->valueint };
		_char_info.size = (rde_vec_2I) { (int)cJSON_GetArrayItem(_size, 0)->valueint, (int)cJSON_GetArrayItem(_size, 1)->valueint };
		_char_info.bearing = (rde_vec_2I) { (int)cJSON_GetArrayItem(_bearing, 0)->valueint, (int)cJSON_GetArrayItem(_bearing, 1)->valueint };
		_char_info.offset = (rde_vec_2F) { (float)cJSON_GetArrayItem(_offset, 0)->valuedouble, (float)cJSON_GetArrayItem(_offset, 1)->valuedouble };
		stbds_shput(hash, _char_info_json->string, _char_info);

		cJSON_free(_advance);
		cJSON_free(_size);
		cJSON_free(_bearing);
		cJSON_free(_offset);
	}

	cJSON_free(_font_json);

	fclose(_file);
	free(_text);

	return hash;
}
