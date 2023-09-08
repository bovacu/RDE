#include <fstream>

rde_atlas_sub_textures rde_file_system_read_atlas_config(const char* _config_rde_path, rde_texture* _atlas) {
	std::ifstream _stream(_config_rde_path);
	_stream.seekg(0, std::ios::end);
	size_t _size = _stream.tellg();
	std::string _buffer(_size, ' ');
	_stream.seekg(0);
	_stream.read(&_buffer[0], _size); 

	rde_atlas_sub_textures _atlas_sub_textures;
	nlohmann::json _atlas_json = nlohmann::json::parse(_buffer);
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
			.atlas_texture = _atlas
		};
	}

	return _atlas_sub_textures;
}