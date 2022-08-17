// Created by borja on 26/12/21.

#include "core/platform/PlatformHeaderSDLImage.h"
#include "core/render/Renderer.h"
#include "core/util/GLUtil.h"

namespace GDE {

    SpriteBatch Renderer::batch;
    Color Renderer::clearColor = Color::Red;

    void Renderer::init(Engine* _engine) {
    
        #if !IS_MOBILE()
        LOG_I("OpenGL Version: ", glGetString(GL_VERSION));
        #endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        #if !IS_MOBILE()
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
        #endif

        int _flags = IMG_INIT_PNG | IMG_INIT_JPG;
        if(IMG_Init(_flags) != _flags) {
            LOG_E("SDL Image loader couldn't initialize all png and jpg")
            return;
        }

        LOG_S("SDL Image loader loaded successfully")

        batch.init(_engine);
        batch.debug.init(&batch);
        CHECK_GL_ERROR("Renderer Initialization")
    }

    void Renderer::clear() {
        glClearColor((float)clearColor.r / 255.f, (float)clearColor.g / 255.f, (float)clearColor.b / 255.f, (float)clearColor.a / 255.f);
        CHECK_GL_ERROR("Renderer Clear")
        resetBuffers();
    }

    void Renderer::resetBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void Renderer::beginDraw(Camera& _camera, Transform* _cameraTransform) {
        batch.beginDraw(_camera, _cameraTransform);
    }

    void Renderer::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        batch.debug.drawLine(_p0, _p1, _color);
    }

    void Renderer::endDraw() {
        batch.flush();
    }

    void Renderer::draw(const SpriteRenderer& _spriteRenderer, const Transform& _transform) {
        batch.draw(_spriteRenderer, _transform);
    }

    void Renderer::draw(const TextRenderer& _text, const Transform& _transform) {
        batch.draw(_text, _transform);
    }

    void Renderer::draw(const NinePatchSprite& _ninePatch, const Transform& _transform) {
        batch.draw(_ninePatch, _transform);
    }

    void Renderer::beginDebugDraw(Camera& _camera, Transform* _cameraTransform, float _thickness) {
        batch.debug.setDebugLinesThickness(_thickness);
        batch.beginDraw(_camera, _cameraTransform);
    }

    void Renderer::endDebugDraw() {
        batch.debug.flushDebug();
    }

    void Renderer::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        batch.debug.drawSquare(_position, _size, _color, _rotation);
    }

    void Renderer::drawShape(Shape& _shape) {
        batch.debug.drawShape(_shape);
    }

    void Renderer::resetDebugInfo() {
        batch.drawCalls = 0;
        batch.totalTriangles = 0;
    }

    int Renderer::getTotalTriangles() {
        return batch.totalTriangles;
    }

    int Renderer::getDrawCalls() {
        return batch.drawCalls;
    }

    void Renderer::destroy() {
        IMG_Quit();
    }

    void Renderer::setClearColor(const Color& _color) {
        clearColor = _color;
    }

    Color Renderer::getClearColor() {
        return clearColor;
    }

    void Renderer::drawGrid(const Color& _color) {
        batch.debug.drawGrid(_color);
    }


}
