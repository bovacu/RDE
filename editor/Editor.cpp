//
// Created by borja on 28/07/22.
//

#include "Editor.h"
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
        engine->setRenderingRedirectionToImGui(_delegate);
    }


}