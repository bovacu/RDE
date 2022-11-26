//
// Created by borja on 10/21/22.
//

#ifndef RDE_UI_INPUT_H
#define RDE_UI_INPUT_H

#include "core/graph/components/ui/UI.h"

namespace RDE {

    FORWARD_DECLARE_CLASS(Font, UIText, UIImage, UI9Slice)

    struct UIInputConfig : CommonUIConfig {
        Vec2F inputSize = { 256.f, 32.f };
        Texture* inputBackgroundTexture = nullptr;
        Color inputBackgroundColor = Color::White;

        Font* font = nullptr;

        std::string placeholderText = "Message...";
        bool showPlaceholderText = true;
        Color placeholderTextColor = Color::Placeholder;

        Color textColor = Color::White;
        std::string text = "";

        Vec2F textsOffsetFromLeft = { 5.f, 0 };

        Texture* caretTexture = nullptr;
        Color caretColor = Color::White;
        float caretHeight = 0.75f;
        float blinkingTimeSeconds = 0.5f;
    };

    class UIInput : public UI {
        private:
            int pointer = 0;
            float textDisplacement = 0;
            float blinkingTimer = 0;

            UIText* textRenderer = nullptr;
            UIText* placeholderTextRenderer = nullptr;
            UIImage* background = nullptr;
            UIImage* caretSprite = nullptr;
            Transform* caretTransform = nullptr;
            Transform* textTransform = nullptr;
            Transform* nineSliceTransform = nullptr;
            Texture* caretTexture;

        public:
            float blinkingTimeSeconds = 0;
            bool showPlaceholderText = true;
            Vec2F textOffsetFromLeft;

            /**
             * @brief This is a % of the height of the input text
             */
            float caretHeight = 0.75;

            UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config);

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

            bool usable();
    };

} // RDE

#endif //RDE_UI_INPUT_H
