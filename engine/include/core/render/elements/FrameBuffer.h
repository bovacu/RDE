// Created by borja on 20/1/22.


#ifndef ENGINE2_0_FRAMEBUFFER_H
#define ENGINE2_0_FRAMEBUFFER_H


#include "core/util/Util.h"

namespace GDE {

    struct FrameBufferSpecification {
        uint32_t    width = -1, height = -1;
        uint32_t    samples = 1;
        bool        swapChainTarget = false;
    };

    class FrameBuffer {
        private:
        uint32_t fboID = 0, rboID = -1, vboID = -1, vao = -1;
        uint32_t framebufferShader = -1;
        uint32_t colorAttachment = -1;
        FrameBufferSpecification specs;

        public:
        explicit FrameBuffer(const FrameBufferSpecification& _specs);
        ~FrameBuffer();

        /// This method checks if there's a valid FrameBuffer already in memory, in that case it removes it and
        /// creates a new one with a custom FrameBufferSpecification.
        void invalidate();

        void bind() const;
        void unbind() const;

        void resize(uint32_t _width, uint32_t _height);

        [[nodiscard]] uint32_t getColorAttachmentRendererID() const { return colorAttachment; }
        [[nodiscard]] const FrameBufferSpecification& getSpecification() const { return specs; }
    };

}


#endif //ENGINE2_0_FRAMEBUFFER_H
