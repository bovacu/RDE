//
// Created by borja on 9/22/22.
//

#ifndef GDE_UI_BUTTON_H
#define GDE_UI_BUTTON_H

#include "core/graph/components/ui/UI.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(Manager, TextRenderer, Font, NineSliceSprite)

    struct UIButtonConfig : CommonUIConfig {
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
    };

    class UIButton : public UI {
        private:
            UIButtonConfig config;

        public:
            TextRenderer* textRenderer = nullptr;
            NineSliceSprite* nineSliceSprite = nullptr;

            UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config);
            UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config);

            [[nodiscard]] Vec2F getSize() const override;

            UIButtonConfig getConfig();
            void setConfig(Manager* _manager, const UIButtonConfig& _config);

            void setInteractable(bool _disabled) override;
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
