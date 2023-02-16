#ifndef RDE_SHADER_H
#define RDE_SHADER_H

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

namespace RDE {

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

	template<typename T>
	struct UniformConfig {
		const T* data = nullptr;
		GLsizei count = 1;
		GLboolean transpose = GL_FALSE;
		void (*uniformNonMatrixFunc)(GLint _location, GLsizei _count, const T* _data) = nullptr;
		void (*uniformMatrixFunc)(GLint _location, GLsizei _count, GLboolean _transpose, const T* _data) = nullptr;
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
			std::unordered_map<const char*, GLint> uniforms;

            /**
             * @brief Shader ID in the GPU so the engine can locate it and enable it to render.
             */
            GLuint shaderID = -1;

            /**
             * @brief VAO of the shader.
             */
            GLuint vao = -1, staticVao = -1;

            /**
             * @brief IBO of the shader.
             */
            GLuint ibo = -1, staticIbo = -1;

            /**
             * @brief VBO of the shader.
             */
            GLuint vbo = -1, staticVbo = -1;

            /**
             * @brief Size of VertexData in bytes.
             */
             long vertexDataSize;

        private:
			 void loadVertexConfigSpecific(const std::vector<VertexConfig>& _verticesConfig, const std::vector<const char*> _uniforms, int _maxIndicesPerDrawCall, GLenum _drawType, GLuint& _vbo, GLuint& _ibo, GLuint& _vao);

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
			void loadVertexConfig(const std::vector<VertexConfig>& _verticesConfig, const std::vector<const char*> _uniforms, int _maxIndicesPerDrawCall);

            /**
             * @brief Returns the ID of the Shader on the GPU
             * @return uint
             */
            GLuint getShaderID() const;

            /**
             * @brief Returns the ID of the dynamic drawing VAO on the GPU
             * @return uint
             */
            GLuint getDynamicShaderVAO() const;

            /**
            * @brief Returns the ID of the dynamic drawing IBO on the GPU
            * @return uint
            */
            GLuint getDynamicShaderIBO() const;

            /**
             * @brief Returns the ID of the dynamic drawing VBO on the GPU
             * @return uint
             */
            GLuint getDynamicShaderVBO() const;

            /**
             * @brief Returns the ID of the static drawing VAO on the GPU
             * @return uint
             */
            GLuint getStaticShaderVAO() const;

            /**
            * @brief Returns the ID of the static drawing IBO on the GPU
            * @return uint
            */
            GLuint getStaticShaderIBO() const;

            /**
             * @brief Returns the ID of the static drawing VBO on the GPU
             * @return uint
             */
            GLuint getStaticShaderVBO() const;


            /**
             * @brief Returns the size of the VertexData structure in bytes.
             * @return long
             */
            long getShaderVertexDataSize() const;

			template<typename T>
			void setUniforms(const char* _uniformName, const UniformConfig<T>& _uniformConfig);

        private:
            /**
             * @brief Compiles the Shader code and sends it to the GPU. Returns true if compilation is successful.
             * @param _shaderCode Vertex or Fragment
             * @param _shaderType Type of shader, Vertex or Fragment
             * @return bool
             */
            bool initFromString(const std::string& _shaderCode, GLenum _shaderType);
    };

	template<typename T>
	void Shader::setUniforms(const char* _uniformName, const UniformConfig<T>& _uniformConfig) {
		if(uniforms[_uniformName] != -1) {
			if(_uniformConfig.uniformMatrixFunc != nullptr) {
				_uniformConfig.uniformMatrixFunc(uniforms[_uniformName], _uniformConfig.count, _uniformConfig.transpose, _uniformConfig.data);
			} else if(_uniformConfig.uniformNonMatrixFunc != nullptr) {
				_uniformConfig.uniformNonMatrixFunc(uniforms[_uniformName], _uniformConfig.count, _uniformConfig.data);
			}
		} else {
			Util::Log::error("Trying to set uniform '", _uniformName, "' but it couldn't be located!");
		}
	}
}

#endif //RDE_SHADER_H
