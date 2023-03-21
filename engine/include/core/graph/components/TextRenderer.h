//
// Created by borja on 9/5/22.
//

#ifndef RDE_TEXT_RENDERER_H
#define RDE_TEXT_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "core/util/Vec.h"
#include "entt/entity/entity.hpp"
#include "core/systems/uiSystem/FontManager.h"

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
		uint32_t fontSize = DEFAULT_FONT_SIZE;
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
            std::tuple<std::vector<LineInfo>, float, float> calculateLinesInfo(CharMap& _chars) const;

        public:
			RDE_FUNC TextRenderer(Node* _node, Scene* _scene, const TextRendererConfig& _config = {});
			RDE_FUNC TextRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const TextRendererConfig& _config = {});
			RDE_FUNC TextRenderer(Node* _node, Manager* _manager, Graph* _graph, const TextRendererConfig& _config = {});
			RDE_FUNC ~TextRenderer() {  }

            RENDERIZABLE_BASIC_METHODS()

            /**
             * @brief Sets the text to be rendered.
             * @param _text The text to render.
             */
			RDE_FUNC void setText(const std::string& _text);

            /**
             * @brief Returns the size of the text.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getTextSize() const;

            /**
             * @brief Sets the font.
             * @param _font The font.
             */
			RDE_FUNC void setFont(Font* _font);

            /**
             * @brief Get sthe font.
             * @return Font*
             */
			RDE_FUNC_ND Font* getFont() const;

            /**
             * @brief Gets the rendered text
             * @return const std::string&
             */
			RDE_FUNC_ND const std::string& getText() const;

            /**
             * @brief Sets the font size.
             * @param _fontSize Size of the font.
             */
			RDE_FUNC void setFontSize(int _fontSize);

            /**
             * @brief Gets the font size.
             * @return int
             */
			RDE_FUNC_ND int getFontSize() const;

            /**
             * @brief Gets the space between characters.
             * @return float
             */
			RDE_FUNC_ND float getSpacesBetweenChars() const;


			RDE_FUNC void setNewLineSize(float _newLineSize);
			RDE_FUNC_ND float getNewLineSize() const;


            /**
             * @brief Sets the spaces between characters.
             * @param _spaceBetweenChars Space between characters.
             */
			RDE_FUNC void setSpacesBetweenChars(float _spaceBetweenChars);
    };

}

#endif //RDE_TEXT_RENDERER_H
