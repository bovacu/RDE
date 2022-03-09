// Created by borja on 26/12/21.


#ifndef ENGINE2_0_RENDERER_H
#define ENGINE2_0_RENDERER_H

#include "core/util/Util.h"
#include "core/render/elements/SpriteBatch.h"
#include "core/systems/uiSystem/Text.h"

namespace engine {

    class Renderer {

        // Debug
        public:
            static void resetDebugInfo();
            static int getTotalTriangles();
            static int getDrawCalls();

        private:
            static SpriteBatch batch;
            static Color clearColor;

        public:
            /**
             * Must be called ONCE before rendering anything, BUT after creating the window.
             **/
            static void init(Window* _window);
            static void destroy();

            /**
             * This must be called before any draw call on every frame (in main loop).
             * @param _color The color of the background
             **/
            static void clear();
            static void setClearColor(const Color& _color);

            /**
             * Renders textures, sprites... It includes 2D batching for
             * a much more performant drawing.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the basic one.
             **/
            static void beginDraw(Camera& _camera);

            /**
             * Renders geometry for debugging. It also includes batching.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the debugging one.
             */
            static void beginDebugDraw(Camera& _camera, float _thickness = 1.f);

            /**
             * Renders a texture. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _texture Pointer to the texture to draw.
             * @param _position 2D position where to draw it.
             * @param _size Size of the texture {width, height}.
             * @param _tintColor Color to draw over the texture (White by default, which means the texture's color).
             * @param _rotation In degrees, 0 by default.
             */
            static void draw(Texture* _texture, const Vec2F& _position, const Vec2F& _size, const Color& _tintColor = Color::White, float _rotation = 0);

            /**
             * Renders a sprite. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _sprite The sprite to be drawn.
             */
            static void draw(SpriteRenderer& _spriteRenderer, Transform& _transform);

            static void draw(Text& _text, Transform& _transform);

            /**
             * Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _p0 Starting point of the line.
             * @param _p1 Ending point of the line.
             * @param _color Color of the line.
             * @param _thickness Thickness of the line.
             */
            static void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::White);

            /**
             * Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _position Center position of the square.
             * @param _size Size of the square.
             * @param _color Color of the line.
             * @param _rotation Rotation of the square.
             */
            static void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::White, float _rotation = 0.f);

            static void drawShape(Shape& _shape);

            /**
             * Sends the data batched to the GPU, must be called after beginDraw and what we want to render.
             */
            static void endDraw();

            /**
             * Sends the debug data batched to the GPU, must be called after beginDebugDraw and what we want to render.
             */
            static void endDebugDraw();
    };
}


#endif //ENGINE2_0_RENDERER_H
