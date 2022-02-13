#include "core/render/window/Window.h"
#include "core/render/window/event/WindowEvent.h"
#include "stb/stb_image.h"
#include "core/platform/PlatformHeaderSDL.h"

#if IS_MOBILE()
    #include <GLES3/gl32.h>
#elif IS_DESKTOP()
    #include <glad/glad.h>
#endif

namespace engine {

    Window::Window(const WindowProperties& _props) : window(nullptr) {
        init(_props);
    }

    Window::~Window() {
        shutdown();
    }

    void Window::init(const WindowProperties& _props) {
        data.title = _props.title;
        data.width = (int)_props.width;
        data.height = (int)_props.height;

#ifdef ENGINE_DEBUG
        LOG_W("Running in debug mode");
        LOG_I("Creating window ", _props.title, " (", _props.width, _props.height, ")");
#endif

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        #if IS_MOBILE()
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        #endif

        window = SDL_CreateWindow(_props.title.c_str(), 0, 0, (int)_props.width, (int)_props.height, SDL_WINDOW_OPENGL| SDL_WINDOW_ALLOW_HIGHDPI);
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
    }

    void Window::setWindowSize(int _width, int _height) {
        data.width = _width;
        data.height = _height;
    }

    void Window::setWindowSizeAndUpdateNativeWindow(int _width, int _height) {
        data.width = _width;
        data.height = _height;
        SDL_SetWindowSize(window, _width, _height);
    }

    void Window::shutdown() {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
        SDL_Quit();
    }

    void Window::update() {
        SDL_GL_SwapWindow(window);
    }

    void Window::setVSync(bool _enabled) {
        data.vSync = _enabled;
    }

    bool Window::isVSyncActive() const {
        return data.vSync;
    }

    void Window::setFullscreen(bool _fullscreen) {
        data.fullscreen = _fullscreen;
        SDL_SetWindowFullscreen(window, _fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    }

    bool Window::isFullscreen() const {
        return data.fullscreen;
    }

    void Window::setIcon(const char* _path) {

    }

    void Window::setWindowOptions(WindowOptions _op, bool _allow) {

    }

    long Window::windowOptionsToGLFW(WindowOptions _options, bool _allow) {
        long _newStyle = 0L;
        int _toAdd = _allow ? 1 : -1;
        return _newStyle;
    }

    std::unique_ptr<Window> Window::createWindow(const WindowProperties& _props) {
        return std::make_unique<Window>(_props);
    }

    void Window::setPosition(const Vec2I& _position) {
        data.position = Vec2I(_position.x, _position.y);
    }

    Vec2I Window::getPosition() const {
        return data.position;
    }

    SDL_Window* Window::getNativeWindow() const {
        return window;
    }

    void Window::setEventCallback(const EventCallbackFn& _callback) {
        data.eventCallback = _callback;
    }

    Vec2I Window::getWindowSize() const {
        return  {getWidth(), getHeight()};
    }

    void Window::setTitle(const std::string& _title) {
        data.title = _title;
        SDL_SetWindowTitle(window, _title.c_str());
    }

    std::string& Window::getTitle() {
        return data.title;
    }

    int Window::getHeight() const {
        return data.height;
    }

    int Window::getWidth() const {
        return data.width;
    }

    void Window::swapBuffers() const {
        SDL_GL_SwapWindow(window);
    }

    SDL_GLContext& Window::getContext() {
        return context;
    }

}
