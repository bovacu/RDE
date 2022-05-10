//
// Created by borja on 09/05/22.
//

#include "core/systems/uiSystem/RmlOpenGLRenderer.h"
#include "glm/gtc/type_ptr.hpp"
#include "core/render/elements/Texture.h"

namespace GDE {

    void RmlOpenGLRenderer::init(SDL_Window* _window) {
        sdlWindow = _window;
        shaderTextureId = ShaderManager::get().getShader("rml");
        shaderColorId = ShaderManager::get().getShader("rmlColor");
    }

    void RmlOpenGLRenderer::update(glm::mat4* _viewProjectionMatrix) {
        viewProjectionMatrix = _viewProjectionMatrix;
    }

    Rml::CompiledGeometryHandle RmlOpenGLRenderer::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture)
    {
        constexpr GLenum draw_usage = GL_STATIC_DRAW;

        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Rml::Vertex) * num_vertices, (const void*)vertices, draw_usage);

        glEnableVertexAttribArray(15);
        glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, position)));

        glEnableVertexAttribArray(16);
        glVertexAttribPointer(16, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, colour)));

        glEnableVertexAttribArray(17);
        glVertexAttribPointer(17, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (const GLvoid*)(offsetof(Rml::Vertex, tex_coord)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, (const void*)indices, draw_usage);
        glBindVertexArray(0);

        auto* geometry = new CompiledGeometryData;
        geometry->texture = (GLuint)texture;
        geometry->vao = vao;
        geometry->vbo = vbo;
        geometry->ibo = ibo;
        geometry->draw_count = num_indices;

        return (Rml::CompiledGeometryHandle)geometry;
    }

    void RmlOpenGLRenderer::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {
        Rml::CompiledGeometryHandle geometry = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);

        if (geometry) {
            RenderCompiledGeometry(geometry, translation);
            ReleaseCompiledGeometry(geometry);
        }
    }

    void RmlOpenGLRenderer::RenderCompiledGeometry(Rml::CompiledGeometryHandle handle, const Rml::Vector2f& translation)
    {
        auto* geometry = (CompiledGeometryData*)handle;

        if (geometry->texture)
        {
            glUseProgram(shaderTextureId);
            glBindTexture(GL_TEXTURE_2D, geometry->texture);
            GLint _location = glGetUniformLocation(shaderTextureId, "viewProjectionMatrix");
            glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(*viewProjectionMatrix)));
        }
        else
        {
            glUseProgram(shaderColorId);
            glBindTexture(GL_TEXTURE_2D, 0);
            GLint _location = glGetUniformLocation(shaderColorId, "viewProjectionMatrix");
            glUniformMatrix4fv(_location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(*viewProjectionMatrix)));
        }

        glBindVertexArray(geometry->vao);
        glDrawElements(GL_TRIANGLES, geometry->draw_count, GL_UNSIGNED_INT, (const GLvoid*)0);
    }

    void RmlOpenGLRenderer::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle handle)
    {
        auto* geometry = (CompiledGeometryData*)handle;

        glDeleteVertexArrays(1, &geometry->vao);
        glDeleteBuffers(1, &geometry->vbo);
        glDeleteBuffers(1, &geometry->ibo);

        delete geometry;
    }

    void GDE::RmlOpenGLRenderer::EnableScissorRegion(bool enable) {
        if (enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void GDE::RmlOpenGLRenderer::SetScissorRegion(int x, int y, int width, int height) {
        if(sdlWindow == nullptr) return;
        int w_width, w_height;
        SDL_GetWindowSize(sdlWindow, &w_width, &w_height);
        glScissor(x, w_height - (y + height), width, height);
    }

    bool GDE::RmlOpenGLRenderer::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {
        LOG_I("Loading Texture")
        Texture _texture;
        if(!_texture.loadFromFile(source.c_str())) return false;
        texture_handle = (Rml::TextureHandle)_texture.getGLTexture();
        return true;
    }

    bool GDE::RmlOpenGLRenderer::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {
        GLuint texture_id = 0;
        glGenTextures(1, &texture_id);
        if (texture_id == 0)
        {
            LOG_E("Failed to generate Rml Texture")
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);

        GLint internal_format = GL_RGBA8;
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        texture_handle = (Rml::TextureHandle)texture_id;

        return true;
    }

    void GDE::RmlOpenGLRenderer::ReleaseTexture(Rml::TextureHandle texture) {
        LOG_I("Releasing texture")
        glDeleteTextures(1, (GLuint*)&texture);
    }

    void GDE::RmlOpenGLRenderer::SetTransform(const Rml::Matrix4f* transform) {
        RenderInterface::SetTransform(transform);
    }
}