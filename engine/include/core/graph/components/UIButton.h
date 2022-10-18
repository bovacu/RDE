//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "core/graph/components/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(TextRenderer, Font, NineSliceSprite)

    struct UIButtonConfig {
        std::string text = "Button Text";
        Texture* idleTexture = nullptr;
        Texture* selectedTexture = nullptr;
        Texture* clickedTexture = nullptr;
        Texture* disabledTexture = nullptr;
        Font* font = nullptr;
        Vec2F size = { 128, 32 };
        Color buttonColor = Color::White;
        Color textColor = Color::White;
    };

    class UIButton : public UI {
        private:
            Vec2F lastTextOffset = { 0.f, 0.f };
            Vec2F lastTextScale = { 1.f, 1.f };
            float lastTextRotation = 0.f;
            UIButtonConfig config;

        public:
            TextRenderer* textRenderer;
            NineSliceSprite* nineSliceSprite;
            Vec2F textOffset = { 0.f, 0.f };
            Vec2F textScale = { 1.f, 1.f };
            float textRotation = 0.f;

            UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);

            /**
             * @brief Returns the NineSlice data. This is almost sure not to be needed by end-users.
             * @return NineSlice&
             */
            [[nodiscard]] NineSlice& getNineSlice() const;

            [[nodiscard]] Vec2F getSize() const override;

            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) override;

            UIButtonConfig getConfig();
            void setConfig(Scene* _scene, const UIButtonConfig& _config);

            ~UIButton() override {  }

        private:
            void onMouseEntered();
            void onMouseExited();
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //GDE_UI_BUTTON_H
