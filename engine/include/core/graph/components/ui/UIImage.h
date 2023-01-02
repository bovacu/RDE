//
// Created by borja on 10/26/22.
//

#ifndef RDE_UI_IMAGE_H
#define RDE_UI_IMAGE_H

#include "core/graph/components/ui/UI.h"
#include "core/render/elements/IRenderizable.h"

namespace RDE {

    class Scene;
    class Canvas;

    enum ImageRenderingType {
        NORMAL,
        NINE_SLICE,
        PARTIAL_VERTICAL,
        PARTIAL_HORIZONTAL,
        PARTIAL_RADIAL
    };

    struct UIImageConfig : public CommonUIConfig {
        Vec2F size = { -1, -1 };
        Texture* texture = nullptr;
        Color color = Color::White;
        ImageRenderingType imageRenderingType = ImageRenderingType::NORMAL;
    };

    class UIImage {
        friend class UIPanel;
        friend class UIButton;
        friend class UIInput;
        friend class UICheckbox;
        friend class UISlider;

        private:
            ImageRenderingType imageRenderingType = ImageRenderingType::NORMAL;
            bool partialRenderingInverted = false;
            float partialRenderingPercentage = 1.f;

        RENDERIZABLE_UI_BASIC_PROPERTIES()

        public:
            UIImage(Node* _node, Scene* _scene, Canvas* _canvas, const UIImageConfig& _config = {});
            UIImage(Node* _node, Manager* _manager, Graph* _graph, const UIImageConfig& _config = {});
            ~UIImage() {  }

            RENDERIZABLE_UI_BASIC_METHODS()

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
             * @brief Sets the rendering type of the UIImage. Can be any element of 'ImageRenderingType'.
             *      - Normal:     Will display the image as usual.
             *      - Vertical:   The image will be displayed normally if percentage is 1f, the lower the percentage, the
             *                    less vertically will be shown (from top to bottom).
             *      - Horizontal: The image will be displayed normally if percentage is 1f, the lower the percentage, the
             *                    less horizontally will be shown (from left to right).
             *      - Radial:     The image will be displayed normally if percentage is 1f, the lower the percentage, the
             *                    less in a 360 angle will be shown (from lower-middle in a counter-clock counter-wise).
             *
             * If Nine-Slice is selected and the texture doesn't have a Nine-Slice, the effect won't have effect and a warning log
             * will be shown.
             * @param _imageRenderingType ImageRenderingType
             */
            void setImageRenderingType(ImageRenderingType _imageRenderingType);

            /**
             * @brief Returns the type of rendering.
             * @return ImageRenderingType
             */
            [[nodiscard]]ImageRenderingType getImageRenderingType() const { return imageRenderingType; }

            /**
             * @brief Returns if the rendering is inverted.
             * @return bool
             */
            [[nodiscard]]bool isPartialRenderingInverted() const { return partialRenderingInverted;}

            /**
             * @brief This will invert the regular patter image rendering. Only applied if the rendering type is
             * PartialVertical, PartialHorizontal or PartialRadial.
             * @param _inverted True/False
             */
            void setPartialRenderingInverted(bool _inverted);

            /**
             * @brief Returns the amount filled.
             * @return float
             */
            [[nodiscard]] float getPartialRenderingPercentage() const { return partialRenderingPercentage; }

            /**
             * @brief Sets how much the partial image will be drawn.
             * @param _percentage Percentage [0f, 1f]
             */
            void setPartialRenderingPercentage(float _percentage);

            /**
             * @see IRenderizable
             */
            void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform* _transform, const ViewPort* _viewport);

        private:
            void calculateNormalGeometry(glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
            void calculate9SliceGeometry(glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
            void calculatePartialHGeometry(glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
            void calculatePartialVGeometry(glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);
            void calculatePartialRGeometry(glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport);

            void batchFourVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
            void batch9SliceVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
            void batchPartialCircularVertexGeometry(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices);
    };

}


#endif //RDE_UI_IMAGE_H
