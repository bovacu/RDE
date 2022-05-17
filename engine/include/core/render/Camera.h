// Created by borja on 30/12/21.


#ifndef ENGINE2_0_CAMERA_H
#define ENGINE2_0_CAMERA_H


#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/render/window/event/MouseEvent.h"
#include "core/render/elements/ViewPort.h"
#include "core/graph/Graph.h"

namespace GDE {

    class Transform;
    class SceneManager;
    class Camera {
        friend class Scene;
        private:
            Vec2I size;
            float zoom = 1, zoomSpeed = 0.25f;
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix {1.f};
            glm::mat4 viewProjectionMatrix;
            IViewPort* viewport;
            SceneManager* sceneManager;

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
            void translate(const Vec2F& _translation);
            void translate(float _x, float _y);
            Vec2F getPosition();

            void setCameraSize(const Vec2I& _cameraSize);
            void setCameraSize(int _width, int _height);
            Vec2I getCameraSize();

            void setRotation(float _rotation);
            float getRotation();

            [[nodiscard]] float getAspectRatio() const;

            [[nodiscard]] float getCurrentZoomLevel() const;
            void setCurrentZoomLevel(float _zoomLevel);

            [[nodiscard]] float getZoomSpeed() const;
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
