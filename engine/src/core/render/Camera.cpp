// Created by borja on 30/12/21.


#include "core/util/Functions.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "core/render/Camera.h"
#include "core/graph/components/Transform.h"
#include "core/Engine.h"
#include "core/graph/components/Node.h"

namespace RDE {

    Camera::Camera(Node* _node, Manager* _manager, Graph* _graph, const Window* _window) : node(_node) {
		viewport = new ViewPort({ (int)_window->getWindowSize().x, (int)_window->getWindowSize().y }, this, _window);
		window = _window;
        onResize(_window->getWidth(), _window->getHeight());

		framebufferID = _manager->renderManager.getDefaultFramebufferID();
    }

	void Camera::onResize(const Vec2I& _size, float _aspectRatio) {
		viewport->onResize(_size.x, _size.y);
        projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -zoom, zoom);
        viewProjectionMatrix = projectionMatrix * viewMatrix;
        dirty = true;
    }

	void Camera::onResize(int _width, int _height) {
		viewport->onResize(_width, _height);
		float _aspectRatio = viewport->getAspectRatio();
		projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -zoom, zoom);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
		dirty = true;
	}

	void Camera::onResize(const Vec2I& _size) {
		onResize(_size.x, _size.y);
	}



    glm::mat4& Camera::getProjectionMatrix() {
        return projectionMatrix;
    }

    void Camera::recalculateViewProjectionMatrix() {
        auto [_mat, _dirty] = node->getTransform()->localToWorld();

        if(_dirty || dirty) {
            auto _screenCoords = Util::Math::worldToScreenCoords(viewport, {_mat[3][0], _mat[3][1]});
            _mat[3][0] = _screenCoords.x;
            _mat[3][1] = _screenCoords.y;

			float _aspectRatio = viewport->getAspectRatio();
			projectionMatrix = glm::ortho(-_aspectRatio * zoom, _aspectRatio * zoom, -zoom, zoom, -zoom, zoom);

            viewMatrix = glm::inverse(_mat);
            viewProjectionMatrix = projectionMatrix * viewMatrix;
            dirty = false;
            //node->getTransform()->update();
        }
    }

    glm::mat4& Camera::getViewMatrix() {
        return viewMatrix;
    }

    glm::mat4& Camera::getViewProjectionMatrix() {
        return viewProjectionMatrix;
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
        dirty = true;
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

    ViewPort* Camera::getViewport() const {
        return viewport;
    }

    bool Camera::isElementInside(Transform* _transform, const Vec2F& _size) const {
        auto _elementTopLeft = _transform->getWorldPosition();
        auto _elementBottomRight = _elementTopLeft;
        _elementTopLeft += { -_size.x / 2.f, _size.y / 2.f };
        _elementBottomRight += { _size.x / 2.f, -_size.y / 2.f };

        auto _cameraTopLeft = node->getTransform()->getWorldPosition();
        auto _cameraBottomRight = node->getTransform()->getWorldPosition();
		auto _viewportVRes = viewport->getSize();
		_cameraTopLeft += { -(float)_viewportVRes.x / 2.f * zoom, (float)_viewportVRes.y / 2.f * zoom };
		_cameraBottomRight += { (float)_viewportVRes.x / 2.f * zoom, -(float)_viewportVRes.y / 2.f * zoom };

        if (_elementTopLeft.x == _elementBottomRight.x || _elementTopLeft.y == _elementBottomRight.y || _cameraBottomRight.x == _cameraTopLeft.x || _cameraTopLeft.y == _cameraBottomRight.y) return false;
        if(_elementTopLeft.x > _cameraBottomRight.x || _cameraTopLeft.x > _elementBottomRight.x) return false;
        if (_elementBottomRight.y > _cameraTopLeft.y || _cameraBottomRight.y > _elementTopLeft.y) return false;

        return true;
    }

    void Camera::update() {
        recalculateViewProjectionMatrix();
        viewport->update();
    }

    bool Camera::isLandscape() {
        return viewport->landscape;
    }

    void Camera::setEnabled(bool _enabled) {
		enabled = _enabled;
    }

    bool Camera::isEnabled() {
        return enabled;
    }

}
