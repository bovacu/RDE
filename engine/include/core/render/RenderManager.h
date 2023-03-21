// Created by borja on 26/12/21.


#ifndef RDE_RENDER_MANAGER_H
#define RDE_RENDER_MANAGER_H

#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/SpriteBatch.h"
#include "core/util/Color.h"
#include "core/util/Vec.h"
#include <vector>

namespace RDE {

    class Engine;

    class RenderManager {
        friend class Scene;
        friend class Engine;
        friend class ImGuiScene;
        friend class FrameBuffer;
        friend class Canvas;
        friend class Manager;

        // Debug
        public:
            RenderManager() {  }

            void resetDebugInfo();
            int getTotalTriangles();
            std::tuple<int, int> getDrawCalls();

        private:
            /**
             * @see SpriteBatch
             */
            SpriteBatch batch;

            /**
             * @brief Color to paint the background.
             */
            Color clearColor = Color::Red;

            /**
             * @brief Engine reference.
             */
            Engine* engine = nullptr;

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
             * @brief This must be called before any drawBatched call on every frame (in main loop).
             * @param _color The color of the background
             */
            void clear();

            void resetBuffers();

            /**
             * @brief Renders textures, sprites... It includes 2D batching for
             * a much more performant drawing.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the basic one.
             */
            void beginDraw(Camera* _camera, Transform* _cameraTransform);

            /**
             * @brief Renders geometry for debugging. It also includes batching.
             * @param _camera It is the main camera of the scene (or other camera if a special drawing is needed).
             * @param _renderShader The shader we want to use to render, by default the debugging one.
             */
            void beginDebugDraw(Camera* _camera, Transform* _cameraTransform, float _thickness = 1.f);

            /**
             * @brief Renders an IRenderizable using batching. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _renderizable Component derived from IRenderizable.
             * @param _transform Nine patch's transform.
             */
            void drawSpriteRenderer(RenderizableInnerData& _innerData, Transform* _transform);
            void drawTextRenderer(RenderizableInnerData& _innerData, Transform* _transform);

            /**
             * @brief Renders an IRenderizable immediately with no batching. MUST BE CALLED INSIDE A BLOCK OF beginDraw/endDraw.
             * @param _batches Static batches to render.
             */
            void endDrawUI(std::vector<Batch>& _batches);

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
             * @brief Sets the color to paint the background.
             * @param _color Color to paint the background
             */
			RDE_FUNC void setClearColor(const Color& _color);

            /**
             * @brief Returns the current clear color.
             * @return Color
             */
			RDE_FUNC_ND Color getClearColor();

            /**
                 * @brief Draws a point at _position. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
                 * @param _position Position.
                 * @param _color Color of the line.
                 */
			RDE_FUNC void drawPoint(const Vec2F& _position, const Color& _color = Color::White);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _p0 Starting point of the line.
             * @param _p1 Ending point of the line.
             * @param _color Color of the line.
             * @param _thickness Thickness of the line.
             */
			RDE_FUNC void drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::White);

            /**
             * @brief Draws a line form P0 to P1. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _position Center position of the square.
             * @param _size Size of the square.
             * @param _color Color of the line.
             * @param _rotation Rotation of the square.
             */
			RDE_FUNC void drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::White, float _rotation = 0.f);

            /**
             * @brief Draws a shape. MUST BE CALLED INSIDE A BLOCK OF beginDebugDraw/endDraw.
             * @param _shape Complex shape
             */
			RDE_FUNC void drawShape(DebugShape& _shape);

            /**
             * @brief Draws a grid on the screen.
             * @param _color Color of the grid
             */
			RDE_FUNC void drawGrid(const Color& _color = Color::White);

            /**
             * @brief Sets line thickness.
             * @param _thickness Line thickness
             */
			RDE_FUNC void setLineThickness(float _thickness);

            /**
             * @brief Sets point size.
             * @param _size Point size
             */
			RDE_FUNC void setPointSize(float _size);

            /**
             * @brief Fills the whole CPUTexture with a specific color.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _color      Color to fill the whole CPUTexture.
             */
			RDE_FUNC void fillBackgroundCPUTexture(CPUTexture* _cpuTexture, const Color& _color);

            /**
             * @brief Draws a point on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _position   Where to draw the point.
             * @param _color      Color to draw the point.
             * @param _size       Size of the point. Minimum is 1, this means that the pixel according to position will be filled and also 1 unit to each side.
             */
			RDE_FUNC void drawPointToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _position, const Color& _color = Color::Blue, int _size = 1);

            /**
             * @brief Draws a line on the CPUTexure.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _p0         Initial position of the line.
             * @param _p1         End position of the line.
             * @param _color      Color to draw the line.
             * @param _thickness  Thickness of the line.
             */
			RDE_FUNC void drawLineToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _p0, const Vec2F& _p1, const Color& _color = Color::Blue, int _thickness = 1);

            /**
             * @brief Draws a rectangle on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _position   The centered position of the rectangle.      
             * @param _size       Size of the rectangle. It will expando half of the size to each side from _position.
             * @param _color      Color to draw the rectangle.
             */
			RDE_FUNC void drawRectangleToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _position, const Vec2F& _size, const Color& _color = Color::Blue);

            /**
             * @brief Draws a filled circle on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _center     Center of the circle.
             * @param _radius     Radius of the circle
             * @param _color      Color to draw the circle.
             */
			RDE_FUNC void drawFilledCircleToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _center, float _radius, const Color& _color = Color::Blue);

            /**
             * @brief Draws the outline of a circle on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _center     Center of the outline of the circle.
             * @param _radius     Radius of the outline of the circle.
             * @param _color      Color to draw the outline of the circle.
             * @param _thickness  Thickness of the outline of the circle.
             */
			RDE_FUNC void drawLinedCircleToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _center, float _radius, const Color& _color = Color::Blue, int _thickness = 1);

            /**
             * @brief Draws a triangle on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _p0         First vertex of the triangle.
             * @param _p1         Second vertex of the triangle.
             * @param _p2         Third vertex of the triangle.
             * @param _color      Color to draw the triangle.
             */
			RDE_FUNC void drawTriangleToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, const Vec2F& _p0, const Vec2F& _p1, const Vec2F& _p2, const Color& _color = Color::Blue);

            /**
             * @brief Draws a shape on the CPUTexture.
             * @param _cpuTexture The CPUTexutre to modify.
             * @param _points     List of the vertices of the shape.
             * @param _color      Color to draw the shape.
             */
			RDE_FUNC void drawShapeToCPUTexture(CPUTexture* _cpuTexture, Transform* _cpuTextureTransform, std::vector<Vec2F> _points, const Color& _color = Color::Blue);
    };
}


#endif //RDE_RENDER_MANAGER_H
