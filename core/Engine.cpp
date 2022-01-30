// Created by borja on 24/12/21.

#include "Engine.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/render/elements/ShaderManager.h"
#include "sandbox/Sandbox.h"
#include "core/systems/soundSystem/SoundManager.h"

namespace engine {
    Engine *Engine::gameInstance = nullptr;

    Engine::Engine() {
        ENGINE_ASSERT(!Engine::gameInstance, "Application already exists!");
        Engine::gameInstance = this;
        window = Window::createWindow();
        window->setEventCallback(ENGINE_BIND_EVENT_FN(Engine::onEvent));
        lastFrame = 0;

        InputSystem::get().init(window.get());
        Console::get().init();
        ShaderManager::get().init();
        FontManager::get().init();
        Renderer::init(window.get());
        SoundManager::get().init();

        imGuiLayer = new ImGuiLayer();
        pushOverlay(imGuiLayer);

        sandbox = new Sandbox;
        pushLayer(sandbox);

        if(timePerFrame < 0)
            timePerFrame = 1.0f / 60.f;

        window->setVSync(false);

        camera.onResize(window->getWindowSize().x, window->getWindowSize().y);

        FrameBufferSpecification _specs = {
                (uint32_t)window->getWindowSize().x,
                (uint32_t)window->getWindowSize().y
        };

        frameBuffer = new FrameBuffer(_specs);

        Console::get().addCommand("background_color",
                                     "Changes background color 0 <= r,b,g,a <= 255",
                                     BIND_FUNC_1(Engine::changeColorConsoleCommand),
                                     "r g b a");
    }

    Engine::~Engine() {
        delete frameBuffer;
    }

    void Engine::onRun() {
        float _accumulator = 0;

        Delta _dt = 0;
        while (running) {
            Uint64 _start = SDL_GetPerformanceCounter();
            _accumulator += _dt;

            engine::Profiler::beginFrame(_dt);

            InputSystem::get().pollEvents();

            if (!minimized) {

                Profiler::begin(ProfilerState::FIXED_UPDATE);
                while (_accumulator >= timePerFrame) {
                    _accumulator -= timePerFrame;
                    onFixedUpdate(timePerFrame);
                }
                Profiler::end(ProfilerState::FIXED_UPDATE);

                Profiler::begin(ProfilerState::UPDATE);
                onUpdate(_dt);
                Profiler::end(ProfilerState::UPDATE);

                Profiler::begin(ProfilerState::RENDERING);
                onRender(_dt);
                Profiler::end(ProfilerState::RENDERING);

                #ifdef ENGINE_DEBUG
                updateFps();
                #endif
            }

            engine::Profiler::begin(ProfilerState::INPUT);
            window->update();
            engine::Profiler::end(ProfilerState::INPUT);

            engine::Profiler::endFrame();
            Renderer::resetDebugInfo();

            Uint64 _end = SDL_GetPerformanceCounter();
            float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
            _dt = _elapsedMS;
            timer += _dt;

            // Cap to 60 FPS
//            int _toWait = 16.666f - _dt * 1000;
//            LOG_I(_toWait)
//            if(_toWait > 0)
//                SDL_Delay(_toWait);
        }
    }

