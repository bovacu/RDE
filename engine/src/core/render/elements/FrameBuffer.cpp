// Created by borja on 20/1/22.


#include "core/render/elements/FrameBuffer.h"
#include "core/Manager.h"
#include "core/render/Renderer.h"

#if IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#elif IS_MAC()
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#elif IS_DESKTOP()
    #include "glad/glad.h"
#endif

namespace GDE {

    FrameBuffer::FrameBuffer(const FrameBufferSpecification& _specs, Manager* _manager) : specs(_specs), manager(_manager) {
        glGenVertexArrays(1, &vao);
        invalidate();
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &fboID);
        glDeleteTextures(1, &frameBufferTexureForColorAttachment);
        glDeleteRenderbuffers(1, &rboID);
        glDeleteBuffers(1, &vboID);
    }

    void FrameBuffer::invalidate() {

        /// In case there's a current frame buffer, delete it.
        if (fboID) {
            glDeleteFramebuffers(1, &fboID);
            glDeleteTextures(1, &frameBufferTexureForColorAttachment);
            glDeleteBuffers(1, &vboID);
            glDeleteRenderbuffers(1, &rboID);
        }

        glBindVertexArray(vao);
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


        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        // create a color attachment texture
        glGenTextures(1, &frameBufferTexureForColorAttachment);
        glBindTexture(GL_TEXTURE_2D, frameBufferTexureForColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (int)specs.width, (int)specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexureForColorAttachment, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

        glGenRenderbuffers(1, &rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, (int)specs.width, (int)specs.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID); // now actually attach it

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);

        framebufferShader = manager->shaderManager.getShader("framebuffer");
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glEnable(GL_DEPTH_TEST);
    }

    void FrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if(specs.renderToWindow) {
            glBindVertexArray(vao);
            {
                glDisable(GL_DEPTH_TEST);
                glUseProgram(framebufferShader);
                glBindBuffer(GL_ARRAY_BUFFER, vboID);

                glBindTexture(GL_TEXTURE_2D, frameBufferTexureForColorAttachment);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glBindVertexArray(0);
        } else{
            Renderer::clear();
        }
    }

    void FrameBuffer::resize(uint32_t _width, uint32_t _height) {
        specs.width = _width;
        specs.height = _height;
        invalidate();
    }

}
