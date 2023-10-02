#ifdef RDE_AUDIO_MODULE

#define DEFAULT_FORMAT ma_format_f32

static bool is_miniaudio_initialized = false;

// TODO: use this https://github.com/raysan5/raylib/blob/master/src/raudio.c as a reference, raylib does a more
// complex implementation that what I need, but it is a really good implementation on how to mix sounds with 
// mini audio and how to load different audio formats

void data_callback(ma_device* _device, void* _output, const void* _input, ma_uint32 _frame_count) {
	(void)_device;
	(void)_input;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_sounds; _i++) {
		rde_sound* _sound = &ENGINE.sounds[_i];
		if(_sound->used && _sound->playing) {
			ma_data_source_read_pcm_frames(&_sound->miniaudio_decoder, _output, _frame_count, NULL);
		}
	}
}

// ======================= API ===========================

void rde_audio_init(rde_sound_config _config) {
	ma_result result;
	ma_context context;
	ma_device_info* pPlaybackDeviceInfos;
	ma_uint32 playbackDeviceCount;
	ma_device_info* pCaptureDeviceInfos;
	ma_uint32 captureDeviceCount;
	ma_uint32 iDevice;

	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
		printf("Failed to initialize context.\n");
		exit(-1);
	}

	result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
	if (result != MA_SUCCESS) {
		printf("Failed to retrieve device information.\n");
		exit(-1);
	}

	printf("Playback Devices\n");
	for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice) {
		printf("    %u: %s\n", iDevice, pPlaybackDeviceInfos[iDevice].name);
	}

	ma_device_config _device_config = ma_device_config_init(ma_device_type_playback);
	_device_config.playback.format = DEFAULT_FORMAT;
	_device_config.playback.channels = _config.channels;
	//_device_config.playback.pDeviceID = &pPlaybackDeviceInfos[2].id;
	_device_config.sampleRate = _config.rate;
	_device_config.dataCallback = data_callback;
	_device_config.pUserData = NULL;

	ma_result _result = ma_device_init(NULL, &_device_config, &ENGINE.miniaudio_device);
	if(_result != MA_SUCCESS) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not init audio module -> error code for MiniAudio %d", _result);
		exit(-1);
	}

	_result = ma_device_start(&ENGINE.miniaudio_device) != MA_SUCCESS;
	if(_result != MA_SUCCESS) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not start the audio module to play sounds -> error code for MiniAudio %d", _result);
		rde_audio_end();
		exit(-1);
	}

	is_miniaudio_initialized = true;
	ENGINE.device_config = _config;
	rde_log_level(RDE_LOG_LEVEL_INFO, "Initiated audio module correctly");
}

rde_sound* rde_audio_load_sound(const char* _sound_path) {
	rde_sound* _sound = NULL;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_sounds; _i++) {
		rde_sound* _s = &ENGINE.sounds[_i];

		if(!_s->used) {
			_sound = _s;
			_sound->used = true;
			break;
		}
	}

	rde_critical_error(_sound == NULL, -1, "Max number of loaded sounds (%d) reached", ENGINE.heap_allocs_config.max_number_of_sounds);
	ma_decoder_config _decoder_config = ma_decoder_config_init(DEFAULT_FORMAT, 
	                                                           ENGINE.device_config.channels, 
	                                                           ENGINE.device_config.rate);
	ma_result _result = ma_decoder_init_file(_sound_path, &_decoder_config, &_sound->miniaudio_decoder);

	if(_result != MA_SUCCESS) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Could not load sound '%s' -> error code for MiniAudio %d", _sound_path, _result);
		exit(-1);
	}

	rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded sound '%s' correctly", _sound_path);
	return _sound;
}

void rde_audio_unload_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, -1, "Tried to unload a NULL sound");
	ma_decoder_uninit(&_sound->miniaudio_decoder);
	_sound->used = false;
}

void rde_audio_play_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, -1, "Tried to play a NULL sound");

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
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_sounds; _i++) {
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