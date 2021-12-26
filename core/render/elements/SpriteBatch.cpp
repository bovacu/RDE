#include "spriteBatcher.h"
#include "core/util/Logger.h"
#include "GL/glew.h"

namespace engine {
    SpriteBatcher::SpriteBatcher(glm::vec2 screenSize)
    {
        m_vertexShader.InitFromFile("assets/vertex.glsl", GL_VERTEX_SHADER);
        m_fragmentShader.InitFromFile("assets/fragment.glsl", GL_FRAGMENT_SHADER);

        // Create a shader program.
        m_shaderProgram = glCreateProgram();

        // Attach the vertex and fragment shaders to our program.
        m_vertexShader.AttachTo(m_shaderProgram);
        m_fragmentShader.AttachTo(m_shaderProgram);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );

        // Build full shader program.
        glLinkProgram(m_shaderProgram);

        // Get the texture uniform from the shader program.
        m_textureUniform = glGetUniformLocation(m_shaderProgram, "tex");
        texture = nullptr;

        // Setup vertex buffer
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)sizeof(glm::vec2));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)sizeof(glm::vec4));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Setup things related to the screen Transform
        m_screenTransformUniform = glGetUniformLocation(m_shaderProgram, "screenTransform");
        SetScreenSize(screenSize);
    }

    SpriteBatcher::~SpriteBatcher()
    {
        glDeleteProgram(m_shaderProgram);
    }



    void SpriteBatcher::Draw(glm::vec4 destRect, glm::vec4 sourceRect, glm::vec4 color, Texture* _texture)
    {
        // Draw doesn't actually have to draw anything.
        // Instead it collects a bunch of vertex information until flush is called, and then draws it all.

        // In one draw call we can only render sprites that have the same _texture in.
        // If we get something with a different _texture, we have to flush and start over.
        if (texture != _texture)
        {
            // Draw everything in the current buffer.
            Flush();


            // Make sure the _texture exists so long as we are holding on to it.
            _texture->IncRefCount();
            if (texture != nullptr)
            {
                texture->DecRefCount();
            }
            // Switch textures
            texture = _texture;
        }

        // Create vertices for the given draw request and add them to the buffer.
        // A more complex spritebatcher may allow for rotation of vertices, which would also happen here
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y), glm::vec2(sourceRect.x, sourceRect.y), color);
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y), glm::vec2(sourceRect.z, sourceRect.y), color);
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y + destRect.w), glm::vec2(sourceRect.x, sourceRect.w), color);
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y), glm::vec2(sourceRect.z, sourceRect.y), color);
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y + destRect.w), glm::vec2(sourceRect.x, sourceRect.w), color);
        m_vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y + destRect.w), glm::vec2(sourceRect.z, sourceRect.w), color);
    }

    void SpriteBatcher::Flush()
    {
        // If there's a false alarm, don't draw anything
        // (this will always happen on the first texture)
        if (m_vertexBuffer.empty() || texture == nullptr)
            return;

        // Set the current shader program.
        glUseProgram(m_shaderProgram);


        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture());
        glUniform1i(m_textureUniform, 0);

        // Copy our vertex buffer into the actual vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2dUVColor) * m_vertexBuffer.size(), &m_vertexBuffer[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        // Set the screen transform matrix
        glUniformMatrix3fv(m_screenTransformUniform, 1, GL_FALSE, &(m_screenTransform[0][0]));

        // Enable vertex attributes for position, uv, and color
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);


        // Draw all indices in the index buffer
        glDrawArrays(GL_TRIANGLES, 0, m_vertexBuffer.size());

        // Disable vertex attribute
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);


        m_vertexBuffer.clear();
    }


    void SpriteBatcher::SetScreenSize(glm::vec2 screenSize)
    {
        // Since opengl normally renders with the screen being -1 to 1, and I want it from 0 to screen size,
        // I have to multiply everything by this scaling and translation matrix
        m_screenTransform[0][0] = 2 / screenSize.x;
        m_screenTransform[1][1] = 2 / screenSize.y;
        m_screenTransform[2][0] = -1;
        m_screenTransform[2][1] = -1;
    }

}