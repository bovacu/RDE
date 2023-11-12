#if IS_WINDOWS()
rde_window* rde_inner_window_create_windows_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);

	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	rde_critical_error(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), RDE_ERROR_GLAD_INIT);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for windows");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_MAC()
rde_window* rde_inner_window_create_mac_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	
	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for mac");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	int _major = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &_major) != 0) {
		exit(-1);
	}
	printf("Major: %d\n", _major);

	int _minor = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &_minor) != 0) {
		exit(-1);
	}
	printf("Minor: %d\n", _minor);

	int _context = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &_context) != 0) {
		exit(-1);
	}
	printf("Context: %d\n", _context);
	return _window;
}
#endif

#if IS_LINUX() && !IS_ANDROID()
rde_window* rde_inner_window_create_linux_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	rde_critical_error(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), RDE_ERROR_GLAD_INIT);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for linux");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_ANDROID()
rde_window* rde_inner_window_create_android_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

	SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");

    SDL_DisplayMode _mode;
    rde_critical_error(SDL_GetDisplayMode(0, 0, &_mode) < 0, "Error getting SDL_DisplayMode -> %s\n", SDL_GetError());
    rde_log_level(RDE_LOG_LEVEL_INFO, "Screen Size: (%d, %d)", _mode.w, _mode.h);
    
    _window->sdl_window = SDL_CreateWindow(NULL, 0, 0, _mode.w, _mode.h ,SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

    int _drawableSizeX = 0, _drawableSizeY = 0;
    SDL_GL_GetDrawableSize(_window->sdl_window, &_drawableSizeX, &_drawableSizeY);
    rde_log_level(RDE_LOG_LEVEL_INFO, "Drawable Size: (%d, %d)", _drawableSizeX, _drawableSizeY);

    _window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
    rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

    SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO, "1");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");

    SDL_GL_SetSwapInterval(1);

    rde_log_level(RDE_LOG_LEVEL_INFO, "OpenGL and SDL2 loaded successfully for Android");

    return _window;
}
#endif

#if IS_IOS()
rde_window* rde_inner_window_create_ios_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac ios creation is not implemented yet");
    return NULL;
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac WASM creation is not implemented yet");
    return NULL;
}
#endif
	



// ==============================================================================
// =									API										=
// ==============================================================================

rde_window* rde_window_create_window_os() {

	size_t _free_window_index = 0;
	rde_window* _window = NULL;

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
		if(ENGINE.windows[_i].sdl_window != NULL) {

			if(_i == ENGINE.heap_allocs_config.max_number_of_windows - 1) {
				rde_critical_error(true, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "windows", ENGINE.heap_allocs_config.max_number_of_windows);
			}

			continue;
		}

		_free_window_index = _i;
		break;
	}

	// TODO: create window depending on platform
	#if IS_WINDOWS()
		_window = rde_inner_window_create_windows_window(_free_window_index);
	#elif IS_LINUX() && !IS_ANDROID()
		_window = rde_inner_window_create_linux_window(_free_window_index);
	#elif IS_MAC()
		_window = rde_inner_window_create_mac_window(_free_window_index);
	#elif IS_ANDROID()
		_window = rde_inner_window_create_android_window(_free_window_index);
	#elif IS_IOS()
		_window = rde_inner_window_create_ios_window(_free_window_index);
	#else
		rde_critical_error(true, RDE_ERROR_UNSUPPORTED_PLATFORM);
	#endif

	return _window;
}

rde_vec_2I rde_window_get_window_size(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _size;
	SDL_GetWindowSize(_window->sdl_window, &_size.x, &_size.y);
	return _size;
}
void rde_window_set_window_size(rde_window* _window, rde_vec_2I _size) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowSize(_window->sdl_window, _size.x, _size.y);
}

rde_vec_2I rde_window_get_position(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _position = {  };
	SDL_GetWindowPosition(_window->sdl_window, &_position.x, &_position.y);
	return _position;
}

void rde_window_set_position(rde_window* _window, rde_vec_2I _position) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowPosition(_window->sdl_window, _position.x, _position.y);
}

const char* rde_window_get_title(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return SDL_GetWindowTitle(_window->sdl_window);
}

void rde_window_set_title(rde_window* _window, const char* _title) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowTitle(_window->sdl_window, _title);
}

bool rde_window_orientation_is_horizontal(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	return _window_size.x >= _window_size.y;
}

void rde_window_take_screen_shot(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size_of_rectangle, const char* _file_name_with_extension) {
	UNUSED(_window)
	UNUSED(_position)
	UNUSED(_size_of_rectangle)
	UNUSED(_file_name_with_extension)
	UNIMPLEMENTED("rde_window_take_screen_shot")
}

unsigned char* getAreaOfScreenPixels(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	unsigned char* _pixels = (unsigned char*)malloc(sizeof(unsigned char) * (4 * _window_size.x * _window_size.y));
	rde_critical_error(_pixels == NULL, RDE_ERROR_NO_MEMORY, 4 * _window_size.x * _window_size.y, "pixels array for screenshot");
	memset(_pixels, 0, 4 * _window_size.x * _window_size.y);
	glReadPixels((int)(_window_size.x * 0.5f + _position.x - _size.x * 0.5f ), (int)(_window_size.y * 0.5f + _position.y - _size.y * 0.5f),
	             _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
	return _pixels;
}

float rde_window_get_aspect_ratio(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	return _window_size.x / (float)_window_size.y;
}

bool rde_window_is_mouse_out_of_window_allowed() {
	return SDL_GetRelativeMouseMode();
}

void rde_window_allow_mouse_out_of_window(bool _allow_mouse_out_of_window) {
	SDL_SetRelativeMouseMode(_allow_mouse_out_of_window ? SDL_FALSE : SDL_TRUE);
}

void rde_window_set_icon(rde_window* _window, const char* _path_to_icon) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	int _width, _height, _channels;

	stbi_set_flip_vertically_on_load(0);
	stbi_uc* _data = stbi_load(_path_to_icon, &_width, &_height, &_channels, 0);
	if(_data == NULL) {
		rde_critical_error(true, RDE_ERROR_FILE_NOT_FOUND, _path_to_icon);
		return;
	}
	
	Uint32 _rmask, _gmask, _bmask, _amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int _shift = (_channels == 3) ? 8 : 0;
	_rmask = 0xff000000 >> _shift;
	_gmask = 0x00ff0000 >> _shift;
	_bmask = 0x0000ff00 >> _shift;
	_amask = 0x000000ff >> _shift;
	#else
	_rmask = 0x000000ff;
	_gmask = 0x0000ff00;
	_bmask = 0x00ff0000;
	_amask = (_channels == 3) ? 0 : 0xff000000;
	#endif

	SDL_Surface* _surface = SDL_CreateRGBSurfaceFrom((void*)_data, _width, _height, 
		_channels * 8, _channels * _width, _rmask, _gmask, _bmask, _amask);


	SDL_SetWindowIcon(_window->sdl_window, _surface);
	SDL_FreeSurface(_surface);
}

void* rde_window_get_native_sdl_window_handle(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return _window->sdl_window;
}

void* rde_window_get_native_sdl_gl_context_handle(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return _window->sdl_gl_context;
}

void rde_window_destroy_window(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");

	if(_window->sdl_gl_context != NULL) {
		SDL_GL_DeleteContext(_window->sdl_gl_context);
	}

	if(_window->sdl_window != NULL) {
		SDL_DestroyWindow(_window->sdl_window);
	}

	memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);

	_window->sdl_window = NULL;
}