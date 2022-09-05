//
// Created by borja on 9/5/22.
//

#ifndef GDE_TEXT_RENDERER_H
#define GDE_TEXT_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "entt/entity/entity.hpp"
#include "core/graph/Scene.h"

typedef entt::entity NodeID;

namespace GDE {

    /**
     * @brief Component used to render text on screen.  End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class TextRenderer : public IRenderizable {

        friend class SpriteBatch;

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
        float spaceBetweenChars {};

        /**
         * @brief Size of the space character.
         */
        float spaceWidth {};

        /**
         * @brief Height of the enter character.
         */
        float enterHeight {};

        /**
         * @brief Size of the font.
         */
        int fontSize {};

        /**
         * @brief The texture of the generated font.
         */
        Texture* texture = nullptr;

        /**
         * @deprecated This needs to go.
         */
        IViewPort* viewport = nullptr;


        private:
        /**
         * @brief Recalculates the dimensions of the new text as a rectangle.
         * @param _text the inner text.
         */
        void recalcTextDimensions(const std::string& _text);

        public:
        TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text);
        TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font);
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
         * @brief Gets the dimensions of the rendered text as a rectangle.
         * @return Vec2F
         */
        [[nodiscard]] Vec2F getTextSize() const;

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
         * @brief Sets the space character width.
         * @param _spaceWidth Space character width.
         */
        void setSpaceWidth(float _spaceWidth);

        /**
         * @brief Get the space character width.
         * @return float
         */
        [[nodiscard]] float getSpaceWidth() const;

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
        [[nodiscard]] Vec2I getSize() const override { return texture->getSize(); }

        /**
         * @see IRenderizable
         */
        [[nodiscard]] IntRect getRegion() const override { return texture->getRegion(); }

        /**
         * @see IRenderizable
         */
        void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const override;
    };

}

#endif //GDE_TEXT_RENDERER_H
