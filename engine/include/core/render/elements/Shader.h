#pragma once

#include <string>
#include <unordered_map>

#include "core/Core.h"
#if IS_MOBILE()
    #include <GLES3/gl32.h>
#elif IS_DESKTOP()
    #include <glad/glad.h>
#endif

namespace GDE {

    /**
     * @brief This class represents the Shader program compiled and stored in the GPU.
     */
    class Shader {

        private:
            /**
             * @brief Map with the loaded shaders, as default only Vertex and Fragment shaders.
             */
            std::unordered_map<GLuint, GLuint> shadersAttached;

            /**
             * @brief Shader ID in the GPU so the engine can locate it and enable it to render.
             */
            GLuint shaderID;

        public:
            Shader();
            ~Shader();

            /**
             * @brief Loads a Shader from a file.
             * @param _vertex Vertex Shader
             * @param _fragment Fragment Shader
             * @return ID of the Shader on the GPU
             */
            GLuint loadFromFiles(const std::string& _vertex, const std::string& _fragment);

            /**
             * @brief Loads a Shader from a string.
             * @param _vertex Vertex Shader
             * @param _fragment Fragment Shader
             * @return ID of the Shader on the GPU
             */
            GLuint loadFromStrings(const std::string& _vertex, const std::string& _fragment);

            /**
             * @brief Returns the ID of the Shader on the GPU
             * @return uint
             */
            GLuint getShaderID() const;

        private:
            /**
             * @brief Compiles the Shader code and sends it to the GPU. Returns true if compilation is successful.
             * @param _shaderCode Vertex or Fragment
             * @param _shaderType Type of shader, Vertex or Fragment
             * @return bool
             */
            bool initFromString(const std::string& _shaderCode, GLenum _shaderType);
    };
}