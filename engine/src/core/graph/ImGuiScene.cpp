#include "core/Core.h"
#if !IS_MOBILE()

#include "imgui.h"
#include "core/graph/ImGuiScene.h"

#include "core/Engine.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/console/Console.h"
#include "core/graph/Graph.h"
#include "core/render/RenderManager.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UIPanel.h"
#include "core/graph/components/ui/UITransform.h"

namespace RDE {
    std::unordered_map<ProfilerState, RollingBuffer> ImGuiScene::plotBuffers;
    static ImGuiContext* i_Context = nullptr;

    static void helpMarker(const char* desc) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    ImGuiScene::ImGuiScene(Engine* _engine) : Scene(_engine, "ImGuiScene") {  }

    void ImGuiScene::onInit() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        i_Context = ImGui::CreateContext();
//        p_Context = ImPlot::CreateContext();
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

        mseDel.bind<&ImGuiScene::onMouseScrolled>(this);
        mbpeDel.bind<&ImGuiScene::onMouseClicked>(this);
        mmeDel.bind<&ImGuiScene::onMouseMovedEvent>(this);
    }

    void ImGuiScene::onEvent(Event& _e) {
        EventDispatcher dispatcher(_e);
        dispatcher.dispatchEvent<MouseScrolledEvent>(mseDel);
        dispatcher.dispatchEvent<MouseButtonPressedEvent>(mbpeDel);
//        dispatcher.dispatchEvent<MouseMovedEvent>(mmeDel);
    }

    void ImGuiScene::onEnd() {
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

        resetID();
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
         auto* _memData = Profiler::getTotalVirtualMemory();
         ImGui::Text("RAM Used: %.2f MBs", (float)_memData[1] / 1000.f);
        ImGui::Separator();
        auto [_drawCalls, _uiDrawCalls] = engine->manager.renderManager.getDrawCalls();
        ImGui::Text("Draw Calls: %d", _drawCalls);
        ImGui::Text("UI Draw Calls: %d", _uiDrawCalls);
        ImGui::Text("Total Triangles: %d", engine->manager.renderManager.getTotalTriangles());
        ImGui::Text("Total Images: %d", engine->manager.renderManager.getTotalTriangles() / 2);
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
                    ImGui::Text("Texture size: %fx%f", _font->getTexture().getSize().x, _font->getTexture().getSize().y);
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
            #if IS_WINDOWS()
            strcpy_s(_input, "");
            #else
            strcpy(_input, "");
            #endif
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

        for (size_t i = 0; i < engine->manager.consoleManager.logs.size(); i++) {
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

    }



    void ImGuiScene::hierarchyRecursionStub(Scene* _scene, Graph* _graph, Node* _node, NodeID& _selectedNode) {
        auto _prefabs = _scene->getPrefabs();
        if(std::find(_prefabs.begin(), _prefabs.end(), _node->getID()) != _prefabs.end()) return;

        auto* _transform =_node->getTransform();
        auto* _tag = _node->getComponent<Tag>();

        if(!_transform->children.empty()) {

            auto _flags = _node->getID() == _selectedNode ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
            if (ImGui::TreeNodeEx(_tag->tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

                if(ImGui::IsItemClicked()) {
                    _selectedNode = _node->getID();
                    if(&selectedNode == &_selectedNode) selectedNodeCanvas = NODE_ID_NULL;
                    else selectedNode = NODE_ID_NULL;
                }

                for(auto _child : _transform->children) {
                    hierarchyRecursionStub(_scene, _graph, _child->node, _selectedNode);
                }

                ImGui::TreePop();
            }
        } else {
            if (ImGui::Selectable(_tag->tag.c_str(), _selectedNode == _node->getID())) {
                _selectedNode = _node->getID();
                if(&selectedNode == &_selectedNode) selectedNodeCanvas = NODE_ID_NULL;
                else selectedNode = NODE_ID_NULL;
            }
        }
    }

    void ImGuiScene::hierarchy(Scene* _scene) {
        ImGui::Begin("Hierarchy");
        windowsHovered[1] = checkForFocus();
        auto* _graph = _scene->getMainGraph();
        hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), selectedNode);
        for(auto* _canvas : _scene->getCanvases()) {
            _graph = _canvas->getGraph();
            hierarchyRecursionStub(_scene, _graph, _graph->getRoot(), selectedNodeCanvas);
        }
        showLoadedPrefabs(_scene, _scene->getMainGraph(), _graph->getRoot(), selectedNode);
        ImGui::End();
    }

    void ImGuiScene::showLoadedPrefabs(Scene* _scene, Graph* _graph, Node* _node, NodeID& _selectedNode) {
        auto _flags = _node->getID() == _selectedNode ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
        if (ImGui::TreeNodeEx("Prefabs loaded in memory", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | _flags)) {

            auto _prefabs = _scene->getPrefabs();
            for(auto& _nodeID : _prefabs) {
                auto _tag = _graph->getComponent<Tag>(_nodeID);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", _tag->tag.c_str());
            }

            ImGui::TreePop();
        }
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

        uiTransformComponent(_graph, _selectedNode);
        uiButtonComponent(_graph, _selectedNode);
        ui9SliceComponent(_graph, _selectedNode);

        ImGui::End();
    }

    void ImGuiScene::activeComponent(Graph* _graph, const NodeID _selectedNode) {
        bool _active = _graph->hasComponent<Active>(_selectedNode);
        auto _tag = _graph->getComponent<Tag>(_selectedNode)->tag.c_str();
        ImGui::Text("%s", _tag);
        ImGui::SameLine(0, ImGui::GetWindowWidth() - ImGui::CalcTextSize(_tag).x - 40 - ImGui::CalcTextSize("(?)").x);
        ImGui::PushID(createID());
        if(ImGui::Checkbox("###Active", &_active)) {
            if(_active) _graph->addComponent<Active>(_selectedNode, _graph->getComponent<Node>(_selectedNode), &engine->manager, _graph);
            else _graph->removeComponent<Active>(_selectedNode);
        }
        helpMarker("This will set the Active property to true/false.\n Setting Active to false will make all of DisabledConfig elements to be disabled for the Node and children.");
        ImGui::PopID();
    }

    void ImGuiScene::tagComponent(Graph* _graph, const NodeID _selectedNode) {
        ImGui::Text("Tag"); ImGui::SameLine();
        char _buffer[256] = { 0 };
        auto& _tag = _graph->getComponent<Tag>(_selectedNode)->tag;
        #if IS_WINDOWS()
        strcpy_s(_buffer, _tag.c_str());
        #else
        strcpy(_buffer, _tag.c_str());
        #endif

        if(ImGui::InputText("###tagName", _buffer, 256)) {
            _tag = std::string(_buffer);
        }
        ImGui::Separator();
    }

    void ImGuiScene::disabledConfigComponent(Transform* _transform) {
        ImGui::NewLine();

        ImGui::Indent();
        if(ImGui::CollapsingHeader("Enabled States")) {
            auto _node = _transform->node;
            auto _flags = _node->getEnabledFlags();

            if(_transform->parentTransform->node->isEnabled()) {
                ImGui::PushID(createID());
                static bool _enabledForEvent = true;
                if(ImGui::Checkbox("Event", &_enabledForEvent)) {
                    if(_enabledForEvent) {
                        _node->setEnabled(_flags | EnabledStates::DS_EVENT);
                    } else {
                        _node->setEnabled(_flags & INVERSE_ENABLED_STATE(EnabledStates::DS_EVENT));
                    }
                }
                helpMarker("This will make the Node and children not to run the main Event function.");
                ImGui::PopID();

                ImGui::PushID(createID());
                static bool _enabledForUpdate = true;
                if(ImGui::Checkbox("Update", &_enabledForUpdate)) {
                    if(_enabledForUpdate) {
                        _node->setEnabled(_flags | EnabledStates::DS_UPDATE);
                    } else {
                        _node->setEnabled(_flags & INVERSE_ENABLED_STATE(EnabledStates::DS_UPDATE));
                    }
                }
                helpMarker("This will make the Node and children not to run the main Update function.");
                ImGui::PopID();

                ImGui::PushID(createID());
                static bool _enabledForFixedUpdate = true;
                if(ImGui::Checkbox("Fixed Update", &_enabledForFixedUpdate)) {
                    if(_enabledForFixedUpdate) {
                        _node->setEnabled(_flags | EnabledStates::DS_FIXED_UPDATE);
                    } else {
                        _node->setEnabled(_flags & INVERSE_ENABLED_STATE(EnabledStates::DS_FIXED_UPDATE));
                    }
                }
                helpMarker("This will make the Node and children not to run the main FixedUpdate function.");
                ImGui::PopID();

                ImGui::PushID(createID());
                static bool _enabledForRender = true;
                if(ImGui::Checkbox("Render", &_enabledForRender)) {
                    if(_enabledForRender) {
                        _node->setEnabled(_flags | EnabledStates::DS_RENDER);
                    } else {
                        _node->setEnabled(_flags & INVERSE_ENABLED_STATE(EnabledStates::DS_RENDER));
                    }
                }
                helpMarker("This will make the Node and children not to run the main Render function.");
                ImGui::PopID();

                ImGui::PushID(createID());
                static bool _enabledForLateUpdate = true;
                if(ImGui::Checkbox("Late Update", &_enabledForLateUpdate)) {
                    if(_enabledForLateUpdate) {
                        _node->setEnabled(_flags | EnabledStates::DS_LATE_UPDATE);
                    } else {
                        _node->setEnabled(_flags & INVERSE_ENABLED_STATE(EnabledStates::DS_LATE_UPDATE));
                    }
                }
                helpMarker("This will make the Node and children not to run the main LateUpdate function.");
                ImGui::PopID();
            }
        }
        ImGui::Unindent();
    }

    void ImGuiScene::transformComponent(Graph* _graph, const NodeID _selectedNode) {
        Transform* _transform = nullptr;
        if(!_graph->hasComponent<Transform>(_selectedNode)) return;
        _transform = _graph->getComponent<Transform>(_selectedNode);

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
            ImGui::Text("ID: %i", (int)_transform->node->getID());
            ImGui::Text("Position ");

            float _pos[2] = {_transform->getPosition().x, _transform->getPosition().y};
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(createID());
            if(ImGui::DragFloat2("##myInput", _pos, 0.5f)) {
                _transform->setPosition(_pos[0], _pos[1]);
            }
            ImGui::PopID();


            ImGui::Text("Rotation ");

            float _angle = _transform->getRotation();
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::PushID(createID());
            if(ImGui::DragFloat("##myInput", &_angle, 0.1f))
                _transform->setRotation(_angle);
            ImGui::PopID();


            ImGui::Text("Scale ");

            float _scale[2] = {_transform->getScale().x, _transform->getScale().y};
            ImGui::SameLine(0, 30);
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(createID());
            if(ImGui::DragFloat2("##myInput", _scale, 0.05))
                _transform->setScale(_scale[0], _scale[1]);
            ImGui::PopID();

            disabledConfigComponent(_transform);

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::cameraComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<Camera>(_selectedNode)) return;

        auto _camera = _graph->getComponent<Camera>(_selectedNode);

        if(ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

//            const char* _viewports[] = { "Free Aspect", "Adaptative Aspect"};
//
//            std::string _viewPortSelected;
//
//            if(dynamic_cast< FreeViewPort* >(_camera->getViewport())) _viewPortSelected = "Free Aspect";
//            else _viewPortSelected = "Adaptative Aspect";
//
//            ImGui::PushID(createID());
//            ImGui::Text("ViewPort"); ImGui::SameLine();
//            ImGui::SetNextItemWidth(175);
//            if (ImGui::BeginCombo("##combo", _viewPortSelected.c_str())){ // The second parameter is the label previewed before opening the combo. {
//                for (auto & _resolution : _viewports) {
//                    bool is_selected = (_viewPortSelected == _resolution);
//                    if (ImGui::Selectable(_resolution, is_selected)) {
//                        _viewPortSelected = _resolution;
//                        if(_viewPortSelected == "Free Aspect") _camera->setFreeViewport(engine->getWindow().getWindowSize());
//                        else _camera->setAdaptiveViewport({1920, 1080}, engine->getWindow().getWindowSize());
//                    }
//                    if (is_selected)
//                        ImGui::SetItemDefaultFocus();
//                }
//                ImGui::EndCombo();
//            }
//            ImGui::PopID();

            ImGui::Text("Zoom Level");
            float _zoomLevel[1] = {_camera->getCurrentZoomLevel()};
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(createID());
            if(ImGui::DragFloat("##myInput", _zoomLevel, 0.5f)) {
                _camera->setCurrentZoomLevel(_zoomLevel[0]);
            }
            ImGui::PopID();

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::bodyComponent(Graph* _graph, const NodeID _selectedNode) {
//        if(!_graph->hasComponent<Body>(_selectedNode)) return;
//
//        auto _body = _graph->getComponent<Body>(_selectedNode);
//
//        if(ImGui::CollapsingHeader("Body", ImGuiTreeNodeFlags_DefaultOpen)) {
//            if(_selectedNode == _graph->getID()) ImGui::BeginDisabled(true);
//
//            if(_selectedNode == _graph->getID()) ImGui::EndDisabled();
//        }
    }

    void ImGuiScene::spriteComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<SpriteRenderer>(_selectedNode)) return;

        auto _spriteRenderer = _graph->getComponent<SpriteRenderer>(_selectedNode);

        if(ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);
            ImGui::Text("Texture"); ImGui::SameLine();
            auto _texturePath = _spriteRenderer->getTexturePath();
            ImGui::BeginDisabled(true);
            ImGui::InputText("###texture", const_cast<char*>(_texturePath.c_str()), _texturePath.size());
            ImGui::EndDisabled();

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::textComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<TextRenderer>(_selectedNode)) return;

        auto _text = _graph->getComponent<TextRenderer>(_selectedNode);
        auto _textTransform = _graph->getComponent<Transform>(_selectedNode);

        if(ImGui::CollapsingHeader("Text Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::uiTransformComponent(Graph* _graph, const NodeID _selectedNode) {
        UITransform* _transform = nullptr;
        if(!_graph->hasComponent<UITransform>(_selectedNode)) return;
        _transform = _graph->getComponent<UITransform>(_selectedNode);
        Anchor _selectedAnchor = _transform->getAnchor();
        Stretch _selectedStretch = _transform->getStretch();

        if(ImGui::CollapsingHeader("UITransform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

            ImGui::Text("ID: %i", (int)_transform->node->getID());

            ImGui::Text("Anchor "); ImGui::SameLine(0, 20);
            ImGui::Text("Stretch ");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            ImVec4 _selectedColor = {204.f / 255.f, 204.f / 255.f, 0, 0.5f};
            ImVec4 _noSelectedColor = {0, 0, 0, 0};
            auto* _topLeft = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomLeft");
            auto* _topMiddle = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomMiddle");
            auto* _topRight = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorBottomRight");

            auto* _middleLeft = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleLeft");
            auto* _middle = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleMiddle");
            auto* _middleRight = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorMiddleRight");

            auto* _bottomLeft = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopLeft");
            auto* _bottomMiddle = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopMiddle");
            auto* _bottomRight = engine->manager.textureManager.getSubTexture("defaultAssets", "anchorTopRight");

            auto* _stretchNone = engine->manager.textureManager.getSubTexture("defaultAssets", "stretchNone");
            auto* _stretchHorizontal = engine->manager.textureManager.getSubTexture("defaultAssets", "stretchHorizontal");
            auto* _stretchVertical = engine->manager.textureManager.getSubTexture("defaultAssets", "stretchVertical");
            auto* _stretchFull = engine->manager.textureManager.getSubTexture("defaultAssets", "stretchFull");


            // Anchor top row
            auto _topLeftInit = Vec2F { _topLeft->getRegion().bottomLeftCorner.x / _topLeft->getSpriteSheetSize().x, _topLeft->getRegion().bottomLeftCorner.y / _topLeft->getSpriteSheetSize().y};
            auto _topLeftEnd = _topLeftInit + Vec2F { _topLeft->getRegion().size.x / _topLeft->getSpriteSheetSize().x, _topLeft->getRegion().size.y / _topLeft->getSpriteSheetSize().y};

            auto _topMiddleInit = Vec2F { _topMiddle->getRegion().bottomLeftCorner.x / _topMiddle->getSpriteSheetSize().x, _topMiddle->getRegion().bottomLeftCorner.y / _topMiddle->getSpriteSheetSize().y};
            auto _topMiddleEnd = _topMiddleInit + Vec2F { _topMiddle->getRegion().size.x / _topMiddle->getSpriteSheetSize().x, _topMiddle->getRegion().size.y / _topMiddle->getSpriteSheetSize().y};

            auto _topRightInit = Vec2F { _topRight->getRegion().bottomLeftCorner.x / _topRight->getSpriteSheetSize().x, _topRight->getRegion().bottomLeftCorner.y / _topRight->getSpriteSheetSize().y};
            auto _topRightEnd = _topRightInit + Vec2F { _topRight->getRegion().size.x / _topRight->getSpriteSheetSize().x, _topRight->getRegion().size.y / _topRight->getSpriteSheetSize().y};

            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topLeft->getGLTexture()), ImVec2(16, 16), {_topLeftInit.x, _topLeftInit.y}, {_topLeftEnd.x, _topLeftEnd.y}, 0, (_selectedAnchor == Anchor::LEFT_TOP ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::LEFT_TOP;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topMiddle->getGLTexture()), ImVec2(16, 16), {_topMiddleInit.x, _topMiddleInit.y}, {_topMiddleEnd.x, _topMiddleEnd.y}, 0, (_selectedAnchor == Anchor::TOP ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::TOP;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_topRight->getGLTexture()), ImVec2(16, 16), {_topRightInit.x, _topRightInit.y}, {_topRightEnd.x, _topRightEnd.y}, 0, (_selectedAnchor == Anchor::RIGHT_TOP ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::RIGHT_TOP;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();


            // Stretch top row
            ImGui::SameLine(0, 20);
            auto _stretchNoneInit = Vec2F { _stretchNone->getRegion().bottomLeftCorner.x / _stretchNone->getSpriteSheetSize().x, _stretchNone->getRegion().bottomLeftCorner.y / _stretchNone->getSpriteSheetSize().y};
            auto _stretchNoneEnd = _stretchNoneInit + Vec2F { _stretchNone->getRegion().size.x / _stretchNone->getSpriteSheetSize().x, _stretchNone->getRegion().size.y / _stretchNone->getSpriteSheetSize().y};

            auto _stretchFullInit = Vec2F { _stretchFull->getRegion().bottomLeftCorner.x / _stretchFull->getSpriteSheetSize().x, _stretchFull->getRegion().bottomLeftCorner.y / _stretchFull->getSpriteSheetSize().y};
            auto _stretchFullEnd = _stretchFullInit + Vec2F { _stretchFull->getRegion().size.x / _stretchFull->getSpriteSheetSize().x, _stretchFull->getRegion().size.y / _stretchFull->getSpriteSheetSize().y};

            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchNone->getGLTexture()), ImVec2(16, 16), {_stretchNoneInit.x, _stretchNoneInit.y}, {_stretchNoneEnd.x, _stretchNoneEnd.y}, 0, (_selectedStretch == Stretch::NO_STRETCH ? _selectedColor : _noSelectedColor))) {
                _selectedStretch = Stretch::NO_STRETCH;
                _transform->setStretch(_selectedStretch);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchFull->getGLTexture()), ImVec2(16, 16), {_stretchFullInit.x, _stretchFullInit.y}, {_stretchFullEnd.x, _stretchFullEnd.y}, 0, (_selectedStretch == Stretch::FULL_STRETCH ? _selectedColor : _noSelectedColor))) {
                _selectedStretch = Stretch::FULL_STRETCH;
                _transform->setStretch(_selectedStretch);
            }
            ImGui::PopID();


            // Anchor middle row
            auto _middleLeftInit = Vec2F { _middleLeft->getRegion().bottomLeftCorner.x / _middleLeft->getSpriteSheetSize().x, _middleLeft->getRegion().bottomLeftCorner.y / _middleLeft->getSpriteSheetSize().y};
            auto _middleLeftEnd = _middleLeftInit + Vec2F { _middleLeft->getRegion().size.x / _middleLeft->getSpriteSheetSize().x, _middleLeft->getRegion().size.y / _middleLeft->getSpriteSheetSize().y};

            auto _middleMiddleInit = Vec2F { _middle->getRegion().bottomLeftCorner.x / _middle->getSpriteSheetSize().x, _middle->getRegion().bottomLeftCorner.y / _middle->getSpriteSheetSize().y};
            auto _middleMiddleEnd = _middleMiddleInit + Vec2F { _middle->getRegion().size.x / _middle->getSpriteSheetSize().x, _middle->getRegion().size.y / _middle->getSpriteSheetSize().y};

            auto _middleRightInit = Vec2F { _middleRight->getRegion().bottomLeftCorner.x / _middleRight->getSpriteSheetSize().x, _middleRight->getRegion().bottomLeftCorner.y / _middleRight->getSpriteSheetSize().y};
            auto _middleRightEnd = _middleRightInit + Vec2F { _middleRight->getRegion().size.x / _middleRight->getSpriteSheetSize().x, _middleRight->getRegion().size.y / _middleRight->getSpriteSheetSize().y};

            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middleLeft->getGLTexture()), ImVec2(16, 16), {_middleLeftInit.x, _middleLeftInit.y}, {_middleLeftEnd.x, _middleLeftEnd.y}, 0, (_selectedAnchor == Anchor::LEFT ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::LEFT;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middle->getGLTexture()), ImVec2(16, 16),   {_middleMiddleInit.x, _middleMiddleInit.y}, {_middleMiddleEnd.x, _middleMiddleEnd.y}, 0, (_selectedAnchor == Anchor::MIDDLE ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::MIDDLE;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_middleRight->getGLTexture()), ImVec2(16, 16), {_middleRightInit.x, _middleRightInit.y}, {_middleRightEnd.x, _middleRightEnd.y}, 0, (_selectedAnchor == Anchor::RIGHT ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::RIGHT;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();



            // Stretch bottom row
            ImGui::SameLine(0, 20);
            auto _stretchHorInit = Vec2F { _stretchHorizontal->getRegion().bottomLeftCorner.x / _stretchHorizontal->getSpriteSheetSize().x, _stretchHorizontal->getRegion().bottomLeftCorner.y / _stretchHorizontal->getSpriteSheetSize().y};
            auto _stretchHorEnd = _stretchHorInit + Vec2F { _stretchHorizontal->getRegion().size.x / _stretchHorizontal->getSpriteSheetSize().x, _stretchHorizontal->getRegion().size.y / _stretchHorizontal->getSpriteSheetSize().y};

            auto _stretchVertInit = Vec2F { _stretchVertical->getRegion().bottomLeftCorner.x / _stretchVertical->getSpriteSheetSize().x, _stretchVertical->getRegion().bottomLeftCorner.y / _stretchVertical->getSpriteSheetSize().y};
            auto _stretchVertEnd = _stretchVertInit + Vec2F { _stretchVertical->getRegion().size.x / _stretchVertical->getSpriteSheetSize().x, _stretchVertical->getRegion().size.y / _stretchVertical->getSpriteSheetSize().y};

            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchHorizontal->getGLTexture()), ImVec2(16, 16), {_stretchHorInit.x, _stretchHorInit.y}, {_stretchHorEnd.x, _stretchHorEnd.y}, 0, (_selectedStretch == Stretch::HORIZONTAL_STRETCH ? _selectedColor : _noSelectedColor))) {
                _selectedStretch = Stretch::HORIZONTAL_STRETCH;
                _transform->setStretch(_selectedStretch);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_stretchVertical->getGLTexture()), ImVec2(16, 16), {_stretchVertInit.x, _stretchVertInit.y}, {_stretchVertEnd.x, _stretchVertEnd.y}, 0, (_selectedStretch == Stretch::VERTICAL_STRETCH ? _selectedColor : _noSelectedColor))) {
                _selectedStretch = Stretch::VERTICAL_STRETCH;
                _transform->setStretch(_selectedStretch);
            }
            ImGui::PopID();



            // Anchor bottom row
            auto _bottomLeftInit = Vec2F { _bottomLeft->getRegion().bottomLeftCorner.x / _bottomLeft->getSpriteSheetSize().x, _bottomLeft->getRegion().bottomLeftCorner.y / _bottomLeft->getSpriteSheetSize().y};
            auto _bottomLeftEnd = _bottomLeftInit + Vec2F { _bottomLeft->getRegion().size.x / _bottomLeft->getSpriteSheetSize().x, _bottomLeft->getRegion().size.y / _bottomLeft->getSpriteSheetSize().y};

            auto _bottomMiddleInit = Vec2F { _bottomMiddle->getRegion().bottomLeftCorner.x / _bottomMiddle->getSpriteSheetSize().x, _bottomMiddle->getRegion().bottomLeftCorner.y / _bottomMiddle->getSpriteSheetSize().y};
            auto _bottomMiddleEnd = _bottomMiddleInit + Vec2F { _bottomMiddle->getRegion().size.x / _bottomMiddle->getSpriteSheetSize().x, _bottomMiddle->getRegion().size.y / _bottomMiddle->getSpriteSheetSize().y};

            auto _bottomRightInit = Vec2F { _bottomRight->getRegion().bottomLeftCorner.x / _bottomRight->getSpriteSheetSize().x, _bottomRight->getRegion().bottomLeftCorner.y / _bottomRight->getSpriteSheetSize().y};
            auto _bottomRightEnd = _bottomRightInit + Vec2F { _bottomRight->getRegion().size.x / _bottomRight->getSpriteSheetSize().x, _bottomRight->getRegion().size.y / _bottomRight->getSpriteSheetSize().y};

            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomLeft->getGLTexture()), ImVec2(16, 16),   {_bottomLeftInit.x, _bottomLeftInit.y}, {_bottomLeftEnd.x, _bottomLeftEnd.y}, 0, (_selectedAnchor == Anchor::LEFT_BOTTOM ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::LEFT_BOTTOM;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomMiddle->getGLTexture()), ImVec2(16, 16), {_bottomMiddleInit.x, _bottomMiddleInit.y}, {_bottomMiddleEnd.x, _bottomMiddleEnd.y}, 0, (_selectedAnchor == Anchor::BOTTOM ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::BOTTOM;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(createID());
            if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(_bottomRight->getGLTexture()), ImVec2(16, 16),  {_bottomRightInit.x, _bottomRightInit.y}, {_bottomRightEnd.x, _bottomRightEnd.y}, 0, (_selectedAnchor == Anchor::RIGHT_BOTTOM ? _selectedColor : _noSelectedColor))) {
                _selectedAnchor = Anchor::RIGHT_BOTTOM;
                _transform->setAnchor(_selectedAnchor);
            }
            ImGui::PopID();

            ImGui::PopStyleVar();

            ImGui::NewLine();

            ImGui::Text("Position ");

            float _pos[2] = {_transform->getPosition().x, _transform->getPosition().y};
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(createID());
            if(ImGui::DragFloat2("##myInput", _pos, 0.5f)) {
                _transform->setPosition(_pos[0], _pos[1]);
            }
            ImGui::PopID();


            ImGui::Text("Rotation ");

            float _angle = _transform->getRotation();
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::PushID(createID());
            if(ImGui::DragFloat("##myInput", &_angle, 0.1f))
                _transform->setRotation(_angle);
            ImGui::PopID();


            ImGui::Text("Scale ");

            float _scale[2] = {_transform->getScale().x, _transform->getScale().y};
            ImGui::SameLine(0, 30);
            ImGui::SetNextItemWidth(100);
            ImGui::PushID(createID());
            if(ImGui::DragFloat2("##myInput", _scale, 0.05))
                _transform->setScale(_scale[0], _scale[1]);
            ImGui::PopID();

            disabledConfigComponent(_transform);

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::ui9SliceComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<UI9Slice>(_selectedNode)) return;

        auto _ui9Slice = _graph->getComponent<UI9Slice>(_selectedNode);

        if(ImGui::CollapsingHeader("UI Panel", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

            {
                ImGui::Text("Size ");
                float _size[2] = {_ui9Slice->getSize().x, _ui9Slice->getSize().y};
                ImGui::SameLine(0, ImGui::CalcTextSize("Origin Offset ").x - ImGui::CalcTextSize("Size ").x + 8);
                ImGui::SetNextItemWidth(100);
                ImGui::PushID(createID());
                if (ImGui::DragFloat2("##myInput", _size, 1.f)) {
                    _ui9Slice->setSize({ Util::Math::clampF(_size[0], 0.f, FLT_MAX), Util::Math::clampF(_size[1], 0.f, FLT_MAX) });
                }
                ImGui::PopID();
            }

            {
                ImGui::Text("Origin Offset ");
                float _originOffset[2] = {_ui9Slice->getOriginOffset().x, _ui9Slice->getOriginOffset().y};
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::PushID(createID());
                if (ImGui::DragFloat2("##myInput", _originOffset, 1.f)) {
                    _ui9Slice->setOriginOffset({ Util::Math::clampF(_originOffset[0], FLT_MIN, FLT_MAX), Util::Math::clampF(_originOffset[1], FLT_MIN, FLT_MAX) });
                }
            }
            ImGui::PopID();

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    void ImGuiScene::uiButtonComponent(Graph* _graph, const NodeID _selectedNode) {
        if(!_graph->hasComponent<UIButton>(_selectedNode)) return;

        auto _uiButton = _graph->getComponent<UIButton>(_selectedNode);

        if(ImGui::CollapsingHeader("UI Button", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(_selectedNode == _graph->getRoot()->getID()) ImGui::BeginDisabled(true);

//            ImGui::Text("Size ");
//            float _size[2] = { _uiButton->nineSliceSprite->getSize().x, _uiButton->nineSliceSprite->getSize().y };
//            ImGui::SameLine();
//            ImGui::SetNextItemWidth(100);
//            ImGui::PushID(createID());
//            if(ImGui::DragFloat2("##myInput", _size, 0.5f)) {
//                auto _config = _uiButton->getConfig();
//                clampF(_size[0], 0, FLT_MAX);
//                clampF(_size[1], 0, FLT_MAX);
//                _config.buttonSize = { _size[0], _size[1] };
//                _uiButton->setConfig(&engine->manager, _config);
//            }
//            ImGui::PopID();

            if(_selectedNode == _graph->getRoot()->getID()) ImGui::EndDisabled();
        }
    }

    bool ImGuiScene::checkForFocus() {
        return ImGui::IsWindowHovered() | ImGui::IsAnyItemActive() | ImGui::IsAnyItemHovered() | ImGui::IsAnyItemFocused();
    }

    int ImGuiScene::createID() {
        return idIndex++;
    }

    void ImGuiScene::resetID() {
        idIndex = 0;
    }
}

#endif
