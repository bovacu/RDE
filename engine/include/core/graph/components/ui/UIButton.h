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
            // [0] -> idle
            // [1] -> selected
            // [2] -> clicked
            // [3] -> disabled
            Texture* statesTextures[4];

        public:
            UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);
            UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config);

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            ~UIButton() override {  }

        private:
            void onMouseEntered();
            void onMouseExited();
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //RDE_UI_BUTTON_H
