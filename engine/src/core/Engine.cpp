// Created by borja on 24/12/21.

#include "core/Engine.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/render/elements/ShaderManager.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/render/window/input/Input.h"
#include "core/render/Renderer.h"
#include "core/render/elements/TextureAtlasManager.h"
#include "core/systems/physicsSystem/Physics.h"
#include "core/systems/ecsSystem/GDESystemManager.h"

namespace GDE {
    Engine *Engine::gameInstance = nullptr;

    Engine::Engine() {
        ENGINE_ASSERT(!Engine::gameInstance, "Application already exists!");
        Engine::gameInstance = this;
        window = Window::createWindow();

        UDelegate<void(Event&)> onEventDelegate;
        onEventDelegate.bind<&Engine::onEvent>(this);
        window->setEventCallback(onEventDelegate);
    }

    void Engine::onInit() {
        lastFrame = 0;

        wreDel.bind<&Engine::onWindowResized>(this);

        FontManager::get().init();
        InputManager::get().init(window.get());
        Console::get().init();
        ShaderManager::get().init();
        Renderer::init();
        SoundManager::get().init();
        Physics::get().init();

        Renderer::setClearColor(backgroundColor);

#if !IS_MOBILE()
        imGuiLayer = new ImGuiScene();
        imGuiLayer->onInit();
#endif

        if(timePerFrame < 0)
            timePerFrame = 1.0f / 60.f;

        window->setVSync(false);

        FrameBufferSpecification _specs = {
                (uint32_t)window->getWindowSize().x,
                (uint32_t)window->getWindowSize().y
        };

        frameBuffer = new FrameBuffer(_specs);

        Console::get().addCommand<&Engine::changeColorConsoleCommand>("background_color",
                                                                      "Changes background color 0 <= r,b,g,a <= 255",
                                                                      this,
                                                                      "r g b a");

        Console::get().addCommand<&Engine::setParentCommand>( "parent_set",
                                                              "Sets the parent of A as B",
                                                              this,
                                                              "A B");
    }

    void Engine::onRun() {
        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);
        scene->getMainCamera()->onResize(_width, _height);

        float _accumulator = 0;

        Delta _dt = 0;
        while (running) {
            Uint64 _start = SDL_GetPerformanceCounter();
            _accumulator += _dt;

            GDE::Profiler::beginFrame(_dt);

            InputManager::get().pollEvents();

            if (!minimized) {

                Profiler::begin(ProfilerState::UPDATE);
                onUpdate(_dt);
                Profiler::end(ProfilerState::UPDATE);

                Profiler::begin(ProfilerState::FIXED_UPDATE);
                while (_accumulator >= timePerFrame) {
                    _accumulator -= timePerFrame;
                    onFixedUpdate(timePerFrame);
                }
                Profiler::end(ProfilerState::FIXED_UPDATE);

                Profiler::begin(ProfilerState::RENDERING);
                onRender(_dt);
                Profiler::end(ProfilerState::RENDERING);

                onDebugRender(_dt);

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
        scene->onEvent(_e);
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        scene->onFixedUpdate(_fixedDt);
        Physics::get().step(_fixedDt);
    }

    void Engine::onUpdate(Delta _dt) {
        scene->getMainCamera()->getViewport()->update(getWindowSize());
        scene->onUpdate(_dt);

        if(InputManager::isKeyJustPressed(KeyCode::F9))
            showImGuiDebugWindow = !showImGuiDebugWindow;
    }

    void Engine::onRender(Delta _dt) {
        frameBuffer->bind();
        {
            Renderer::clear();
            Renderer::beginDraw(*scene->getMainCamera());
            scene->onRender(_dt);
            Renderer::endDraw();
        }
        frameBuffer->unbind();

        // Imgui rendering
        Profiler::begin(ProfilerState::IMGUI);
        #if !IS_MOBILE()
        imGuiLayer->begin();
        scene->onImGuiRender(_dt);

        if (showImGuiDebugWindow)
            imGuiLayer->drawDebugInfo(scene->getMainGraph());

        imGuiLayer->end();
        #endif
        Profiler::end(ProfilerState::IMGUI);
    }

    void Engine::onDebugRender(Delta _dt) {
        Renderer::beginDebugDraw(*scene->getMainCamera());
        scene->onDebugRender(_dt);
        Renderer::endDebugDraw();
    }

    bool Engine::onWindowClosed(WindowClosedEvent &_e) {
        running = false;
        return true;
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);
        frameBuffer->resize(_width, _height);
        scene->getMainCamera()->onResize(_width, _height);
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

    Logs Engine::changeColorConsoleCommand(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        Renderer::setClearColor(backgroundColor);
        return {"Changed color"};
    }

    void Engine::destroy() {
        TextureAtlasManager::get().destroy();
        ShaderManager::get().destroy();
        FontManager::get().destroy();
        SoundManager::get().destroy();
        Renderer::destroy();
        InputManager::get().destroy();
        Physics::get().destroy();
        GDESystemManager::get().destroy();

        delete frameBuffer;
        delete scene;
        #if !IS_MOBILE()
        delete imGuiLayer;
        #endif
    }

    void Engine::setScene(Scene* _scene) {
        if(scene != nullptr) scene->onEnd();
        scene = _scene;
        if(scene != nullptr) scene->onInit();
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
            scene->getMainGraph()->setParent(scene->getMainGraph()->getNode(_a), scene->getMainGraph()->getNode(_b));
            return {APPEND_S("Set ", _b, " as parent of ", _a) };
        } catch (const std::runtime_error& _e) {
            return {APPEND_S("[error] '", _a, "' or '", _b, "' or both don't exist on the scene!") };
        }
    }

}