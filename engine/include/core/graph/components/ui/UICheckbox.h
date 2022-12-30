//
// Created by borja on 10/18/22.
//

#ifndef RDE_UI_CHECKBOX_H
#define RDE_UI_CHECKBOX_H

#include "core/graph/components/ui/UI.h"
#include "core/render/elements/IRenderizable.h"

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

    class UICheckbox {
        private:
            bool checked = false;
            UIText* textRenderer = nullptr;
            UIImage* checkboxBackgroundSprite = nullptr;
            UIImage* tickSprite = nullptr;
            Vec2F checkboxTextOffset;

            Node* textNode = nullptr;
            Node* tickNode = nullptr;

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            MDelegate<void(bool)> onClickedCheckbox;
            

        public:
            UICheckbox(Node* _node, Manager* _manager, Graph* _graph, const UICheckboxConfig& _config = {});
            ~UICheckbox() {  }

            
            RENDERIZABLE_UI_BASIC_METHODS()

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

            Node* getTickNode();
            Node* getTextNode();

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
