#ifndef RDE_SPRITE_BATCH_H
#define RDE_SPRITE_BATCH_H

#include <vector>
#include "core/graph/components/Transform.h"
#include "core/render/Camera.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/shapes/DebugShape.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/components/Components.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/ParticleSystem.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"

namespace RDE {

    class Engine;

    /**
     * @brief This is the class in charge of sending the data that needs to be rendered to the GPU.
     * It has a special function for each type of different complex geometry.
     */
    class SpriteBatch {
        friend class Batch;
        friend class RenderManager;
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
                 * @brief vertices to drawBatched geometry more complex than a line.
                 */
                std::vector<OpenGLVertexDebug> vertexDebugBufferGeometrics {};

                /**
                 * @brief vertices to drawBatched simple lines.
                 */
                std::vector<OpenGLVertexDebug> vertexDebugBufferLines {};

                /**
                 * @brief vertices to drawBatched simple points.
                 */
                std::vector<OpenGLVertexDebug> vertexDebugBufferPoints {};

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
                 * @brief Draws a point at _position on _color.
                 * @param _position Position
                 * @param _color Color of the line
                 */
                void drawPoint(const Vec2F& _position, const Color& _color = Color::Green);

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
                 * @param _shape Shape to drawBatched
                 * @see Shape
                 */
                void drawShape(DebugShape& _shape);

                /**
                 * @attention This is not meant to be called by end-users.
                 * @brief Sends the data to the GPU, this is a Draw Call.
                 */
                void flushDebug();

                /**
                 * @brief Sets the lines thickness.
                 * @param _thickness Line thickness
                 */
                void setLinesThickness(float _thickness);

                /**
                 * @brief Sets the point size.
                 * @param _size Point size
                 */
                void setPointSize(float _size);
        };

        public:
            /**
             * @brief Total number of triangles, only used for debugging purposes.
             */
            int totalTriangles = 0;

            /**
             * @brief Amount of Draw Calls, only used for debugging purposes.
             */
            int drawCalls = 0;

            /**
             * @brief Amount of Draw Calls for the UI, only used for debugging purposes.
             */
            int uiDrawCalls = 0;

            /**
             * @see Debug
             */
            Debug debug {};

            /**
             * @brief Maximum number of indices that can be accumulated in a single Draw Call, if it is exceeded, an
             * automatic Draw Call will be called to free the buffer.
             */
            const int maxIndicesPerDrawCall = 65000; // Between 35.000 and 50.000 should be a good range

        private:
            /**
             * @see ShaderManager
             */
            Engine* engine = nullptr;

            /**
             * @see ViewPort
             */
            ViewPort* viewport = nullptr;

            /**
             * @brief View Projection Matrix, this is the matrix that tells the GPU how to render the geometry.
             */
            glm::mat4 viewProjectionMatrix {1.0f};

            /**
             * @brief All the batches to be drawn.
             */
            std::vector<Batch> batches {};

            SpriteBatch() {  }

        private:
            /**
             * @brief Returns a Batch that matches some specific information. If no match, it creates a new one.
             * @param _renderer The element to be rendered
             * @param _layer How near or far this element is being rendered.
             * @param _priority Whether this element must be rendered after or before othe types.
             * @return Batch&
             */
            Batch& getBatch(const RenderizableInnerData& _innerData);

            /**
             * @brief Orders the batches to be sent to the GPU based on some specific data, such as the texture used,
             * the layer, the shader used...
             */
            void orderBatches();

        public:
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
            void beginDraw(Camera* _camera, Transform* _cameraTransform);

            /**
             * This method is used to drawBatched anything that extends IRenderizable.
             * @param _renderizable An IRenderizable instance.
             * @param _transform Transform of the IRenderizable.
             */
            void drawSpriteRenderer(RenderizableInnerData& _innerData, Transform* _transform);
            void drawTextRenderer(RenderizableInnerData& _innerData, Transform* _transform);

            /**
             * This method is used to drawBatched anything that extends IRenderizable and that is UI.
             * @param _batches Static batches to render the UI.
             */
            void drawUI(std::vector<Batch>& _batches);

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

#endif //RDE_SPRITE_BATCH_H
