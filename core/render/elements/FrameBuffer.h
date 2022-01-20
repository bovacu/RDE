// Created by borja on 20/1/22.


#ifndef ENGINE2_0_FRAMEBUFFER_H
#define ENGINE2_0_FRAMEBUFFER_H


#include "core/util/Util.h"

namespace engine {

    struct FrameBufferSpecification {
        uint32_t    width = -1, height = -1;
        uint32_t    samples = 1;
        bool        swapChainTarget = false;
    };

    class FrameBuffer {
        private:
        uint32_t rendererID = 0;
        uint32_t colorAttachment = 0, depthAttachment = 0;
        FrameBufferSpecification specs;

        public:
        explicit FrameBuffer(const FrameBufferSpecification& _specs);
        ~FrameBuffer();

        /// This method checks if there's a valid FrameBuffer already in memory, in that case it removes it and
        /// creates a new one with a custom FrameBufferSpecification.
        void invalidate();

        void bind();
        void unbind();

        void resize(uint32_t _width, uint32_t _height);

        [[nodiscard]] uint32_t getColorAttachmentRendererID() const { return this->colorAttachment; }
        [[nodiscard]] const FrameBufferSpecification& getSpecification() const { return this->specs; }
    };

}


#endif //ENGINE2_0_FRAMEBUFFER_H
