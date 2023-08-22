#include "engine/engine.h"
#include "events/event_structs.h"
#include "SDL2/SDL.h"
#include "window/window.h"
#include "events/mobile_events.h"
#include "private_structs.h"
#include "engine/sdl_to_engine_helper.h"

void rde_engine_on_event(rde_engine* _engine);
void rde_engine_on_update(rde_engine* _engine, float _dt);
void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt);
void rde_engine_on_late_update(rde_engine* _engine, float _dt);
void rde_engine_on_render(rde_engine* _engine, float _dt);
void rde_engine_sync_events(rde_engine* _engine);


rde_engine* rde_engine_create_engine(int _argc, char** _argv) {
	rde_engine* _engine = new rde_engine {  };
	rde_window* _default_window = rde_window_create_window(_engine);

	memset(_engine->engine_windows, 0, RDE_MAX_NUMBER_OF_WINDOWS);

	_engine->engine_windows[0] = _default_window;

	rde_engine_entry_point(_argc, _argv, _engine);

	return _engine;
}

RDE_PLATFORM_TYPE_ rde_engine_get_platform(rde_engine* _engine) {
	return _engine->platform_type;
}

float rde_engine_get_fixed_delta(rde_engine* _engine) {
	return _engine->fixed_delta_time;
}

void rde_engine_set_fixed_delta(rde_engine* _engine, float _delta_time) {
	UNUSED(_engine)
	UNUSED(_delta_time)
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_set_current_scene(rde_engine* _engine, rde_scene* _scene) {
	UNUSED(_engine)
	UNUSED(_scene)
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_on_run(rde_engine* _engine) {

	#if IS_MOBILE()
	SDL_SetEventFilter(rde_mobile_system_events, _engine);
	#endif

	while(_engine->running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		_engine->fixed_time_step_accumulator += _engine->delta_time;

		rde_engine_on_event(_engine);

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

	rde_engine_destroy_engine(_engine);
}

void rde_engine_init_imgui_layer(rde_engine* _engine) {
	UNUSED(_engine)
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_end_imgui_layer(rde_engine* _engine) {
	UNUSED(_engine)
	UNIMPLEMENTED("Not implemented")
}

rde_window* rde_engine_get_window(rde_engine* _engine, int _index) {
	return _engine->engine_windows[_index];
}


bool rde_engine_is_running(rde_engine* _engine) {
	return _engine->running;
}

void rde_engine_set_running(rde_engine* _engine, bool _running) {
	_engine->running = _running;
}

rde_vec2I rde_engine_get_display_size(rde_engine* _engine) {
	UNUSED(_engine)
	SDL_DisplayMode _displayMode;
	SDL_GetCurrentDisplayMode(0, &_displayMode);
	return { _displayMode.w, _displayMode.h };
}

void rde_engine_destroy_engine(rde_engine* _engine) {
	SDL_GL_DeleteContext(_engine->engine_windows[0]->sdl_gl_context);
	SDL_DestroyWindow(_engine->engine_windows[0]->sdl_window);
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

rde_event rde_engine_sdl_event_to_rde_event(SDL_Event* _sdl_event) {

	rde_event _event;

	switch(_sdl_event->type) {
		case SDL_WINDOWEVENT:
		case SDL_DISPLAYEVENT: rde_sdl_to_rde_helper_transform_window_event(_sdl_event, &_event); break;
	}

	return _event;
}

void rde_engine_on_event(rde_engine* _engine) {
	SDL_Event _event;
        
	SDL_PumpEvents();
	
	// TODO: this now handles only one possible window, to manage multiple window events
	// a window must now its SDL window id and each event gives you which windowID the event
	// has happened on.

	while (SDL_PollEvent(&_event)) {
		rde_event _rde_event = rde_engine_sdl_event_to_rde_event(&_event);
		rde_window_consume_event(_engine, _engine->engine_windows[0], &_rde_event);
	}
}

void rde_engine_on_update(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
	UNUSED(_dt)
}

void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt) {
	UNUSED(_engine)
	UNUSED(_fixed_dt)
}

void rde_engine_on_late_update(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
	UNUSED(_dt)
}

void rde_engine_on_render(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
	UNUSED(_dt)
}

void rde_engine_sync_events(rde_engine* _engine) {
	UNUSED(_engine)
}

rde_display_info* rde_engine_get_available_displays() {
	UNIMPLEMENTED("Not implemented");
	return nullptr;
}

void rde_engine_switch_window_display(rde_window* _window, size_t _new_display) {
	UNUSED(_window)
	UNUSED(_new_display)
	UNIMPLEMENTED("Not implemented");
}
