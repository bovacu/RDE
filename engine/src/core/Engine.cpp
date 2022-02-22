// Created by borja on 24/12/21.

#include "core/Engine.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/render/elements/ShaderManager.h"
//#include "../../sandbox/Sandbox.h"
#include "core/systems/soundSystem/SoundManager.h"

namespace engine {
    Engine *Engine::gameInstance = nullptr;

    Engine::Engine() {
        ENGINE_ASSERT(!Engine::gameInstance, "Application already exists!");
        Engine::gameInstance = this;
        window = Window::createWindow();
        window->setEventCallback(ENGINE_BIND_EVENT_FN(Engine::onEvent));
        lastFrame = 0;

        InputManager::get().init(window.get());
        Console::get().init();
        ShaderManager::get().init();
        FontManager::get().init();
        Renderer::init(window.get());
        SoundManager::get().init();

        #if !IS_MOBILE()
        imGuiLayer = new ImGuiLayer();
        pushOverlay(imGuiLayer);
        #endif

        if(timePerFrame < 0)
            timePerFrame = 1.0f / 60.f;

        window->setVSync(false);

        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);
        camera.onResize(_width, _height);

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

    void Engine::onRun() {
        float _accumulator = 0;

        Delta _dt = 0;
        while (running) {
            Uint64 _start = SDL_GetPerformanceCounter();
            _accumulator += _dt;

            engine::Profiler::beginFrame(_dt);

            InputManager::get().pollEvents();

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
        }
    }

    void Engine::onEvent(Event &_e) {

        EventDispatcher _ed(_e);
        _ed.dispatchEvent<WindowResizedEvent>(BIND_FUNC_1(Engine::onWindowResized));

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

//        // Debug rendering
//        Renderer::beginDebugDraw(camera);
//        Renderer::drawSquare({0, 0}, {2, 2}, Color::Blue);
//        Renderer::drawSquare({-100, 0}, {50, 50}, Color::Green);
//        Renderer::endDebugDraw();

        frameBuffer->unbind();

        // Imgui rendering
        Profiler::begin(ProfilerState::IMGUI);
        #if !IS_MOBILE()
        imGuiLayer->begin();
        for (Layer* _layer : layerStack)
            _layer->onImGuiRender(_dt);
            if(showImGuiDebugWindow)
                imGuiLayer->drawDebugInfo();
        imGuiLayer->end();
        #endif
        Profiler::end(ProfilerState::IMGUI);
    }

    bool Engine::onWindowClosed(WindowClosedEvent &_e) {
        running = false;
        return true;
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);
        frameBuffer->resize(_width, _height);
        camera.onResize(_width, _height);
        return true;
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
        window->setWindowSizeAndUpdateNativeWindow(_width, _height);
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

    void Engine::destroy() {
        TextureAtlasManager::get().destroy();
        ShaderManager::get().destroy();
        FontManager::get().destroy();
        SoundManager::get().destroy();
        Renderer::destroy();
        InputManager::get().destroy();
        delete frameBuffer;
    }

}