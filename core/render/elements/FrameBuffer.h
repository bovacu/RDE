// Created by borja on 24/12/21.


#ifndef ENGINE2_0_FRAMEBUFFER_H
#define ENGINE2_0_FRAMEBUFFER_H


#include "core/util/Util.h"

namespace engine {

    struct FrameBufferSpecification {
        uint32_t    width{}, height{};
        uint32_t    samples = 1;
        bool        swapChainTarget = false;
    };

    class FrameBuffer;
    typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

    class FrameBuffer {
        private:
            uint32_t rendererID = 0;
            uint32_t colorAttachment = 0, depthAttachment = 0;
            FrameBufferSpecification specs;

        public:
            explicit FrameBuffer(const FrameBufferSpecification& _specs);
            ~FrameBuffer();

            void invalidate();

            void bind();
            void unbind();

            void resize(uint32_t _width, uint32_t _height);
            uint32_t getColorAttachmentRendererID() const;
            const FrameBufferSpecification& getSpecification() const;

            static FrameBufferPtr create(const FrameBufferSpecification& _specs);
    };

}



#endif //ENGINE2_0_FRAMEBUFFER_H
