#include "core/render/window/Window.h"
#include "core/systems/eventSystem/WindowEvent.h"
#include "core/platform/PlatformHeaderSDL.h"
#include "core/Engine.h"
#include "core/util/Functions.h"
#include "core/util/Vec.h"
#include <SDL_mouse.h>
#include <SDL_stdinc.h>

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl32.h>
#elif IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#else
    #include "glad/glad.h"
#endif

namespace RDE {

    static bool shouldUpdateWindowDefault() {
        return true;
    }

    Window::Window(RDEConfig* _config) : window(nullptr) {
        properties = _config;

        #ifdef ENGINE_DEBUG
        Util::Log::debug("Creating window ", _config->windowData.title, " (", _config->windowData.size.x, _config->windowData.size.y, ")");
        #endif

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            Util::Log::error("At least one module of SDL couldn't be initialized, so can't start the engine: ", SDL_GetError());
            return;
        }

        SDL_version compiled;
        SDL_version linked;
    
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        Util::Log::debug("We compiled against SDL version ", (int)compiled.major, ".", (int)compiled.minor, ".", (int)compiled.patch, " and linking to ", (int)linked.major, ".", (int)linked.minor, ".", (int)linked.patch);

        shouldUpdateWindow.bind<shouldUpdateWindowDefault>();
    }

    Window::~Window() {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
        SDL_Quit();
    }

    void Window::setWindowSize(int _width, int _height) {
        SDL_SetWindowSize(window, _width, _height);
    }

    void Window::update() {
        SDL_GL_SwapWindow(window);
    }

    void Window::setVSync(bool _enabled) {
        SDL_GL_SetSwapInterval(_enabled);
    }

    bool Window::isVSyncActive() const {
        return properties->windowData.vsync;
    }

    void Window::setFullscreen(bool _fullscreen) {
        properties->windowData.fullScreen = _fullscreen;
        SDL_SetWindowFullscreen(window, _fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    }

    bool Window::isFullscreen() const {
        return properties->windowData.fullScreen;
    }

    void Window::setIcon(const std::string& _path) {
        properties->projectData.iconPath = _path;
        auto* _surface = Util::Texture::getSDLSurface(_path);
		if(_surface == nullptr) {
			Util::Log::error("Error loading icon: ", SDL_GetError());
		} else {
			Util::Log::debug("Icon loaded successfully from ", _path);
		}
        SDL_SetWindowIcon(window, _surface);
        SDL_FreeSurface(_surface);
    }

    void Window::setPosition(const Vec2I& _position) {
        SDL_SetWindowPosition(window, _position.x, _position.y);
    }

    Vec2I Window::getPosition() const {
        int _x, _y;
        SDL_GetWindowPosition(window, &_x, &_y);
        return {_x, _y};
    }

    SDL_Window* Window::getNativeWindow() const {
        return window;
    }

    void Window::setEventCallback(const UniqueDelegate<void(Event&)>& _callback) {
        eventCallback = _callback;
    }

    Vec2I Window::getWindowSize() const {
        int _width, _height;
        SDL_GL_GetDrawableSize(window, &_width, &_height);
        return  { _width, _height };
    }

    Vec2I Window::getDisplaySize() const {
        SDL_DisplayMode _displayMode;
        SDL_GetCurrentDisplayMode(0, &_displayMode);
        return { _displayMode.w, _displayMode.h };
    }

    void Window::setTitle(const std::string& _title) {
        properties->windowData.title= _title;
        SDL_SetWindowTitle(window, _title.c_str());
    }

    std::string& Window::getTitle() {
        return properties->windowData.title;
    }

    int Window::getHeight() const {
        return getWindowSize().y;
    }

    int Window::getWidth() const {
        return getWindowSize().x;
    }

    SDL_GLContext& Window::getContext() {
        return context;
    }

    bool Window::isMinimized() {
        return minimized;
    }

    bool Window::isRunning() {
        return running;
    }

    bool Window::isFocused() {
        return hasFocus;
    }

    void Window::stop() {
        running = false;
    }

    void Window::refreshDpi() {
        if(SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(window), &properties->windowData.diagonalDpi, &properties->windowData.horizontalDpi, &properties->windowData.verticalDpi) != 0) {
            Util::Log::warn("No DPI could be obtained on the current display with index: ", SDL_GetWindowDisplayIndex(window));
        } else {
            Util::Log::debug("DPI of the current window is -> (Diagonal, Horizontal, Vertical) - (", properties->windowData.diagonalDpi, ", ", properties->windowData.horizontalDpi, ", ", properties->windowData.verticalDpi, ")");
        }
    }

    void Window::allowMouseToMoveOutOfWindow(bool _allow) {
        SDL_SetRelativeMouseMode(_allow ? SDL_FALSE : SDL_TRUE);
    }
}
