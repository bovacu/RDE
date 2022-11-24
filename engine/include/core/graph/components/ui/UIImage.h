//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_IMAGE_H
#define RDE_UI_IMAGE_H


#include "core/graph/components/SpriteRenderer.h"
#include "UI.h"

namespace RDE {

    struct UIImageConfig : public CommonUIConfig {
        Texture* texture = nullptr;
        Color color = Color::White;
    };

    class UIImage : public UI {
        private:
            /**
             * @brief Texture that contains the sprite. This is the whole SpriteSheet, but it is limited by the region and
             * only its region will be rendered.
             */
            Texture* texture = nullptr;

            OpenGLVertex geometry[4];

        public:
            UIImage(Node* _node, Scene* _scene, Canvas* _canvas, const UIImageConfig& _config);
            UIImage(Node* _node, Manager* _manager, Graph* _graph, const UIImageConfig& _config);
            ~UIImage() override {  }

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
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            void setInteractable(bool _interactable) override;
            bool isInteractable() override;

            /**
             * @see IRenderizable
             */
            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) override;

        private:
            void calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport);
    };

}


#endif //RDE_UI_IMAGE_H
