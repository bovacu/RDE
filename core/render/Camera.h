// Created by borja on 30/12/21.


#ifndef ENGINE2_0_CAMERA_H
#define ENGINE2_0_CAMERA_H


#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/render/window/Window.h"
#include "core/systems/Components.h"

namespace engine {

    class Camera {

        private:
            int width, height;
            int vpSize[2]{0, 0};
            float aspectRatio;
            glm::mat4 projection;
            Transform transform;

        public:
            void update(Window* _window);
            glm::mat4& getProjectionMatrix();
            Transform& getTransform();

            glm::mat4& operator() (const Camera& _camera) {
                return projection;
            }
    };

}


#endif //ENGINE2_0_CAMERA_H
