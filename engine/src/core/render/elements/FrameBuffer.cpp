// Created by borja on 20/1/22.


#include "core/render/elements/FrameBuffer.h"
#include "core/Manager.h"
#include "core/render/RenderManager.h"

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl32.h>
#elif IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#else
    #include "glad/glad.h"
#endif

namespace RDE {

    FrameBuffer::FrameBuffer(const FrameBufferSpecification& _specs, Manager* _manager) : specs(_specs), manager(_manager) {
		manager = _manager;
		glGenVertexArrays(1, &vao);
        invalidate();
        Util::GL::checkError("FrameBuffer constructor");
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
            glDeleteRenderbuffers(1, &rboID);
        }

		vao = manager->shaderManager.getShader(FRAMEBUFFER_SHADER)->getDynamicShaderVAO();
		vboID = manager->shaderManager.getShader(FRAMEBUFFER_SHADER)->getDynamicShaderVBO();
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);

		glGenFramebuffers(1, &fboID);
		glGenRenderbuffers(1, &rboID);

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
		glGenTextures(1, &frameBufferTexureForColorAttachment);

		glBindTexture(GL_TEXTURE_2D, frameBufferTexureForColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (int)specs.width, (int)specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexureForColorAttachment, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, (int)specs.width, (int)specs.height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID); // now actually attach it

        ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        framebufferShader = manager->shaderManager.getShader(FRAMEBUFFER_SHADER)->getShaderID();
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        Util::GL::checkError("FrameBuffer bindFrameBuffer");
    }

    void FrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Util::GL::checkError("FrameBuffer unbind");
		flush();
    }

	void FrameBuffer::flush() const {
		if(specs.renderToWindow) {
			glBindVertexArray(vao);
			{
				glUseProgram(framebufferShader);
				glBindBuffer(GL_ARRAY_BUFFER, vboID);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, frameBufferTexureForColorAttachment);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			glBindVertexArray(0);
		}
	}

    void FrameBuffer::resize(uint32_t _width, uint32_t _height) {
        specs.width = _width;
        specs.height = _height;
        invalidate();
    }

}
