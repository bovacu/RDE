// Created by borja on 24/12/21.


#ifndef ENGINE2_0_RENDERER_H
#define ENGINE2_0_RENDERER_H

#include "core/render/elements/Texture.h"
#include "core/render/camera/Camera.h"
#include "core/render/shapes/Shape.h"
#include "core/render/elements/TextureRegion.h"
#include "core/render/elements/VertexArray.h"
#include "core/render/elements/Shader.h"

namespace engine {

    class Renderer {
        private:
            struct SceneData {
                glm::mat4 viewProjectionMatrix;
            };

            static std::unique_ptr<SceneData> sceneData;

        public:
            static void init();
            static void shutdown();

            static void setViewport(int _x, int _y, int _width, int _height);
            static void clear();
            static void drawIndexed(const VertexArrayPtr& _vertexArray, uint32_t _indexCount = 0);

            static void onWindowResize(uint32_t _width, uint32_t _height);

            static void beginDrawCall(const Camera& _camera, glm::mat4& _transform);
            static void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color, float _thickness = 1.0f);

            static void drawRectangle(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation = 0.f, const Color& _tintColor = Color::Transparent);
            static void drawRectangle(const glm::mat4& _transform, const Color& _color);

            static void drawShape(Shape& _shape, const Color& _color, float _thickness = 1.0f);
            static void drawTexture(const Vec2F& _position, const Vec2F& _size, const TexturePtr& _texture, float _rotation = 0.0f, const Color& _tintColor = Color::Transparent);
            static void drawTexture(const Vec2F& _position, const Vec2F& _size, const TextureRegionPtr& _textureRegion, float _rotation = 0.0f, const Color& _tintColor = Color::Transparent);

            static void endDrawCall();

            static void submit(const ShaderPtr& _shader, const VertexArrayPtr& _vertexArray, const glm::mat4& _transform = glm::mat4(1.0f));

            static void setClearColor(const Color& _color);

        private:
            static void flushAndReset();
            static void flush();
            static void initOpenGL();

            static void _drawRect(const Vec2F& _position, const Vec2F& _size, const Color& _color, const Color& _tintColor);
            static void _drawRotatedRect(const Vec2F& _position, const Vec2F& _size, float _rotation, const Color& _color, const Color& _tintColor);
    };

}


#endif //ENGINE2_0_RENDERER_H
