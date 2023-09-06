rde_atlas_sub_textures rde_file_system_read_atlas_config(const char* _config_rde_path, rde_texture* _atlas) {
	FILE* _file = nullptr;
	char* _text = nullptr;

	#if IS_WINDOWS()
	errno_t _err = fopen_s(&_file, _config_rde_path, "r");
	assert(_err == 0 && "File could not be opened");
	#else
	_file = fopen(_config_rde_path, "r");
	assert(_file != nullptr && "File could not be opened");
	#endif

	long _length = 0;
	fseek(_file, 0, SEEK_END);
	_length = ftell(_file);
	fseek(_file, 0, SEEK_SET);
	_text = (char*)malloc(sizeof(char) * _length);
	assert(_text != nullptr && "Could not allocate memory for reading the config file");
	fread(_text, 1, _length, _file);

	fclose(_file);

	rde_atlas_sub_textures _atlas_sub_textures;
	nlohmann::json _atlas_json = nlohmann::json::parse(_text);

	for (nlohmann::json::iterator _it = _atlas_json.begin(); _it != _atlas_json.end(); _it++) {
		nlohmann::json _position = _it.value()["position"];
		nlohmann::json _size = _it.value()["size"];

		_atlas_sub_textures[_it.key()] = rde_texture {
			.opengl_texture_id = _atlas->opengl_texture_id,
			.size = rde_vec_2UI { _size[0].get<uint>(), _size[1].get<uint>() },
			.position = rde_vec_2UI { _position[0].get<uint>(), _position[1].get<uint>() },
			.channels = _atlas->channels,
			.internal_format = _atlas->internal_format,
			.data_format = _atlas->data_format,
			.file_path = _atlas->file_path,
		};
	}

	free(_text);

	return _atlas_sub_textures;
}