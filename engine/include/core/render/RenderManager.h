// Created by borja on 26/12/21.


#ifndef GDE_RENDER_MANAGER_H
#define GDE_RENDER_MANAGER_H

#include "core/util/Util.h"
#include "core/render/elements/SpriteBatch.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(Engine)

    class RenderManager {
        FRIEND_CLASS(Graph, Engine, ImGuiScene, FrameBuffer, Canvas, Manager)

        // Debug
        public:
            void resetDebugInfo();
            int getTotalTriangles();
            int getDrawCalls();

        private:
            /**
             * @see SpriteBatch
             */
            SpriteBatch batch;

            /**
             * @brief Color to paint the background.
             */
            Color clearColor = Color::Red;

        private:
            /**
             * @attention This is not meant to be called by end-users.
             * @brief Must be called ONCE before rendering anything, BUT after creating the window.
             */
            void init(Engine* _engine);

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Destroys all related data to the GPU.
             */
            void destroy();

            /**
             * @brief This must be called before any draw call on every frame (in main loop).
             * @param _color The color of the background
             */
            void clear();

            void resetBuffers();

            /**
             * @brief Sets the color to paint the background.
             * @param _color Color to paint the background
             */
            void setClearColor(const Color& _color);

            /**
             * @brief Returns the current clear color.
             * @return Color
             */
            Color getClearColor();

            /**
             * @brief Renders textures, sprites... It includes 2D batching for
             * a much more performant drawing.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the basic one.
             */
            void beginDraw(Camera& _camera, Transform* _cameraTransform);

            /**
             * @brief Renders geometry for debugging. It also includes batching.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the debugging one.
             */
            void beginDebugDraw(Camera& _camera, Transform* _cameraTransform, float _thickness = 1.f);

            /**
             * @brief Renders an IRenderizable. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _renderizable Component derived from IRenderizable.
             * @param _transform Nine patch's transform.
             */
            void draw(IRenderizable* _renderizable, Transform& _transform);

            /**
             * @brief Sends the data batched to the GPU, must be called after beginDraw and what we want to render.
             */
            void endDraw();

            /**
             * @brief Sends the debug data batched to the GPU, must be called after beginDebugDraw and what we want to render.
             */
            void endDebugDraw();

        public:
            /**
                 * @brief Draws a point at _position. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
                 * @param _position Position.
                 * @param _color Color of the line.
                 */
            void drawPoint(const Vec2F& _position, const Color& _color = Color::White);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _p0 Starting point of the line.
             * @param _p1 Ending point of the line.
             * @param _color Color of the line.
             * @param _thickness Thickness of the line.
             */
            void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::White);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _position Center position of the square.
             * @param _size Size of the square.
             * @param _color Color of the line.
             * @param _rotation Rotation of the square.
             */
            void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::White, float _rotation = 0.f);

            /**
             * @brief Draws a shape. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _shape Complex shape
             */
            void drawShape(DebugShape& _shape);

            /**
             * @brief Draws a grid on the screen.
             * @param _color Color of the grid
             */
            void drawGrid(const Color& _color = Color::White);

            /**
             * @brief Sets line thickness.
             * @param _thickness Line thickness
             */
            void setLineThickness(float _thickness);

            /**
             * @brief Sets point size.
             * @param _size Point size
             */
            void setPointSize(float _size);
    };
}


#endif //GDE_RENDER_MANAGER_H
