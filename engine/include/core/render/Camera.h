// Created by borja on 30/12/21.


#ifndef ENGINE2_0_CAMERA_H
#define ENGINE2_0_CAMERA_H


#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/render/window/event/MouseEvent.h"
#include "core/render/elements/ViewPort.h"
#include "core/graph/Graph.h"

namespace engine {

    class Transform;
    class Camera {
        friend class Scene;
        private:
            int width, height;
            float zoom = 1, zoomSpeed = 0.25f;
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix {1.f};
            glm::mat4 viewProjectionMatrix;
            IViewPort* viewport;

        public:
            NodeID ID;
            explicit Camera(const Window* _window, const NodeID& _mainCameraID);
            ~Camera();
            void onEvent(Event& _event);
            bool onMouseScrolled(MouseScrolledEvent& _event);
            void onResize(int _width, int _height);

            glm::mat4& getProjectionMatrix();
            glm::mat4& getViewMatrix();
            glm::mat4& getViewProjectionMatrix();
            Transform& getTransform();

            void setPosition(const Vec2F& _position);
            Vec2F getPosition();

            void setRotation(float _rotation);
            float getRotation();

            [[nodiscard]] float getAspectRatio() const;

            float getCurrentZoomLevel() const;
            void setCurrentZoomLevel(float _zoomLevel);

            float getZoomSpeed() const;
            void setZoomSpeed(float _zoomSpeed);

            glm::mat4& operator() (const Camera& _camera) {
                return projectionMatrix;
            }

            [[nodiscard]] IViewPort* getViewport() const;
            void setFreeViewport(const Window* _window);
            void setAdaptiveViewport(const Vec2I& _virtualDesiredSize, const Vec2I& _currentDeviceSize);

        private:
            void recalculateViewMatrix();
    };

}


#endif //ENGINE2_0_CAMERA_H
