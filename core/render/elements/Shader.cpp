#include "Shader.h"
#include "core/util/Logger.h"
#include <iostream>
#include <fstream>

namespace engine {
    Shader::Shader() {  }

    Shader::~Shader() {
        // Only delete the shader index if it was initialized successfully.
        if (shader != 0) {
            glDeleteShader(shader);
        }
    }

    bool Shader::initFromFile(const std::string& _filePath, GLenum _shaderType) {

        std::ifstream _file(_filePath);
        if (!_file.good()) {
            LOG_E("Can't read file: ", _filePath)
            return false;
        }

        _file.seekg(0, std::ios::end);
        std::string _shaderCode;
        _shaderCode.resize((size_t)_file.tellg());
        _file.seekg(0, std::ios::beg);
        _file.read(&_shaderCode[0], (int)_shaderCode.size());
        _file.close();

        return initFromString(_shaderCode, _shaderType);
    }

    bool Shader::initFromString(const std::string& _shaderCode, GLenum _shaderType) {
        type = _shaderType;
        shader = glCreateShader(_shaderType);

        // Get the char* and length
        const char* _shaderCodePointer = _shaderCode.data();
        int _shaderCodeLength = (int)_shaderCode.size();

        // Set the source code and compile.
        glShaderSource(shader, 1, &_shaderCodePointer, &_shaderCodeLength);
        glCompileShader(shader);

        GLint _isCompiled;

        // Check if the fragmentShader compiles:
        // If it failed, print an error and delete it.
        glGetShaderiv(shader, GL_COMPILE_STATUS, &_isCompiled);

        if (!_isCompiled) {
            char _infolog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, _infolog);
            LOG_E("Shader compile failed with error: ", _infolog)

            // Delete the shader, and set the index to zero so that this object knows it doesn't have a shader.
            glDeleteShader(shader);
            shader = 0;
            return false;
        } else
            return true;
    }


    void Shader::attachTo(GLuint program) const {
        if (shader != 0)
            glAttachShader(program, shader);
        else
            LOG_E("Failed to attach shader: Shader not initialized.")
    }

}