#ifdef RDE_RENDERING_MODULE
rde_atlas_sub_textures* rde_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
		errno_t _err = fopen_s(&_file, _atlas_path, "r");
		rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#else
		_file = fopen(_atlas_path, "r");
		rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_atlas_sub_textures* hash = NULL;
	cJSON* _atlas_json = cJSON_Parse(_text);

	if(_atlas_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _atlas_path, _error_ptr);
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

rde_font_char_info* rde_file_system_read_font_config(const char* _font_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
	errno_t _err = fopen_s(&_file, _font_path, "r");
	rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#else
	_file = fopen(_font_path, "r");
	rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_font_char_info* _chars = NULL;
	cJSON* _font_json = cJSON_Parse(_text);

	if(_font_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _font_path, _error_ptr);
	}

	cJSON* _char_info_json = NULL;
	rde_font_char_info _fci_default = rde_struct_create_font_char_info();
	cJSON* _extra_data = cJSON_GetObjectItemCaseSensitive(_font_json, "extra_data");
	cJSON* _offset_from_start = cJSON_GetObjectItemCaseSensitive(_extra_data, "offset_from_start");
	for(int _i = 0; _i < _offset_from_start->valueint; _i++) {
		stbds_arrput(_chars, _fci_default);
	}

	cJSON* _characters = cJSON_GetObjectItemCaseSensitive(_font_json, "characters");

	cJSON_ArrayForEach(_char_info_json, _characters) {
		cJSON* _advance = cJSON_GetObjectItemCaseSensitive(_char_info_json, "advance");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_char_info_json, "size");
		cJSON* _bearing = cJSON_GetObjectItemCaseSensitive(_char_info_json, "bearing");
		cJSON* _offset = cJSON_GetObjectItemCaseSensitive(_char_info_json, "offset");
		cJSON* _metrics = cJSON_GetObjectItemCaseSensitive(_char_info_json, "metrics");

		rde_font_char_info _char_info = rde_struct_create_font_char_info();
		_char_info.advance = (rde_vec_2I) { cJSON_GetArrayItem(_advance, 0)->valueint, cJSON_GetArrayItem(_advance, 1)->valueint };
		_char_info.size = (rde_vec_2I) { cJSON_GetArrayItem(_size, 0)->valueint, cJSON_GetArrayItem(_size, 1)->valueint };
		_char_info.bearing = (rde_vec_2I) { cJSON_GetArrayItem(_bearing, 0)->valueint, cJSON_GetArrayItem(_bearing, 1)->valueint };
		_char_info.offset = (rde_vec_2I) { cJSON_GetArrayItem(_offset, 0)->valueint, cJSON_GetArrayItem(_offset, 1)->valueint };
		_char_info.metrics = (rde_vec_2I) { cJSON_GetArrayItem(_metrics, 0)->valueint, cJSON_GetArrayItem(_metrics, 1)->valueint };

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = _char_info.size.x;
		_texture.size.y = _char_info.size.y;
		_texture.position.x = _char_info.offset.x;
		_texture.position.y = _char_info.offset.y;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		_texture.file_path = _atlas->file_path;
		_texture.atlas_texture = _atlas;
		_char_info.texture = _texture;

		stbds_arrput(_chars, _char_info);

		cJSON_free(_advance);
		cJSON_free(_size);
		cJSON_free(_bearing);
		cJSON_free(_offset);
	}

	cJSON_free(_font_json);

	fclose(_file);
	free(_text);

	return _chars;
}
#endif