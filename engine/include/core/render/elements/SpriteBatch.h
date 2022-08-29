#pragma once

#include <vector>
#include <any>

#include "core/render/Camera.h"
#include "core/render/shapes/Shape.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/Components.h"
#include "core/systems/particleSystem/ParticleSystem.h"

namespace GDE {

    /**
     * @brief This is the data that is sent to the GPU when rendering default vertices.
     */
    struct OpenGLVertex {

        /**
         * @brief Position in screen coordinates where the vertex should be placed.
         */
        glm::vec3 position {0, 0, 0};

        /**
         * @brief Color of the vertex.
         */
        glm::vec4 color { 1, 1, 1, 1 };

        /**
         * @brief Coordinate of the texture that this vertex has attached to it.
         */
        glm::vec2 texCoord {0, 0};

        OpenGLVertex(const glm::vec3& _position, const glm::vec2& _texCoord, const glm::vec4& _color) : position(_position), color(_color), texCoord(_texCoord) {  }
    };

    /**
     * @brief This is the data that is sent to the GPU when rendering default debugging information.
     */
    struct OpenGLVertexDebug {

        /**
         * @brief Position in screen coordinates where the vertex should be placed.
         */
        glm::vec3 position {0, 0, 0};

        /**
         * @brief Color of the vertex.
         */
        glm::vec4 color {1, 1, 1, 1};

        OpenGLVertexDebug(const glm::vec3& _position, const glm::vec4& _color) : position(_position), color(_color) {  }
    };

    /**
     * @brief This is the order of rendering of different Renderizable elements.
     */
    enum BatchPriority {
        SpritePriority = 0,
        TextPriority = 1
    };

    /**
     * @brief This is the class in charge of sending the data that needs to be rendered to the GPU.
     * It has a special function for each type of different complex geometry.
     */
    class SpriteBatch {

        /**
         * @brief This class represents a group of geometry that shares common elements and therefor can be sent all
         * together to the GPU so the number of Draw Calls is minimized as much as possible.
         */
        class Batch {
            friend class SpriteBatch;

            private:

                /**
                 * @see SpriteBatch
                 */
                SpriteBatch* spriteBatch = nullptr;

                /**
                 * @see BatchPriority
                 */
                BatchPriority priority = BatchPriority::SpritePriority;

                /**
                 * @brief How far or near it should be rendered on the Z-Axis.
                 */
                int layer = 0;

                /**
                 * @brief Shader that is going to be used to render the whole batch.
                 */
                Shader* shader = nullptr;

                /**
                 * @brief Data struct that contains all of the info of the vertices to be sent to the GPU.
                 */
                std::vector<std::any> vertexBuffer {};

                /**
                 * @brief Indices of the vertices to be sent to the GPU. We use indices as it is cheaper to send individual
                 * uint32_t values that repeated OpenGLVertex.
                 */
                std::vector<uint32_t> indexBuffer {};

                /**
                 * @brief Amount of vertices to be sent to the GPU.
                 */
                int vertexCount = 0;

                /**
                 * @brief Texture to be enabled on the GPU to be drawn by the vertices.
                 */
                GLuint textureID = -1;

                /**
                 * @brief Adds a SpriteRenderer data to the list of vertices to be sent to the GPU.
                 * @param _spriteRenderer SpriteRenderer to be rendered
                 * @param _transform SpriteRenderer's Transform
                 */
                void addSprite(const SpriteRenderer& _spriteRenderer, const Transform& _transform);

                /**
                 * @brief Adds a TextRenderer data to the list of vertices to be sent to the GPU.
                 * @param _text TextRenderer to be rendered
                 * @param _transform TextRenderer's Transform
                 */
                void addText(const TextRenderer& _text, const Transform& _transform);

                /**
                 * @brief Adds a NinePatchSprite data to the list of vertices to be sent to the GPU.
                 * @param _ninePatch NinePatchSprite to be rendered
                 * @param _transform NinePatchSprite's Transform
                 */
                void addNinePatchSprite(const NinePatchSprite& _ninePatch, const Transform& _transform);

                /**
                 * @brief Specific method to upload vertices for the NinePatch.
                 * @param _transform NinePatchSprite's Transform
                 * @param _ninePatch NinePatchSprite to be rendered
                 * @param _subTexture IntRect of the NinePatch
                 */
                void uploadVertices(const glm::mat4& _transform, const NinePatchSprite& _ninePatch, const IntRect& _subTexture);
        };

        /**
         * @brief This class is like a batch but specific for rendering debug elements, such as lines, shapes..
         */
        class Debug {
            friend class SpriteBatch;

            private:
                /**
                 * @see SpriteBatch
                 */
                static SpriteBatch* batch;

                /**
                 * @brief vertices to draw geometry more complex than a line.
                 */
                std::vector<OpenGLVertexDebug> vertexDebugBufferGeometrics {};

                /**
                 * @brief vertices to draw simple lines.
                 */
                std::vector<OpenGLVertexDebug> vertexDebugBufferLines {};

                /**
                 * @brief Debug VertexBuffer ID on the GPU.
                 */
                GLuint debugVbo = -1;

