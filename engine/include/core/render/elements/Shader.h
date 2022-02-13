#pragma once

#include <string>
#include <unordered_map>

#include "core/Core.h"
#if IS_MOBILE()
    #include <GLES3/gl32.h>
#elif IS_DESKTOP()
    #include <glad/glad.h>
#endif

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