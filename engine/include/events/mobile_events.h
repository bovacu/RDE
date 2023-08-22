#if IS_MOBILE()

#ifndef RDE_MOBILE_EVENTS_H
#define RDE_MOBILE_EVENTS_H

#include "SDL2/SDL_Events.h"

RDE_FUNC int rde_mobile_system_events(void* _user_data, SDL_Event* _event);

#endif

#endif