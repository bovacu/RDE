#pragma once
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "core/render/elements/Texture.h"
#include "core/render/Camera.h"
#include "core/render/shapes/Shape.h"
#include "core/systems/uiSystem/Text.h"
#include "core/render/elements/ShaderManager.h"

namespace engine {
    struct Vertex2dUVColor {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;
        Vertex2dUVColor(const glm::vec3& _position, const glm::vec2& _texCoord, const glm::vec4& _color) : position(_position), texCoord(_texCoord), color(_color) {  }
    };

    struct VertexColorDebug {
        glm::vec3 position;
        glm::vec4 color;
        VertexColorDebug(const glm::vec3& _position, const glm::vec4& _color) : position(_position), color(_color) {  }
    };

    enum BatchPriority {
        SpritePriority = 0,
        TextPriority = 1
    };


    class SpriteBatch {

        class Batch {
            private:
                BatchPriority priority;
                int layer;
                ShaderID shaderID = -1;
                float aspectRatio;
                Vec2F scalingFactor = {1, 1};
                std::vector<Vertex2dUVColor> vertexBuffer;
                Texture* texture;
                Window* window = nullptr;
                friend class SpriteBatch;
                void addSprite(SpriteRenderer& _spriteRenderer, Transform& _transform);
                void addText(Text& _text, SpriteRenderer& _spriteRenderer, Transform& _transform);
        };

        public:
            int totalTriangles = 0;
            int drawCalls = 0;

        private:
            std::vector<VertexColorDebug> vertexDebugBufferGeometrics;
            std::vector<VertexColorDebug> vertexDebugBufferLines;
            GLuint vbo = -1, debugVbo = -1;

            glm::mat4 viewProjectionMatrix;
            float aspectRatio = 1;
            Vec2F scalingFactor = {1, 1};

            Texture* texture = nullptr;
            Window* window = nullptr;

            std::vector<Batch> batches;

        private:
            Batch& getBatch(SpriteRenderer& _spriteRenderer, int _layer, BatchPriority _priority);
            void orderBatches();

        public:
            SpriteBatch() = default;
            ~SpriteBatch();

            void init(Window* _window);
            void beginDraw(Camera& _camera);
            void draw(const glm::vec4& _destRect, const glm::vec4& _sourceRect, const glm::vec4& _color, Texture* _texture, float _rotation = 0);
            void draw(SpriteRenderer& _spriteRenderer, Transform& _transform);
            void draw(Text& _text, Transform& _transform);
            void flush();

            void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::Green);
            void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::Green, float _rotation = 0.f);
            void drawShape(Shape& _shape);
            void flushDebug();

            void setDebugLinesThickness(float _thickness);

        private:
            void initVbo();
            void initDebugVbo();
    };
}