// Created by borja on 30/12/21.


#include "glm/ext/matrix_clip_space.hpp"
#include "core/render/Camera.h"
#include "core/graph/Components.h"
#include "core/Engine.h"

namespace GDE {

    Camera::Camera(const Window* _window, const NodeID& _mainCameraID) {
        ID = _mainCameraID;
        viewport = new FreeViewPort(_window->getWindowSize());
    }

    void Camera::onResize(int _width, int _height) {
        viewport->update({_width, _height});
        float _aspectRatio = viewport->getAspectRatio();
        glViewport(0, 0, _width, _height);
        projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -zoom, zoom);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    glm::mat4& Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    Transform& Camera::getTransform() {
        return *Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID);
    }

    void Camera::recalculateViewMatrix() {
        glm::mat4 _transform = glm::translate(glm::mat4(1.0f), {0, 0, 0.0f}) *
                               glm::rotate(glm::mat4(1.0f),
                               glm::radians(Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID)->getRotationLocal()), glm::vec3(0, 0, 1));
        viewMatrix = glm::inverse(_transform);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    void Camera::setPosition(const Vec2F& _position) {
        Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID)->setPosition(_position);
        recalculateViewMatrix();
    }

    Vec2F Camera::getPosition() {
        return Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID)->getPositionLocal();
    }

    void Camera::setRotation(float _rotation) {
        Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID)->setRotation(_rotation);
        recalculateViewMatrix();
    }

    float Camera::getRotation() {
        return Engine::get().getScene()->getMainGraph()->getComponent<Transform>(ID)->getRotationLocal();
    }

    glm::mat4& Camera::getViewMatrix() {
        return viewMatrix;
    }

    glm::mat4& Camera::getViewProjectionMatrix() {
        return viewProjectionMatrix;
    }

    void Camera::onEvent(Event& _event) {

    }

    bool Camera::onMouseScrolled(MouseScrolledEvent& _event) {
        zoom -= _event.getScrollY() * 0.1f;
        zoom = std::max(zoom, 0.5f);
        float _aspectRatio = viewport->getAspectRatio();
        projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -1.f, 1.f);
        return false;
    }

    float Camera::getAspectRatio() const {
        return viewport->getAspectRatio();
    }

    float Camera::getCurrentZoomLevel() const {
        return zoom;
    }

    void Camera::setCurrentZoomLevel(float _zoomLevel) {
        zoom = _zoomLevel;
        float _aspectRatio = viewport->getAspectRatio();
        projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -1.f, 1.f);
    }

    float Camera::getZoomSpeed() const {
        return zoomSpeed;
    }

    void Camera::setZoomSpeed(float _zoomSpeed) {
        zoomSpeed = _zoomSpeed;
    }

    Camera::~Camera() {
        delete viewport;
    }

    IViewPort* Camera::getViewport() const {
        return viewport;
    }

    void Camera::setFreeViewport(const Window* _window) {
        delete viewport;
        viewport = new FreeViewPort(_window->getWindowSize());
    }

    void Camera::setAdaptiveViewport(const Vec2I& _virtualDesiredSize, const Vec2I& _currentDeviceSize) {
        delete viewport;
        viewport = new AdaptiveViewPort(_virtualDesiredSize);
        viewport->update(_currentDeviceSize);
    }

}