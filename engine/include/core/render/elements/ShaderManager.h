// Created by borja on 20/1/22.


#ifndef ENGINE2_0_SHADER_MANAGER_H
#define ENGINE2_0_SHADER_MANAGER_H


#include "core/util/Util.h"
#include "Shader.h"

namespace GDE {

    typedef GLuint ShaderID;

    /// This class is in charge of compiling and storing the references of all of the shaders.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class ShaderManager {
        private:
            std::unordered_map<std::string, Shader*> shaders;

        public:
            /// This should not be called by end users, the engine calls it when needed.
            void init();

            /// This should not be called by end users, the engine calls it when needed.
            void destroy();

            /// Loads a shader in the GPU.
            /// @param _shaderName the name you will use later to reference the shader.
            /// @param _vertex the path to the vertex shader code.
            /// @param _fragment the path to the fragment shader code.
            ShaderID loadShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment);

            /// Unloads a shader from the GPU.
            void unloadShader(const std::string& _name);

            /// Returns the OpenGL reference to the shader in memory.
            ShaderID getShader(const std::string& _name);
    };

}


#endif //ENGINE2_0_SHADER_MANAGER_H
