//
// Created by borja on 11/22/22.
//

#ifndef RDE_DYNAMIC_SPRITE_RENDERER_H
#define RDE_DYNAMIC_SPRITE_RENDERER_H

#include "core/render/elements/IRenderizable.h"

namespace RDE {

    class Manager;
    class Graph; 
    class Scene;
    class Canvas;

    struct Node;

    struct DynamicSpriteRendererConfig {
        Vec2<uint> size = { 64, 64 };
        unsigned char* pixels = nullptr;

		/**
		 * @brief Use RDE_IMAGE_EXTENSION_ values.
		 */
		uint32_t imageType = 0;
    };

    class DynamicSpriteRenderer {

        friend class Graph;

        RENDERIZABLE_BASIC_PROPERTIES()

        public:
            DynamicSpriteRenderer(Node* _node, Scene* _scene, const DynamicSpriteRendererConfig& _config = {});
            DynamicSpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const DynamicSpriteRendererConfig& _config = {});
            DynamicSpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const DynamicSpriteRendererConfig& _config = {});
            ~DynamicSpriteRenderer();

            RENDERIZABLE_BASIC_METHODS()

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
    };

} // RDE

#endif //RDE_DYNAMIC_SPRITE_RENDERER_H
