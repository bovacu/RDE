//
// Created by borja on 9/22/22.
//

#ifndef RDE_UI_BUTTON_H
#define RDE_UI_BUTTON_H

#include "core/graph/components/ui/UI.h"
#include "core/render/elements/IRenderizable.h"

namespace RDE {

    class Scene;
    class Canvas; 
    class Manager;
    class UIText;
    class Font;
    class UIImage;

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

    class UIButton {
        private:
            UIText* textRenderer = nullptr;
            UIImage* uiImage = nullptr;
            Node* textNode = nullptr;

            /**
             * @brief Those are the possible states associated with a different texture.
                [0] -> idle
                [1] -> selected
                [2] -> clicked
                [3] -> disabled
             */
            Texture* statesTextures[4];

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config = {});
            UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config = {});

            RENDERIZABLE_UI_BASIC_METHODS()

            Node* getTextNode();

            ~UIButton() {  }

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
