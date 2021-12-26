#pragma once
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"

namespace engine {
    struct Vertex2dUVColor {

        glm::vec2 position;
        glm::vec2 texCoord;
        glm::vec4 color;

        Vertex2dUVColor(const glm::vec2& _position, const glm::vec2& _texCoord, const glm::vec4& _color) : position(_position),
                        texCoord(_texCoord), color(_color) {  }
    };


    class SpriteBatch {

        public:
            SpriteBatch() = default;
            ~SpriteBatch();

            void init();
            void draw(const glm::vec4& _destRect, const glm::vec4& _sourceRect, const glm::vec4& _color, Texture* _texture);
            void flush();

            // Call this to tell the spritebatcher how many pixels wide/tall the window is. Setting to 1 will make the entire screen render 1 pixel
            void setScreenSize(const glm::vec2& _screenSize);

        private:
            std::vector<Vertex2dUVColor> vertexBuffer;
            GLuint vbo;

            // Shaders.
            Shader vertexShader;
            Shader fragmentShader;

            // GL index for the shader program
            GLuint shaderProgram;

            // Texture and uniform location
            Texture* texture;
            GLint textureUniform;

            // stuff to send word matrices
            glm::mat3 screenTransform;
            GLint screenTransformUniform;
    };
}