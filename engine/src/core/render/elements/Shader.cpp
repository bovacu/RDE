#include "core/render/elements/Shader.h"
#include "core/util/Logger.h"
#include <iostream>

namespace GDE {
    Shader::Shader() {  }

    Shader::~Shader() {
        for(auto& _attachedShader : shadersAttached)
            glDeleteShader(_attachedShader.second);
        glDeleteProgram(shaderID);
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
        return shaderID;
    }

}