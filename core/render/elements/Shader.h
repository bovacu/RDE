// Created by borja on 24/12/21.


#ifndef ENGINE2_0_SHADER_H
#define ENGINE2_0_SHADER_H

#include "core/util/Util.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace engine {

    class Shader;
    typedef std::shared_ptr<Shader> ShaderPtr;

    class Shader {
        private:
            uint32_t        renderID = 0;
            std::string     name;

        private:
            std::string readFile(const std::string& _filepath);
            std::unordered_map<GLenum, std::string> preProcess(const std::string& _source);
            void compile(const std::unordered_map<GLenum, std::string>& _shaderSources);

        public:
            Shader(const std::string& _filepath);
            Shader(const std::string& _name, const std::string& _vertexSrc, const std::string& _fragmentSrc);
            ~Shader();
    
            void bind() const;
            void unbind() const;
    
            void setInt(const std::string& _name, int _value);
            void setIntArray(const std::string& _name, int* _values, uint32_t _count);
            void setFloat(const std::string& _name, float _value);
            void setFloat3(const std::string& _name, const glm::vec3& _value);
            void setFloat2(const std::string& _name, const glm::vec2& _value);
            void setFloat4(const std::string& _name, const glm::vec4& _value);
            void setMat4(const std::string& _name, const glm::mat4& _value);

            void uploadUniformInt(const std::string& _name, int _value) const;
            void uploadUniformIntArray(const std::string& _name, int* _values, uint32_t _count) const;

            void uploadUniformFloat(const std::string& _name, float _value) const;
            void uploadUniformFloat2(const std::string& _name, const glm::vec2& _value) const;
            void uploadUniformFloat3(const std::string& _name, const glm::vec3& _value) const;
            void uploadUniformFloat4(const std::string& _name, const glm::vec4& _value) const;

            void uploadUniformMat3(const std::string& _name, const glm::mat3& _matrix) const;
            void uploadUniformMat4(const std::string& _name, const glm::mat4& _matrix) const;

            const std::string& getName() const;

            static ShaderPtr create(const std::string& _filepath);
            static ShaderPtr create(const std::string& _name, const std::string& _vertexSrc, const std::string& _fragmentSrc);
    };

    class ShaderLibrary;
    typedef std::shared_ptr<ShaderLibrary> ShaderLibraryPtr;

    class ShaderLibrary {
        private:
        std::unordered_map<std::string, ShaderPtr> shaders;

        public:
        void add(const std::string& _name, const ShaderPtr& _shader);
        void add(const ShaderPtr& _shader);
        ShaderPtr load(const std::string& _filepath);
        ShaderPtr load(const std::string& _name, const std::string& _filepath);

        ShaderPtr get(const std::string& _name);

        bool exists(const std::string& _name) const;
    };
}


#endif //ENGINE2_0_SHADER_H
