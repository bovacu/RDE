#include "SpriteBatch.h"
#include "core/util/Logger.h"
#include "GL/glew.h"

namespace engine {
    void SpriteBatch::init() {
        vertexShader.initFromFile("assets/vertex.glsl", GL_VERTEX_SHADER);
        LOG_I("Loaded Vertex Shader")
        fragmentShader.initFromFile("assets/fragment.glsl", GL_FRAGMENT_SHADER);
        LOG_I("Loaded Fragment Shader")

        // Create a shader program.
        shaderProgram = glCreateProgram();

        // Attach the vertex and fragment shaders to our program.
        vertexShader.attachTo(shaderProgram);
        fragmentShader.attachTo(shaderProgram);

        // Build full shader program.
        glLinkProgram(shaderProgram);

        // Get the texture uniform from the shader program.
        textureUniform = glGetUniformLocation(shaderProgram, "tex");
        texture = nullptr;

        // Setup vertex buffer
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)sizeof(glm::vec2));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2dUVColor), (void*)sizeof(glm::vec4));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Setup things related to the screen Transform
        screenTransformUniform = glGetUniformLocation(shaderProgram, "screenTransform");
    }

    SpriteBatch::~SpriteBatch() {
        glDeleteProgram(shaderProgram);
    }



    void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& sourceRect, const glm::vec4& color, Texture* _texture) {
        // Draw doesn't actually have to draw anything.
        // Instead it collects a bunch of vertex information until flush is called, and then draws it all.

        // In one draw call we can only render sprites that have the same _texture in.
        // If we get something with a different _texture, we have to flush and start over.
        if (texture != _texture) {
            // Draw everything in the current buffer.
            flush();


            // Make sure the _texture exists so long as we are holding on to it.
            _texture->incRefCount();
            if (texture != nullptr)
                texture->decRefCount();

            // Switch textures
            texture = _texture;
        }

        // Create vertices for the given draw request and add them to the buffer.
        // A more complex spritebatcher may allow for rotation of vertices, which would also happen here
        vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y), glm::vec2(sourceRect.x, sourceRect.y), color);
        vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y), glm::vec2(sourceRect.z, sourceRect.y), color);
        vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y + destRect.w), glm::vec2(sourceRect.x, sourceRect.w), color);
        vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y), glm::vec2(sourceRect.z, sourceRect.y), color);
        vertexBuffer.emplace_back(glm::vec2(destRect.x, destRect.y + destRect.w), glm::vec2(sourceRect.x, sourceRect.w), color);
        vertexBuffer.emplace_back(glm::vec2(destRect.x + destRect.z, destRect.y + destRect.w), glm::vec2(sourceRect.z, sourceRect.w), color);
    }

    void SpriteBatch::flush() {
        // If there's a false alarm, don't draw anything
        // (this will always happen on the first texture)
        if (vertexBuffer.empty() || texture == nullptr)
            return;

        // Set the current shader program.
        glUseProgram(shaderProgram);


        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getGLTexture());
        glUniform1i(textureUniform, 0);

        // Copy our vertex buffer into the actual vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (long)(sizeof(Vertex2dUVColor) * vertexBuffer.size()), &vertexBuffer[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        // Set the screen transform matrix
        glUniformMatrix3fv(screenTransformUniform, 1, GL_FALSE, &(screenTransform[0][0]));

        // Enable vertex attributes for position, uv, and color
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);


        // Draw all indices in the index buffer
        glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.size());

        // Disable vertex attribute
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);


        vertexBuffer.clear();
    }


    void SpriteBatch::setScreenSize(const glm::vec2& screenSize) {
        // Since opengl normally renders with the screen being -1 to 1, and I want it from 0 to screen size,
        // I have to multiply everything by this scaling and translation matrix
        screenTransform[0][0] = 2 / screenSize.x;
        screenTransform[1][1] = 2 / screenSize.y;
        screenTransform[2][0] = -1;
        screenTransform[2][1] = -1;
    }

}