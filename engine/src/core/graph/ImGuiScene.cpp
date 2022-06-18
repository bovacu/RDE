#include "core/Core.h"
#if !IS_MOBILE()

#include "imgui.h"
#include "core/graph/ImGuiScene.h"

#include "core/Engine.h"
#include "engine/include/core/systems/eventSystem/MouseEvent.h"
#include "core/systems/console/Console.h"
#include "implot.h"
#include "imgui_node_editor.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "core/graph/Graph.h"
#include "core/render/Renderer.h"
#include "core/systems/uiSystem/Canvas.h"

namespace GDE {
    std::unordered_map<ProfilerState, RollingBuffer> ImGuiScene::plotBuffers;
    namespace ed = ax::NodeEditor;
    static ed::EditorContext* g_Context = nullptr;
    static ImGuiContext* i_Context = nullptr;
    static ImPlotContext* p_Context = nullptr;
    imgui_addons::ImGuiFileBrowser file_dialog;

    ImGuiScene::ImGuiScene(Engine* _engine) : Scene(_engine, "ImGuiScene") {  }

    void ImGuiScene::onInit() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        i_Context = ImGui::CreateContext();
        p_Context = ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        auto* window = static_cast<SDL_Window*>(engine->getWindow().getNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplSDL2_InitForOpenGL(window, engine->getWindow().getContext());
        ImGui_ImplOpenGL3_Init("#version 410");

        for(auto& _state : State::stateToNameDict) {
            plotBuffers[_state.first] = {};
        }

        ed::Config config;
        config.SettingsFile = "Simple.json";
        g_Context = ed::CreateEditor(&config);
        ed::SetCurrentEditor(g_Context);

        mseDel.bind<&ImGuiScene::onMouseScrolled>(this);
        mbpeDel.bind<&ImGuiScene::onMouseClicked>(this);
        mmeDel.bind<&ImGuiScene::onMouseMovedEvent>(this);
    }

    void ImGuiScene::onEvent(Event& _e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<MouseScrolledEvent>(mseDel);
        dispatcher.dispatchEvent<MouseButtonPressedEvent>(mbpeDel);
        dispatcher.dispatchEvent<MouseMovedEvent>(mmeDel);
    }

    void ImGuiScene::onEnd() {
        ed::DestroyEditor(g_Context);
        ImPlot::DestroyContext(p_Context);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext(i_Context);
    }

