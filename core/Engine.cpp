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

        this->window->setVSync(true);

        atlasManager.addAtlas(50, 50, "assets/test.png");

        int _line = 0;
        int _row = 0;
        for(int _i = 0; _i < 36 ; _i++) {
            if(_i != 0 && _i % 6 == 0) {
                _line++;
                _row = 0;
            }

            Sprite _s;
            int _index = _i > 35 ? 35 : _i;
            _s.setTexture(atlasManager.getTexture("test", "test_" + std::to_string(_index)));
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
            engine::Profiler::beginFrame();

            camera.update(window.get());

            auto _time = (float) glfwGetTime();
            Delta _dt = _time - this->lastFrame;
            this->lastFrame = _time;
            _accumulator += _dt;

            if (!this->minimized) {

                while (_accumulator >= this->timePerFrame) {
                    _accumulator -= this->timePerFrame;
                    this->onFixedUpdate(this->timePerFrame);
                }

                this->onUpdate(_dt);
                this->onRender(_dt);

                #ifdef ENGINE_DEBUG
                this->updateFps();
                #endif
            }

            engine::Profiler::begin(ProfilerState::INPUT);
            this->window->update();
            engine::Profiler::end(ProfilerState::INPUT);

            engine::Profiler::endFrame();
        }
    }

    void Engine::onEvent(Event &_e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<WindowClosedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowClosed));
        dispatcher.dispatchEvent<WindowResizedEvent>(ENGINE_BIND_EVENT_FN(Engine::onWindowResized));

        for (auto _it = this->layerStack.rbegin(); _it != this->layerStack.rend(); ++_it) {
            (*_it)->onEvent(_e);
            if (_e.handled)
                break;
        }
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        Profiler::begin(ProfilerState::FIXED_UPDATE);
        for (Layer* _layer : this->layerStack)
            _layer->onFixedUpdate(_fixedDt);
        Profiler::end(ProfilerState::FIXED_UPDATE);
    }

    void Engine::onUpdate(Delta _dt) {
        Profiler::begin(ProfilerState::UPDATE);
        for (Layer* _layer : this->layerStack)
            _layer->onUpdate(_dt);

        if(Input::isKeyJustPressed(KeyCode::F9))
            showImGuiDebugWindow = !showImGuiDebugWindow;

        Profiler::end(ProfilerState::UPDATE);
    }

    void Engine::onRender(Delta _dt) {

        Renderer::clear(Color::Red);

        Profiler::begin(ProfilerState::RENDERING);
        for (Layer* _layer : this->layerStack)
            _layer->onRender(_dt);

        Renderer::beginDraw(camera);
//        Renderer::draw(sprite);
//        Renderer::draw(subSprite);
        for(auto& _sprite : sprites)
            Renderer::draw(_sprite);
        Renderer::endDraw();

        Renderer::beginDebugDraw(camera);
        Renderer::drawShape(shape);
//        Renderer::drawSquare({-200, 0}, {400, 400});
//        Renderer::drawSquare({200, 0}, {400, 400}, Color::Green);
//        Renderer::drawLine({0, 0}, {400, 0}, Color::Blue);
        Renderer::endDebugDraw();
        Profiler::end(ProfilerState::RENDERING);

        Profiler::begin(ProfilerState::IMGUI);
        this->imGuiLayer->begin();
        for (Layer* _layer : this->layerStack)
            _layer->onImGuiRender(_dt);
            if(showImGuiDebugWindow)
                engine::ImGuiLayer::drawDebugInfo();
        this->imGuiLayer->end();
        Profiler::end(ProfilerState::IMGUI);
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