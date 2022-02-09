#pragma once

#ifdef __ANDROID__
    #include <GLES3/gl32.h>
#elif __linux__
    #include "external/glad/include/glad/glad.h"
#endif

#include <string>
#include <unordered_map>

namespace engine {
    class Shader {

        private:
            std::unordered_map<GLuint, GLuint> shadersAttached;
            GLuint shaderID;

        public:
            Shader();
            ~Shader();

            GLuint loadFromFiles(const std::string& _vertex, const std::string& _fragment);
            GLuint loadFromStrings(const std::string& _vertex, const std::string& _fragment);
            GLuint getShaderID() const;

        private:
            bool initFromString(const std::string& _shaderCode, GLenum _shaderType);
    };
}