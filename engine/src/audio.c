#ifdef RDE_AUDIO_MODULE

// ======================= API ===========================

rde_sound* rde_audio_load_sound(const char* _sound_path) {
	UNUSED(_sound_path)
	UNIMPLEMENTED("rde_audio_load_sound")
	return NULL;
}

void rde_audio_unload_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_unload_sound")
}

void rde_audio_play_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED("rde_audio_play_sound")
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

#endif