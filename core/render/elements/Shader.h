#pragma once
#include <GL/glew.h>
#include <string>

namespace engine {
    class Shader {

        private:
            GLuint shader;
            GLenum type;

        public:
            Shader();
            ~Shader();

            bool initFromFile(const std::string& _filePath, GLenum _shaderType);
            bool initFromString(const std::string& _shaderCode, GLenum _shaderType);
            void attachTo(GLuint _program) const;
    };
}