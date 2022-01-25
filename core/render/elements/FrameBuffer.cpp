// Created by borja on 20/1/22.


#include "FrameBuffer.h"
#include <glad/glad.h>

namespace engine {

    FrameBuffer::FrameBuffer(const FrameBufferSpecification& _specs) : specs(_specs) {
        this->invalidate();
    }

    FrameBuffer::~FrameBuffer() {
        /// First, we delete the frame buffer from memory.
        glDeleteFramebuffers(1, &this->rendererID);

        /// Second, we delete the color associated texture.
        glDeleteTextures(1, &this->colorAttachment);

        /// Second, we delete the depth associated texture.
        glDeleteTextures(1, &this->depthAttachment);
    }

    void FrameBuffer::invalidate() {

        /// In case there's a current frame buffer, delete it.
        if (this->rendererID) {
            glDeleteFramebuffers(1, &this->rendererID);
            glDeleteTextures(1, &this->colorAttachment);
            glDeleteTextures(1, &this->depthAttachment);
        }

        /// First, we create the buffer in memory.
        glCreateFramebuffers(1, &this->rendererID);

        /// Second, we select it to use it.
        glBindFramebuffer(GL_FRAMEBUFFER, this->rendererID);

        /// Now we need to create the Texture for its color attachment.
        ///
        /// First, we create the texture in memory.
        glCreateTextures(GL_TEXTURE_2D, 1, &this->colorAttachment);

        /// Second, we select it to be used.
        glBindTexture(GL_TEXTURE_2D, this->colorAttachment);

        /// Third, we set the basic information for the texture.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->specs.width, this->specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        /// Finally, we set the algorithm to use when resizing bigger and smaller the texture.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /// We attach this Texture to the current bind frame buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorAttachment, 0);

        /// Now we need to create the Texture for its depth attachment. The process is the same as before.
        glCreateTextures(GL_TEXTURE_2D, 1, &this->depthAttachment);
        glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, this->specs.width, this->specs.height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->depthAttachment, 0);

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->rendererID);
        glViewport(0, 0, this->specs.width, this->specs.height);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::resize(uint32_t _width, uint32_t _height) {
        this->specs.width = _width;
        this->specs.height = _height;
        this->invalidate();
    }

}