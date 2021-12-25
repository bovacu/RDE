#include "Window.h"
#include "core/render/window/event/WindowEvent.h"
#include "stb_image.h"
#include "core/render/window/event/KeyEvent.h"
#include "core/render/window/input/Input.h"
#include "core/render/window/event/MouseEvent.h"

namespace engine {

    static uint8_t GLFWWindowCount = 0;

    static void GLFWErrorCallback(int _error, const char* _description) {
//        LOG_ERROR_CORE("GLFW Error ({0}): {1}", _error, _description);
    }

    Window::Window(const WindowProperties& _props) : window(nullptr) {
        this->init(_props);
    }

    Window::~Window() {
        this->shutdown();
    }

    void Window::init(const WindowProperties& _props) {

        this->data.title = _props.title;
        this->data.width = (int)_props.width;
        this->data.height = (int)_props.height;

#ifdef ENGINE_DEBUG
        LOG_W("Running in debug mode");
        LOG_I("Creating window ", _props.title, " (", _props.width, _props.height, ")");
#endif

#pragma region Initializing

        /// Initialize GLFW, just for the first window.
        if (GLFWWindowCount == 0) {
            int success = glfwInit();
            ENGINE_ASSERT(success, "Could not initialize GLFW!")
            glfwSetErrorCallback(GLFWErrorCallback);
        }

#if defined(ENGINE_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        /// Creating the main window.
        this->window = glfwCreateWindow((int)_props.width, (int)_props.height, this->data.title.c_str(), nullptr, nullptr);
        ++GLFWWindowCount;

        /// Initializing the graphics context to be able to draw.
        this->context = GraphicsContext::create(this->window);
        this->context->init();

        /// Setting the basic data of the window to the UserPointer.
        glfwSetWindowUserPointer(this->window, &this->data);
        this->setVSync(false);

#pragma endregion Initializing

#pragma region CenteringAndEnablingFullscreen
        /// This region is to center the window on the screen and to enable fullscreen.
        this->monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* _mode = glfwGetVideoMode(this->monitor);
        if (!_mode)
            return;

        int _monitorX, _monitorY;
        glfwGetMonitorPos(this->monitor, &_monitorX, &_monitorY);
        int _windowWidth, _windowHeight;
        glfwGetWindowSize(this->window, &_windowWidth, &_windowHeight);

        this->data.position.x = _monitorX + (_mode->width - _windowWidth) / 2;
        this->data.position.y = _monitorY + (_mode->height - _windowHeight) / 2;

        glfwSetWindowPos(this->window, this->data.position.x, this->data.position.y);

#pragma endregion CenteringAndEnablingFullscreen

#pragma region GLFWCallbacksSetup

        /// Setting GLFW callbacks for polled events
        glfwSetWindowSizeCallback(this->window, [](GLFWwindow* _window, int _width, int _height) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);
            _data.width = _width;
            _data.height = _height;

            WindowResizedEvent _event(_width, _height);
            _data.eventCallback(_event);
        });

        glfwSetWindowPosCallback(this->window, [](GLFWwindow* _window, int _x, int _y) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);
            _data.position.x = _x;
            _data.position.y = _y;

            WindowMovedEvent _event(_x, _y);
            _data.eventCallback(_event);
        });

        glfwSetWindowCloseCallback(this->window, [](GLFWwindow* _window) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);
            WindowClosedEvent _event;
            _data.eventCallback(_event);
        });

        glfwSetKeyCallback(this->window, [](GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            switch (_action) {
                case GLFW_PRESS: {
                    KeyPressedEvent _event(static_cast<KeyCode>(_key), 0);
                    _data.eventCallback(_event);
                    break;
                }

                case GLFW_RELEASE: {
                    KeyReleasedEvent _event(static_cast<KeyCode>(_key));
                    _data.eventCallback(_event);
                    Input::pressedKeyboardKeys[static_cast<KeyCode>(_key)] = false;
                    break;
                }

                case GLFW_REPEAT: {
                    KeyPressedEvent _event(static_cast<KeyCode>(_key), 1);
                    _data.eventCallback(_event);
                    break;
                }

                default: {  };
            }
        });

        glfwSetCharCallback(this->window, [](GLFWwindow* _window, unsigned int _key) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            KeyTypedEvent _event(static_cast<KeyCode>(_key));
            _data.eventCallback(_event);
        });

        glfwSetMouseButtonCallback(this->window, [](GLFWwindow* _window, int _button, int _action, int _mods) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            switch (_action) {
                case GLFW_PRESS : {
                    MouseButtonPressedEvent _event(static_cast<MouseCode>(_button));
                    _data.eventCallback(_event);
                    break;
                }
                case GLFW_RELEASE : {
                    MouseButtonReleasedEvent _event(static_cast<MouseCode>(_button));
                    _data.eventCallback(_event);
                    Input::pressedMouseButtons[static_cast<MouseCode>(_button)] = false;
                    break;
                }

                default: {  };
            }
        });

        glfwSetScrollCallback(this->window, [](GLFWwindow* _window, double _xOffset, double _yOffset) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            MouseScrolledEvent _event((float)_xOffset, (float)_yOffset);
            _data.eventCallback(_event);
        });

        glfwSetCursorPosCallback(this->window, [](GLFWwindow* _window, double _xPos, double _yPos) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            MouseMovedEvent _event((float)_xPos, (float)_yPos);
            _data.eventCallback(_event);
        });

        glfwSetWindowIconifyCallback(this->window, [](GLFWwindow* _window, int _iconified) {
            WindowData& _data = *(WindowData*)glfwGetWindowUserPointer(_window);

            WindowMinimizedEvent _event(_iconified);
            _data.eventCallback(_event);
        });

