//
// Created by borja on 14/04/2022.
//

#ifndef ENGINE_I_RENDERIZABLE_H
#define ENGINE_I_RENDERIZABLE_H

#include "Texture.h"
#include "ShaderManager.h"

namespace GDE {

    class IViewPort;
    struct OpenGLVertex;
    struct Transform;

    /**
     * @brief Interface that elements to be rendered by the engine should follow. This gives the SpriteRenderer the
     * exact data it needs to render elements on screen.
     */
    class IRenderizable {
        public:
            /**
             * @brief Color that the sprite should be tint in. White means natural color.
             */
            Color color = Color::White;

            /**
             * @brief The Z-index, over and under what sprites this one should be rendered.
             */
            int layer = 0;

            /**
             * @brief The ID of the shader that should render the sprite.
             */
            GLuint shaderID = -1;

        public:
            /**
             * @brief Gets the ID on the GPU of the SpriteSheet that contains the sprite.
             * @return uint
             */
            [[nodiscard]] virtual GLuint getTexture() const = 0;

            /**
             * @brief Gets the size of the sprite, same as the size of the region, not the whole SpriteSheet.
             */
            [[nodiscard]] virtual Vec2I getSize() const = 0;

            /**
             * @brief Gets the size of the sprite (same as getSize()) and the bottom-left coordinate that represents the origin in the SpriteSheet.
             * @return IntRect 
             */
            [[nodiscard]] virtual IntRect getRegion() const = 0;

            virtual void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform) = 0;

            virtual ~IRenderizable() {  }
    };

}

#endif //ENGINE_I_RENDERIZABLE_H
