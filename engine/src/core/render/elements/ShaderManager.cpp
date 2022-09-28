// Created by borja on 20/1/22.

#include "core/render/elements/ShaderManager.h"
#include "glm/gtc/type_ptr.hpp"
#include "core/util/GLUtil.h"
#include "core/render/elements/IRenderizable.h"

namespace GDE {

    void ShaderManager::init(FileManager* _fileManager) {
        fileManager = _fileManager;
        #if IS_MOBILE()
        IRenderizable::defaultShaders[SPRITE_RENDERER_SHADER] = loadShader(SPRITE_RENDERER_SHADER, TEXTURE_VERTEX_SHADER_ES, TEXTURE_FRAGMENT_SHADER_ES);
        IRenderizable::defaultShaders[DEBUG_SHADER] = loadShader(DEBUG_SHADER, DEBUG_VERTEX_SHADER_ES, DEBUG_FRAGMENT_SHADER_ES);
        IRenderizable::defaultShaders[TEXT_RENDERER_SHADER] = loadShader(TEXT_RENDERER_SHADER, TEXTURE_VERTEX_SHADER_ES, TEXT_FRAGMENT_SHADER_ES);
        IRenderizable::defaultShaders[FRAMEBUFFER_SHADER] = loadShader(FRAMEBUFFER_SHADER, FRAMEBUFFER_VERTEX_SHADER_CORE, FRAMEBUFFER_FRAGMENT_SHADER_CORE);
        #else
        IRenderizable::defaultShaders[SPRITE_RENDERER_SHADER] = loadShader(SPRITE_RENDERER_SHADER, TEXTURE_VERTEX_SHADER_CORE, TEXTURE_FRAGMENT_SHADER_CORE);
        IRenderizable::defaultShaders[DEBUG_SHADER] = loadShader(DEBUG_SHADER, DEBUG_VERTEX_SHADER_CORE, DEBUG_FRAGMENT_SHADER_CORE);
        IRenderizable::defaultShaders[TEXT_RENDERER_SHADER] = loadShader(TEXT_RENDERER_SHADER, TEXTURE_VERTEX_SHADER_CORE, TEXT_FRAGMENT_SHADER_CORE);
        IRenderizable::defaultShaders[FRAMEBUFFER_SHADER] = loadShader(FRAMEBUFFER_SHADER, FRAMEBUFFER_VERTEX_SHADER_CORE, FRAMEBUFFER_FRAGMENT_SHADER_CORE);
    #endif
    CHECK_GL_ERROR("Shader Initializaation")
    }

    ShaderID ShaderManager::loadShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment) {
        auto* _shader = new Shader;
        _shader->loadFromFiles(_vertex, _fragment, fileManager);
        shadersByName[_shaderName] = _shader;
        shadersById[shadersByName[_shaderName]->getShaderID()] = _shader;
        LOG_DEBUG("Created Shader ", _shaderName, " with value ", shadersByName[_shaderName]->getShaderID())
        return shadersByName[_shaderName]->getShaderID();
    }

    void ShaderManager::unloadShader(const std::string& _name) {
        auto _shaderID = shadersByName[_name]->getShaderID();
        delete shadersByName[_name];
        shadersByName.erase(_name);
        shadersById.erase(_shaderID);
    }

    Shader* ShaderManager::getShader(const std::string& _name) {
        return shadersByName[_name];
    }

    Shader* ShaderManager::getShader(const ShaderID& _shaderID) {
        return shadersById[_shaderID];
    }

    void ShaderManager::loadShaderVertexConfig(const std::string& _shaderName,const std::vector<VertexConfig>& _vertexConfig, int _maxIndicesPerDrawCall) {
        shadersByName[_shaderName]->loadVertexConfig(_vertexConfig, _maxIndicesPerDrawCall);
    }

    void ShaderManager::destroy() {
        LOG_DEBUG("Cleaning up ShaderManager")
        for(auto& _shader : shadersByName) {
            delete _shader.second;
        }
    }

    void ShaderManager::setInt(ShaderID _shaderID, const std::string& _uniformName, int _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform1i(location, _value);
        glUseProgram(0);
    }

    void ShaderManager::setInt2(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec2& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform2i(location, _value.x, _value.y);
        glUseProgram(0);
    }

    void ShaderManager::setInt3(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec3& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform3i(location, _value.x, _value.y, _value.z);
        glUseProgram(0);
    }

    void ShaderManager::setInt4(ShaderID _shaderID, const std::string& _uniformName, const glm::ivec4& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform4i(location, _value.x, _value.y, _value.z, _value.w);
        glUseProgram(0);
    }

    void ShaderManager::setFloat(ShaderID _shaderID, const std::string& _uniformName, float _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform1f(location, _value);
        glUseProgram(0);
    }

    void ShaderManager::setFloat2(ShaderID _shaderID, const std::string& _uniformName, const glm::vec2& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform2f(location, _value.x, _value.y);
        glUseProgram(0);
    }

    void ShaderManager::setFloat3(ShaderID _shaderID, const std::string& _uniformName, const glm::vec3& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform3f(location, _value.x, _value.y, _value.z);
        glUseProgram(0);
    }

    void ShaderManager::setFloat4(ShaderID _shaderID, const std::string& _uniformName, const glm::vec4& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniform4f(location, _value.x, _value.y, _value.z, _value.w);
        glUseProgram(0);
    }

    void ShaderManager::setMat2(ShaderID _shaderID, const std::string& _uniformName, const glm::mat2& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix2fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(_value)));
        glUseProgram(0);
    }

    void ShaderManager::setMat3(ShaderID _shaderID, const std::string& _uniformName, const glm::mat3& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(_value)));
        glUseProgram(0);
    }

    void ShaderManager::setMat4(ShaderID _shaderID, const std::string& _uniformName, const glm::mat4& _value) const {
        glUseProgram(_shaderID);
        GLint location = glGetUniformLocation(_shaderID, _uniformName.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(glm::value_ptr(_value)));
        glUseProgram(0);
    }
}