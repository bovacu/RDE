#include "ImGuiLayer.h"

#include "core/Engine.h"
#include "core/render/window/event/MouseEvent.h"

namespace engine {
    std::unordered_map<ProfilerState, RollingBuffer> ImGuiLayer::plotBuffers;

    ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {  }

    void ImGuiLayer::onInit() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Engine& _game = Engine::get();
        GLFWwindow* window = static_cast<GLFWwindow*>(_game.getWindow().getNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");

        for(auto& _state : State::stateToNameDict) {
            plotBuffers[_state.first] = {};
        }
    }

    void ImGuiLayer::onEvent(Event& _e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(ImGuiLayer::onMouseScrolled));
        dispatcher.dispatchEvent<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(ImGuiLayer::onMouseClicked));
        dispatcher.dispatchEvent<MouseMovedEvent>(ENGINE_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
    }

    void ImGuiLayer::onEnd() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end() {
        ImGuiIO& io = ImGui::GetIO();
        Engine& _game = Engine::get();
        io.DisplaySize = ImVec2((float)_game.getWindow().getWidth(), (float)_game.getWindow().getHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::drawDebugInfo() {
        ImGui::SetNextWindowSize({(float)Engine::get().getWindowSize().x, (float)Engine::get().getWindowSize().y});
        ImGui::Begin("Master", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        ImGui::End();

        static bool _vsync = Engine::get().isVSync(), _fullscreen = false;
        static int _windowRes[2] = {(int) engine::Engine::get().getWindowSize().x,(int) engine::Engine::get().getWindowSize().y};

        std::string _windowResolution = std::to_string(Engine::get().getWindowSize().x) + "x" + std::to_string(Engine::get().getWindowSize().y);
        static const char* _resSelected = _windowResolution.c_str();

        ImGui::Begin("Debugging", nullptr, ImGuiWindowFlags_NoMove);

        if(ImGui::Checkbox("VSync Active", &_vsync))
            engine::Engine::get().getWindow().setVSync(_vsync);

        if(ImGui::Checkbox("Fullscreen", &_fullscreen))
            engine::Engine::get().getWindow().setFullscreen(_fullscreen);

        const char* _resolutions[] = { "2560x1440", "1920x1080", "1366x768", "1280x720", "1920x1200", "1680x1050",
                                       "1440x900" ,"1280x800" ,"1024x768" ,"800x600", "800x480","640x480", "320x240"
        };

        ImGui::Text("Resolution"); ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        if (ImGui::BeginCombo("##combo", _resSelected)){ // The second parameter is the label previewed before opening the combo. {
            for (auto & _resolution : _resolutions) {
                bool is_selected = (_resSelected == _resolution);
                if (ImGui::Selectable(_resolution, is_selected)) {
                    _resSelected = _resolution;
                    charToIntSize(std::string(_resolution), _windowRes);
                    engine::Engine::get().setWindowSize(_windowRes[0], _windowRes[1]);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();
        ImGui::Text("FPS: %d", engine::Engine::get().getFps());
        ImGui::Separator();
        ImGui::Text("X: %d, Y: %d", Input::getMousePosition().x, Input::getMousePosition().y);
        ImGui::Separator();
        int _freeGpuMb = 0;
        glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI,&_freeGpuMb);
        ImGui::Text("GPU free MBs: %.2f", (float)_freeGpuMb / 1000.f);
        ImGui::Separator();
        ImGui::Text("Draw Calls: %d", Renderer::drawCalls);

        ImGui::End();

        ImGui::Begin("Metrics", nullptr, ImGuiWindowFlags_NoMove);
        metrics();
        ImGui::End();
    }

    void ImGuiLayer::metrics() {
        static bool _capture = true;
        static float t = 0;
        t += ImGui::GetIO().DeltaTime;

        static float history = 20.0f;
        if(_capture) {
            for(auto& _state : State::stateToNameDict) {
                auto _s = Profiler::getStates()[_state.first];
                auto _diff = std::chrono::duration_cast<std::chrono::milliseconds>(_s.end - _s.init);
                plotBuffers[_state.first].AddPoint(t, (float)_diff.count());
                plotBuffers[_state.first].Span = history;
            }
        }

        if (ImPlot::BeginPlot("##Rolling", ImVec2(-1,125))) {
            ImPlot::SetupAxes(nullptr, "ms", ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxisLimits(ImAxis_X1,0,history, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1,0,50);

            for(auto& _state : State::stateToNameDict) {
                if(Profiler::getStates()[_state.first].active) {
                    auto _s = plotBuffers[_state.first];
                    ImPlot::PlotLine(State::stateToNameDict[_state.first].c_str(),&_s.Data[0].x, &_s.Data[0].y, _s.Data.size(), 0, 2 * sizeof(float));
                }
            }

            ImPlot::EndPlot();
        }
    }

    void ImGuiLayer::charToIntSize(const std::string& _size, int* _resolution) {
        size_t pos;
        std::string _token;
        std::string _delimiter = "x";
        pos = _size.find(_delimiter);
        int _width = atoi(_size.substr(0, pos).c_str());
        int _height = atoi( _size.substr(pos + _delimiter.length()).c_str());

        _resolution[0] = _width;
        _resolution[1] = _height;
    }

    bool ImGuiLayer::onMouseClicked(MouseButtonPressedEvent& _e) {
//        return ImGui::IsAnyItemHovered();
        return false;
    }

    bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& _e) {
//        return ImGui::IsAnyItemHovered();
        return false;
    }

    bool ImGuiLayer::onMouseScrolled(MouseScrolledEvent& _e) {
//        return ImGui::IsAnyItemHovered();
        return false;
    }

    void ImGuiLayer::mouseInfo() {
        ImGui::SetNextWindowSize({150, -1}, ImGuiCond_Once);
        ImGui::GetStyle().Alpha = 0.65;
        ImGui::Begin("MouseInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 1;
        ImGui::Text("X: %d, Y: %d", Input::getMousePosition().x, Input::getMousePosition().y);
        ImGui::End();
    }
}