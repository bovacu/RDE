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

            /**
             * @see UI
             * @param _interactable
             */
            void setInteractable(bool _interactable) override;

            /**
             * @see UI
             * @return bool
             */
            bool isInteractable() override;

            /**
             * @brief Sets the inner state as checked/unchecked, changes the tick sprite and fires 'onClickedCheckbox' callback.
             * @param _checked True/False
             */
            void setChecked(bool _checked);

            /**
             * @brief Returns if the Checkbox is checked or unchecked.
             * @return
             */
            [[nodiscard]] bool isChecked() const;

        private:
            /**
            * @brief Function that executes and makes internal calculations when mouse enters, clicks and then is released the UITransform::size area.
            */
            void onMouseReleased(MouseCode _mouseButton);

            /**
            * @brief Function that executes and makes internal calculations when mouse enters and clicks the UITransform::size area.
            */
            void onMouseClicked(MouseCode _mouseButton) {  }

            /**
            * @brief Function that executes and makes internal calculations when mouse exits the UITransform::size area.
            */
            void onMouseEntered() {  }
    };

}


#endif //RDE_UI_CHECKBOX_H
