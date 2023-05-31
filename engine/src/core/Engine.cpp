// Created by borja on 24/12/21.

#include "core/Engine.h"
#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/systems/ecsSystem/ECSManager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/systems/profiling/Profiler.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/util/Functions.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/graph/ImGuiScene.h"
#include "core/systems/eventSystem/WindowEvent.h"
#include "core/render/window/Window.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/SceneManager.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

namespace RDE {


    Engine::Engine() {
        manager.configManager.loadRDEConfig(&rdeConfig, &manager.fileManager);
        window = platform.createWindow(this, &rdeConfig);
        
        #if IS_MOBILE()
        rdeConfig.windowData.resolution = window->getWindowSize();
        #endif
        
        currentDPI = rdeConfig.windowData.diagonalDpi;

        UniqueDelegate<void(Event&)> onEventDelegate;
        onEventDelegate.bind<&Engine::onEvent>(this);
        window->setEventCallback(onEventDelegate);

        manager.init(this);
    }

    void Engine::onInit(Scene* _scene) {
        wreDel.bind<&Engine::onWindowResized>(this);

        #if !IS_MOBILE()
        imGuiLayer = new ImGuiScene(this);
        imGuiLayer->onInit();
        #endif

        fixedDelta = 1.0f / 60.f;
        window->setVSync(true);
        manager.renderManager.setClearColor(backgroundColor);
		
        manager.configManager.loadResources(&rdeConfig, &manager);

        manager.consoleManager.addCommand<&Engine::changeColorConsoleCommand>("background_color"," Changes background color 0 <= r,b,g,a <= 255", this, "r g b a");
        manager.consoleManager.addCommand<&Engine::setParentCommand>( "parent_set", "Sets the parent of A as B", this, "A B");

        manager.sceneManager.loadScene(_scene, _scene->getName());
        manager.sceneManager.displayScene(_scene->getName());
		imGuiLayer->show = true;
    }

    void Engine::loopCode() {
        Uint64 _start = SDL_GetPerformanceCounter();
        accumulator += dt;

        if(manager.sceneManager.getDisplayedScene() == nullptr) return;
        Profiler::beginFrame(dt);
        manager.inputManager.pollEvents();

        if (window->shouldUpdateWindow()) {
            Profiler::begin(ProfilerState::UPDATE);
            onUpdate(dt);
            Profiler::end(ProfilerState::UPDATE);

            Profiler::begin(ProfilerState::FIXED_UPDATE);
            while (accumulator >= fixedDelta) {
                accumulator -= fixedDelta;
                onFixedUpdate(fixedDelta);
            }
            Profiler::end(ProfilerState::FIXED_UPDATE);


            Profiler::begin(ProfilerState::LATE_UPDATE);
            onLateUpdate(dt);
            Profiler::end(ProfilerState::LATE_UPDATE);

            Profiler::begin(ProfilerState::RENDERING);
            onRender(dt);
            Profiler::end(ProfilerState::RENDERING);

			manager.inputManager.syncEvents();

            Profiler::begin(ProfilerState::INPUT);
            window->update();
            Profiler::end(ProfilerState::INPUT);
        }

        Profiler::endFrame();
        manager.renderManager.resetDebugInfo();

        Uint64 _end = SDL_GetPerformanceCounter();
        float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
        dt = _elapsedMS;
    }

    void Engine::onRun() {
        accumulator = 0;
        dt = 0;
        #ifdef __EMSCRIPTEN__
            loopCode();
        #else
        while (window->isRunning()) {
            loopCode();
        }
        #endif
    }

    void Engine::onEvent(Event& _e) {
        EventDispatcher _ed(_e);
        _ed.dispatchEvent<WindowResizedEvent>(wreDel);
        #if !IS_MOBILE()
        imGuiLayer->onEvent(_e);
        #endif
        manager.sceneManager.getDisplayedScene()->onInnerEvent(_e);
    }

    void Engine::onUpdate(Delta _dt) {
        manager.physics.update(_dt);
        manager.sceneManager.getDisplayedScene()->onInnerUpdate(_dt);

#if !IS_MOBILE()
		if(manager.inputManager.isKeyJustPressed(RDE_KEYBOARD_KEY_GRACE_ACCENT)) imGuiLayer->show = !imGuiLayer->show;
#endif
    }

