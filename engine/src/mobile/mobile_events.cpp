#include "core.h"

#if IS_MOBILE()

#include "mobile/mobile_events.h"

int rde_mobile_system_events(void* _user_data, SDL_Event* _event) {
	switch(_event->type) {
		case SDL_APP_TERMINATING: {
			// TODO: terminate app and clean everything
		} break;

		case SDL_APP_LOWMEMORY: {
			// TODO: not sure what to do in this case
		} break;

		case SDL_APP_WILLENTERBACKGROUND: {
			// TODO: stop everything, main loop, music, nothing can run or the app may crash unexpectedly
		} break;

		case SDL_APP_DIDENTERBACKGROUND: {
			// TODO: stop everything, main loop, music, nothing can run or the app may crash unexpectedly
		} break;

		case SDL_APP_WILLENTERFOREGROUND: {
			// TODO: restart everything stoped on SDL_APP_DIDENTERBACKGROUND and SDL_APP_WILLENTERBACKGROUND
		} break;

		case SDL_APP_DIDENTERFOREGROUND: {
			// TODO: restart everything stoped on SDL_APP_DIDENTERBACKGROUND and SDL_APP_WILLENTERBACKGROUND
		} break;

		case SDL_LOCALECHANGED: {
			// TODO: not sure what to do in this case
		} break;
	}

	// returning 0 means the event gets out of the queue, and 1 means it is pre-processed and re-added to the queue.
	return 1;
}

#endif