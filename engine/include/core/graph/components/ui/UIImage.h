//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_IMAGE_H
#define RDE_UI_IMAGE_H


#include "core/graph/components/SpriteRenderer.h"
#include "UI.h"

namespace RDE {

    enum ImageRenderingType {
        NORMAL,
        NINE_SLICE,
        PARTIAL_VERTICAL,
        PARTIAL_HORIZONTAL,
        PARTIAL_CIRCULAR
    };

    struct UIImageConfig : public CommonUIConfig {
        Vec2F size = { -1, -1 };
        Texture* texture = nullptr;
        Color color = Color::White;
        ImageRenderingType imageRenderingType = ImageRenderingType::NORMAL;
    };

    class UIImage : public UI {
        private:
            /**
             * @brief Texture that contains the sprite. This is the whole SpriteSheet, but it is limited by the region and
             * only its region will be rendered.
             */
            Texture* texture = nullptr;

            OpenGLVertex geometry[36];

            ImageRenderingType imageRenderingType = ImageRenderingType::NORMAL;

        public:
            UIImage(Node* _node, Scene* _scene, Canvas* _canvas, const UIImageConfig& _config);
            UIImage(Node* _node, Manager* _manager, Graph* _graph, const UIImageConfig& _config);
            ~UIImage() override {  }

            /**
             * @brief Sets the texture. This should be taken from the TextureAtlasManager.
             * @return Vec2 with {width, height}.
             * @see TextureAtlasManager
             */
            void setTexture(Texture* _texture) { dirty = true; texture = _texture; }

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
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            void setImageRenderingType(ImageRenderingType _imageRenderingType);
            [[nodiscard]]ImageRenderingType getImageRenderingType() const { return imageRenderingType; }

            void setSize(const Vec2F& _size);

            /**
             * @see IRenderizable
             */
            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) override;

        private:
            void calculateNormalGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);
            void calculate9SliceGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);
            void calculatePartialHGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);
            void calculatePartialVGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);
            void calculatePartialCGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);

            void batchFourVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
            void batch9SliceVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
            void batchPartialCircularVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
    };

}


#endif //RDE_UI_IMAGE_H
