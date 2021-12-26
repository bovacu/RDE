#pragma once
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "texture.h"

namespace engine {
    struct Vertex2dUVColor
    {

        glm::vec2 m_position;
        glm::vec2 m_texCoord;
        glm::vec4 m_color;

        // Makes a 2d vertex with uc and color data.
        Vertex2dUVColor(glm::vec2 position, glm::vec2 texCoord, glm::vec4 color)
        {
            m_position = position;
            m_texCoord = texCoord;
            m_color = color;
        }
    };



    class SpriteBatcher
    {

        public:
        SpriteBatcher(glm::vec2 screenSize);
        ~SpriteBatcher();

        void Draw(glm::vec4 destRect, glm::vec4 sourceRect, glm::vec4 color, Texture* engineTexture);
        void Flush();

        // Call this to tell the spritebatcher how many pixels wide/tall the window is. Setting to 1 will make the entire screen render 1 pixel
        void SetScreenSize(glm::vec2 screenSize);

        private:

        // Collection of vertices
        std::vector<Vertex2dUVColor> m_vertexBuffer;
        GLuint m_vbo;

        // Shaders.
        Shader m_vertexShader;
        Shader m_fragmentShader;

        // GL index for the shader program
        GLuint m_shaderProgram;

        // Texture and uniform location
        Texture* texture;
        GLint m_textureUniform;

        // stuff to send word matrices
        glm::mat3 m_screenTransform;
        GLint m_screenTransformUniform;
    };
}