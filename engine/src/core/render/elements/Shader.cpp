#include "core/render/elements/Shader.h"
#include "core/util/Logger.h"
#include <iostream>

namespace GDE {

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
        ENGINE_ASSERT(!_verticesConfig.empty(), "Cannot have a Shader with 0 vertices configs")
        const int NUMBER_OF_INDICES = 6;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        auto _structSize = _verticesConfig[0].structSize;
        vertexDataSize = _structSize;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, _structSize * _maxIndicesPerDrawCall, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(sizeof(uint32_t) * _maxIndicesPerDrawCall * NUMBER_OF_INDICES), nullptr, GL_DYNAMIC_DRAW);

        for(auto& _vertexConfig : _verticesConfig) {
            glVertexAttribPointer(_vertexConfig.pointerIndex, _vertexConfig.numberOfElements, _vertexConfig.openglDataType, GL_FALSE, _structSize, reinterpret_cast<const void*>(_vertexConfig.stride));
            glEnableVertexAttribArray(_vertexConfig.pointerIndex);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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
            LOG_E("Shader compile failed with error: ", _infolog)

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
            LOG_E("Can't read file: ", _vertex)
            return false;
        }
        _vertexCode = _fileManager->readFullFile(_vertexFile).content;
        _fileManager->close(_vertexFile);

        auto* _fragmentFile = _fileManager->open(_fragment, FileMode::READ);
        if (_fragmentFile == nullptr) {
            LOG_E("Can't read file: ", _fragment)
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
        #if DEBUG
        if(shaderID == -1) {
            throw LoadVertexConfigNotInvoked();
        }
        #endif
        return shaderID;
    }

    GLuint Shader::getShaderVAO() const {
        return vao;
    }

    GLuint Shader::getShaderIBO() const {
        return ibo;
    }

    GLuint Shader::getShaderVBO() const {
        return vbo;
    }

    long Shader::getShaderVertexDataSize() const {
        return vertexDataSize;
    }

}