// Created by borja on 20/1/22.


#ifndef RDE_FRAMEBUFFER_H
#define RDE_FRAMEBUFFER_H


#include "core/util/Util.h"
#include "core/render/elements/ShaderManager.h"

namespace RDE {

    /**
     * @brief Data about the FrameBuffer
     */
    struct FrameBufferSpecification {
        /**
         * @brief Width of the FrameBuffer.
         */
        uint32_t width = -1;

        /**
         * @brief Height of the FrameBuffer.
         */
        uint32_t height = -1;

        /**
         * @brief Number of samples.
         */
        uint32_t samples = 1;

        /**
         * @brief Render to window
         */
        bool renderToWindow = true;
    };

    class Manager;

    /**
     * @brief This class adds final rendering effects to the whole scene.
     */
    class FrameBuffer {
        friend class Engine;

        private:
            /**
             * @brief FrameBuffer ID on the GPU.
             */
            uint32_t fboID = 0;

            /**
             * @brief RenderBuffer ID on the GPU.
             */
            uint32_t rboID = -1;

            /**
             * @brief VertexBuffer ID on the GPU.
             */
            uint32_t vboID = -1;

            /**
             * @brief VertexArrayBuffer ID on the GPU.
             */
            uint32_t vao = -1;

            /**
             *
             */
            ShaderID framebufferShader = -1;

            /**
             * @brief ColorAttachment buffer ID on the GPU
             */
            uint32_t frameBufferTexureForColorAttachment = -1;

            /**
             * @see FrameBufferSpecification
             */
            FrameBufferSpecification specs;

            /**
             * @see Manager
             */
            Manager* manager;

        public:
			RDE_FUNC_EXPLICIT FrameBuffer(const FrameBufferSpecification& _specs, Manager* _manager);
			RDE_FUNC ~FrameBuffer();

            /**
             * @brief This method checks if there's a valid FrameBuffer already in memory, in that case it removes it and
             * creates a new one with a custom FrameBufferSpecification.
             */
			RDE_FUNC void invalidate();

            /**
             * @brief Binds the FrameBuffer, it will render the whole scene as its shader dictates.
             */
			RDE_FUNC void bind() const;

            /**
             * @brief Unbinds the FrameBuffer, so the scene can be rendered as it would normally do.
             */
			RDE_FUNC void unbind() const;

			RDE_FUNC void flush();

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Resizes the FrameBuffer according to the Window size.
             * @param _width Width of the Window
             * @param _height Height of the Window
             */
			RDE_FUNC void resize(uint32_t _width, uint32_t _height);

            /**
             * @brief Returns the ID of the Color Attachment Buffer.
             * @return uint32_t
             */
			RDE_FUNC_ND uint32_t getColorAttachmentRendererID() const { return frameBufferTexureForColorAttachment; }

			RDE_FUNC_ND uint32_t getID() { return fboID; }

            /**
             * @see FrameBufferSpecification
             * @return FrameBufferSpecification&
             */
			RDE_FUNC_ND const FrameBufferSpecification& getSpecification() const { return specs; }
    };

}


#endif //RDE_FRAMEBUFFER_H
