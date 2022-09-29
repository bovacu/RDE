//
// Created by borja on 9/5/22.
//

#ifndef GDE_SPRITE_RENDERER_H
#define GDE_SPRITE_RENDERER_H

#include "core/render/elements/IRenderizable.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace GDE {

    class Scene;
    class Canvas;

    /**
     * @brief Component that allows to render a texture or part of a texture on screen. End user doesn't have, and in fact can't
     * make it render manually, it is all done internally by the ECS system.
     */
    class SpriteRenderer : public IRenderizable {

        friend class SpriteBatch;

        private:
            /**
             * @brief Texture that contains the sprite. This is the whole SpriteSheet, but it is limited by the region and
             * only its region will be rendered.
             */
            Texture* texture = nullptr;

        public:
            SpriteRenderer(const NodeID& _nodeId, Scene* _scene, Texture* _texture);
            SpriteRenderer(const NodeID& _nodeId, Canvas* _canvas, Texture* _texture);
            ~SpriteRenderer() override {  }

            /**
             * @brief Sets the texture. This should be taken from the TextureAtlasManager.
             * @return Vec2 with {width, height}.
             * @see TextureAtlasManager
             */
            void setTexture(Texture* _texture) { texture = _texture; }

            /**
             * @brief Gets the path where the SpriteSheet is stored.
             * @return std::string
             */
            [[nodiscard]] std::string getTexturePath();

            /**
             * @brief Gets the name of the file od the SpriteSheet without the extension.
             * @return std::string
             */
            [[nodiscard]] std::string getTextureName();

            /**
             * @brief Gets the extension of the file of the SpriteSheet.
             * @return std::string
             */
            [[nodiscard]] std::string getTextureExtension();


            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2F getSize() const override { return {(float)texture->getSize().x * transform->getScale().x, (float)texture->getSize().y *
                                                                                                                          transform->getScale().y}; }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            /**
             * @see IRenderizable
             */
            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const override;
    };

}

#endif //GDE_SPRITE_RENDERER_H