#pragma endregion GLFWCallbacksSetup

    }

    void Window::setWindowSize(int _width, int _height) {
        this->data.width = _width;
        this->data.height = _height;
        glfwSetWindowSize(this->window, _width, _height);
    }

    void Window::shutdown() {
        glfwDestroyWindow(this->window);
        --GLFWWindowCount;

        if (GLFWWindowCount == 0) {
            glfwTerminate();
        }
    }

    void Window::update() {
        glfwPollEvents();
        this->context->swapBuffers();
    }

    void Window::setVSync(bool _enabled) {
        if (_enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        this->data.vSync = _enabled;
    }

    bool Window::isVSyncActive() const {
        return this->data.vSync;
    }

    void Window::setFullscreen(bool _fullscreen) {
        if ( _fullscreen ) {
            // backup window position and window size
            int _x, _y;
            glfwGetWindowPos( this->window, &_x, &_y );
            this->data.position = {_x, _y};
            glfwGetWindowSize( this->window, &this->data.width, &this->data.height );

            // get resolution of monitor
            const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            // switch to full screen
            glfwSetWindowMonitor( this->window, this->monitor, 0, 0, mode->width, mode->height, 0 );
            this->setWindowSize(mode->width, mode->height);
        }
        else {
            // restore last window size and position
            glfwSetWindowMonitor( this->window, nullptr, this->data.position.x, this->data.position.y
                    , this->data.width, this->data.height, 0 );
            this->setWindowSize(this->data.width, this->data.height);
        }
    }

    bool Window::isFullscreen() const {
//        LOG_CRITICAL_CORE("isFullscreen from WindowsWindow not implemented yet");
        return false;
    }

    void Window::setIcon(const char* _path) {
        int _w = 0, _h = 0;

        /// First getting the pixels.
        unsigned char* _pixels = stbi_load(_path, &_w, &_h, nullptr, 4);

        /// Checking pixels could be got.
        if (_pixels == nullptr) {
            ENGINE_ASSERT(false, "Couldn't load ImGui Texture")
        }

        /// Removing the previous icon.
        glfwSetWindowIcon(this->window, 0, nullptr);

        /// Loading the new icon.
        GLFWimage _image[1];
        _image[0].width = _w;
        _image[0].height = _h;
        _image[0].pixels = _pixels;

        /// Setting the new icon to the window.
        glfwSetWindowIcon(this->window, 1, _image);

        /// Freeing space.
        stbi_image_free(_pixels);
    }

    void Window::setWindowOptions(WindowOptions _op, bool _allow) {
//        auto _windowHandle = GetForegroundWindow();
//
//        /// GetWindowLongPtr is used instead of GetWindowLong for x64 system compatibilities
//        auto _style = GetWindowLongPtr(_windowHandle, GWL_STYLE);
//
//        /// The style in GLFW works as an int, so to enable or disable a function we add or subtract the desired amount.
//        _style += this->windowOptionsToGLFW(_op, _allow);
//
//        SetWindowLongPtr(_windowHandle, GWL_STYLE, _style);
    }

    long Window::windowOptionsToGLFW(WindowOptions _options, bool _allow) {
        long _newStyle = 0L;
        int _toAdd = _allow ? 1 : -1;

        /// Checking each of the mask in case the need to be enabled/disabled and adding it to the new style value.
//        if(((unsigned)WindowOptions_::WindowOptions_Minimize & (unsigned)_options) == WindowOptions_::WindowOptions_Minimize)
//            _newStyle += WS_MINIMIZEBOX * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_Maximize & (unsigned)_options) == WindowOptions_::WindowOptions_Maximize)
//            _newStyle += WS_MAXIMIZEBOX * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_Resize & (unsigned)_options) == WindowOptions_::WindowOptions_Resize)
//            _newStyle += WS_THICKFRAME * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_Visible & (unsigned)_options) == WindowOptions_::WindowOptions_Visible)
//            _newStyle += WS_VISIBLE * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_Disabled & (unsigned)_options) == WindowOptions_::WindowOptions_Disabled)
//            _newStyle += WS_DISABLED * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_H_Scroll & (unsigned)_options) == WindowOptions_::WindowOptions_H_Scroll)
//            _newStyle += WS_HSCROLL * _toAdd;
//
//        if(((unsigned)WindowOptions_::WindowOptions_V_Scroll & (unsigned)_options) == WindowOptions_::WindowOptions_V_Scroll)
//            _newStyle += WS_VSCROLL * _toAdd;

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

    GLFWwindow* Window::getNativeWindow() const {
        return window;
    }

    void Window::setEventCallback(const EventCallbackFn& _callback) {
        data.eventCallback = _callback;
    }

    Vec2F Window::getWindowSize() const {
        return  Vec2F((float)this->getWidth(), (float)this->getHeight());;
    }

    void Window::setTitle(const std::string& _title) {
        data.title = _title; glfwSetWindowTitle(this->window, _title.c_str());
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

}