    void ImGuiScene::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiScene::end() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)engine->getWindow().getWidth(), (float)engine->getWindow().getHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            SDL_Window* backup_current_window = engine->getWindow().getNativeWindow();
            SDL_GLContext backup_current_context = engine->getWindow().getContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }

    void ImGuiScene::drawDebugInfo(Scene* _scene) {
        if(!show) return;
        ImGui::Begin("Debugging");
        windowsHovered[0] = checkForFocus();
        printResolutionFullscreenAndVSync();
        ImGui::Separator();
        printAtlases();
        printFPSDrawCallsAndRAM();
        showFileExplorer();
        ImGui::End();
        console();

        // This part is a bit messy for the moment. Scene elements and canvas are separated in two different EnTT registries,
        // so I need to split how both are selected and shown and switch between one registry and the other, so that's why there
        // is selectedNode and selectedNodeCanvas, if one is used, the other is set as null, this way just one of the registries
        // is shown and everything works fine, and we reuse all the code just by using 2 switching variables.
        auto* _mainGraph = _scene->getMainGraph();
        hierarchy(_scene);

        if(selectedNode != NODE_ID_NULL) nodeComponents(_mainGraph, selectedNode);

        if(selectedNodeCanvas != NODE_ID_NULL)
            for(auto* _canvas : _scene->getCanvases()) {
                auto* _graph = _canvas->getGraph();
                nodeComponents(_graph, selectedNodeCanvas);
            }
    }

    void ImGuiScene::metrics() {

    }

    void ImGuiScene::charToIntSize(const std::string& _size, int* _resolution) {
        size_t pos;
        std::string _token;
        std::string _delimiter = "x";
        pos = _size.find(_delimiter);
        int _width = atoi(_size.substr(0, pos).c_str());
        int _height = atoi( _size.substr(pos + _delimiter.length()).c_str());

        _resolution[0] = _width;
        _resolution[1] = _height;
    }

    bool ImGuiScene::onMouseClicked(MouseButtonPressedEvent& _e) {
        return anyWindowHovered;
    }

    bool ImGuiScene::onMouseMovedEvent(MouseMovedEvent& _e) {
        return ImGui::IsAnyItemHovered();
    }

    bool ImGuiScene::onMouseScrolled(MouseScrolledEvent& _e) {
        auto _blockInput = false;
        for(auto _windowHovered : windowsHovered)
            _blockInput |= _windowHovered;
        return _blockInput;
    }

    void ImGuiScene::mouseInfo() {
        ImGui::SetNextWindowSize({150, -1}, ImGuiCond_Once);
        ImGui::GetStyle().Alpha = 0.65;
        ImGui::Begin("MouseInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 1;
        ImGui::Text("X: %f, Y: %f", engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
        ImGui::End();
    }

    void ImGuiScene::printFPSDrawCallsAndRAM() {
        static bool _showMetrics = false;

        ImGui::Text("FPS: %d", engine->getFps());
        ImGui::Separator();
        ImGui::Text("X: %f, Y: %f", engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
        ImGui::Separator();
//        int _freeGpuMb = 0;
//        #if !IS_MOBILE()
//        glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI,&_freeGpuMb);
//        #endif
//        ImGui::Text("GPU Used Memory: %.2f MBs", (float)_freeGpuMb / 1000.f);
        auto* _memData = Profiler::getTotalVirtualMemory();
        ImGui::Text("RAM Used: %.2f MBs", (float)_memData[1] / 1000.f);
        ImGui::Separator();
        ImGui::Text("Draw Calls: %d", Renderer::getDrawCalls());
        ImGui::Text("Total Triangles: %d", Renderer::getTotalTriangles());
        ImGui::Text("Total Images: %d", Renderer::getTotalTriangles() / 2);
        ImGui::Separator();
        if(ImGui::Button("Show Metrics")) {
            _showMetrics = !_showMetrics;
        }

        if(_showMetrics) metrics();
    }

    void ImGuiScene::printAtlases() {
        float _totalAtlasesSize = 0;
        for(auto& _ti : engine->manager.textureManager.getTexturesInfo())
            _totalAtlasesSize += _ti.kb;

        int _child = 1;
        if(ImGui::TreeNode((void*)(intptr_t)0, "Atlases -> %.2f KBs", _totalAtlasesSize)) {
            for(auto& _ti : engine->manager.textureManager.getTexturesInfo()) {
                if(ImGui::TreeNode((void*)(intptr_t)_child, "%s", _ti.name)) {
                    ImGui::Text("Kb: %.2f", _ti.kb);
                    ImGui::Text("Texture size: %dx%d", _ti.textureWidth, _ti.textureHeight);
                    ImGui::Text("Number of tiles: %d", _ti.numberOfTiles);
                    _child++;
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        float _totalFontsSize = 0;
        for(auto& _font : engine->manager.fontManager.getAllFonts())
            _totalFontsSize += _font->getTexture().getKb();

        ImGui::Separator();
        _child++;

        if(ImGui::TreeNode((void*)(intptr_t)_child, "Fonts -> %.2f KBs", _totalFontsSize)) {
            for(auto& _font : engine->manager.fontManager.getAllFonts()) {
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

    void ImGuiScene::printResolutionFullscreenAndVSync() {
        static bool _vsync = engine->getWindow().isVSyncActive(), _fullscreen = false;
        static int _windowRes[2] = {(int) engine->getWindow().getWindowSize().x, (int) engine->getWindow().getWindowSize().y};

        std::string _windowResolution = std::to_string(engine->getWindow().getWindowSize().x) + "x" + std::to_string(engine->getWindow().getWindowSize().y);
        static const char* _resSelected = _windowResolution.c_str();

        if(ImGui::Checkbox("VSync Active", &_vsync))
            engine->getWindow().setVSync(_vsync);

        if(ImGui::Checkbox("Fullscreen", &_fullscreen))
            engine->getWindow().setFullscreen(_fullscreen);

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
                    engine->getWindow().setWindowSize(_windowRes[0], _windowRes[1]);
                    WindowResizedEvent _e(_windowRes[0], _windowRes[1]);
                    engine->onEvent(_e);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    int ImGuiScene::consoleStub(ImGuiInputTextCallbackData* _data) {
        auto* console = (ImGuiScene*)_data->UserData;
        return console->consoleIntro(_data);
    }

    int ImGuiScene::consoleIntro(ImGuiInputTextCallbackData* _data) {
        switch (_data->EventFlag){
            case ImGuiInputTextFlags_CallbackHistory:{
                if (_data->EventKey == ImGuiKey_UpArrow){
                    _data->DeleteChars(0, _data->BufTextLen);
                    _data->InsertChars(0, engine->manager.consoleManager.getUpCommand().c_str());
                }
                else if (_data->EventKey == ImGuiKey_DownArrow) {
                    _data->DeleteChars(0, _data->BufTextLen);
                    _data->InsertChars(0, engine->manager.consoleManager.getDownCommand().c_str());
                }
            }
        }
        return 0;
    }

    void ImGuiScene::console() {
        static bool autoscroll = true;
        static bool scrollToBottom = false;

        ImGui::Begin("Console");
        windowsHovered[2] = checkForFocus();
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
            auto _result = engine->manager.consoleManager.call(_command);
            for(auto& _r : _result)
                engine->manager.consoleManager.logs.push_back(_r);

            reclaim_focus = true;
            strcpy(_input, "");
        }

        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        windowsHovered[3] = checkForFocus();
        auto wPos = ImGui::GetWindowPos();
        auto wSize = ImGui::GetWindowSize();
        ImGui::GetWindowDrawList()->AddRect(wPos, { wPos.x + wSize.x, wPos.y + wSize.y }, ImColor(1.f, 1.f, 1.f, 1.f));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        ImGui::Indent(2.5f);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        for (int i = 0; i < engine->manager.consoleManager.logs.size(); i++) {
            const char* item = engine->manager.consoleManager.logs[i].c_str();

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

    void ImGuiScene::showFileExplorer() {
        bool open = false, save = false;
        if(ImGui::Button("Open File Manager"))
            open = true;

        //Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
        if(open)
            ImGui::OpenPopup("Open File");

        /* Optional third parameter. Support opening only compressed rar/zip files.
         * Opening any other file will show error, return false and won't close the dialog.
         */
        if(file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".rar,.zip,.7z")) {
            std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
            std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
        }
    }



    void ImGuiScene::hierarchyRecursionStub(Graph* _graph, NodeID _node, NodeID& _selectedNode) {
        auto* _transform = _graph->getComponent<Transform>(_node);
        auto* _tag = _graph->getComponent<Tag>(_node);

        if(!_transform->children.empty()) {

            auto _flags = _node == _selectedNode ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
            if (ImGui::TreeNodeEx(_tag->tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

                if(ImGui::IsItemClicked()) {
                    _selectedNode = _node;
                    if(&selectedNode == &_selectedNode) selectedNodeCanvas = NODE_ID_NULL;
                    else selectedNode = NODE_ID_NULL;
                }

                for(auto _child : _transform->children) {
                    hierarchyRecursionStub(_graph, _child, _selectedNode);
                }

                ImGui::TreePop();
            }
        } else {
            if (ImGui::Selectable(_tag->tag.c_str(), _selectedNode == _node)) {
                _selectedNode = _node;
                if(&selectedNode == &_selectedNode) selectedNodeCanvas = NODE_ID_NULL;
                else selectedNode = NODE_ID_NULL;
            }
        }
    }

    void ImGuiScene::hierarchy(Scene* _scene) {
        ImGui::Begin("Hierarchy");
        windowsHovered[1] = checkForFocus();
        auto* _graph = _scene->getMainGraph();
        hierarchyRecursionStub(_graph, _graph->getID(), selectedNode);
        for(auto* _canvas : _scene->getCanvases()) {
            _graph = _canvas->getGraph();
            hierarchyRecursionStub(_graph, _graph->getID(), selectedNodeCanvas);
        }
        ImGui::End();
    }

    void ImGuiScene::nodeComponents(Graph* _graph, const NodeID _selectedNode) {
        if(_selectedNode == NODE_ID_NULL) return;

        ImGui::Begin("Components");

        activeComponent(_graph, _selectedNode);
        tagComponent(_graph, _selectedNode);
        transformComponent(_graph, _selectedNode);
        cameraComponent(_graph, _selectedNode);
        spriteComponent(_graph, _selectedNode);
        bodyComponent(_graph, _selectedNode);
        textComponent(_graph, _selectedNode);

        ImGui::End();
    }

    void ImGuiScene::activeComponent(Graph* _graph, const NodeID _selectedNode) {
        bool _active = _graph->hasComponent<Active>(_selectedNode);
        auto _tag = _graph->getComponent<Tag>(_selectedNode)->tag.c_str();
        ImGui::Text("%s", _tag);
        ImGui::SameLine(0, ImGui::GetWindowWidth() - ImGui::CalcTextSize(_tag).x - 30);
        ImGui::PushID(1);
        if(ImGui::Checkbox("###Active", &_active)) {
            if(_active) _graph->addComponent<Active>(_selectedNode);
            else _graph->removeComponent<Active>(_selectedNode);
        }
        ImGui::PopID();
    }

    void ImGuiScene::tagComponent(Graph* _graph, const NodeID _selectedNode) {
        ImGui::Text("Tag"); ImGui::SameLine();
        char _buffer[256] = { 0 };
        auto& _tag = _graph->getComponent<Tag>(_selectedNode)->tag;
        strcpy(_buffer, _tag.c_str());
        if(ImGui::InputText("###tagName", _buffer, 256)) {
            _tag = std::string(_buffer);
        }
        ImGui::Separator();
    }

    void ImGuiScene::transformComponent(Graph* _graph, const NodeID _selectedNode) {
        auto _transform = _graph->getComponent<Transform>(_selectedNode);

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);
            ImGui::Text("Position ");

            float _pos[2] = {_transform->getPositionLocal().x, _transform->getPositionLocal().y};
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(1);
            if(ImGui::DragFloat2("##myInput", _pos, 0.5f)) {
                _transform->setPosition(_pos[0], _pos[1]);
            }
            ImGui::PopID();


            ImGui::Text("Rotation ");

            float _angle = _transform->getRotationLocal();
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::PushID(2);
            if(ImGui::DragFloat("##myInput", &_angle, 0.1f))
                _transform->setRotation(_angle);
            ImGui::PopID();


            ImGui::Text("Scale ");

            float _scale[2] = {_transform->getScaleLocal().x, _transform->getScaleLocal().y};
            ImGui::SameLine(0, 30);
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(3);
            if(ImGui::DragFloat2("##myInput", _scale, 0.05))
                _transform->setScale(_scale[0], _scale[1]);
            ImGui::PopID();
            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();

        }
    }

    void ImGuiScene::cameraComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<Camera>(_selectedNode)) return;

        auto _camera = _graph->getComponent<Camera>(_selectedNode);

        if(ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);

            const char* _viewports[] = { "Free Aspect", "Adaptative Aspect"};

            std::string _viewPortSelected;
            auto* _mainCamera = engine->manager.sceneManager.getDisplayedScene()->getMainCamera();

            if(dynamic_cast< FreeViewPort* >(_mainCamera->getViewport())) _viewPortSelected = "Free Aspect";
            else _viewPortSelected = "Adaptative Aspect";

            ImGui::Text("ViewPort"); ImGui::SameLine();
            ImGui::SetNextItemWidth(175);
            if (ImGui::BeginCombo("##combo", _viewPortSelected.c_str())){ // The second parameter is the label previewed before opening the combo. {
                for (auto & _resolution : _viewports) {
                    bool is_selected = (_viewPortSelected == _resolution);
                    if (ImGui::Selectable(_resolution, is_selected)) {
                        _viewPortSelected = _resolution;
                        if(_viewPortSelected == "Free Aspect") _mainCamera->setFreeViewport(engine->getWindow().getWindowSize());
                        else _mainCamera->setAdaptiveViewport({1920, 1080}, engine->getWindow().getWindowSize());
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if(_viewPortSelected == "Adaptative Aspect") {
                ImGui::Text("Virtual Resolution");
                int _pos[2] = {_mainCamera->getViewport()->getVirtualResolution().x, _mainCamera->getViewport()->getVirtualResolution().y};
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::PushID(2);
                if(ImGui::InputInt2("##myInput", _pos)) {
                    _mainCamera->getViewport()->updateVirtualResolution({_pos[0], _pos[1]});
                }
                ImGui::PopID();
            }

            ImGui::Text("Zoom Level");
            float _zoomLevel[1] = {_camera->getCurrentZoomLevel()};
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(1);
            if(ImGui::DragFloat("##myInput", _zoomLevel, 0.5f)) {
                _mainCamera->setCurrentZoomLevel(_zoomLevel[0]);
            }
            ImGui::PopID();

            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::bodyComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<Body>(_selectedNode)) return;

        auto _body = _graph->getComponent<Body>(_selectedNode);

        if(ImGui::CollapsingHeader("Body", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);

            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::spriteComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<SpriteRenderer>(_selectedNode)) return;

        auto _spriteRenderer = _graph->getComponent<SpriteRenderer>(_selectedNode);

        if(ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);
            ImGui::Text("Texture"); ImGui::SameLine();
            auto _texturePath = _spriteRenderer->getTexturePath();
            ImGui::BeginDisabled(true);
            ImGui::InputText("###texture", const_cast<char*>(_texturePath.c_str()), _texturePath.size());
            ImGui::EndDisabled();

            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::textComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<TextRenderer>(_selectedNode)) return;

        auto _body = _graph->getComponent<TextRenderer>(_selectedNode);

        if(ImGui::CollapsingHeader("Text Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);

            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();
        }
    }

    bool ImGuiScene::checkForFocus() {
        return ImGui::IsWindowHovered() | ImGui::IsAnyItemActive() | ImGui::IsAnyItemHovered() | ImGui::IsAnyItemFocused();
    }
}

#endif