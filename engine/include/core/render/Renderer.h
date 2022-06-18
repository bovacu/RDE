// Created by borja on 26/12/21.


#ifndef ENGINE2_0_RENDERER_H
#define ENGINE2_0_RENDERER_H

#include "core/util/Util.h"
#include "core/render/elements/SpriteBatch.h"

namespace GDE {

    class Renderer {

        // Debug
        public:
            static void resetDebugInfo();
            static int getTotalTriangles();
            static int getDrawCalls();

        private:
            /**
             * @see SpriteBatch
             */
            static SpriteBatch batch;

            /**
             * @brief Color to paint the background.
             */
            static Color clearColor;

        public:
            /**
             * @attention This is not meant to be called by end-users.
             * @brief Must be called ONCE before rendering anything, BUT after creating the window.
             */
            static void init(Engine* _engine);

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Destroys all related data to the GPU.
             */
            static void destroy();

            /**
             * @brief This must be called before any draw call on every frame (in main loop).
             * @param _color The color of the background
             */
            static void clear();

            /**
             * @brief Sets the color to paint the background.
             * @param _color Color to paint the background
             */
            static void setClearColor(const Color& _color);

            /**
             * @brief Renders textures, sprites... It includes 2D batching for
             * a much more performant drawing.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the basic one.
             */
            static void beginDraw(Camera& _camera, Transform* _cameraTransform);

            /**
             * @brief Renders geometry for debugging. It also includes batching.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the debugging one.
             */
            static void beginDebugDraw(Camera& _camera, Transform* _cameraTransform, float _thickness = 1.f);

            /**
             * @brief Renders a sprite. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _sprite The sprite to be drawn.
             */
            static void draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform);

            /**
             * @brief Renders text on the screen. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _text Text renderer
             * @param _transform Text renderer's transform
             */
            static void draw(const TextRenderer& _text, const Transform& _transform);

            /**
             * @brief Renders a nine patch. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _ninePatch Nine patch sprite
             * @param _transform Nine patch's transform
             */
            static void draw(const NinePatchSprite& _ninePatch, const Transform& _transform);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _p0 Starting point of the line.
             * @param _p1 Ending point of the line.
             * @param _color Color of the line.
             * @param _thickness Thickness of the line.
             */
            static void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::White);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _position Center position of the square.
             * @param _size Size of the square.
             * @param _color Color of the line.
             * @param _rotation Rotation of the square.
             */
            static void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::White, float _rotation = 0.f);

            /**
             * @brief Draws a shape. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _shape Complex shape
             */
            static void drawShape(Shape& _shape);

            /**
             * @brief Draws a grid on the screen.
             * @param _color Color of the grid
             */
            static void drawGrid(const Color& _color = Color::White);

            /**
             * @brief Sends the data batched to the GPU, must be called after beginDraw and what we want to render.
             */
            static void endDraw();

            /**
             * @brief Sends the debug data batched to the GPU, must be called after beginDebugDraw and what we want to render.
             */
            static void endDebugDraw();
    };
}


#endif //ENGINE2_0_RENDERER_H
