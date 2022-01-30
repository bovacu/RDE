// Created by borja on 30/12/21.


#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Camera.h"

namespace engine {

    void Camera::onResize(int _width, int _height) {
        aspectRatio = (float)_width/(float)_height;
        glViewport(0, 0, _width, _height);
        projectionMatrix = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -zoom, zoom);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    glm::mat4& Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    Transform& Camera::getTransform() {
        return transform;
    }

    void Camera::recalculateViewMatrix() {
        glm::mat4 _transform = glm::translate(glm::mat4(1.0f), {transform.getPosition().x, transform.getPosition().y, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(transform.getRotation()), glm::vec3(0, 0, 1));
        viewMatrix = glm::inverse(_transform);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
    }

    void Camera::setPosition(const Vec2F& _position) {
        transform.setPosition(_position);
        recalculateViewMatrix();
    }

    Vec2F Camera::getPosition() {
        return transform.getPosition();
    }

    void Camera::setRotation(float _rotation) {
        transform.setRotation(_rotation);
        recalculateViewMatrix();
    }

    float Camera::getRotation() {
        return transform.getRotation();
    }

    glm::mat4& Camera::getViewMatrix() {
        return viewMatrix;
    }

    glm::mat4& Camera::getViewProjectionMatrix() {
        return viewProjectionMatrix;
    }

    void Camera::onEvent(Event& _event) {
//        EventDispatcher dispatcher(_event);
//        dispatcher.dispatchEvent<MouseScrolledEvent>(ENGINE_BIND_EVENT_FN(Camera::onMouseScrolled));
    }

    bool Camera::onMouseScrolled(MouseScrolledEvent& _event) {
        zoom -= _event.getScrollY() * 0.1f;
        zoom = std::max(zoom, 0.5f);
        projectionMatrix = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -1.f, 1.f);
        return false;
    }

    float Camera::getAspectRatio() const {
        return aspectRatio;
    }

    float Camera::getCurrentZoomLevel() const {
        return zoom;
    }

    void Camera::setCurrentZoomLevel(float _zoomLevel) {
        zoom = _zoomLevel;
        projectionMatrix = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -1.f, 1.f);
    }

    float Camera::getZoomSpeed() const {
        return zoomSpeed;
    }

    void Camera::setZoomSpeed(float _zoomSpeed) {
        zoomSpeed = _zoomSpeed;
    }

}