    void Engine::onEvent(Event &_e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<WindowClosedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowClosed));
        dispatcher.dispatchEvent<WindowResizedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowResized));

        // TODO this must be in another layer, this is why the scroll is called event when on ImGui windows
        dispatcher.dispatchEvent<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(Engine::onMouseScrolled));

        for (auto _it = layerStack.rbegin(); _it != layerStack.rend(); ++_it) {
            (*_it)->onEvent(_e);
            if (_e.handled)
                break;
        }
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        for (Layer* _layer : layerStack)
            _layer->onFixedUpdate(_fixedDt);
    }

    void Engine::onUpdate(Delta _dt) {
        auto _fbSpec = frameBuffer->getSpecification();
        if(window->getWindowSize().x > 0 && window->getWindowSize().y > 0 && (_fbSpec.width != window->getWindowSize().x || _fbSpec.height != window->getWindowSize().y)) {
            frameBuffer->resize(window->getWindowSize().x, window->getWindowSize().y);
            camera.onResize(window->getWindowSize().x, window->getWindowSize().y);
        }

        for (Layer* _layer : layerStack)
            _layer->onUpdate(_dt);

        if(InputManager::isKeyJustPressed(KeyCode::F9))
            showImGuiDebugWindow = !showImGuiDebugWindow;

    }

    void Engine::onRender(Delta _dt) {
        frameBuffer->bind();

        Renderer::clear(backgroundColor);

        Renderer::beginDraw(camera);
        for (Layer* _layer : layerStack)
            _layer->onRender(_dt);
        Renderer::endDraw();

        // Debug rendering
        Renderer::beginDebugDraw(camera);
        Renderer::drawSquare({0, 0}, {2, 2}, Color::Blue);
        Renderer::endDebugDraw();

        frameBuffer->unbind();

        // Imgui rendering
        Profiler::begin(ProfilerState::IMGUI);
        imGuiLayer->begin();
        for (Layer* _layer : layerStack)
            _layer->onImGuiRender(_dt);
            if(showImGuiDebugWindow)
                imGuiLayer->drawDebugInfo();
        imGuiLayer->end();
        Profiler::end(ProfilerState::IMGUI);
    }

    bool Engine::onMouseScrolled(MouseScrolledEvent& _e) {
        float _zoom = camera.getCurrentZoomLevel();
        _zoom -= _e.getScrollY() * camera.getZoomSpeed();
        _zoom = std::max(_zoom, 0.1f);
        camera.setCurrentZoomLevel(_zoom);

        return false;
    }

    bool Engine::onWindowClosed(WindowClosedEvent &_e) {
        running = false;
        return true;
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        if (_e.getWidth() == 0 || _e.getHeight() == 0) {
            minimized = true;
            return false;
        }

        minimized = false;

        return false;
    }

    int Engine::getFps() const { return (int)fpsCounter; }

    void Engine::updateFps() {
        if (timer >= 1.f) {
            fpsCounter = frameCounter;
            setTitle("Engine: " + std::to_string(fpsCounter));
            frameCounter = 0;
            timer = 0;
        }
        ++frameCounter;
    }

    void Engine::setTitle(const std::string& _title) {
        window->setTitle(_title);
    }

    void Engine::setFullscreen(bool _fullscreen) {
        window->setFullscreen(_fullscreen);
    }

    void Engine::setVSync(bool _vsync) {
        window->setVSync(_vsync);
    }

    bool Engine::isVSync() {
        return window->isVSyncActive();
    }

    void Engine::setWindowSize(int _width, int _height) {
        window->setWindowSize(_width, _height);
    }

    void Engine::pushLayer(Layer* _layer) {
        layerStack.pushLayer(_layer);
        _layer->onInit();
    }

    void Engine::pushOverlay(Layer* _layer) {
        layerStack.pushOverlay(_layer);
        _layer->onInit();
    }

    void Engine::popLayer(Layer* _layer) {
        layerStack.popLayer(_layer);
        _layer->onEnd();
    }

    void Engine::popOverlay(Layer* _layer) {
        layerStack.popOverlay(_layer);
        _layer->onEnd();
    }

    void Engine::closeApplication() {
        running = false;
    }

    bool Engine::fromRunToRoll(const TransitionParams& _foo) {
        return InputManager::isKeyJustPressed(KeyCode::A);
    }

    bool Engine::fromRollToRun(const TransitionParams& _foo) {
        return InputManager::isKeyJustPressed(KeyCode::S);
    }

    Logs Engine::changeColorConsoleCommand(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        return {"Changed color"};
    }

}