// Created by borja on 30/12/21.


#ifndef ENGINE2_0_CAMERA_H
#define ENGINE2_0_CAMERA_H


#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/render/window/Window.h"
#include "core/systems/Components.h"
#include "core/render/window/event/MouseEvent.h"

namespace engine {

    class Camera {

        private:
            int width, height;
            int vpSize[2]{0, 0};
            float aspectRatio;
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix {1.f};
            glm::mat4 viewProjectionMatrix;
            Transform transform;

        public:
            void update(Window* _window);
            void onEvent(Event& _event);
            bool onMouseScrolled(MouseScrolledEvent& _event);

            glm::mat4& getProjectionMatrix();
            glm::mat4& getViewMatrix();
            glm::mat4& getViewProjectionMatrix();
            Transform& getTransform();

            void setPosition(const Vec2F& _position);
            Vec2F getPosition();

            void setRotation(float _rotation);
            float getRotation();

            float getAspectRatio() const;

            glm::mat4& operator() (const Camera& _camera) {
                return projectionMatrix;
            }

        private:
            void recalculateViewMatrix();
    };

}


#endif //ENGINE2_0_CAMERA_H
