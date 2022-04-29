// Created by borja on 20/1/22.

#include "core/render/elements/ShaderManager.h"

namespace GDE {


    ShaderManager& ShaderManager::get() {
        static ShaderManager _shaderManager;
        return _shaderManager;
    }

    void ShaderManager::init() {
    #if IS_MOBILE()
        addShader("basic", TEXTURE_VERTEX_SHADER_ES, TEXTURE_FRAGMENT_SHADER_ES);
        addShader("debug", DEBUG_VERTEX_SHADER_ES, DEBUG_FRAGMENT_SHADER_ES);
        addShader("basicText", TEXTURE_VERTEX_SHADER_ES, TEXT_FRAGMENT_SHADER_ES);
//        addShader("outline", TEXTURE_VERTEX_SHADER_ES, OUTLINE_FRAGMENT_SHADER_ES);
        addShader("framebuffer", "assets/shaders/es/framebuffer/FrameBufferVertex.glsl", "assets/shaders/es/framebuffer/FrameBufferFragment.glsl");
    #else
        addShader("basic", TEXTURE_VERTEX_SHADER_CORE, TEXTURE_FRAGMENT_SHADER_CORE);
        addShader("debug", DEBUG_VERTEX_SHADER_CORE, DEBUG_FRAGMENT_SHADER_CORE);
        addShader("basicText", TEXTURE_VERTEX_SHADER_CORE, TEXT_FRAGMENT_SHADER_CORE);
        addShader("outline", TEXTURE_VERTEX_SHADER_CORE, OUTLINE_FRAGMENT_SHADER_CORE);
        addShader("grid", DEBUG_GRID_VERTEX_SHADER_CORE, DEBUG_GRID_FRAGMENT_SHADER_CORE);
//        addShader("mirror", MIRROR_VERTEX_SHADER_CORE, MIRROR_FRAGMENT_SHADER_CORE);
        addShader("framebuffer", FRAMEBUFFER_VERTEX_SHADER_CORE, FRAMEBUFFER_FRAGMENT_SHADER_CORE);
    #endif
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