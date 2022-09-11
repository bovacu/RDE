#ifndef GDE_SHADER_H
#define GDE_SHADER_H

#include <string>
#include <unordered_map>

#include "core/Core.h"
#if IS_ANDROID()
    #include <GLES3/gl32.h>
#elif IS_IOS()
    #include <OpenGLES/ES3/gl.h>
#else
    #include "glad/glad.h"
#endif

#include "core/systems/fileSystem/FileManager.h"

namespace GDE {

    class LoadVertexConfigNotInvoked : public std::exception {
        public:
        [[nodiscard]] auto what() const noexcept -> const char* override {
            return "Load vertex config function was not called for shader, so the shader won't work.";
        }
    };

    struct VertexConfig {

        /**
         * @brief First parameter of 'glVertexAttribPointer', the value that in shader is (layout = 'pointerIndex').
         */
        unsigned int pointerIndex = 0;

        /**
         * @brief Number of elements that data has. Ex: glm::vec3 -> 3, glm::vec4 -> 4.
         */
        int numberOfElements = 0;

        /**
         * @brief GL_FLOAT, GL_INT, GL_UNSIGNED_INT...
         */
        unsigned int openglDataType = 0;

        /**
         * @brief Offset in bytes from the first VertexData to the current one.
         */
        unsigned int stride = 0;

        /**
         * @brief This method must return how many bytes VertexData uses.
         */
        int structSize = 0;
    };

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
            GLuint shaderID = -1;

            /**
             * @brief VAO of the shader.
             */
            GLuint vao = -1;

            /**
             * @brief IBO of the shader.
             */
            GLuint ibo = -1;

            /**
             * @brief VBO of the shader.
             */
            GLuint vbo = -1;

            /**
             * @brief Size of VertexData in bytes.
             */
             long vertexDataSize;

        public:
            Shader();
            ~Shader();

            /**
             * @brief Loads a Shader from a file.
             * @param _vertex Vertex Shader
             * @param _fragment Fragment Shader
             * @return ID of the Shader on the GPU
             */
            GLuint loadFromFiles(const std::string& _vertex, const std::string& _fragment, FileManager* _fileManager);

            /**
             * @brief Loads a Shader from a string.
             * @param _vertex Vertex Shader
             * @param _fragment Fragment Shader
             * @return ID of the Shader on the GPU
             */
            GLuint loadFromStrings(const std::string& _vertex, const std::string& _fragment);

            /**
             * @brief Loads the data structure that is going to be sent to the GPU. This method MUST be called
             */
            void loadVertexConfig(const std::vector<VertexConfig>& _verticesConfig, int _maxIndicesPerDrawCall);

            /**
             * @brief Returns the ID of the Shader on the GPU
             * @return uint
             */
            GLuint getShaderID() const;

            /**
             * @brief Returns the ID of the VAO on the GPU
             * @return uint
             */
            GLuint getShaderVAO() const;

            /**
            * @brief Returns the ID of the IBO on the GPU
            * @return uint
            */
            GLuint getShaderIBO() const;

            /**
             * @brief Returns the ID of the VBO on the GPU
             * @return uint
             */
            GLuint getShaderVBO() const;

            /**
             * @brief Returns the size of the VertexData structure in bytes.
             * @return long
             */
            long getShaderVertexDataSize() const;

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

#endif //GDE_SHADER_H
