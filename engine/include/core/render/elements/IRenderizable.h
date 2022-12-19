//
// Created by borja on 14/04/2022.
//

#ifndef RDE_RENDERIZABLE_H
#define RDE_RENDERIZABLE_H

#include "core/render/elements/Texture.h"
#include "core/render/elements/ShaderManager.h"
#include "core/render/elements/Batch.h"
#include "core/graph/components/Node.h"
#include "core/graph/components/ComponentBase.h"

namespace RDE {

    FORWARD_DECLARE_STRUCT(OpenGLVertex)
    FORWARD_DECLARE_CLASS(IViewPort, Transform)

    struct DrawAndFlushData {
        std::vector<OpenGLVertex> vertices;
        std::vector<uint32_t> indices;
        ShaderID shaderID;
        GLuint textureID;
    };

    /**
     * @brief Interface that elements to be rendered by the engine should follow. This gives the SpriteRenderer the
     * exact data it needs to render elements on screen.
     *
     * Any new element that end-users want to create and render, must extend this method.
     */
    class IRenderizable : public ComponentBase {
        FRIEND_CLASS(ShaderManager, SpriteBatch, ConfigManager, Canvas)

        protected:
            inline static std::unordered_map<std::string, ShaderID> defaultShaders;

        protected:
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

            bool dirty = false;

        public:
            Node* node = nullptr;

        public:
            IRenderizable(Node* _node) : node(_node) {  }

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

            // All the getters and setters are created because non-ui elements will use this default implementation, as they are simple
            // but ui elements are complex and the result of this simple functions could be drastically different internally.
            virtual ShaderID getShaderID() { return shaderID; }
            virtual Color getColor() { return color; }
            virtual int getLayer() { return layer; }
            BatchPriority getBatchPriority() { return batchPriority; }
            bool isEnabled() override { return !node->hasComponent<DisabledForRender>(); }

            /**
             * @brief Sets the ShaderID. This will trigger an inner hierarchy update.
             * @param _shaderID
             */
            virtual void setShaderID(ShaderID _shaderID) { shaderID = _shaderID; dirty = true; }

            /**
             * @brief Sets the Color. This will trigger an inner hierarchy update.
             * @param _shaderID
             */
            virtual void setColor(const Color& _color) { color = _color; dirty = true; }

            /**
             * @brief Sets the layer. This will trigger an inner hierarchy update.
             * @param _shaderID
             */
            virtual void setLayer(int _layer) { layer = _layer; dirty = true; }

            /**
             * @brief Sets the BatchPriority. This will trigger an inner hierarchy update.
             * @param _shaderID
             */
            void setBatchPriority(BatchPriority _batchPriority) { batchPriority = _batchPriority; dirty = true; }

            /**
             * @brief Enables or disables the Component. It will add or remove the DisabledForRender component.
             * This will trigger an inner hierarchy update.
             * @param _shaderID
             */
            void setEnabled(bool _enabled) override {
                if(_enabled && node->hasComponent<DisabledForRender>()) {
                    node->removeComponent<DisabledForRender>();
                    dirty = true;
                    return;
                }

                if(!_enabled && !node->hasComponent<DisabledForRender>()) {
                    node->addComponent<DisabledForRender>();
                    dirty = true;
                }
            }

            /**
             * @brief Method that every renderizable can implement to update its inner states.
             * @param _dt Delta time
             */
            virtual void onUpdate(Delta _dt) {  };

            /**
             * @brief Method that every renderizable can implement and it tells the SpriteBatch how to send the data to the GPU.
             * @param _vertices List with the current added vertices and where the new vertices must be added.
             * @param _indices List with the current added indices and where the new indices must be added.
             * @param _transform Transform of the renderizable.
             * @param _viewport Viewport of the scene.
             */
            virtual void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) {  };

            /**
             * @brief Method that every renderizable can implement and it tells the SpriteBatch how to send the data to the GPU.
             * @param _vertices List with the current added vertices and where the new vertices must be added.
             * @param _indices List with the current added indices and where the new indices must be added.
             * @param _transform Transform of the renderizable.
             * @param _viewport Viewport of the scene.
             */
            virtual void drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const ViewPort& _viewport) {  };

            virtual ~IRenderizable() {  }
    };

}

#endif //RDE_RENDERIZABLE_H
