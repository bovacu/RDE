#pragma once

#ifndef ENGINE2_0_CAMERA_H
#define ENGINE2_0_CAMERA_H

#include <glm/glm.hpp>

namespace engine {

    class Camera {
        protected:
            glm::mat4 projectionMatrix = glm::mat4(1.0f);

        public:
            Camera() = default;
            explicit Camera(const glm::mat4& _projectionMatrix) : projectionMatrix(_projectionMatrix) {  }
            virtual ~Camera() = default;
            const glm::mat4& getProjectionMatrix() const { return this->projectionMatrix; }
    };

}

#endif //ENGINE2_0_CAMERA_H
