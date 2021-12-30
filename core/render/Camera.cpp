// Created by borja on 30/12/21.


#include <glm/ext/matrix_clip_space.hpp>
#include "Camera.h"

namespace engine {


    void Camera::update(Window* _window) {
        glfwGetFramebufferSize(_window->getNativeWindow(), &width, &height);
        if (width != vpSize[0] ||  height != vpSize[1]) {
            vpSize[0] = width; vpSize[1] = height;
            glViewport(0, 0, vpSize[0], vpSize[1]);
            aspectRatio = (float)width/(float)height;
            projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        }
    }

    glm::mat4& Camera::getProjectionMatrix() {
        return projection;
    }

    Transform& Camera::getTransform() {
        return transform;
    }
}