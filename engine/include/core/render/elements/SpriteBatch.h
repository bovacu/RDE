#pragma once

#include <vector>

#include "core/render/Camera.h"
#include "core/render/shapes/Shape.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/Components.h"
#include "core/systems/particleSystem/ParticleSystem.h"

namespace GDE {
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
                SpriteBatch* spriteBatch = nullptr;
                BatchPriority priority;
                int layer;
                ShaderID shaderID = -1;
                std::vector<Vertex2dUVColor> vertexBuffer;
                Texture* texture;
                friend class SpriteBatch;
                void addSprite(const SpriteRenderer& _spriteRenderer, const Transform& _transform);
                void addText(const TextRenderer& _text, const Transform& _transform);
        };

        class Debug {
            friend class SpriteBatch;
            private:
                static SpriteBatch* batch;
                std::vector<VertexColorDebug> vertexDebugBufferGeometrics;
                std::vector<VertexColorDebug> vertexDebugBufferLines;
                GLuint debugVbo = -1, debugVao = -1;
                Vec2F scalingFactor = {1, 1};

            private:
                void initDebugVbo();

            public:
                void init(SpriteBatch* _batch);
                void drawGrid(const Color& _color);

                void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::Green);
                void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::Green, float _rotation = 0.f);
                void drawShape(Shape& _shape);
                void flushDebug();

                void setDebugLinesThickness(float _thickness);
        };

        public:
            int totalTriangles = 0;
            int drawCalls = 0;
            Debug debug;

        private:
            GLuint vbo = -1, vao = -1;
            int vertices = 0;
            glm::mat4 viewProjectionMatrix;
            std::vector<Batch> batches;

        private:
            Batch& getBatch(const IRenderizable& _renderer, int _layer, BatchPriority _priority);
            void orderBatches();

        public:
            SpriteBatch() = default;
            ~SpriteBatch();

            void init();
            void beginDraw(Camera& _camera);
            void draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform);
            void draw(const TextRenderer& _text, const Transform& _transform);
            void draw(const ParticleSystem& _particleSystem, const Transform& _transform);
            void flush();

        private:
            void initVbo();
    };
}
