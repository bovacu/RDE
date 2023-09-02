#if IS_WINDOWS()
rde_window* rde_inner_window_create_windows_window(size_t _free_window_index) {
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		
	#ifdef RENDER_DOC_COMPATIBILITY
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window->sdl_window == nullptr) {
		printf("SDL window creation failed: %s \n", SDL_GetError());
		exit(-1);
	}
	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);

	if(_window->sdl_gl_context == nullptr) {
		printf("OpenGL context couldn't initialize -> %s \n", SDL_GetError());
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to initialize GLAD \n");
		exit(-1);
	}
	
	printf("GLAD and SDL2 loaded successfully \n");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_MAC()
rde_window* rde_window_create_mac_window() {
	UNIMPLEMENTED("Mac window creation is not implemented yet");
}
#endif

#if IS_LINUX()
rde_window* rde_inner_window_create_linux_window(size_t _free_window_index) {
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		
	#ifdef RENDER_DOC_COMPATIBILITY
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window->sdl_window == nullptr) {
		printf("OpenGL context couldn't initialize -> %s \n", SDL_GetError());
		exit(-1);
	}
	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);

	if(_window->sdl_gl_context == nullptr) {
		printf("OpenGL context couldn't initialize -> %s \n", SDL_GetError());
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to initialize GLAD \n");
		exit(-1);
	}
	printf("GLAD and SDL2 loaded successfully \n");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_ANDROID()
rde_window* rde_window_create_android_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac android creation is not implemented yet");
}
#endif

#if IS_IOS()
rde_window* rde_window_create_ios_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac ios creation is not implemented yet");
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac WASM creation is not implemented yet");
}
#endif
	


// ======================= API ===========================

rde_window* rde_window_create_window() {

	size_t _free_window_index = 0;
	rde_window* _window = nullptr;

	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		if(ENGINE.windows[_i].sdl_window != nullptr) {

			if(_i == RDE_MAX_NUMBER_OF_WINDOWS - 1) {
				assert(false && "[ERROR]: Tried to create a new window but the limit of simultaneous windows has been reached.");
			}

			continue;
		}

		_free_window_index = _i;
		break;
	}

	// TODO: create window depending on platform
	#if IS_WINDOWS()
		_window = rde_inner_window_create_windows_window(_free_window_index);
	#elif IS_LINUX()
		_window = rde_inner_window_create_linux_window(_free_window_index);
	#else
	assert(false && "[Error]: Unsupported or unimplemented platform");
	#endif
	
	return _window;
}

void rde_window_set_callbacks(rde_window* _window, rde_window_callbacks _callbacks) {
	UNUSED(_window)
		UNUSED(_callbacks)
}

rde_vec_2I rde_window_get_window_size(rde_window* _window) {
	rde_vec_2I _size {  };
	SDL_GetWindowSize(_window->sdl_window, &_size.x, &_size.y);
	return _size;
}
void rde_window_set_window_size(rde_window* _window, const rde_vec_2I _size) {
	SDL_SetWindowSize(_window->sdl_window, _size.x, _size.y);
}

rde_vec_2I rde_window_get_position(rde_window* _window) {
	rde_vec_2I _position = {  };
	SDL_GetWindowPosition(_window->sdl_window, &_position.x, &_position.y);
	return _position;
}

void rde_window_set_position(rde_window* _window, const rde_vec_2I _position) {
	SDL_SetWindowPosition(_window->sdl_window, _position.x, _position.y);
}

const char* rde_window_get_title(rde_window* _window) {
	return SDL_GetWindowTitle(_window->sdl_window);
}

void rde_window_set_title(rde_window* _window, const char* _title) {
	SDL_SetWindowTitle(_window->sdl_window, _title);
}

//
//RDE_FUNC_ND bool			rde_window_is_fullscreen(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_fullscreen(rde_window* _window, bool _fullscreen);
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

void* rde_window_get_native_sdl_window_handle(rde_window* _window) {
	return _window->sdl_window;
}

void* rde_window_get_native_sdl_gl_context_handle(rde_window* _window) {
	return &_window->sdl_gl_context;
}

void rde_window_destroy_window(rde_window* _window) {
	SDL_GL_DeleteContext(_window->sdl_gl_context);
	SDL_DestroyWindow(_window->sdl_window);

	_window->sdl_window = nullptr;
	_window->sdl_gl_context = {};
}