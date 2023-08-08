#include "engine/engine.h"
#include "window/event.h"
#include "structs.h"
#include "SDL2/SDL.h"
#include "window/window.h"

void rde_engine_on_event(rde_engine* _engine, rde_event* _event);
void rde_engine_on_update(rde_engine* _engine, float _dt);
void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt);
void rde_engine_on_late_update(rde_engine* _engine, float _dt);
void rde_engine_on_render(rde_engine* _engine, float _dt);
void rde_engine_sync_events(rde_engine* _engine);


#ifdef DEFAULT_IMPLEMENTATION
rde_engine* rde_engine_create_engine(int _argc, char** _argv) {
	rde_engine* _engine = new rde_engine;
	rde_window* _default_window = rde_window_create_window(_engine);
	_engine->engine_windows[0] = _default_window;

	return _engine;
}
#endif


RDE_PLATFORM_TYPE_ rde_engine_get_platform(rde_engine* _engine) {
	return _engine->platform_type;
}

float rde_engine_get_fixed_delta(rde_engine* _engine) {
	return _engine->fixed_delta_time;
}

void rde_engine_set_fixed_delta(rde_engine* _engine, float _delta_time) {
	
}

void rde_engine_set_current_scene(rde_engine* _engine, rde_scene* _scene) {
	
}

void rde_engine_on_run(rde_engine* _engine) {
	while(_engine->running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		_engine->fixed_time_step_accumulator += _engine->delta_time;

		rde_engine_on_event(_engine, nullptr);

		rde_engine_on_update(_engine, _engine->delta_time);

		while (_engine->fixed_time_step_accumulator >= _engine->fixed_delta_time) {
			_engine->fixed_time_step_accumulator -= _engine->fixed_delta_time;
			rde_engine_on_fixed_update(_engine, _engine->fixed_delta_time);
		}

		rde_engine_on_late_update(_engine, _engine->delta_time);

		rde_engine_on_render(_engine, _engine->delta_time);

		rde_engine_sync_events(_engine);

		Uint64 _end = SDL_GetPerformanceCounter();
		float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
		_engine->delta_time = _elapsedMS;
	}
}

void rde_engine_init_imgui_layer(rde_engine* _engine) {

}

void rde_engine_end_imgui_layer(rde_engine* _engine) {

}

rde_window* rde_engine_get_window(rde_engine* _engine, int _index) {
	return nullptr;
}


bool rde_engine_is_running(rde_engine* _engine) {
	return _engine->running;
}

void rde_engine_set_running(rde_engine* _engine, bool _running) {
	_engine->running = _running;
}

rde_vec2I rde_engine_get_display_size(rde_engine* _engine) {
	//SDL_DisplayMode _displayMode;
	//SDL_GetCurrentDisplayMode(0, &_displayMode);
	//return { _displayMode.w, _displayMode.h };
	return { 0, 0 };
}

void rde_engine_destroy_engine(rde_engine* _engine) {
	
}




void rde_engine_on_event(rde_engine* _engine, rde_event* _event) {

}

void rde_engine_on_update(rde_engine* _engine, float _dt) {

}

void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt) {

}

void rde_engine_on_late_update(rde_engine* _engine, float _dt) {

}

void rde_engine_on_render(rde_engine* _engine, float _dt) {

}

void rde_engine_sync_events(rde_engine* _engine) {

}