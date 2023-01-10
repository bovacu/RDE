// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/WindowInput.h"
#include "core/systems/eventSystem/WindowEvent.h"
#include "core/systems/uiSystem/SceneManager.h"
#include "core/Engine.h"
#include "SDL2/SDL_video.h"

namespace RDE {

    void WindowInput::init(Engine* _engine, Window* _window) {
        window = _window;
        engine = _engine;

        UniqueDelegate<void(SDL_Event&)> weDel, wdeDel, wdebeDel, wdefeDel, wwebeDel, wwefeDel;
        weDel.bind<&WindowInput::onWindowEvent>(this);
        wdeDel.bind<&WindowInput::onDestroyApp>(this);
        wdebeDel.bind<&WindowInput::onDidEnterBackground>(this);
        wdefeDel.bind<&WindowInput::onDidEnterForegroundApp>(this);
        wwebeDel.bind<&WindowInput::onWillEnterBackground>(this);
        wwefeDel.bind<&WindowInput::onWillEnterForegroundApp>(this);

        events[(int)SystemEventEnum::WINDOW_EVENT] = weDel;
        events[(int)SystemEventEnum::APP_ON_DESTROY_E] = wdeDel;
        events[(int)SystemEventEnum::APP_DID_ENTER_BACK_E] = wdebeDel;
        events[(int)SystemEventEnum::APP_DID_ENTER_FOREG_E] = wdefeDel;
        events[(int)SystemEventEnum::APP_WILL_ENTER_BACK_E] = wwebeDel;
        events[(int)SystemEventEnum::APP_WILL_ENTER_FOREG] = wwefeDel;

        ignoredEvents = { WINDOW_AUDIO_DEVICE_CONNECTED_E, WINDOW_AUDIO_DEVICE_DISCONNECTED_E };
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
        window->hasFocus = true;

        if(!window->onWindowFocusedCallback.isEmpty()) {
            window->onWindowFocusedCallback();
        }
    }

    void WindowInput::onWindowLostFocus(SDL_Event& _event) {
        window->hasFocus = false;
        
        if(!window->onWindowUnfocusedCallback.isEmpty()) {
            window->onWindowUnfocusedCallback();
        }
    }

    void WindowInput::onWindowResize(SDL_Event& _event) {
        WindowResizedEvent _rwEvent(_event.window.data1, _event.window.data2);
        window->consumeEvent(_rwEvent);
        
        if(!window->onWindowResizedCallback.isEmpty()) {
            window->onWindowResizedCallback(Vec2I { _event.window.data1, _event.window.data2 });
        }
    }

    void WindowInput::onWindowMoved(SDL_Event& _event) {
        WindowMovedEvent _wmEvent(_event.window.data1, _event.window.data2);
        window->consumeEvent(_wmEvent);

        if(!window->onWindowMovedCallback.isEmpty()) {
            window->onWindowMovedCallback(Vec2F { (float)_event.window.data1, (float)_event.window.data2 });
        }

        onWindowDisplayChanged(SDL_GetWindowDisplayIndex(window->window));
    }

    void WindowInput::onWindowDisplayChanged(int _display) {
        if(window->currentDisplayIndex == _display) {
            return;
        }

        window->currentDisplayIndex = _display;
        WindowDisplayChangedEvent _e(_display);
        window->consumeEvent(_e);

        window->refreshDpi();
        engine->manager.sceneManager.getDisplayedScene()->onDisplayChanged();

        if(!window->onWindowDisplayChangedCallback.isEmpty()) {
            window->onWindowDisplayChangedCallback(_display);
        }
    }

    void WindowInput::onWindowMinimized(SDL_Event& _event) {
        WindowMinimizedEvent _e(1);
        window->consumeEvent(_e);
        window->minimized = true;

        if(!window->onWindowMinimizedCallback.isEmpty()) {
            window->onWindowMinimizedCallback();
        }
    }

    void WindowInput::onWindowMaximized(SDL_Event& _event) {
        WindowMinimizedEvent _e(0);
        window->consumeEvent(_e);
        window->minimized = false;

        if(!window->onWindowMaximizedCallback.isEmpty()) {
            window->onWindowMaximizedCallback();
        }
    }

    void WindowInput::onQuit(SDL_Event& _event) {
        engine->getWindow().stop();
    }

    void WindowInput::onDidEnterForegroundApp(SDL_Event& _event) {
        Util::Log::warn("DID ENTER FOREGROUND");
        window->hasFocus = true;
    }

    void WindowInput::onDidEnterBackground(SDL_Event& _event) {
        Util::Log::warn("DID ENTER BACKGROUND");
        window->hasFocus = false;
    }

    void WindowInput::onDestroyApp(SDL_Event& _event) {
        Util::Log::warn("AUTO TERMINATING!!!!!");
        engine->getWindow().stop();
    }

    void WindowInput::onWillEnterForegroundApp(SDL_Event& _event) {
        Util::Log::warn("WILL ENTER FOREGROUND");
        window->hasFocus = false;
    }

    void WindowInput::onWillEnterBackground(SDL_Event& _event) {
        Util::Log::warn("WILL ENTER BACKGROUND");
        window->hasFocus = true;
    }
}
