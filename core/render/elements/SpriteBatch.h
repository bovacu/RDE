#pragma once
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"
#include "Sprite.h"
#include "core/render/Camera.h"
#include "core/render/shapes/Shape.h"
#include "core/systems/uiSystem/Text.h"

namespace engine {
    struct Vertex2dUVColor {

        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;

        Vertex2dUVColor(const glm::vec3& _position, const glm::vec2& _texCoord, const glm::vec4& _color) : position(_position),
                        texCoord(_texCoord), color(_color) {  }
    };

    struct VertexColorDebug {
        glm::vec3 position;
        glm::vec4 color;

        VertexColorDebug(const glm::vec3& _position, const glm::vec4& _color) : position(_position), color(_color) {  }
    };

    enum RenderShader {
        BASIC = 0,
        DEBUG = 1
    };

    class SpriteBatch {

        public:
            int totalTriangles;
            int drawCalls;

        private:
            std::vector<Vertex2dUVColor> vertexBuffer;
            std::vector<VertexColorDebug> vertexDebugBufferGeometrics;
            std::vector<VertexColorDebug> vertexDebugBufferLines;
            GLuint vbo = -1, debugVbo = -1;

            // Shaders.
            Shader basicShader;
            Shader debugShader;

            // GL index for the shader program
            GLuint basicShaderID = -1, debugShaderID = -1;

            // Texture and uniform location
            Texture* texture = nullptr;
            GLint textureUniform = -1;

            std::unordered_map<RenderShader, GLuint> shadersDict;
            Window* window;

            float aspectRatio = 1;

        public:
            SpriteBatch() = default;
            ~SpriteBatch();

            void init(Window* _window);
            void beginDraw(Camera& _camera, RenderShader _renderShader = BASIC);
            void draw(const glm::vec4& _destRect, const glm::vec4& _sourceRect, const glm::vec4& _color, Texture* _texture, float _rotation = 0);
            void draw(const Sprite& _sprite);
            void draw(Text& _text);
            void flush();

            void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::Green);
            void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::Green, float _rotation = 0.f);
            void drawShape(Shape& _shape);
            void flushDebug();

            void setDebugLinesThickness(float _thickness);

        private:
            void initShader();
            void initDebugShader();

            void initVbo();
            void initDebugVbo();

            Vec2F worldToScreenCoords(Vec2F _position);
            Vec2F worldToScreenSize(Vec2F _size);
    };
}