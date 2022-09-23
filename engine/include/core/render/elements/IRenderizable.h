//
// Created by borja on 14/04/2022.
//

#ifndef GDE_RENDERIZABLE_H
#define GDE_RENDERIZABLE_H

#include "core/render/elements/Texture.h"
#include "core/render/elements/ShaderManager.h"
#include "core/render/elements/Batch.h"

namespace GDE {

    class IViewPort;
    struct OpenGLVertex;
    class Transform;

    /**
     * @brief Interface that elements to be rendered by the engine should follow. This gives the SpriteRenderer the
     * exact data it needs to render elements on screen.
     *
     * Any new element that end-users want to create and render, must extend this method.
     */
    class IRenderizable {
        protected:
            friend class ShaderManager;
            inline static std::unordered_map<std::string, ShaderID> defaultShaders;

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

            /**
             * @brief The priority to be rendered, the higher, the later it will be drawn, so it will be over lower elements.
             */
            BatchPriority batchPriority = BatchPriority::SpritePriority;

            Transform* transform;

        public:
            IRenderizable(Transform* _transform) : transform(_transform) {  }

            /**
             * @brief Gets the ID on the GPU of the SpriteSheet that contains the sprite.
             * @return uint
             */
            [[nodiscard]] virtual GLuint getTexture() const = 0;

            /**
             * @brief Gets the size of the sprite, same as the size of the region, not the whole SpriteSheet.
             */
            [[nodiscard]] virtual Vec2F getSize() const = 0;

            /**
             * @brief Gets the size of the sprite (same as getSize()) and the bottom-left coordinate that represents the origin in the SpriteSheet.
             * @return IntRect 
             */
            [[nodiscard]] virtual FloatRect getRegion() const = 0;

            /**
             * @brief Method that every renderizable must implement and it tells the SpriteBatch how to send the data to the GPU.
             * @param _vertices List with the current added vertices and where the new vertices must be added.
             * @param _indices List with the current added indices and where the new indices must be added.
             * @param _transform Transform of the renderizable.
             * @param _viewport Viewport of the scene.
             */
            virtual void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const = 0;

            virtual ~IRenderizable() {  }
    };

}

#endif //GDE_RENDERIZABLE_H
