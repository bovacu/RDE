//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_TEXT_RENDERER_H
#define RDE_UI_TEXT_RENDERER_H

#include "core/graph/components/ui/UI.h"
#include "core/systems/uiSystem/FontManager.h"

namespace RDE {

    struct UITextConfig : public CommonUIConfig {
        Font* font = nullptr;
        std::string text = "Hello Duck!";
        Color textColor = Color::White;
    };

    class UIText : public UI {
        FRIEND_CLASS(Canvas, UIButton, UICheckbox, UIInput)

        private:
            protected:
            /**
             * @brief The font used to render the text.
             */
            Font* font;

            /**
             * @brief Rendered text.
             */
            std::string innerText;

            /**
             * @brief Size of the whole text as a rectangle.
             */
            Vec2F textSize {};

            /**
             * @brief Amount of space between each character.
             */
            float spaceBetweenChars = 0.f;

            /**
             * @brief It is calculated as a % of the total height of the full text. Only applicable if it is multiline text.
             */
            float newLineSize = 1.05f;

            /**
             * @brief Size of the font.
             */
            int fontSize {};

        protected:
            /**
             * @brief Recalculates the dimensions of the new text as a rectangle.
             * @param _text the inner text.
             */
            void recalcTextDimensions(const std::string& _text);

            struct LineInfo {
                std::string line;
                float biggestCharHeight;
            };
            std::tuple<std::vector<LineInfo>, float, float> calculateLinesInfo(CharInfo* _chars) const;

        public:
            UIText(Node* _node, Scene* _scene, Canvas* _canvas, const UITextConfig& _config);
            UIText(Node* _node, Manager* _manager, Graph* _graph, const UITextConfig& _config);
            ~UIText() override {  }

            bool isInteractable() override;
            void setInteractable(bool _interactable) override;

            /**
             * @brief Sets the text to be rendered.
             * @param _text The text to render.
             */
            void setText(const std::string& _text);
            Vec2F getTextSize();

            /**
             * @brief Sets the font.
             * @param _font The font.
             */
            void setFont(Font* _font);

            /**
             * @brief Get sthe font.
             * @return Font*
             */
            [[nodiscard]] Font* getFont() const;

            /**
             * @brief Gets the rendered text
             * @return const std::string&
             */
            [[nodiscard]] const std::string& getText() const;

            /**
             * @brief Sets the font size.
             * @param _fontSize Size of the font.
             */
            void setFontSize(int _fontSize);

            /**
             * @brief Gets the font size.
             * @return int
             */
            [[nodiscard]] int getFontSize() const;

            /**
             * @brief Gets the space between characters.
             * @return float
             */
            [[nodiscard]] float getSpacesBetweenChars() const;

            /**
             * @brief Sets the spaces between characters.
             * @param _spaceBetweenChars Space between characters.
             */
            void setSpacesBetweenChars(float _spaceBetweenChars);

            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) override;
            void drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const ViewPort& _viewport) override;
    };

} // RDE

#endif //RDE_UI_TEXT_RENDERER_H
