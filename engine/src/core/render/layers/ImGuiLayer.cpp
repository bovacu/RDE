#include "core/Core.h"
#if !IS_MOBILE()

#include <imgui_internal.h>
#include "core/render/layers/ImGuiLayer.h"

#include "core/Engine.h"
#include "core/render/window/event/MouseEvent.h"
#include "core/systems/console/Console.h"

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
        auto* window = static_cast<SDL_Window*>(_game.getWindow().getNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window, _game.getWindow().getContext());
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
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
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
            SDL_Window* backup_current_window = _game.getWindow().getNativeWindow();
            SDL_GLContext backup_current_context = _game.getWindow().getContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }

    void ImGuiLayer::drawDebugInfo() {
        ImGui::Begin("Debugging");
        printResolutionFullscreenAndVSync();
        ImGui::Separator();
        printAtlases();
        printFPSDrawCallsAndRAM();
        anyWindowHovered = ImGui::IsWindowHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemFocused();
        ImGui::End();
        console();
    }

    void ImGuiLayer::metrics() {
        ImGui::Begin("Metrics");
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
        ImGui::End();
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
        return anyWindowHovered;
    }

    bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& _e) {
        return ImGui::IsAnyItemHovered();
    }

    bool ImGuiLayer::onMouseScrolled(MouseScrolledEvent& _e) {
        return anyWindowHovered;
    }

    void ImGuiLayer::mouseInfo() {
        ImGui::SetNextWindowSize({150, -1}, ImGuiCond_Once);
        ImGui::GetStyle().Alpha = 0.65;
        ImGui::Begin("MouseInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 1;
        ImGui::Text("X: %f, Y: %f", InputManager::getMousePosition().x, InputManager::getMousePosition().y);
        ImGui::End();
    }

    void ImGuiLayer::printFPSDrawCallsAndRAM() {
        static bool _showMetrics = false;

        ImGui::Text("FPS: %d", engine::Engine::get().getFps());
        ImGui::Separator();
        ImGui::Text("X: %f, Y: %f", InputManager::getMousePosition().x, InputManager::getMousePosition().y);
        ImGui::Separator();
        int _freeGpuMb = 0;
        #if !IS_MOBILE()
        glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI,&_freeGpuMb);
        #endif
        ImGui::Text("GPU free Memory: %.2f MBs", (float)_freeGpuMb / 1000.f);
        auto* _memData = Profiler::getTotalVirtualMemory();
        ImGui::Text("RAM Used: %.2f MBs", (float)_memData[1] / 1000.f);
        ImGui::Separator();
        ImGui::Text("Draw Calls: %d", Renderer::getDrawCalls());
        ImGui::Text("Total Triangles: %d", Renderer::getTotalTriangles());
        ImGui::Separator();
        if(ImGui::Button("Show Metrics")) {
            _showMetrics = !_showMetrics;
        }

        if(_showMetrics) metrics();
    }

    void ImGuiLayer::printAtlases() {
        float _totalAtlasesSize = 0;
        for(auto& _ti : TextureAtlasManager::get().getTexturesInfo())
            _totalAtlasesSize += _ti.kb;

        int _child = 1;
        if(ImGui::TreeNode((void*)(intptr_t)0, "Atlases -> %.2f KBs", _totalAtlasesSize)) {
            for(auto& _ti : TextureAtlasManager::get().getTexturesInfo()) {
                if(ImGui::TreeNode((void*)(intptr_t)_child, "%s", _ti.name)) {
                    ImGui::Text("Kb: %.2f", _ti.kb);
                    ImGui::Text("Texture size: %dx%d", _ti.textureWidth, _ti.textureHeight);
                    ImGui::Text("Number of tiles: %d", _ti.numberOfTiles);
                    ImGui::Text("Tile size: %dx%d", _ti.tileWidth, _ti.tileHeight);
                    _child++;
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        float _totalFontsSize = 0;
        for(auto& _font : FontManager::get().getAllFonts())
            _totalFontsSize += _font->getTexture().getKb();

        ImGui::Separator();
        _child++;

        if(ImGui::TreeNode((void*)(intptr_t)_child, "Fonts -> %.2f KBs", _totalFontsSize)) {
            for(auto& _font : FontManager::get().getAllFonts()) {
                if(ImGui::TreeNode((void*)(intptr_t)_child, "%s(font size %d)", _font->getFontName().c_str(), _font->getFontSize())) {
                    ImGui::Text("Kb: %.2f", _font->getTexture().getKb());
                    ImGui::Text("Texture size: %dx%d", _font->getTexture().getSize().x, _font->getTexture().getSize().y);
                    _child++;
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        ImGui::Separator();
    }

    void ImGuiLayer::printResolutionFullscreenAndVSync() {
        static bool _vsync = Engine::get().isVSync(), _fullscreen = false;
        static int _windowRes[2] = {(int) engine::Engine::get().getWindowSize().x,(int) engine::Engine::get().getWindowSize().y};

        std::string _windowResolution = std::to_string(Engine::get().getWindowSize().x) + "x" + std::to_string(Engine::get().getWindowSize().y);
        static const char* _resSelected = _windowResolution.c_str();

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
                    WindowResizedEvent _e(_windowRes[0], _windowRes[1]);
                    engine::Engine::get().onEvent(_e);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    int ImGuiLayer::consoleStub(ImGuiInputTextCallbackData* _data) {
        auto* console = (ImGuiLayer*)_data->UserData;
        return console->consoleIntro(_data);
    }

    int ImGuiLayer::consoleIntro(ImGuiInputTextCallbackData* _data) {
        switch (_data->EventFlag){
            case ImGuiInputTextFlags_CallbackHistory:{
                if (_data->EventKey == ImGuiKey_UpArrow){
                    _data->DeleteChars(0, _data->BufTextLen);
                    _data->InsertChars(0, Console::get().getUpCommand().c_str());
                }
                else if (_data->EventKey == ImGuiKey_DownArrow) {
                    _data->DeleteChars(0, _data->BufTextLen);
                    _data->InsertChars(0, Console::get().getDownCommand().c_str());
                }
            }
        }
        return 0;
    }

    void ImGuiLayer::console() {
        static bool autoscroll = true;
        static bool scrollToBottom = false;

        ImGui::Begin("Console");

        static char _input[256];
        bool reclaim_focus = false;
        ImGuiInputTextFlags _inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;

        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Command").x * 1.5f);
        if(ImGui::InputText("Command", _input, IM_ARRAYSIZE(_input), _inputTextFlags, &consoleStub, (void*)this)) {
            std::stringstream _ss(_input);
            std::vector<std::string> _splits;

            std::string _s;
            while (std::getline(_ss, _s, ' '))
                _splits.push_back(_s);

            if(_splits.empty()) return;

            Command _command;
            _command.name = _splits[0];
            _command.arguments = std::vector<std::string>(_splits.begin() + 1, _splits.end());
            auto _result = Console::get().call(_command);
            for(auto& _r : _result)
                Console::get().logs.push_back(_r);

            reclaim_focus = true;
            strcpy(_input, "");
        }

        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        auto wPos = ImGui::GetWindowPos();
        auto wSize = ImGui::GetWindowSize();
        ImGui::GetWindowDrawList()->AddRect(wPos, { wPos.x + wSize.x, wPos.y + wSize.y }, ImColor(1.f, 1.f, 1.f, 1.f));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        ImGui::Indent(2.5f);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        for (int i = 0; i < Console::get().logs.size(); i++) {
            const char* item = Console::get().logs[i].c_str();

            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strstr(item, "#")) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        ImGui::Unindent(2.5f);
        if (scrollToBottom || (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;

        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::End();
    }

}

#endif