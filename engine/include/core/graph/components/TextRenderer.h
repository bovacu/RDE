//
// Created by borja on 9/5/22.
//

#ifndef GDE_TEXT_RENDERER_H
#define GDE_TEXT_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace GDE {

    FORWARD_DECLARE_STRUCT(CharInfo)
    FORWARD_DECLARE_CLASS(Font, Scene, Canvas)

    /**
     * @brief Component used to render text on screen.  End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class TextRenderer : public IRenderizable {

        FRIEND_CLASS(UIButton, UICheckbox, UIInput)

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

            /**
             * @brief The texture of the generated font.
             */
            Texture* texture = nullptr;

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
            std::tuple<std::vector<LineInfo>, float, float> calculateLinesInfo(CharInfo* _chars) const;

        public:
            TextRenderer(const NodeID& _nodeId, Scene* _scene, const std::string& _text, Font* _font = nullptr);
            TextRenderer(const NodeID& _nodeId, Scene* _scene, Canvas* _canvas, const std::string& _text, Font* _font = nullptr);
            ~TextRenderer() override {  }

            /**
             * @brief Sets the text to be rendered.
             * @param _text The text to render.
             */
            void setText(const std::string& _text);

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
            [[nodiscard]] Vec2F getSize() const override { return { size.x / 2.f * transform->getScale().x, size.y / 2.f * transform->getScale().y }; }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            /**
             * @see IRenderizable
             */
            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) override;
            void drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const IViewPort& _viewport) override;
    };

}

#endif //GDE_TEXT_RENDERER_H
