#include "core/render/elements/Shader.h"
#include "core/util/Logger.h"
#include <iostream>

namespace engine {
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

    GLuint Shader::loadFromFiles(const std::string& _vertex, const std::string& _fragment) {
        std::string _vertexCode;
        std::string _fragmentCode;

        SDL_RWops* _vertexFile = SDL_RWFromFile(_vertex.c_str(), "rb");
        if (_vertexFile == nullptr) {
            LOG_E("Can't read file: ", _vertex)
            return false;
        }

        SDL_RWseek(_vertexFile, 0, RW_SEEK_END);
        _vertexCode.resize((size_t)SDL_RWtell(_vertexFile));
        SDL_RWseek(_vertexFile, 0, RW_SEEK_SET);
        SDL_RWread(_vertexFile, &_vertexCode[0], (int)_vertexCode.size(), (int)_vertexCode.size());
        SDL_RWclose(_vertexFile);

        SDL_RWops* _fragmentFile = SDL_RWFromFile(_fragment.c_str(), "rb");
        if (_fragmentFile == nullptr) {
            LOG_E("Can't read file: ", _fragment)
            return false;
        }

        SDL_RWseek(_fragmentFile, 0, RW_SEEK_END);
        _fragmentCode.resize((size_t)SDL_RWtell(_fragmentFile));
        SDL_RWseek(_fragmentFile, 0, RW_SEEK_SET);
        SDL_RWread(_fragmentFile, &_fragmentCode[0], (int)_fragmentCode.size(), (int)_fragmentCode.size());
        SDL_RWclose(_fragmentFile);

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