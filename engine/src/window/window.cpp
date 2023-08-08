#include "window/window.h"
#include "structs.h"
#include "SDL2/SDL.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif

rde_window* rde_window_create_windows_window(rde_engine* _engine);


rde_window* rde_window_create_window(rde_engine* _engine) {
	return rde_window_create_windows_window(_engine);
}

rde_window* rde_window_create_windows_window(rde_engine* _engine) {
	rde_window* _window = new rde_window;

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
	_window->sdl_window = SDL_CreateWindow("Title", 0, 0,
	                          1280, 720,
	                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window->sdl_window == nullptr) {
		std::cout << "SDL window creation failed: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);

	if(_window->sdl_gl_context == nullptr) {
		std::cout << "OpenGL context couldn't initialize -> " << SDL_GetError() << std::endl;
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "GLAD and SDL2 loaded successfully" << std::endl;

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	_window->display_of_window = SDL_GetWindowDisplayIndex(_window->sdl_window);

	return _window;
	//setIcon(properties->projectData.iconPath);

	//refreshDpi();
}