    void Engine::onFixedUpdate(Delta _fixedDt) {
        manager.sceneManager.getDisplayedScene()->onInnerFixedUpdate(_fixedDt);
        manager.physics.step(_fixedDt);
    }

    void Engine::onLateUpdate(Delta _dt) {
        manager.sceneManager.getDisplayedScene()->onInnerLateUpdate(_dt);
    }

    void Engine::onRender(Delta _dt) {
		for(auto& _pair : manager.renderManager.framebuffers) {
			_pair.second->bind();
			manager.renderManager.clear();
			manager.sceneManager.getDisplayedScene()->onInnerRender(_dt, _pair.second);
			manager.sceneManager.getDisplayedScene()->onInnerDebugRender(_dt, _pair.second);
			_pair.second->unbind();
		}

		manager.renderManager.clearBatches();

        Profiler::begin(ProfilerState::IMGUI);
        #if !IS_MOBILE()
        imGuiLayer->begin();

        manager.sceneManager.getDisplayedScene()->onImGuiRender(_dt);
        imGuiLayer->drawDebugInfo(manager.sceneManager.getDisplayedScene());

        imGuiLayer->end();

        #endif
        Profiler::end(ProfilerState::IMGUI);
    }

    bool Engine::onWindowResized(WindowResizedEvent &_e) {
        int _width, _height;
        SDL_GL_GetDrawableSize(window->getNativeWindow(), &_width, &_height);

		manager.renderManager.onResize(_width, _height);
		manager.sceneManager.getDisplayedScene()->mainCamera->onResize(_width, _height);
		manager.sceneManager.getDisplayedScene()->updateRenderTree(true);
		manager.sceneManager.getDisplayedScene()->canvas->onResize(_width, _height);
		manager.sceneManager.getDisplayedScene()->canvas->setCanvasResolution( { _width, _height });

        return false;
    }

    int Engine::getFps() const { return (int)fpsCounter; }

    Logs Engine::changeColorConsoleCommand(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        manager.renderManager.setClearColor(backgroundColor);
        return {"Changed color"};
    }

    void Engine::destroy() {

        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        #endif

        manager.destroy();
        #if !IS_MOBILE() && !defined(__EMSCRIPTEN__)
        delete imGuiLayer;
        window->destroy();
        #endif
    }

    Logs Engine::componentsCommands(const std::vector<std::string>& _args) {
        backgroundColor = {(unsigned char)std::stoi(_args[0]), (unsigned char)std::stoi(_args[1]),
                           (unsigned char)std::stoi(_args[2]), (unsigned char)std::stoi(_args[3])};
        manager.renderManager.setClearColor(backgroundColor);
        return {"Changed color"};
    }

    Logs Engine::setParentCommand(const std::vector<std::string>& _args) {

        if(_args.size() != 2) return {"[error] expected 2 parameters A, the child, and B, the father"};

        auto _a = _args[0];
        auto _b = _args[1];

        try {
            auto _scene = manager.sceneManager.getDisplayedScene();
            _scene->graph->setParent(_scene->graph->getNode(_a), _scene->graph->getNode(_b));
            return {Util::String::appendToString("Set ", _b, " as parent of ", _a) };
        } catch (const std::runtime_error& _e) {
            return {Util::String::appendToString("[error] '", _a, "' or '", _b, "' or both don't exist on the scene!") };
        }
    }

    float Engine::geFixedDelta() const {
        return fixedDelta;
    }

    void Engine::setFixedDelta(float _fixedDelta) {
        fixedDelta = _fixedDelta;
    }

    void Engine::setRenderingRedirection(UniqueDelegate<void(FrameBuffer*)>& _redirectionFunc) {
        redirectionFunc = _redirectionFunc;
    }

    void Engine::setRenderingRedirectionToImGui(UniqueDelegate<void(FrameBuffer*)>& _redirectionFunc) {
        imGuiRedirectionFunc = _redirectionFunc;
    }

	RDE_PLATFORM_TYPE_ Engine::getPlatform() const {
		return platform.currentPlatform;
	}
}