                /**
                 * @brief Debug VertexArrayBuffer ID on the GPU.
                 */
                GLuint debugVao = -1;

            private:
                /**
                 * @brief Initiates all the crucial elements of the debugging rendering.
                 */
                void configDebugShader();

            public:
                /**
                 * @brief Sets the inner SpriteBatch reference.
                 * @param _batch SpriteBatch
                 */
                void init(SpriteBatch* _batch);

                /**
                 * @brief Draws a grid on the screen.
                 * @param _color Color of the grid
                 */
                void drawGrid(const Color& _color);

                /**
                 * @brief Draws a line from _p0 to _p1 on _color
                 * @param _p0 Starting point
                 * @param _p1 End point
                 * @param _color Color of the line
                 */
                void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::Green);

                /**
                 * @brief Draws a square at _position, with _size, on _color and _rotation. The center of the square is the middle of the square.
                 * @param _position Position of the square
                 * @param _size Size of the square
                 * @param _color Color of the square
                 * @param _rotation Rotation of the square
                 */
                void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::Green, float _rotation = 0.f);

                /**
                 * @brief Draws a complex shape.
                 * @param _shape Shape to draw
                 * @see Shape
                 */
                void drawShape(Shape& _shape);

                /**
                 * @attention This is not meant to be called by end-users.
                 * @brief Sends the data to the GPU, this is a Draw Call.
                 */
                void flushDebug();

                /**
                 * @brief Sets the lines thickness.
                 * @param _thickness Line thickness
                 */
                void setDebugLinesThickness(float _thickness);
        };

        public:
            /**
             * @brief Total number of triangles, only used for debugging purposes.
             */
            int totalTriangles = 0;

            /**
             * @brief Amount of Draw Calls per frame, only used for debugging purposes.
             */
            int drawCalls = 0;

            /**
             * @see Debug
             */
            Debug debug {};

            /**
             * @brief Maximum number of indices that can be accumulated in a single Draw Call, if it is exceeded, an
             * automatic Draw Call will be called to free the buffer.
             */
            const int maxIndicesPerDrawCall = 50000; // Between 35.000 and 50.000 should be a good range

        private:
            /**
             * @see ShaderManager
             */
            ShaderManager* shaderManager = nullptr;

            /**
             * @see IViewPort
             */
            IViewPort* viewport = nullptr;

            /**
             * @brief VertexBuffer ID on the GPU.
             */
            GLuint vbo = -1;

            /**
             * @brief VertexArrayBuffer ID on the GPU.
             */
            GLuint vao = -1;

            /**
             * @brief IndexBuffer ID on the GPU.
             */
            GLuint ibo = -1;

            /**
             * @brief Number of vertices drawn on a Draw Call, only used for debugging purposes.
             */
            int vertices = 0;

            /**
             * @brief View Projection Matrix, this is the matrix that tells the GPU how to render the geometry.
             */
            glm::mat4 viewProjectionMatrix {1.0f};

            /**
             * @brief All the batches to be drawn.
             */
            std::vector<Batch> batches {};

        private:
            /**
             * @brief Returns a Batch that matches some specific information. If no match, it creates a new one.
             * @param _renderer The element to be rendered
             * @param _layer How near or far this element is being rendered.
             * @param _priority Whether this element must be rendered after or before othe types.
             * @return Batch&
             */
            Batch& getBatch(const IRenderizable& _renderer, int _layer, BatchPriority _priority);

            /**
             * @brief Orders the batches to be sent to the GPU based on some specific data, such as the texture used,
             * the layer, the shader used...
             */
            void orderBatches();

        public:
            SpriteBatch() {};
            ~SpriteBatch();

            /**
             * @brief Initiates all of the crucial elements of the SpriteBatch.
             * @param _engine Engine
             */
            void init(Engine* _engine);

            /**
             * @brief This marks the beginning of a new Draw Call.
             * @param _camera Camera that will render the Draw Call
             * @param _cameraTransform Cameras position to be combined with the View Projection Matrix.
             */
            void beginDraw(Camera& _camera, Transform* _cameraTransform);

            /**
             * @brief Draws a SpriteRenderer
             * @param _spriteRenderer SpriteRenderer to be drawn
             * @param _transform SpriteRenderer's Transform
             */
            void draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform);

            /**
             * @brief Draws a TextRenderer
             * @param _text TextRenderer to be drawn
             * @param _transform TextRenderer's Transform
             */
            void draw(const TextRenderer& _text, const Transform& _transform);

            /**
             * @brief Draws a ParticleSystem
             * @param _particleSystem ParticleSystem to be drawn
             * @param _transform ParticleSystem's Transform
             */
            void draw(const ParticleSystem& _particleSystem, const Transform& _transform);

            /**
             * @brief Draws a NinePatchSprite
             * @param _ninePatch NinePatchSprite to be drawn
             * @param _transform NinePatchSprite's Transform
             */
            void draw(const NinePatchSprite& _ninePatch, const Transform& _transform);

            /**
             * @brief This method sends all of the geometry to the GPU and ends a Draw Call.
             */
            void flush();

        private:
            /**
             * @brief This method initiates the VertexBuffer on the GPU.
             */
            void configBasicShader();
    };
}
