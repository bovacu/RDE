// Created by borja on 26/12/21.

#include "core/platform/PlatformHeaderSDLImage.h"
#include "core/render/RenderManager.h"
#include "core/util/GLUtil.h"

namespace GDE {

    void RenderManager::init(Engine* _engine) {
    
        #if !IS_MOBILE()
        LOG_DEBUG("OpenGL Version: ", glGetString(GL_VERSION));
        #endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_FRONT);
//        glFrontFace(GL_CW);

        #if !IS_MOBILE()
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
        #endif

        int _flags = IMG_INIT_PNG | IMG_INIT_JPG;
        if(IMG_Init(_flags) != _flags) {
            LOG_E("SDL Image loader couldn't initialize all png and jpg")
            return;
        }

        batch.init(_engine);
        batch.debug.init(&batch);
        CHECK_GL_ERROR("RenderManager Initialization")
    }

    void RenderManager::clear() {
        glClearColor((float)clearColor.r / 255.f, (float)clearColor.g / 255.f, (float)clearColor.b / 255.f, (float)clearColor.a / 255.f);
        CHECK_GL_ERROR("RenderManager Clear")
        resetBuffers();
    }

    void RenderManager::resetBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderManager::beginDraw(Camera& _camera, Transform* _cameraTransform) {
        batch.beginDraw(_camera, _cameraTransform);
    }

    void RenderManager::drawPoint(const Vec2F& _position, const Color& _color) {
        batch.debug.drawPoint(_position, _color);
    }

    void RenderManager::drawLine(const Vec2F& _p0, const Vec2F& _p1, const Color& _color) {
        batch.debug.drawLine(_p0, _p1, _color);
    }

    void RenderManager::endDraw() {
        batch.flush();
    }

    void RenderManager::draw(IRenderizable* _renderizable, Transform& _transform) {
        batch.draw(_renderizable, _transform);
    }

    void RenderManager::beginDebugDraw(Camera& _camera, Transform* _cameraTransform, float _thickness) {
        batch.debug.setLinesThickness(_thickness);
        batch.beginDraw(_camera, _cameraTransform);
    }

    void RenderManager::endDebugDraw() {
        batch.debug.flushDebug();
    }

    void RenderManager::drawSquare(const Vec2F& _position, const Vec2F& _size, const Color& _color, float _rotation) {
        batch.debug.drawSquare(_position, _size, _color, _rotation);
    }

    void RenderManager::drawShape(DebugShape& _shape) {
        batch.debug.drawShape(_shape);
    }

    void RenderManager::resetDebugInfo() {
        batch.drawCalls = 0;
        batch.totalTriangles = 0;
    }

    int RenderManager::getTotalTriangles() {
        return batch.totalTriangles;
    }

    int RenderManager::getDrawCalls() {
        return batch.drawCalls;
    }

    void RenderManager::destroy() {
        LOG_DEBUG("Cleaning up RenderManager")
        IMG_Quit();
    }

    void RenderManager::setClearColor(const Color& _color) {
        clearColor = _color;
    }

    Color RenderManager::getClearColor() {
        return clearColor;
    }

    void RenderManager::drawGrid(const Color& _color) {
        batch.debug.drawGrid(_color);
    }

    void RenderManager::setLineThickness(float _thickness) {
        batch.debug.setLinesThickness(_thickness);
    }

    void RenderManager::setPointSize(float _size) {
        batch.debug.setPointSize(_size);
    }


}
