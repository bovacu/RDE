#include "core/render/window/Window.h"
#include "core/render/window/event/WindowEvent.h"
#include "stb/stb_image.h"
#include "core/platform/PlatformHeaderSDL.h"
#include "core/Engine.h"

#if IS_MOBILE()
    #include <GLES3/gl32.h>
#elif IS_DESKTOP()
    #include <glad/glad.h>
#endif

namespace GDE {

    Window::Window(GDEConfig* _config) : window(nullptr) {
        properties = _config;

        #ifdef ENGINE_DEBUG
        SDL_version _compiled, _linked;
        LOG_I("Creating window ", _config->windowData.title, " (", _config->windowData.size.x, _config->windowData.size.y, ")");
        #endif

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            LOG_E("At least one module of SDL couldn't be initialized, so can't start the engine")
            return;
        }

        SDL_version compiled;
        SDL_version linked;

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        SDL_Log("We compiled against SDL version %u.%u.%u ...\n", compiled.major, compiled.minor, compiled.patch);
        SDL_Log("But we are linking against SDL version %u.%u.%u.\n", linked.major, linked.minor, linked.patch);

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        #if IS_MOBILE()
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        #endif

        window = SDL_CreateWindow(_config->windowData.title.c_str(), 0, 0,
                                  (int)_config->windowData.size.x, (int)_config->windowData.size.y,
                                  SDL_WINDOW_OPENGL| SDL_WINDOW_ALLOW_HIGHDPI);
        context = SDL_GL_CreateContext(window);

        SDL_GL_MakeCurrent(window, context);
        SDL_GL_SetSwapInterval(1);

        #if IS_DESKTOP() && !IS_MOBILE()
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_SetWindowResizable(window, SDL_TRUE);
        gladLoadGLLoader(SDL_GL_GetProcAddress);
        LOG_S("GLAD and SDL2 initiated correctly");
        #elif IS_MOBILE()
        SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
        SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO, "1");
        SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");
        #endif

        if(!properties->projectData.iconPath.empty()) setIcon(properties->projectData.iconPath);
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
        LOG_W("Setting icon is not supported yet!!!!")
        SDL_SetWindowIcon(window, nullptr);
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

    void Window::setEventCallback(const UDelegate<void(Event&)>& _callback) {
        eventCallback = _callback;
    }

    Vec2I Window::getWindowSize() const {
        int _width, _height;
        SDL_GL_GetDrawableSize(window, &_width, &_height);
        return  {_width, _height};
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

    void Window::swapBuffers() const {
        SDL_GL_SwapWindow(window);
    }

    SDL_GLContext& Window::getContext() {
        return context;
    }

}
