#include "core/render/elements/Shader.h"
#include <iostream>

namespace RDE {

    Shader::Shader() {  }

    Shader::~Shader() {
        for(auto& _attachedShader : shadersAttached)
            glDeleteShader(_attachedShader.second);
        glDeleteProgram(shaderID);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
    }

    void Shader::loadVertexConfig(const std::vector<VertexConfig>& _verticesConfig, int _maxIndicesPerDrawCall) {
        vertexDataSize = _verticesConfig[0].structSize;
        loadVertexConfigSpecific(_verticesConfig, _maxIndicesPerDrawCall, GL_STATIC_DRAW, vbo, ibo, vao);
//        loadVertexConfigSpecific(_verticesConfig, _maxIndicesPerDrawCall, GL_STATIC_DRAW, staticVbo, staticIbo, staticVao);
    }

    bool Shader::initFromString(const std::string& _shaderCode, GLenum _shaderType) {
        GLuint _shaderID = glCreateShader(_shaderType);

        // Get the char* and length
        const char* _shaderCodePointer = _shaderCode.c_str();
        int _shaderCodeLength = (int)_shaderCode.size();

        // Set the source code and compile.
        glShaderSource(_shaderID, 1, &_shaderCodePointer, &_shaderCodeLength);
        glCompileShader(_shaderID);

        GLint _isCompiled;
        glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &_isCompiled);

        if (!_isCompiled) {
            char _infolog[1024];
            glGetShaderInfoLog(_shaderID, 1024, nullptr, _infolog);
            Util::Log::error("Shader compile failed with error: ", _infolog);

            // Delete the shader, and set the index to zero so that this object knows it doesn't have a shader.
            glDeleteShader(_shaderID);
            _shaderID = 0;
            return false;
        }

        shadersAttached[_shaderType] = _shaderID;
        return true;
    }

    GLuint Shader::loadFromFiles(const std::string& _vertex, const std::string& _fragment, FileManager* _fileManager) {
        std::string _vertexCode;
        std::string _fragmentCode;
        
        auto* _vertexFile = _fileManager->open(_vertex, FileMode::READ);
        if (_vertexFile == nullptr) {
            Util::Log::error("Can't read file: ", _vertex);
            return false;
        }
        _vertexCode = _fileManager->readFullFile(_vertexFile).content;
        _fileManager->close(_vertexFile);

        auto* _fragmentFile = _fileManager->open(_fragment, FileMode::READ);
        if (_fragmentFile == nullptr) {
            Util::Log::info("Can't read file: ", _fragment);
            return false;
        }

        _fragmentCode = _fileManager->readFullFile(_fragmentFile).content;
        _fileManager->close(_fragmentFile);

        return loadFromStrings(_vertexCode, _fragmentCode);
    }

    GLuint Shader::loadFromStrings(const std::string& _vertex, const std::string& _fragment) {
        initFromString(_vertex, GL_VERTEX_SHADER);
        initFromString(_fragment, GL_FRAGMENT_SHADER);

        GLuint _shaderID = glCreateProgram();
        for(auto& _shaderAttached : shadersAttached)
            glAttachShader(_shaderID, _shaderAttached.second);

        glLinkProgram(_shaderID);
        shaderID = _shaderID;
        return _shaderID;
    }

    GLuint Shader::getShaderID() const {
        #ifdef ENGINE_DEBUG
        if(shaderID == -1) {
            throw LoadVertexConfigNotInvoked();
        }
        #endif
        return shaderID;
    }

    GLuint Shader::getDynamicShaderVAO() const {
        return vao;
    }

    GLuint Shader::getDynamicShaderIBO() const {
        return ibo;
    }

    GLuint Shader::getDynamicShaderVBO() const {
        return vbo;
    }

    long Shader::getShaderVertexDataSize() const {
        return vertexDataSize;
    }

    GLuint Shader::getStaticShaderVAO() const {
        return staticVao;
    }

    GLuint Shader::getStaticShaderIBO() const {
        return staticIbo;
    }

    GLuint Shader::getStaticShaderVBO() const {
        return staticVbo;
    }

    void Shader::loadVertexConfigSpecific(const std::vector<VertexConfig>& _verticesConfig, int _maxIndicesPerDrawCall, GLenum _drawType, GLuint& _vbo, GLuint& _ibo, GLuint& _vao) {
        ENGINE_ASSERT(!_verticesConfig.empty(), "Cannot have a Shader with 0 vertices configs")
        const int NUMBER_OF_VERTICES = 6;

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glGenBuffers(1, &_vbo);


        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize * _maxIndicesPerDrawCall * NUMBER_OF_VERTICES, nullptr, _drawType);
        Util::Log::debug("Shader ", shaderID, " total VB space = ", vertexDataSize * _maxIndicesPerDrawCall * NUMBER_OF_VERTICES);

        for(auto& _vertexConfig : _verticesConfig) {
            glVertexAttribPointer(_vertexConfig.pointerIndex, _vertexConfig.numberOfElements, _vertexConfig.openglDataType, GL_FALSE, vertexDataSize, reinterpret_cast<const void*>(_vertexConfig.stride));
            glEnableVertexAttribArray(_vertexConfig.pointerIndex);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

}