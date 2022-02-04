// Created by borja on 20/1/22.


#include "ShaderManager.h"

namespace engine {


    ShaderManager& ShaderManager::get() {
        static ShaderManager _shaderManager;
        return _shaderManager;
    }

    void ShaderManager::init() {
        addShader("basic", TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER);
        addShader("debug", DEBUG_VERTEX_SHADER, DEBUG_FRAGMENT_SHADER);
        addShader("basicText", TEXTURE_VERTEX_SHADER, TEXT_FRAGMENT_SHADER);
        addShader("outline", TEXTURE_VERTEX_SHADER, OUTLINE_FRAGMENT_SHADER);
        addShader("framebuffer", "assets/shaders/framebuffer/FrameBufferVertex.glsl", "assets/shaders/framebuffer/FrameBufferFragment.glsl");
    }

    ShaderID ShaderManager::addShader(const std::string& _shaderName, const std::string& _vertex, const std::string& _fragment) {
        auto* _shader = new Shader;
        _shader->loadFromFiles(_vertex, _fragment);
        shaders[_shaderName] = _shader;
        LOG_S("Created Shader ", _shaderName, " with value ", shaders[_shaderName]->getShaderID())
        return shaders[_shaderName]->getShaderID();
    }

    void ShaderManager::removeShader(const std::string& _name) {
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
}