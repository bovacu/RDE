//
// Created by borja on 9/22/22.
//

#ifndef RDE_UI_BUTTON_H
#define RDE_UI_BUTTON_H

#include "core/graph/components/ui/UI.h"

namespace RDE {

    FORWARD_DECLARE_CLASS(Manager, UIText, Font, UIImage)

    struct UIButtonConfig : CommonUIConfig {
        std::string text = "Button Text";
        Texture* idleTexture = nullptr;
        Texture* selectedTexture = nullptr;
        Texture* clickedTexture = nullptr;
        Texture* disabledTexture = nullptr;
        Font* font = nullptr;
        Vec2F buttonSize = { 128, 32 };
        Color buttonColor = Color::White;
        Color textColor = Color::White;
    };

    class UIButton : public UI {
        private:
            UIText* textRenderer = nullptr;
            UIImage* uiImage = nullptr;

            /**
             * @brief Those are the possible states associated with a different texture.
                [0] -> idle
                [1] -> selected
                [2] -> clicked
                [3] -> disabled
             */
            Texture* statesTextures[4];

        public:
            UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);
            UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config);

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
             * @see IRenderizable
             * @param _interactable
             */
            void setColor(const Color& _color) override;

            /**
             * @see IRenderizable
             * @return Color
             */
            Color getColor() override;

            ~UIButton() override {  }

        private:
            /**
             * @brief Function that executes and makes internal calculations when mouse enters the UITransform::size area.
             */
            void onMouseEntered();

            /**
            * @brief Function that executes and makes internal calculations when mouse exits the UITransform::size area.
            */
            void onMouseExited();

            /**
            * @brief Function that executes and makes internal calculations when mouse enters and clicks the UITransform::size area.
            */
            void onMouseClicked(MouseCode _mouseCode);

            /**
            * @brief Function that executes and makes internal calculations when mouse enters, clicks and then is released the UITransform::size area.
            */
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //RDE_UI_BUTTON_H
