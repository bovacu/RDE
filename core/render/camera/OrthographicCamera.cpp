#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

    bool OrthographicCamera::usingAspectRatio = true;

    OrthographicCamera::OrthographicCamera(float _left, float _right, float _bottom, float _top)
            : projectionMatrix(glm::ortho(_left, _right, _bottom, _top, -1.0f, 1.0f)), viewMatrix(1.0f),
            viewProjectionMatrix(this->projectionMatrix * this->viewMatrix) {
        this->viewProjectionMatrix = this->projectionMatrix * this->viewMatrix;
    }

    void OrthographicCamera::setProjection(float _left, float _right, float _bottom, float _top) {
        this->projectionMatrix = glm::ortho(_left, _right, _bottom, _top, -1.0f, 1.0f);
        this->viewProjectionMatrix = this->projectionMatrix * this->viewMatrix;
    }

    void OrthographicCamera::recalculateViewMatrix() {
        glm::mat4 _transform = glm::translate(glm::mat4(1.0f), this->position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(this->rotation), glm::vec3(0, 0, 1));

        this->viewMatrix = glm::inverse(_transform);
        this->viewProjectionMatrix = this->projectionMatrix * this->viewMatrix;
    }

}