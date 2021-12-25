// Created by borja on 24/12/21.


#include <glm/ext/matrix_clip_space.hpp>
#include "SceneCamera.h"

namespace engine {

    SceneCamera::SceneCamera() {
        this->recalcProjectionMatrix();
    }

    void SceneCamera::recalcProjectionMatrix() {
        float _orthoLeft    = -this->orthographicSize * this->aspectRatio * 0.5f;
        float _orthoRight   =  this->orthographicSize * this->aspectRatio * 0.5f;
        float _orthoTop     =  this->orthographicSize * 0.5f;
        float _orthoBottom  = -this->orthographicSize * 0.5f;
        Camera::projectionMatrix = glm::ortho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, this->distanceNear, this->distanceFar);
    }

    void SceneCamera::setOrthographic(float _size, float _distanceNear, float _distanceFar) {
        this->orthographicSize = _size;
        this->distanceNear = _distanceNear;
        this->distanceFar = _distanceFar;
        this->recalcProjectionMatrix();
    }

    void SceneCamera::setViewportSize(int _width, int _height) {
        this->aspectRatio = (float)_width / (float)_height;
        this->recalcProjectionMatrix();
    }

    Transform& SceneCamera::getTransform() {
        return transform;
    }
}