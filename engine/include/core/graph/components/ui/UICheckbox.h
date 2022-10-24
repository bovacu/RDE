//
// Created by borja on 10/18/22.
//

#ifndef GDE_UI_CHECKBOX_H
#define GDE_UI_CHECKBOX_H

#include "core/graph/components/ui/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(TextRenderer, Font, NineSliceSprite, SpriteRenderer)

    struct UICheckboxConfig : CommonUIConfig {
        std::string text = "Checkbox text";
        Texture* checkboxTickTexture = nullptr;
        Texture* checkboxBackgroundTexture = nullptr;

        Font* font = nullptr;
        Color checkboxColor = Color::White;
        Color textColor = Color::White;
        Color tickColor = Color::White;

        bool checked = false;
        Vec2F checkboxTextOffset = { 5, 0 };
    };

    class UICheckbox : public UI {
        private:
            UICheckboxConfig config;
            TextRenderer* textRenderer;
            SpriteRenderer* checkboxBackgroundSprite;
            SpriteRenderer* tickSprite;
            bool checked;
            Transform* textTransform, *checkboxBackgroundTransform, *tickTransform;

        public:
            UICheckbox(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas,const UICheckboxConfig& _config);
            ~UICheckbox() override {  }

            [[nodiscard]] Vec2F getSize() const override;

            UICheckboxConfig getConfig();
            void setConfig(Scene* _scene, const UICheckboxConfig& _config);

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

        private:
            void onMouseReleased(MouseCode _mouseButton);
            void onMouseClicked(MouseCode _mouseButton) {  }
            void onMouseEntered() {  }
    };

}


#endif //GDE_UI_CHECKBOX_H
