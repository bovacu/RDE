// Created by borja on 20/1/22.


#include "FrameBuffer.h"
#include "ShaderManager.h"
#include <glad/glad.h>

namespace engine {

    FrameBuffer::FrameBuffer(const FrameBufferSpecification& _specs) : specs(_specs) {
        this->invalidate();
    }

    FrameBuffer::~FrameBuffer() {
        /// First, we delete the frame buffer from memory.
        glDeleteFramebuffers(1, &this->fboID);

        /// Second, we delete the color associated texture.
        glDeleteTextures(1, &this->colorAttachment);

        /// Second, we delete the depth associated texture.
//        glDeleteTextures(1, &this->depthAttachment);

        glDeleteRenderbuffers(1, &rboID);
    }

    void FrameBuffer::invalidate() {

        /// In case there's a current frame buffer, delete it.
        if (this->fboID) {
            glDeleteFramebuffers(1, &this->fboID);
            glDeleteTextures(1, &this->colorAttachment);
//            glDeleteBuffers(1, &vboID);
//            glDeleteTextures(1, &this->depthAttachment);
        }

        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glDisableVertexAttribArray(10);
        glDisableVertexAttribArray(11);

        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        // create a color attachment texture
        glGenTextures(1, &colorAttachment);
        glBindTexture(GL_TEXTURE_2D, colorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specs.width, specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

        glGenRenderbuffers(1, &rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, specs.width, specs.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID); // now actually attach it

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        framebufferShader = ShaderManager::get().getShader("framebuffer");
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->fboID);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(framebufferShader);
        glBindVertexArray(vboID);

        glEnableVertexAttribArray(10);
        glEnableVertexAttribArray(11);

        glBindTexture(GL_TEXTURE_2D, colorAttachment);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(10);
        glDisableVertexAttribArray(11);
    }

    void FrameBuffer::resize(uint32_t _width, uint32_t _height) {
        this->specs.width = _width;
        this->specs.height = _height;
        this->invalidate();
    }

}