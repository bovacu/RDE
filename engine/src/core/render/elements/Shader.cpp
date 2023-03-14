#include "core/render/elements/Shader.h"
#include <iostream>
#include "core/util/Functions.h"

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

	void Shader::loadVertexConfig(const std::vector<VertexConfig>& _verticesConfig, const std::vector<const char*> _uniforms, int _maxIndicesPerDrawCall) {
        vertexDataSize = _verticesConfig[0].structSize;
        loadVertexConfigSpecific(_verticesConfig, _uniforms, _maxIndicesPerDrawCall, GL_STATIC_DRAW, vbo, ibo, vao);
//        loadVertexConfigSpecific(_verticesConfig, _uniforms, _maxIndicesPerDrawCall, GL_STATIC_DRAW, staticVbo, staticIbo, staticVao);
    }

    bool Shader::initFromString(const std::string& _shaderCode, GLenum _shaderType) {
        GLuint _shaderID = glCreateShader(_shaderType);
		Util::GL::checkError("Error after init from string");

        // Get the char* and length
        const char* _shaderCodePointer = _shaderCode.c_str();
        int _shaderCodeLength = (int)_shaderCode.size();

        // Set the source code and compile.
        glShaderSource(_shaderID, 1, &_shaderCodePointer, &_shaderCodeLength);
		Util::GL::checkError("Error after ShaderSource");

        glCompileShader(_shaderID);
		Util::GL::checkError("Error after Compile shader");

        GLint _isCompiled;
        glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &_isCompiled);
		Util::GL::checkError("Error after getShaderiv");

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
        
		auto* _vertexFile = _fileManager->open(_vertex, RDE_FILE_MODE_READ);
        if (_vertexFile == nullptr) {
            Util::Log::error("Can't read file: ", _vertex);
            return false;
        }
        _vertexCode = _fileManager->readFullFile(_vertexFile).content;
        _fileManager->close(_vertexFile);

		auto* _fragmentFile = _fileManager->open(_fragment, RDE_FILE_MODE_READ);
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
		for(auto& _shaderAttached : shadersAttached) {
			glAttachShader(_shaderID, _shaderAttached.second);
			Util::GL::checkError("Error after shaderAttach");
		}

        glLinkProgram(_shaderID);
		Util::GL::checkError("Error after linkProgram");
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

	void Shader::loadVertexConfigSpecific(const std::vector<VertexConfig>& _verticesConfig, const std::vector<const char*> _uniforms, int _maxIndicesPerDrawCall, GLenum _drawType, GLuint& _vbo, GLuint& _ibo, GLuint& _vao) {
        ENGINE_ASSERT(!_verticesConfig.empty(), "Cannot have a Shader with 0 vertices configs")
        const int NUMBER_OF_VERTICES = 6;

		Util::GL::checkError("Error BEFORE GenVertexArrays");
        glGenVertexArrays(1, &_vao);
		Util::GL::checkError("Error after GenVertexArrays");
        glBindVertexArray(_vao);
		Util::GL::checkError("Error after glBindVertexArray");
        glGenBuffers(1, &_vbo);


        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		Util::GL::checkError("Error after glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize * _maxIndicesPerDrawCall * NUMBER_OF_VERTICES, nullptr, _drawType);
		Util::GL::checkError("Error glBufferData");
		Util::Log::debug("Shader ", shaderID, " total VB space = ", vertexDataSize * _maxIndicesPerDrawCall * NUMBER_OF_VERTICES);

        for(auto& _vertexConfig : _verticesConfig) {
            glVertexAttribPointer(_vertexConfig.pointerIndex, _vertexConfig.numberOfElements, _vertexConfig.openglDataType, GL_FALSE, vertexDataSize, reinterpret_cast<const void*>(_vertexConfig.stride));
			Util::GL::checkError("Error after glVertexAttribPointer");
			glEnableVertexAttribArray(_vertexConfig.pointerIndex);
			Util::GL::checkError("Error glEnableVertexAttribArray");
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

		glUseProgram(shaderID);
		for(auto& _uniform : _uniforms) {
			uniforms[_uniform] = glGetUniformLocation(shaderID, _uniform);
			if(uniforms[_uniform] == -1) {
				Util::Log::error("Uniform with name ", _uniform, " couldn't be located!");
			}
		}
		glUseProgram(0);
    }

	void Shader::setUniformValueFloat(const char* _uniformName, RDE_UNIFORM_FV_ _type, GLfloat* _data, GLboolean _transpose) {
		if(uniforms[_uniformName] != -1) {
			auto _location = uniforms[_uniformName];
			switch(_type) {
				case RDE_UNIFORM_FV_1 		   : glUniform1fv(_location, 1, _data); break;
				case RDE_UNIFORM_FV_2 		   : glUniform2fv(_location, 1, _data); break;
				case RDE_UNIFORM_FV_3 		   : glUniform3fv(_location, 1, _data); break;
				case RDE_UNIFORM_FV_4 		   : glUniform4fv(_location, 1, _data); break;
				case RDE_UNIFORM_FV_MATRIX_2   : glUniformMatrix2fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_3   : glUniformMatrix3fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_4   : glUniformMatrix4fv(_location, 1, _transpose, _data); break;
				
				#ifndef __EMSCRIPTEN__
				case RDE_UNIFORM_FV_MATRIX_2x3 : glUniformMatrix2x3fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_3x2 : glUniformMatrix3x2fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_3x4 : glUniformMatrix3x4fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_4x3 : glUniformMatrix4x3fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_2x4 : glUniformMatrix2x4fv(_location, 1, _transpose, _data); break;
				case RDE_UNIFORM_FV_MATRIX_4x2 : glUniformMatrix4x2fv(_location, 1, _transpose, _data); break;
				#endif
			}
		} else {
			Util::Log::error("Trying to set uniform '", _uniformName, "' but it couldn't be located!");
		}	
	}

	void Shader::setUniformValueInt(const char* _uniformName, RDE_UNIFORM_IV_ _type, GLint* _data) {
		if(uniforms[_uniformName] != -1) {
			auto _location = uniforms[_uniformName];
			switch(_type) {
				case RDE_UNIFORM_IV_1 		   : glUniform1iv(_location, 1, _data); break;
				case RDE_UNIFORM_IV_2 		   : glUniform2iv(_location, 1, _data); break;
				case RDE_UNIFORM_IV_3 		   : glUniform3iv(_location, 1, _data); break;
				case RDE_UNIFORM_IV_4 		   : glUniform4iv(_location, 1, _data); break;
			}
		} else {
			Util::Log::error("Trying to set uniform '", _uniformName, "' but it couldn't be located!");
		}	
	}

	void Shader::setUniformValueUInt(const char* _uniformName, RDE_UNIFORM_UIV_ _type, GLuint* _data) {
		if(uniforms[_uniformName] != -1) {
			auto _location = uniforms[_uniformName];
			switch(_type) {
				case RDE_UNIFORM_UIV_1 		   : glUniform1uiv(_location, 1, _data); break;
				case RDE_UNIFORM_UIV_2 		   : glUniform2uiv(_location, 1, _data); break;
				case RDE_UNIFORM_UIV_3 		   : glUniform3uiv(_location, 1, _data); break;
				case RDE_UNIFORM_UIV_4 		   : glUniform4uiv(_location, 1, _data); break;
			}
		} else {
			Util::Log::error("Trying to set uniform '", _uniformName, "' but it couldn't be located!");
		}	
	}
}