// Created by borja on 20/1/22.

#include "core/render/elements/ShaderManager.h"
#include "glm/gtc/type_ptr.hpp"

namespace GDE {

    void ShaderManager::init() {
    #if IS_MOBILE()
        loadShader("basic", TEXTURE_VERTEX_SHADER_ES, TEXTURE_FRAGMENT_SHADER_ES);
        loadShader("debug", DEBUG_VERTEX_SHADER_ES, DEBUG_FRAGMENT_SHADER_ES);
        loadShader("basicText", TEXTURE_VERTEX_SHADER_ES, TEXT_FRAGMENT_SHADER_ES);
        loadShader("framebuffer", "assets/shaders/es/framebuffer/FrameBufferVertex.glsl", "assets/shaders/es/framebuffer/FrameBufferFragment.glsl");
    #else
        loadShader("basic", TEXTURE_VERTEX_SHADER_CORE, TEXTURE_FRAGMENT_SHADER_CORE);
        loadShader("debug", DEBUG_VERTEX_SHADER_CORE, DEBUG_FRAGMENT_SHADER_CORE);
        loadShader("basicText", TEXTURE_VERTEX_SHADER_CORE, TEXT_FRAGMENT_SHADER_CORE);
        loadShader("outline", TEXTURE_VERTEX_SHADER_CORE, OUTLINE_FRAGMENT_SHADER_CORE);
        loadShader("grid", DEBUG_GRID_VERTEX_SHADER_CORE, DEBUG_GRID_FRAGMENT_SHADER_CORE);
        loadShader("framebuffer", FRAMEBUFFER_VERTEX_SHADER_CORE, FRAMEBUFFER_FRAGMENT_SHADER_CORE);
    #endif
    }

    ShaderID ShaderManager::loadShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment) {
        auto* _shader = new Shader;
        _shader->loadFromFiles(_vertex, _fragment);
        shaders[_shaderName] = _shader;
        LOG_S("Created Shader ", _shaderName, " with value ", shaders[_shaderName]->getShaderID())
        return shaders[_shaderName]->getShaderID();
    }

    void ShaderManager::unloadShader(const std::string& _name) {
        delete shaders[_name];
        shaders.erase(_name);
    }

    ShaderID ShaderManager::getShader(const std::string& _name) {
        return shaders[_name]->getShaderID();
    }

    void ShaderManager::destroy() {
        LOG_S("Cleaning up Shader Manager")
        for(auto& _shader : shaders) {
            LOG_I("     Cleaning ", _shader.first)
            delete _shader.second;
        }
    }

    void ShaderManager::setInt(ShaderID _shaderID, const std::string& _uniformName, int _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform1i(location, _value);
    }

    void ShaderManager::setInt2(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec2& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform2i(location, _value.x, _value.y);
    }

    void ShaderManager::setInt3(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec3& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform3i(location, _value.x, _value.y, _value.z);
    }

    void ShaderManager::setInt4(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec4& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform4i(location, _value.x, _value.y, _value.z, _value.w);
    }

    void ShaderManager::setFloat(ShaderID _shaderID, const std::string& _uniformName, float _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform1f(location, _value);
    }

    void ShaderManager::setFloat2(ShaderID _shaderID, const std::string& _uniformName, const glm::vec2& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform2f(location, _value.x, _value.y);
    }

    void ShaderManager::setFloat3(ShaderID _shaderID, const std::string& _uniformName, const glm::vec3& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform3f(location, _value.x, _value.y, _value.z);
    }

    void ShaderManager::setFloat4(ShaderID _shaderID, const std::string& _uniformName, const glm::vec4& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform4f(location, _value.x, _value.y, _value.z, _value.w);
    }

    void ShaderManager::setMat2(ShaderID _shaderID, const std::string& _uniformName, const glm::mat2& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(_value));
    }

    void ShaderManager::setMat3(ShaderID _shaderID, const std::string& _uniformName, const glm::mat3& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(_value));
    }

    void ShaderManager::setMat4(ShaderID _shaderID, const std::string& _uniformName, const glm::mat4& _value) const {
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(_value));
    }
}