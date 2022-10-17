// Created by borja on 30/12/21.


#include "glm/ext/matrix_clip_space.hpp"
#include "core/render/Camera.h"
#include "core/graph/components/Transform.h"
#include "core/Engine.h"

namespace GDE {

    Camera::Camera(const NodeID& _mainCameraID, const Window* _window, Transform* _entityTransform) {
        ID = _mainCameraID;
        viewport = new FreeViewPort(_window->getWindowSize());
        onResize(_window->getWidth(), _window->getHeight());
        transform = _entityTransform;
    }

    void Camera::onResize(int _width, int _height) {
        viewport->update({_width, _height});
        float _aspectRatio = viewport->getAspectRatio();
        glViewport(0, 0, _width, _height);
        projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -zoom, zoom);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
        size = {_width, _height};
    }

    glm::mat4& Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    Transform& Camera::getTransform() {
        return *transform;
    }

    void Camera::recalculateViewMatrix() {
        glm::mat4 _transform = glm::translate(glm::mat4(1.0f), {0, 0, 0.0f}) *
                               glm::rotate(glm::mat4(1.0f),
                                           glm::radians(transform->getRotation()), glm::vec3(0, 0, 1));
        viewMatrix = glm::inverse(_transform);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    void Camera::setPosition(const Vec2F& _position) {
        transform->setPosition(_position);
        recalculateViewMatrix();
    }

    Vec2F Camera::getPosition() {
        return transform->getPosition();
    }

    void Camera::setRotation(float _rotation) {
        transform->setRotation(_rotation);
        recalculateViewMatrix();
    }

    float Camera::getRotation() {
        return transform->getRotation();
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

    void Camera::setFreeViewport(const Vec2I& _windowSize) {
        delete viewport;
        viewport = new FreeViewPort(_windowSize);
    }

    void Camera::setAdaptiveViewport(const Vec2I& _virtualDesiredSize, const Vec2I& _currentDeviceSize) {
        delete viewport;
        viewport = new AdaptiveViewPort(_virtualDesiredSize);
        viewport->update(_currentDeviceSize);
    }

    void Camera::translate(const Vec2F& _translation) {
        translate(_translation.x, _translation.y);
    }

    void Camera::translate(float _x, float _y) {
        auto _position = getPosition();
        setPosition({_position.x + _x, _position.y + _y});
    }

    void Camera::setCameraSize(const Vec2I& _cameraSize) {
        setCameraSize(_cameraSize.x, _cameraSize.y);
    }

    void Camera::setCameraSize(int _width, int _height) {
        onResize(_width, _height);
    }

    Vec2I Camera::getCameraSize() {
        return size;
    }

    bool Camera::isElementInside(Transform* _transform, const Vec2F& _size) const {
        auto _elementTopLeft = _transform->getModelMatrixPosition();
        auto _elementBottomRight = _elementTopLeft;
        _elementTopLeft += { -_size.x / 2.f, _size.y / 2.f };
        _elementBottomRight += { _size.x / 2.f, -_size.y / 2.f };

        auto _cameraTopLeft = transform->getModelMatrixPosition();
        auto _cameraBottomRight = transform->getModelMatrixPosition();
        _cameraTopLeft += { -(float)size.x / 2.f * zoom, (float)size.y / 2.f * zoom };
        _cameraBottomRight += { (float)size.x / 2.f * zoom, -(float)size.y / 2.f * zoom };

        if (_elementTopLeft.x == _elementBottomRight.x || _elementTopLeft.y == _elementBottomRight.y || _cameraBottomRight.x == _cameraTopLeft.x || _cameraTopLeft.y == _cameraBottomRight.y) return false;
        if(_elementTopLeft.x > _cameraBottomRight.x || _cameraTopLeft.x > _elementBottomRight.x) return false;
        if (_elementBottomRight.y > _cameraTopLeft.y || _cameraBottomRight.y > _elementTopLeft.y) return false;

        return true;
    }

}