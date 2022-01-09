// Created by borja on 24/12/21.

#include "Engine.h"

namespace engine {

    Engine *Engine::gameInstance = nullptr;

    Engine::Engine() {
        ENGINE_ASSERT(!Engine::gameInstance, "Application already exists!");
        Engine::gameInstance = this;
        this->window = Window::createWindow();
        this->window->setEventCallback(ENGINE_BIND_EVENT_FN(Engine::onEvent));
        this->lastFrame = 0;

        this->imGuiLayer = new ImGuiLayer();
        pushOverlay(this->imGuiLayer);

        if(this->timePerFrame < 0)
            this->timePerFrame = 1.0f / 60.f;

        this->window->setVSync(false);

        texture.loadFromFile("assets/test.png");
        TextureAtlasManager::get().addAtlas(50, 50, "assets/test.png");

        int _line = 0;
        int _row = 0;
        for(int _i = 0; _i < 36 ; _i++) {
            if(_i != 0 && _i % 6 == 0) {
                _line++;
                _row = 0;
            }

            Sprite _s;
            int _index = _i > 35 ? 35 : _i;
            _s.setTexture(TextureAtlasManager::get().getTexture("test", "test_" + std::to_string(_index)));
            _s.setPosition({60.f + (float)52 * _row, (float)(200 - _line * 52)});
            sprites.push_back(_s);

            _row++;
        }

        Renderer::init(window.get());
        shape.setOutlineColor(Color::Blue);
        shape.makeCircle({0, 0}, 0.1);
    }

    Engine::~Engine() {

    }

    void Engine::onRun() {
        float _accumulator = 0;

        while (this->running) {
            auto _time = (float) glfwGetTime();
            Delta _dt = _time - this->lastFrame;
            this->lastFrame = _time;
            _accumulator += _dt;

            engine::Profiler::beginFrame(_dt);

            camera.update(window.get());

            if (!this->minimized) {

                Profiler::begin(ProfilerState::FIXED_UPDATE);
                while (_accumulator >= this->timePerFrame) {
                    _accumulator -= this->timePerFrame;
                    this->onFixedUpdate(this->timePerFrame);
                }
                Profiler::end(ProfilerState::FIXED_UPDATE);

                Profiler::begin(ProfilerState::UPDATE);
                this->onUpdate(_dt);
                Profiler::end(ProfilerState::UPDATE);

                Profiler::begin(ProfilerState::RENDERING);
                this->onRender(_dt);
                Profiler::end(ProfilerState::RENDERING);

                #ifdef ENGINE_DEBUG
                this->updateFps();
                #endif
            }

            engine::Profiler::begin(ProfilerState::INPUT);
            this->window->update();
            engine::Profiler::end(ProfilerState::INPUT);

            engine::Profiler::endFrame();
            Renderer::resetDebugInfo();
        }
    }

    void Engine::onEvent(Event &_e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<WindowClosedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowClosed));
        dispatcher.dispatchEvent<WindowResizedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowResized));

        // TODO this must be in another layer, this is why the scroll is called event when on ImGui windows
        dispatcher.dispatchEvent<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(Engine::onMouseScrolled));

        camera.onEvent(_e);

        for (auto _it = this->layerStack.rbegin(); _it != this->layerStack.rend(); ++_it) {
            (*_it)->onEvent(_e);
            if (_e.handled)
                break;
        }
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        for (Layer* _layer : this->layerStack)
            _layer->onFixedUpdate(_fixedDt);
    }

    void Engine::onUpdate(Delta _dt) {
        for (Layer* _layer : this->layerStack)
            _layer->onUpdate(_dt);

        if(Input::isKeyJustPressed(KeyCode::F9))
            showImGuiDebugWindow = !showImGuiDebugWindow;
    }

    void Engine::onRender(Delta _dt) {

        Renderer::clear(Color::Red);

        for (Layer* _layer : this->layerStack)
            _layer->onRender(_dt);

        Renderer::beginDraw(camera);
        for(auto& _sprite : sprites)
            Renderer::draw(_sprite);
        Renderer::endDraw();

        Renderer::beginDebugDraw(camera);
        Renderer::endDebugDraw();

        Profiler::begin(ProfilerState::IMGUI);
        this->imGuiLayer->begin();
        for (Layer* _layer : this->layerStack)
            _layer->onImGuiRender(_dt);
            if(showImGuiDebugWindow)
                engine::ImGuiLayer::drawDebugInfo();
        this->imGuiLayer->end();
        Profiler::end(ProfilerState::IMGUI);
    }

    bool Engine::onMouseScrolled(MouseScrolledEvent& _e) {
        LOG_I("engine")
        float _zoom = camera.getCurrentZoomLevel();
        _zoom -= _e.getScrollY() * camera.getZoomSpeed();
        _zoom = std::max(_zoom, 0.25f);
        camera.setCurrentZoomLevel(_zoom);

        return false;
    }

    bool Engine::onWindowClosed(WindowClosedEvent &_e) {
        this->running = false;
        return true;
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        if (_e.getWidth() == 0 || _e.getHeight() == 0) {
            this->minimized = true;
            return false;
        }

        this->minimized = false;

        return false;
    }

    int Engine::getFps() const { return (int)this->fpsCounter; }

    void Engine::updateFps() {
        if (this->clock.getElapsedTimeSc() >= 1.f) {
            fpsCounter = frameCounter;
            setTitle("Engine: " + std::to_string(fpsCounter));
            frameCounter = 0;
            this->clock.restart();
        }
        ++frameCounter;
    }

    void Engine::setTitle(const std::string& _title) {
        this->window->setTitle(_title);
    }

    void Engine::setFullscreen(bool _fullscreen) {
        this->window->setFullscreen(_fullscreen);
    }

    void Engine::setVSync(bool _vsync) {
        this->window->setVSync(_vsync);
    }

    bool Engine::isVSync() {
        return this->window->isVSyncActive();
    }

    void Engine::setWindowSize(int _width, int _height) {
        this->window->setWindowSize(_width, _height);
    }

    void Engine::pushLayer(Layer* _layer) {
        this->layerStack.pushLayer(_layer);
        _layer->onInit();
    }

    void Engine::pushOverlay(Layer* _layer) {
        this->layerStack.pushOverlay(_layer);
        _layer->onInit();
    }

    void Engine::popLayer(Layer* _layer) {
        this->layerStack.popLayer(_layer);
        _layer->onEnd();
    }

    void Engine::popOverlay(Layer* _layer) {
        this->layerStack.popOverlay(_layer);
        _layer->onEnd();
    }

    void Engine::closeApplication() {
        this->running = false;
    }

}