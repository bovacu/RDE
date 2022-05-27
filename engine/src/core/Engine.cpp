// Created by borja on 24/12/21.

#include "core/Engine.h"
#include "core/render/Renderer.h"
#include "core/systems/physicsSystem/Physics.h"
#include "core/systems/ecsSystem/ECSManager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/systems/profiling/Profiler.h"

namespace GDE {


    Engine::Engine() {
        ConfigManager::loadGDEConfig(&gdeConfig);
        window = new Window(&gdeConfig);

        UDelegate<void(Event&)> onEventDelegate;
        onEventDelegate.bind<&Engine::onEvent>(this);
        window->setEventCallback(onEventDelegate);
    }

    void Engine::onInit(Scene* _scene) {
        wreDel.bind<&Engine::onWindowResized>(this);

        manager.init(this);

        #if !IS_MOBILE()
        imGuiLayer = new ImGuiScene(this);
        imGuiLayer->onInit();
        #endif

        fixedDelta = 1.0f / 60.f;
        window->setVSync(true);
        Renderer::setClearColor(backgroundColor);

        FrameBufferSpecification _specs = {(uint32_t)window->getWindowSize().x,(uint32_t)window->getWindowSize().y};
        frameBuffer = new FrameBuffer(_specs, &manager);

        manager.consoleManager.addCommand<&Engine::changeColorConsoleCommand>("background_color"," Changes background color 0 <= r,b,g,a <= 255", this, "r g b a");
        manager.consoleManager.addCommand<&Engine::setParentCommand>( "parent_set", "Sets the parent of A as B", this, "A B");

        manager.sceneManager.loadScene(_scene, _scene->getName());
        manager.sceneManager.displayScene(_scene->getName());
    }

    void Engine::onRun() {
        float _accumulator = 0;

        Delta _dt = 0;
        while (window->isRunning()) {
            Uint64 _start = SDL_GetPerformanceCounter();
            _accumulator += _dt;

            if(manager.sceneManager.getDisplayedScene() == nullptr) return;

            GDE::Profiler::beginFrame(_dt);
            manager.inputManager.pollEvents();

            if (!window->isMinimized()) {

                Profiler::begin(ProfilerState::UPDATE);
                onUpdate(_dt);
                Profiler::end(ProfilerState::UPDATE);

                Profiler::begin(ProfilerState::FIXED_UPDATE);
                while (_accumulator >= fixedDelta) {
                    _accumulator -= fixedDelta;
                    onFixedUpdate(fixedDelta);
                }
                Profiler::end(ProfilerState::FIXED_UPDATE);

                Profiler::begin(ProfilerState::RENDERING);
                onRender(_dt);
                Profiler::end(ProfilerState::RENDERING);

                #ifdef ENGINE_DEBUG
                updateFps();
                #endif
            }

            GDE::Profiler::begin(ProfilerState::INPUT);
            window->update();
            GDE::Profiler::end(ProfilerState::INPUT);

            GDE::Profiler::endFrame();
            Renderer::resetDebugInfo();

            Uint64 _end = SDL_GetPerformanceCounter();
            float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
            _dt = _elapsedMS;
            timer += _dt;
        }
    }

    void Engine::onEvent(Event& _e) {
        EventDispatcher _ed(_e);
        _ed.dispatchEvent<WindowResizedEvent>(wreDel);
        imGuiLayer->onEvent(_e);
        manager.sceneManager.getDisplayedScene()->onEvent(_e);
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        Physics::get().step(_fixedDt);
        manager.sceneManager.getDisplayedScene()->onFixedUpdate(_fixedDt);
    }

    void Engine::onUpdate(Delta _dt) {
        manager.sceneManager.getDisplayedScene()->getMainCamera()->getViewport()->update(window->getWindowSize());
        manager.sceneManager.getDisplayedScene()->onUpdate(_dt);

        if(manager.inputManager.isKeyJustPressed(KeyCode::F9)) imGuiLayer->show = !imGuiLayer->show;
    }

    void Engine::onRender(Delta _dt) {
//        frameBuffer->bind();
        manager.sceneManager.getDisplayedScene()->onRender(_dt);
//        frameBuffer->unbind();

        manager.sceneManager.getDisplayedScene()->onDebugRender(_dt);

        Profiler::begin(ProfilerState::IMGUI);
        #if !IS_MOBILE()
        imGuiLayer->begin();
        manager.sceneManager.getDisplayedScene()->onImGuiRender(_dt);
        imGuiLayer->drawDebugInfo(manager.sceneManager.getDisplayedScene()->getMainGraph());
        imGuiLayer->end();
        #endif
        Profiler::end(ProfilerState::IMGUI);
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);

        frameBuffer->resize(_width, _height);
        manager.sceneManager.getDisplayedScene()->getMainCamera()->onResize(_width, _height);
        return true;
    }

    int Engine::getFps() const { return (int)fpsCounter; }

    void Engine::updateFps() {
        if (timer >= 1.f) {
            fpsCounter = frameCounter;
            window->setTitle("Engine: " + std::to_string(fpsCounter));
            frameCounter = 0;
            timer = 0;
            LOG_I("FPS: ", fpsCounter)
        }
        ++frameCounter;
    }

    Logs Engine::changeColorConsoleCommand(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        Renderer::setClearColor(backgroundColor);
        return {"Changed color"};
    }

    void Engine::destroy() {
        manager.destroy();

        delete frameBuffer;
        #if !IS_MOBILE()
        delete imGuiLayer;
        delete window;
        #endif
    }

    Logs Engine::componentsCommands(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        Renderer::setClearColor(backgroundColor);
        return {"Changed color"};
    }

    Logs Engine::setParentCommand(const std::vector<std::string>& _args) {

        if(_args.size() != 2) return {"[error] expected 2 parameters A, the child, and B, the father"};

        auto _a = _args[0];
        auto _b = _args[1];

        try {
            auto _scene = manager.sceneManager.getDisplayedScene();
            _scene->getMainGraph()->setParent(_scene->getMainGraph()->getNode(_a), _scene->getMainGraph()->getNode(_b));
            return {APPEND_S("Set ", _b, " as parent of ", _a) };
        } catch (const std::runtime_error& _e) {
            return {APPEND_S("[error] '", _a, "' or '", _b, "' or both don't exist on the scene!") };
        }
    }

    float Engine::geFixedDelta() const {
        return fixedDelta;
    }

    void Engine::setFixedDelta(float _fixedDelta) {
        fixedDelta = _fixedDelta;
    }

}