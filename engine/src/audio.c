#ifdef RDE_AUDIO_MODULE

#define DEFAULT_FORMAT ma_format_f32

static bool is_miniaudio_initialized = false;

// TODO: use this https://github.com/raysan5/raylib/blob/master/src/raudio.c as a reference, raylib does a more
// complex implementation that what I need, but it is a really good implementation on how to mix sounds with 
// mini audio and how to load different audio formats

void data_callback(ma_device* _device, void* _output, const void* _input, ma_uint32 _frame_count) {
	(void)_device;
	(void)_input;

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_number_of_sounds; _i++) {
		rde_sound* _sound = &ENGINE.sounds[_i];
		if(_sound->used && _sound->playing) {
			ma_data_source_read_pcm_frames(&_sound->miniaudio_decoder, _output, _frame_count, NULL);
		}
	}
}

// ======================= API ===========================

void rde_audio_init(rde_sound_config _config) {
	ma_context _context;
	ma_device_info* _playback_devices_info;
	ma_uint32 _playback_device_count;
	ma_device_info* _capture_device_infos;
	ma_uint32 _capture_device_count;
	// ma_uint32 iDevice;

	rde_critical_error(ma_context_init(NULL, 0, NULL, &_context) != MA_SUCCESS, RDE_ERROR_MA_CONTEXT);
	rde_critical_error(ma_context_get_devices(&_context, &_playback_devices_info, &_playback_device_count, &_capture_device_infos, &_capture_device_count) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_INFO);

	// printf("Playback Devices\n");
	// for (iDevice = 0; iDevice < _playback_device_count; ++iDevice) {
	// 	printf("    %u: %s\n", iDevice, _playback_devices_info[iDevice].name);
	// }

	ma_device_config _device_config = ma_device_config_init(ma_device_type_playback);
	_device_config.playback.format = DEFAULT_FORMAT;
	_device_config.playback.channels = _config.channels;
	//_device_config.playback.pDeviceID = &_playback_devices_info[2].id;
	_device_config.sampleRate = _config.rate;
	_device_config.dataCallback = data_callback;
	_device_config.pUserData = NULL;

	rde_critical_error(ma_device_init(NULL, &_device_config, &ENGINE.miniaudio_device) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_INIT);
	rde_critical_error(ma_device_start(&ENGINE.miniaudio_device) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_START);

	is_miniaudio_initialized = true;
	ENGINE.device_config = _config;
	rde_log_level(RDE_LOG_LEVEL_INFO, "Initiated audio module correctly");
}

rde_sound* rde_audio_load_sound(const char* _sound_path) {
	rde_sound* _sound = NULL;

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_number_of_sounds; _i++) {
		rde_sound* _s = &ENGINE.sounds[_i];

		if(!_s->used) {
			_sound = _s;
			_sound->used = true;
			break;
		}
	}

	rde_critical_error(_sound == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "sounds", ENGINE.init_info.heap_allocs_config.max_number_of_sounds);
	ma_decoder_config _decoder_config = ma_decoder_config_init(DEFAULT_FORMAT, 
	                                                           ENGINE.device_config.channels, 
	                                                           ENGINE.device_config.rate);

	ma_result _result = ma_decoder_init_file(_sound_path, &_decoder_config, &_sound->miniaudio_decoder);
	rde_critical_error(_result != MA_SUCCESS, RDE_ERROR_MA_FILE_NOT_FOUND, _sound_path, _result);

	rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded sound '%s' correctly", _sound_path);
	return _sound;
}

void rde_audio_unload_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, RDE_ERROR_NO_NULL_ALLOWED, "sound");
	ma_decoder_uninit(&_sound->miniaudio_decoder);
	_sound->used = false;
}

void rde_audio_play_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, RDE_ERROR_NO_NULL_ALLOWED, "sound");

	_sound->playing = true;
	_sound->paused = false;
	_sound->played_frame = 0;
}

void rde_audio_pause_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_pause_sound")
}

void rde_audio_stop_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_stop_sound")
}

void rde_audio_restart_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_restart_sound")
}

bool rde_audio_is_sound_playing(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_is_sound_playing")
	return false;
}

bool rde_audio_set_sound_volume(rde_sound* _sound, float _volume) {
	UNUSED(_sound)
	UNUSED(_volume)
	UNIMPLEMENTED("rde_audio_set_sound_volume")
	return false;
}

void rde_audio_end() {
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_number_of_sounds; _i++) {
		rde_sound* _sound = &ENGINE.sounds[_i];

		if(_sound->used) {
			rde_audio_unload_sound(_sound);
		}
	}

	if(is_miniaudio_initialized) {
		ma_device_uninit(&ENGINE.miniaudio_device);
	}
}

#endif