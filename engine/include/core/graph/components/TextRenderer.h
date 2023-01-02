//
// Created by borja on 9/5/22.
//

#ifndef RDE_TEXT_RENDERER_H
#define RDE_TEXT_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "core/util/Vec.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace RDE {

    struct CharInfo;
    class Font;
    class Scene;
    class Canvas;
    class Graph;
    class Manager;

    struct TextRendererConfig {
        std::string text = "Hello Duck!";
        Font* font = nullptr;
        Color color = Color::White;
    };

    /**
     * @brief Component used to render text on screen.  End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class TextRenderer {

        friend class UIText;
        friend class UIButton;
        friend class UICheckbox;
        friend class UIInput;

        private:
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
            Vec2F size {};

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

        RENDERIZABLE_BASIC_PROPERTIES()

        private:
            /**
             * @brief Recalculates the dimensions of the new text as a rectangle.
             * @param _text the inner text.
             */
            void recalcTextDimensions(const std::string& _text);

            struct LineInfo {
                std::string line;
                float biggestCharHeight;
            };

        // TODO (RDE): this needs a better fix than making it public, the affected part is SpriteBatchRenderFunctions.cpp -> drawBatchedForTextRenderer. 
        public:
            std::tuple<std::vector<LineInfo>, float, float> calculateLinesInfo(CharInfo* _chars) const;

        public:
            TextRenderer(Node* _node, Scene* _scene, const TextRendererConfig& _config = {});
            TextRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const TextRendererConfig& _config = {});
            TextRenderer(Node* _node, Manager* _manager, Graph* _graph, const TextRendererConfig& _config = {});
            ~TextRenderer() {  }

            RENDERIZABLE_BASIC_METHODS()

            /**
             * @brief Sets the text to be rendered.
             * @param _text The text to render.
             */
            void setText(const std::string& _text);

            /**
             * @brief Returns the size of the text.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getTextSize() const;

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


            void setNewLineSize(float _newLineSize);
            [[nodiscard]] float getNewLineSize() const;


            /**
             * @brief Sets the spaces between characters.
             * @param _spaceBetweenChars Space between characters.
             */
            void setSpacesBetweenChars(float _spaceBetweenChars);
    };

}

#endif //RDE_TEXT_RENDERER_H
