//
// Created by borja on 28/07/22.
//

#include "Editor.h"
#include "core/graph/Components.h"
#include "core/graph/Scene.h"
#include "imgui.h"

namespace Editor {

    void Editor::redirectRendering(FrameBuffer* _frameBuffer) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        auto viewportSize = ImVec2 { viewportPanelSize.x, viewportPanelSize.y };

        uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
        ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Editor::onInit() {
        UDelegate<void(FrameBuffer*)> _delegate;
        _delegate.bind<&Editor::redirectRendering>(this);
        // engine->setRenderingRedirectionToImGui(_delegate);
    }

    void Editor::onUpdate(Delta _dt) {
        Scene::onUpdate(_dt);
        
    }

    void Editor::textStressTest() {
            Random _r;
            auto _texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
            // 40000 is the maximum I could get with 55fps of average performance, with texts -> "Text[0-40000]" ~350000 images

            for(int _i = 0; _i < 300000; _i++) {
                auto _text = getMainGraph()->createNode("Block" + std::to_string(_i));
                nodes.push_back(_text);
                auto* _textTransform = getMainGraph()->getComponent<Transform>(_text);
                _textTransform->setPosition(_r.randomf(-(float)engine->getWindow().getWindowSize().x / 2.f, (float)engine->getWindow().getWindowSize().x / 2.f),
                                            _r.randomf(-(float)engine->getWindow().getWindowSize().y / 2.f, (float)engine->getWindow().getWindowSize().y / 2.f));
                getMainGraph()->addComponent<SpriteRenderer>(_text, this, _texture);
            }
        }

    }