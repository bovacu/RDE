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

    class RmlOpenGLRenderer : public Rml::RenderInterface {
        private:
            Rml::Matrix4f transform;

        private:
            void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
            Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* _vertices, int _numVertices, int* _indices, int _numIndices, Rml::TextureHandle _texture) override;
            void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
            void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
            void EnableScissorRegion(bool enable) override;
            void SetScissorRegion(int x, int y, int width, int height) override;
            bool LoadTexture(Rml::TextureHandle& _textureHandle, Rml::Vector2i& _textureDimension, const Rml::String& _source) override;
            bool GenerateTexture(Rml::TextureHandle& _textureHandle, const Rml::byte* _source, const Rml::Vector2i& _sourceDimensions) override;
            void ReleaseTexture(Rml::TextureHandle texture_handle) override;
            void SetTransform(const Rml::Matrix4f* transform) override;

        public:
            void SetViewport(int width, int height);
            void BeginFrame();
    };

}

#endif //ENGINE_RML_OPENGL_RENDERER_H
