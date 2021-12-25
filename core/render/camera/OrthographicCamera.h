#pragma once

#ifndef ENGINE2_0_ORTHOGRAPHIC_CAMERA_H
#define ENGINE2_0_ORTHOGRAPHIC_CAMERA_H

#include <glm/glm.hpp>
#include "core/util/Util.h"

namespace engine {

    class OrthographicCamera;
    typedef std::shared_ptr<OrthographicCamera> OrthographicCameraPtr;

    class OrthographicCamera {

        private:
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 viewProjectionMatrix;

            glm::vec3 position = { 0.0f, 0.0f, 0.0f };
            float rotation = 0.0f;

        public:
            static bool usingAspectRatio;

        public:
            OrthographicCamera(float _left, float _right, float _bottom, float _top);

            void setProjection(float _left, float _right, float _bottom, float _top);

            [[nodiscard]] const glm::vec3& getPosition() const { return this->position; }
            void setPosition(const Vec2F& _position) { this->setPosition({_position.x, _position.y, 0.0f}); }
            void setPosition(const glm::vec3& _position) { this->position = _position; this->recalculateViewMatrix(); }

            [[nodiscard]] float getRotation() const { return this->rotation; }
            void setRotation(float _rotation) { this->rotation = _rotation; this->recalculateViewMatrix(); }

            [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return this->projectionMatrix; }
            [[nodiscard]] const glm::mat4& getViewMatrix() const { return this->viewMatrix; }
            [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return this->viewProjectionMatrix; }

        private:
            void recalculateViewMatrix();
    };

}

#endif // ENGINE2_0_ORTHOGRAPHIC_CAMERA_H
