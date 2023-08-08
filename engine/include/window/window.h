#ifndef RDE_WINDOW_H
#define RDE_WINDOW_H

#include "core.h"

RDE_FUNC_ND rde_window*		rde_window_create_window(rde_engine* _engine);

//RDE_FUNC	void 			rde_window_consume_event(rde_window* _window, rde_event* _event);
//
//RDE_FUNC_ND Vec2I			rde_window_get_window_size(rde_window* _window);
//RDE_FUNC	void			rde_window_set_window_size(rde_window* _window, const Vec2I& _size);
//
//RDE_FUNC_ND Vec2I			rde_window_get_position(rde_window* _window);
//RDE_FUNC_ND void			rde_window_set_position(rde_window* _window, const Vec2I& _position);
//
//RDE_FUNC_ND std::string 	rde_window_get_title(rde_window* _window);
//RDE_FUNC	void			rde_window_set_title(rde_window* _window, const std::string& _name);
//
//RDE_FUNC_ND bool			rde_window_is_fullscreen(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_fullscreen(rde_window* _window, bool _fullscreen);
//
//RDE_FUNC_ND bool			rde_window_is_vsync_active(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_vsync_active(rde_window* _window, bool _vsync);
//
//RDE_FUNC_ND SDL_Window* 	rde_window_get_native_sdl_window(rde_window* _window);
//RDE_FUNC_ND SDL_GLContext* 	rde_window_get_native_sdl_gl_window(rde_window* _window);
//
//RDE_FUNC	void			rde_window_set_icon(const std::string& _path_to_icon);
//
//RDE_FUNC_ND bool			rde_window_is_minimize(rde_window* _window);
//RDE_FUNC 	void			rde_window_minimize(rde_window* _window, bool _minimize);
//
//RDE_FUNC_ND bool			rde_window_is_maximized(rde_window* _window);
//RDE_FUNC 	void			rde_window_maximized(rde_window* _window, bool _maximized);
//
//RDE_FUNC_ND bool			rde_window_is_focused(rde_window* _window);
//
//RDE_FUNC_ND bool			rde_window_is_mouse_out_of_window_allowed(rde_window* _window);
//RDE_FUNC 	void			rde_window_allow_mouse_out_of_window(rde_window* _window, bool _allow_mouse_out_of_window);
//
//RDE_FUNC	void			rde_window_refresh_dpi(rde_window* _window);
//
//RDE_FUNC	void 			rde_window_destroy_window(Engine* _engine, rde_window _window);

#endif