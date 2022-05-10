//
// Created by borja on 09/05/22.
//

#ifndef ENGINE_RML_OPENGL_RENDERER_H
#define ENGINE_RML_OPENGL_RENDERER_H

#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Core.h>
#include "core/platform/PlatformHeaderSDL.h"
#include "core/render/elements/ShaderManager.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "core/render/elements/Texture.h"
#include <glad/glad.h>

namespace GDE {

    struct CompiledGeometryData {
        GLuint texture;
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLsizei draw_count;
    };

    class RmlOpenGLRenderer : public Rml::RenderInterface {
        private:
            SDL_Window* sdlWindow;
            ShaderID shaderTextureId, shaderColorId;
            glm::mat4* viewProjectionMatrix;
            CompiledGeometryData compiledGeometryData;

        public:
            void init(SDL_Window* _window);
            void update(glm::mat4* _viewProjectionMatrix);

        private:
            void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
            void EnableScissorRegion(bool enable) override;
            void SetScissorRegion(int x, int y, int width, int height) override;
            bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
            bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
            void ReleaseTexture(Rml::TextureHandle texture) override;
            void SetTransform(const Rml::Matrix4f* transform) override;
            Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) override;
            void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle handle) override;
            void RenderCompiledGeometry(Rml::CompiledGeometryHandle handle, const Rml::Vector2f& translation) override;
    };

}

#endif //ENGINE_RML_OPENGL_RENDERER_H
