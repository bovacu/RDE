#ifndef RMLUI_BACKENDS_RENDERER_GL3_H
#define RMLUI_BACKENDS_RENDERER_GL3_H

#include <RmlUi/Core/RenderInterface.h>

namespace GDE {
    class RenderInterface_GL3 : public Rml::RenderInterface {
        public:
            RenderInterface_GL3();
            ~RenderInterface_GL3();

            void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture,
                                const Rml::Vector2f& translation) override;

            Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices,
                                                        Rml::TextureHandle texture) override;
            void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
            void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;

            void EnableScissorRegion(bool enable) override;
            void SetScissorRegion(int x, int y, int width, int height) override;

            bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
            bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
            void ReleaseTexture(Rml::TextureHandle texture_handle) override;

            void SetTransform(const Rml::Matrix4f* transform) override;

            static const Rml::TextureHandle TextureIgnoreBinding = Rml::TextureHandle(-1);

            void SetViewport(int width, int height);
            void BeginFrame();
            void Clear();

        private:
            enum class ProgramId { None, Texture = 1, Color = 2, All = (Texture | Color) };
            void SubmitTransformUniform(ProgramId program_id, int uniform_location);

            Rml::Matrix4f transform;
            ProgramId transform_dirty_state = ProgramId::All;
    };

    namespace RmlGL3 {

        bool Initialize();
        void Shutdown();


        void EndFrame();

    } // namespace RmlGL3
}

#endif
