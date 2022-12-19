//
// Created by borja on 11/22/22.
//

#ifndef RDE_DYNAMIC_SPRITE_RENDERER_H
#define RDE_DYNAMIC_SPRITE_RENDERER_H

#include "core/render/elements/IRenderizable.h"

namespace RDE {

    struct DynamicSpriteRendererConfig {
        Vec2<uint> size = { 64, 64 };
        unsigned char* pixels = nullptr;
        ImageType imageType = ImageType::PNG;
    };

    class DynamicSpriteRenderer : public IRenderizable {

        FRIEND_CLASS(Graph)

        protected:
            /**
             * @brief Texture that contains the sprite. This is the whole SpriteSheet, but it is limited by the region and
             * only its region will be rendered.
             */
            CPUTexture* texture = nullptr;

            OpenGLVertex geometry[4];

        private:
            void calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);

        public:
            DynamicSpriteRenderer(Node* _node, Scene* _scene, const DynamicSpriteRendererConfig& _config);
            DynamicSpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const DynamicSpriteRendererConfig& _config);
            DynamicSpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const DynamicSpriteRendererConfig& _config);
            ~DynamicSpriteRenderer() override;

            /**
             * @brief Sets the color of a pixel, coordinate system starts at bottom-left.
             * @param _x X coord.
             * @param _y Y coord.
             * @param _color Color to set.
             */
            void setPixel(int _x, int _y, const Color& _color);

            /**
             * @brief Gets the color of a pixel, coordinate system starts at bottom-left.
             * @param _x X coord.
             * @param _y Y coord.
             * @return Color
             */
            Color getPixel(int _x, int _y);


            void resizeImage(const Vec2<uint>& _newSize);

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
            [[nodiscard]] Vec2F getSize() const override { return {(float)texture->getSize().x * IRenderizable::node->getTransform()->getScale().x,
                                                                   (float)texture->getSize().y * IRenderizable::node->getTransform()->getScale().y}; }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            /**
             * @see IRenderizable
             */
            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) override;
    };

} // RDE

#endif //RDE_DYNAMIC_SPRITE_RENDERER_H
