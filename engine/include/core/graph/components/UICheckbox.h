//
// Created by borja on 10/18/22.
//

#ifndef GDE_UI_CHECKBOX_H
#define GDE_UI_CHECKBOX_H

#include "core/graph/components/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(TextRenderer, Font, NineSliceSprite, SpriteRenderer)

    struct UICheckboxConfig {
        std::string text = "Checkbox text";
        Texture* checkboxTickTexture = nullptr;
        Texture* checkboxBackgroundTexture = nullptr;
        Vec2F checkboxSize = { -1, -1 };

        Font* font = nullptr;
        Color checkboxColor = Color::White;
        Color textColor = Color::White;
        Color tickColor = Color::White;

        bool checked = false;
    };

    class UICheckbox : public UI {
        private:
            UICheckboxConfig config;
            TextRenderer* textRenderer;
            SpriteRenderer* spriteRenderer;
            NineSliceSprite* nineSliceSprite;
            bool checked;

        public:
            UICheckbox(const NodeID& nodeId, Canvas* canvas, const NodeID& _nodeID, Scene* _scene, Canvas* _canvas,const UICheckboxConfig& _config);
            ~UICheckbox() override {  }

            [[nodiscard]] NineSlice& getNineSlice() const;
            [[nodiscard]] Vec2F getSize() const override;

            UICheckboxConfig getConfig();
            void setConfig(Scene* _scene, const UICheckboxConfig& _config);

        private:
            void onMouseReleased(MouseCode _mouseButton);
    };

}


#endif //GDE_UI_CHECKBOX_H
