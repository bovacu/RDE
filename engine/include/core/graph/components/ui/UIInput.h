//
// Created by borja on 10/21/22.
//

#ifndef GDE_UI_INPUT_H
#define GDE_UI_INPUT_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/NineSliceSprite.h"

namespace GDE {

    struct UIInputConfig : CommonUIConfig {
        Vec2F inputSize = { 256, 32 };
        Texture* inputBackgroundTexture = nullptr;
        Color inputBackgroundColor = Color::White;

        Font* font = nullptr;

        std::string placeholderText = "Message...";
        bool showPlaceholderText = true;
        Color placeholderTextColor = Color::Placeholder;

        Color textColor = Color::White;
        std::string text = "";

        Vec2F textsOffsetFromLeft = { 5, 0 };

        Texture* caretTexture = nullptr;
        float caretHeight = 20.f;
        float blinkingTimeSeconds = 0.5f;
    };

    class UIInput : public UI {
        private:
            UIInputConfig config;
            int pointer = 0;
            float textDisplacement = 0;
            float blinkingTimer = 0;

        public:
            TextRenderer* textRenderer = nullptr;
            TextRenderer* placeholderTextRenderer = nullptr;
            NineSliceSprite* nineSliceSprite = nullptr;
            SpriteRenderer* caretSprite = nullptr;
            Transform* caretTransform = nullptr;
            Transform* textTransform = nullptr;
            Transform* nineSliceTransform = nullptr;

            UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config);

            [[nodiscard]] Vec2F getSize() const override;

            UIInputConfig getConfig();
            void setConfig(Manager* _manager, const UIInputConfig& _config);

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            void onUpdate(Delta _dt) override;

            ~UIInput() override {  }

        private:
            void onMouseEntered();
            void onMouseExited();
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
            void onKeyPressed(KeyCode _keyCode, char _char);
            void onKeyReleased(KeyCode _keyCode, char _char);
            void onUnfocused();
            void updatePlaceholder();
            void updateCaret();
    };

} // GDE

#endif //GDE_UI_INPUT_H
