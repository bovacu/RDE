#pragma once
#include <GL/glew.h>
#include <string>

namespace engine {
    class Shader {

        private:
        GLuint m_shader;
        GLenum m_type;

        public:
        Shader();
        ~Shader();

        bool InitFromFile(std::string filePath, GLenum shaderType);
        bool InitFromString(std::string shaderCode, GLenum shaderType);
        void AttachTo(GLuint program);
    };
}