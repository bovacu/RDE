//
// Created by borja on 10/18/22.
//

#ifndef RDE_UI_CHECKBOX_H
#define RDE_UI_CHECKBOX_H

#include "core/graph/components/ui/UI.h"

namespace RDE {

    FORWARD_DECLARE_CLASS(UIText, Font, UI9Slice, UIImage, UITransform)

    struct UICheckboxConfig : public CommonUIConfig {
        std::string text = "Checkbox text";
        Texture* checkboxTickTexture = nullptr;
        Texture* checkboxBackgroundTexture = nullptr;

        Font* font = nullptr;
        Color checkboxColor = Color::White;
        Color textColor = Color::White;
        Color tickColor = Color::White;

        bool checked = false;
        Vec2F checkboxTextOffset = { 5.f, 0 };
    };

    class UICheckbox : public UI {
        private:
            bool checked = false;
            UIText* textRenderer = nullptr;
            UIImage* checkboxBackgroundSprite = nullptr;
            UIImage* tickSprite = nullptr;

        public:
            MDelegate<void(bool)> onClickedCheckbox;
            Vec2F checkboxTextOffset;

        public:
            UICheckbox(Node* _node, Manager* _manager, Graph* _graph, const UICheckboxConfig& _config);
            ~UICheckbox() override {  }

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setChecked(bool _checked);
            [[nodiscard]] bool isChecked() const;

        private:
            void onMouseReleased(MouseCode _mouseButton);
            void onMouseClicked(MouseCode _mouseButton) {  }
            void onMouseEntered() {  }
    };

}


#endif //RDE_UI_CHECKBOX_H
