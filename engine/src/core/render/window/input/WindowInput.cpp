// Created by borja on 6/2/22.


#include "core/render/window/input/WindowInput.h"
#include "core/render/window/event/WindowEvent.h"
#include "core/Engine.h"

namespace engine {

    void WindowInput::init(Window* _window) {
        window = _window;
        events[(int)SystemEventEnum::WINDOW_EVENT] = BIND_FUNC_1(WindowInput::onWindowEvent);

        ignoredEvents = { SDL_AUDIODEVICEADDED };
    }

    void WindowInput::onWindowEvent(SDL_Event& _event) {
        switch (_event.window.event) {
            case SDL_WINDOWEVENT_RESIZED: onWindowResize(_event); break;
            case SDL_WINDOWEVENT_FOCUS_GAINED: onWindowGainFocus(_event); break;
            case SDL_WINDOWEVENT_FOCUS_LOST: onWindowLostFocus(_event); break;
            case SDL_WINDOWEVENT_ENTER: onWindowEnter(_event); break;
            case SDL_WINDOWEVENT_LEAVE: onWindowExit(_event); break;
            case SDL_WINDOWEVENT_MOVED: onWindowMoved(_event); break;
            case SDL_WINDOWEVENT_MINIMIZED: onWindowMinimized(_event); break;
            case SDL_WINDOWEVENT_MAXIMIZED: onWindowMaximized(_event); break;
            case SDL_WINDOWEVENT_CLOSE: onQuit(_event); break;
        }
    }

    void WindowInput::onWindowEnter(SDL_Event& _event) {

    }

    void WindowInput::onWindowExit(SDL_Event& _event) {

    }

    void WindowInput::onWindowGainFocus(SDL_Event& _event) {

    }

    void WindowInput::onWindowLostFocus(SDL_Event& _event) {

    }

    void WindowInput::onWindowResize(SDL_Event& _event) {
        window->setWindowSize(_event.window.data1, _event.window.data2);
        WindowResizedEvent _rwEvent(_event.window.data1, _event.window.data2);
        window->consumeEvent(_rwEvent);
    }

    void WindowInput::onWindowMoved(SDL_Event& _event) {
        WindowMovedEvent _wmEvent(_event.window.data1, _event.window.data2);
        window->setPosition({_event.window.data1, _event.window.data2});
        window->consumeEvent(_wmEvent);
    }

    void WindowInput::onWindowMinimized(SDL_Event& _event) {
        WindowMinimizedEvent _e(1);
        window->consumeEvent(_e);
    }

    void WindowInput::onWindowMaximized(SDL_Event& _event) {
        WindowMinimizedEvent _e(0);
        window->consumeEvent(_e);
    }

    void WindowInput::onQuit(SDL_Event& _event) {
        Engine::get().setRunning(false);
    }
}