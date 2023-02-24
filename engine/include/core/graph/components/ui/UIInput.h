//
// Created by borja on 10/21/22.
//

#ifndef RDE_UI_INPUT_H
#define RDE_UI_INPUT_H

#include "core/graph/components/ui/UI.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Delta.h"

namespace RDE {

    class Font;
    class UIText;
    class UIImage;

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

    class UIInput {
        private:
            int pointer = 0;
            float textDisplacement = 0;
            float blinkingTimer = 0;

            UIText* textRenderer = nullptr;
            Node* textNode = nullptr;

            UIText* placeholderTextRenderer = nullptr;
            Node* placeholderNode = nullptr;

            UIImage* background = nullptr;
            
            UIImage* caretSprite = nullptr;
            Node* caretNode = nullptr;

            Transform* caretTransform = nullptr;
            Transform* textTransform = nullptr;
            Transform* nineSliceTransform = nullptr;
            Texture* caretTexture;

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            float blinkingTimeSeconds = 0;
            bool showPlaceholderText = true;
            Vec2F textOffsetFromLeft;

            /**
             * @brief This is a % of the height of the input text
             */
            float caretHeight = 0.75;


        public:
            UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config = {});

            RENDERIZABLE_UI_BASIC_METHODS()

            void onUpdate(Delta _dt);

            Node* getTextNode();
            Node* getTextPlaceholderNode();
            Node* getCaretNode();

            ~UIInput() {  }

        private:
            void onMouseEntered();
            void onMouseExited();
			void onMouseClicked(RDE_MOUSE_BUTTON_ _mouseCode);
			void onMouseReleased(RDE_MOUSE_BUTTON_ _mouseCode);
            void onKeyPressed(RDE_KEYBOARD_KEY_ _keyCode);
			void onKeyReleased(RDE_KEYBOARD_KEY_ _keyCode);
            void onTextTyping(const std::string& _text);
            void onUnfocused();
            void updatePlaceholder();
            void updateCaret();

            bool usable();
    };

} // RDE

#endif //RDE_UI_INPUT_H
