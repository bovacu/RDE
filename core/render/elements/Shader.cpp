#include "shader.h"
#include <iostream>
#include <fstream>

namespace engine {
    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
        // Only delete the shader index if it was initialized successfully.
        if (m_shader != 0)
        {
            glDeleteShader(m_shader);
        }
    }

    bool Shader::InitFromFile(std::string filePath, GLenum shaderType)
    {

        std::ifstream file(filePath);

        // Check if the file exists
        if (!file.good())
        {
            // If we encounter an error, print a message and return false.
            std::cout << "Can't read file: " << filePath << std::endl;
            return false;
        }

        // ifstream internally keeps track of where in the file.

        // Here we find the end of the file.
        file.seekg(0, std::ios::end);

        // Make a string and set its size equal to the length of the file.
        std::string shaderCode;
        shaderCode.resize((size_t)file.tellg());

        // Go back to the beginning of the file.
        file.seekg(0, std::ios::beg);

        // Read the file into the string until we reach the end of the string.
        file.read(&shaderCode[0], shaderCode.size());

        // Close the file.
        file.close();

        // Init using the string.
        return InitFromString(shaderCode, shaderType);
    }

    bool Shader::InitFromString(std::string shaderCode, GLenum shaderType)
    {
        m_type = shaderType;
        m_shader = glCreateShader(shaderType);

        // Get the char* and length
        const char* shaderCodePointer = shaderCode.data();
        int shaderCodeLength = shaderCode.size();

        // Set the source code and compile.
        glShaderSource(m_shader, 1, &shaderCodePointer, &shaderCodeLength);
        glCompileShader(m_shader);

        GLint isCompiled;

        // Check if the fragmentShader compiles:
        // If it failed, print an error and delete it.
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &isCompiled);

        if (!isCompiled)
        {
            char infolog[1024];
            glGetShaderInfoLog(m_shader, 1024, NULL, infolog);
            std::cout << "Shader compile failed with error: " << std::endl << infolog << std::endl;

            // Delete the shader, and set the index to zero so that this object knows it doesn't have a shader.
            glDeleteShader(m_shader);
            m_shader = 0;
            return false;
        }
        else
        {
            return true;
        }
    }


    void Shader::AttachTo(GLuint program)
    {
        // Attach the owned shader to the given shader program.
        if (m_shader != 0)
        {
            glAttachShader(program, m_shader);
        }
        else
        {
            // Print an error if trying to attach an uninitialized shader.
            std::cout << "Failed to attach shader: Shader not initialized." << std::endl;
        }
    }

}