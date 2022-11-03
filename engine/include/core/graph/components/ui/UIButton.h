//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "core/graph/components/ui/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(Manager, UIText, Font, UI9Slice)

    struct UIButtonConfig : CommonUIConfig {
        std::string text = "Button Text";
        Texture* idleTexture = nullptr;
        Texture* selectedTexture = nullptr;
        Texture* clickedTexture = nullptr;
        Texture* disabledTexture = nullptr;
        Font* font = nullptr;
        Vec2F buttonSize = { 0.1, 0.05 };
        Color buttonColor = Color::White;
        Color textColor = Color::White;
    };

    class UIButton : public UI {
        private:
            UIButtonConfig config;
            UIText* textRenderer = nullptr;
            UI9Slice* nineSliceSprite = nullptr;

        public:
            UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);
            UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config);

            [[nodiscard]] Vec2F getSize() const override;

            UIButtonConfig getConfig();
            void setConfig(Manager* _manager, const UIButtonConfig& _config);

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

#endif //GDE_UI_BUTTON_H
