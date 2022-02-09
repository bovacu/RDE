// Created by borja on 20/1/22.


#include "core/render/elements/FrameBuffer.h"
#include "core/render/elements/ShaderManager.h"

#ifdef __ANDROID__
    #include <GLES3/gl3.h>
#elif __linux__
    #include "external/glad/include/glad/glad.h"
#endif

namespace engine {

    FrameBuffer::FrameBuffer(const FrameBufferSpecification& _specs) : specs(_specs) {
        aspectRatio = (float)_specs.width / (float)_specs.height;
        invalidate();
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &fboID);
        glDeleteTextures(1, &colorAttachment);
        glDeleteRenderbuffers(1, &rboID);
        glDeleteBuffers(1, &vboID);
    }

    void FrameBuffer::invalidate() {

        /// In case there's a current frame buffer, delete it.
        if (fboID) {
            glDeleteFramebuffers(1, &fboID);
            glDeleteTextures(1, &colorAttachment);
            glDeleteBuffers(1, &vboID);
            glDeleteRenderbuffers(1, &rboID);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (int)specs.width, (int)specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

        glGenRenderbuffers(1, &rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, (int)specs.width, (int)specs.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID); // now actually attach it

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        framebufferShader = ShaderManager::get().getShader("framebuffer");
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    }

    void FrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(framebufferShader);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        glEnableVertexAttribArray(10);
        glEnableVertexAttribArray(11);

        glBindTexture(GL_TEXTURE_2D, colorAttachment);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(10);
        glDisableVertexAttribArray(11);
    }

    void FrameBuffer::resize(uint32_t _width, uint32_t _height) {
        specs.width = _width;
        specs.height = _height;
        aspectRatio = (float)specs.width / (float)specs.height;
        invalidate();
    }

}