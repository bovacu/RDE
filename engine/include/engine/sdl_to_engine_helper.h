#ifndef RDE_SDL_TO_ENGINE_HELPER_H
#define RDE_SDL_TO_ENGINE_HELPER_H

/**
 * ==================================================
 * =				READ PLEASE						=
 * ==================================================
 * 
 * This header file should not be included in the projects developed by RDE.
 * 
 * It only contains transformations from SDL data to RDE data, so end-user
 * does not have to deal with both SDL and the engine.
*/

#include "core.h"
#include "SDL2/SDL_Events.h"
#include "events/event_structs.h"

RDE_FUNC void rde_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event);

#endif