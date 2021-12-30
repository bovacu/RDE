#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>

namespace engine {
    class Shader {

        private:
            std::unordered_map<GLuint, GLuint> shadersAttached;

        public:
            Shader();
            ~Shader();

            GLuint loadFromFiles(const std::string& _vertex, const std::string& _fragment);
            GLuint loadFromStrings(const std::string& _vertex, const std::string& _fragment);

        private:
            bool initFromString(const std::string& _shaderCode, GLenum _shaderType);
    };
}