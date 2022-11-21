// Created by borja on 20/1/22.


#ifndef RDE_SHADER_MANAGER_H
#define RDE_SHADER_MANAGER_H


#include "core/util/Util.h"
#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "core/systems/fileSystem/FileManager.h"

namespace RDE {

    typedef GLuint ShaderID;

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief TThis class is in charge of compiling and storing the references of all of the shaders.
     */
    class ShaderManager {
        private:
            /**
             * @brief Map with all of the loaded Shaders on the GPU.
             */
            std::unordered_map<std::string, Shader*> shadersByName;
            std::unordered_map<ShaderID, Shader*> shadersById;
            FileManager* fileManager = nullptr;

        public:
            /**
             * @attention This is not meant to be called by end-users.
             * @brief Initializes all the crucial elements of the Shader Manager.
             */
            void init(FileManager* _fileManager);

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Destroys all of the Shaders from the GPU.
             */
            void destroy();

            /**
             * @brief Loads a shader in the GPU.
             * @param _shaderName the name you will use later to reference the shader.
             * @param _vertex the path to the vertex shader code.
             * @param _fragment the path to the fragment shader code.
             */
            ShaderID loadShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment);

            /**
             * @brief Unloads a shader from the GPU.
             * @param _name Shader name
             */
            void unloadShader(const std::string& _name);

            /**
             * @brief Returns the shader.
             * @param _name Shader name
             * @return Shader
             */
            Shader* getShader(const std::string& _name);

            /**
             * @brief Returns the shader.
             * @param _shadeID Shader ID
             * @return Shader
             */
            Shader* getShader(const ShaderID& _shaderID);

            /**
             * @brief Sets the VertexData of a shader. Call this method only AFTER loadShader
             * @param _shaderName the name of the shader.
             * @param _vertexConfig List of data for each vertex.
             * @param _maxIndicesPerDrawCall Maximum number of indices per drawBatched call.
             */
            void loadShaderVertexConfig(const std::string& _shaderName, const std::vector<VertexConfig>& _vertexConfig, int _maxIndicesPerDrawCall);

            /**
             * @brief Sets an 'int' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setInt(ShaderID _shaderID, const std::string& _uniformName, int _value) const;

            /**
             * @brief Sets an 'int2' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setInt2(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec2& _value) const;

            /**
             * @brief Sets an 'int3' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setInt3(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec3& _value) const;

            /**
             * @brief Sets an 'int4' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setInt4(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec4& _value) const;

            /**
             * @brief Sets a 'float' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setFloat(ShaderID _shaderID, const std::string& _uniformName, float _value) const;

            /**
             * @brief Sets a 'float2' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setFloat2(ShaderID _shaderID, const std::string& _uniformName, const glm::vec2& _value) const;

            /**
             * @brief Sets a 'float3' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setFloat3(ShaderID _shaderID, const std::string& _uniformName, const glm::vec3& _value) const;

            /**
             * @brief Sets a 'float4' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setFloat4(ShaderID _shaderID, const std::string& _uniformName, const glm::vec4& _value) const;

            /**
             * @brief Sets a 'mat2' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setMat2(ShaderID _shaderID, const std::string& _uniformName, const glm::mat2& _value) const;

            /**
             * @brief Sets a 'mat3' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setMat3(ShaderID _shaderID, const std::string& _uniformName, const glm::mat3& _value) const;

            /**
             * @brief Sets a 'mat4' uniform for a specific shader
             * @param _shaderID Shader that this value is uploaded to.
             * @param _uniformName Name of the uniform to upload the value.
             * @param _value Value to upload
             */
            void setMat4(ShaderID _shaderID, const std::string& _uniformName, const glm::mat4& _value) const;
    };

}


#endif //RDE_SHADER_MANAGER_H
