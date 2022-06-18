// Created by borja on 20/1/22.


#ifndef ENGINE2_0_SHADER_MANAGER_H
#define ENGINE2_0_SHADER_MANAGER_H


#include "core/util/Util.h"
#include "Shader.h"

namespace GDE {

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
            std::unordered_map<std::string, Shader*> shaders;

        public:
            /**
             * @attention This is not meant to be called by end-users.
             * @brief Initializes all the crucial elements of the Shader Manager.
             */
            void init();

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
             * @brief Returns the OpenGL reference to the shader in memory.
             * @param _name Shader name
             * @return ShaderID
             */
            ShaderID getShader(const std::string& _name);
    };

}


#endif //ENGINE2_0_SHADER_MANAGER_H
