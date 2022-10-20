//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(TextRenderer, Font, NineSliceSprite)

    struct UIButtonConfig {
        std::string text = "Button Text";
        Texture* idleTexture = nullptr;
        Texture* selectedTexture = nullptr;
        Texture* clickedTexture = nullptr;
        Texture* disabledTexture = nullptr;
        Font* font = nullptr;
        Vec2F buttonTextureSize = {128, 32 };
        Vec2F interactableArea = {128, 32 };
        Color buttonColor = Color::White;
        Color textColor = Color::White;
        bool disabled = false;
    };

    class UIButton : public UI {
        private:
            UIButtonConfig config;

        public:
            TextRenderer* textRenderer;
            NineSliceSprite* nineSliceSprite;

            UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);

            [[nodiscard]] NineSlice& getNineSlice() const;
            [[nodiscard]] Vec2F getSize() const override;

            UIButtonConfig getConfig();
            void setConfig(Scene* _scene, const UIButtonConfig& _config);

            void setDisabled(bool _disabled);
            bool isDisabled();

            ~UIButton() override {  }

        private:
            void onMouseEntered();
            void onMouseExited();
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //GDE_UI_BUTTON_H
