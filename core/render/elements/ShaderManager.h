// Created by borja on 20/1/22.


#ifndef ENGINE2_0_SHADER_MANAGER_H
#define ENGINE2_0_SHADER_MANAGER_H


#include "core/util/Util.h"
#include "Shader.h"

namespace engine {

    typedef GLuint ShaderID;

    class ShaderManager {
        private:
            std::unordered_map<std::string, Shader*> shaders;

        public:
            static ShaderManager& get();
            void init();
            void destroy();

            ShaderID addShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment);
            void removeShader(const std::string& _name);
            ShaderID getShader(const std::string& _name);
    };

}


#endif //ENGINE2_0_SHADER_MANAGER_H
