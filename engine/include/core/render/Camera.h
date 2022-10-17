// Created by borja on 30/12/21.


#ifndef GDE_CAMERA_H
#define GDE_CAMERA_H


#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/render/elements/ViewPort.h"
#include "entt/entity/entity.hpp"

namespace GDE {

    typedef entt::entity NodeID;
    FORWARD_DECLARE_CLASS(Transform, SceneManager, Window)

    /**
     * @brief This class represents what the engine sees and what is going to be rendered. This is used as a Component of
     * the inner ECS System, so you can add Cameras to any desired Node.
     */
    class Camera {
        FRIEND_CLASS(Scene)
        private:
            /**
             * @brief Size of the camera, as a rectangle.
             */
            Vec2I size;

            /**
             * @brief Zoom of the camera.
             */
            float zoom = 1;

            /**
             * @brief How fast it zooms in/out.
             */
            float zoomSpeed = 0.25f;

            /**
             * @brief Projection Matrix, basically if it is Perspective or Orthographic.
             */
            glm::mat4 projectionMatrix;

            /**
             * @brief View Matrix, what the camera actually sees.
             */
            glm::mat4 viewMatrix {1.f};

            /**
             * @brief View Projection Matrix, the combination of both matrices.
             */
            glm::mat4 viewProjectionMatrix;

            /**
             * @see IViewPort
             */
            IViewPort* viewport;

            /**
             * @brief Transform of the camera.
             */
            Transform* transform;

        public:
            /**
             * @brief ID of the Node that contains this component.
             */
            NodeID ID;

            Camera(const NodeID& _mainCameraID, const Window* _window, Transform* _entityTransform);
            ~Camera();

            /**
             * @brief Handles events that concerns the camera.
             * @param _event Event to be handled
             */
            void onEvent(Event& _event);

            /**
             * @brief Handles the event of mouse scrolling.
             * @param _event Event to be handled
             * @return bool
             */
            bool onMouseScrolled(MouseScrolledEvent& _event);

            /**
             * @brief Resizes the camera components depending on the Window size.
             * @param _width Window's width
             * @param _height Window's height
             */
            void onResize(int _width, int _height);

            /**
             * @brief Returns the Projection Matrix.
             * @return glm::mat4&
             */
            glm::mat4& getProjectionMatrix();

            /**
             * @brief Returns the View Matrix.
             * @return glm::mat4&
             */
            glm::mat4& getViewMatrix();

            /**
             * @brief Returns the View Projection Matrix.
             * @return glm::mat4&
             */
            glm::mat4& getViewProjectionMatrix();

            /**
             * @brief Returns the Transform.
             * @return Transform&
             */
            Transform& getTransform();

            /**
             * @brief Sets the camera position.
             * @param _position Position to set the camera
             */
            void setPosition(const Vec2F& _position);

            /**
             * @brief Translates the camera position by _translation. It is not the same as setPosition.
             * @param _translation Amount of space to move the camera
             */
            void translate(const Vec2F& _translation);

            /**
             * @brief Translates the camera position by _translation. It is not the same as setPosition.
             * @param _x Amount of space to move the camera on X axis
             * @param _y Amount of space to move the camera on Y axis
             */
            void translate(float _x, float _y);

            /**
             * @brief Returns the camera position.
             * @return Vec2F
             */
            Vec2F getPosition();

            /**
             * @brief Sets the rectangle view size of the camera.
             * @param _cameraSize Size of the rectangle view of the camera
             */
            void setCameraSize(const Vec2I& _cameraSize);

            /**
             * @brief Sets the rectangle view size of the camera.
             * @param _width Width of the rectangle view of the camera
             * @param _height Height of the rectangle view of the camera
             */
            void setCameraSize(int _width, int _height);

            /**
             * @brief Returns the camera size.
             * @return Vec2I
             */
            Vec2I getCameraSize();

            void setRotation(float _rotation);
            float getRotation();

            /**
             * @brief Returns the aspect ratio of the Window.
             * @return float
             */
            [[nodiscard]] float getAspectRatio() const;

            /**
             * @brief Returns the current level of zoom of the camera.
             * @return float
             */
            [[nodiscard]] float getCurrentZoomLevel() const;

            /**
             * @brief Sets the current level of zoom of the camera.
             * @param _zoomLevel Zoom to set
             */
            void setCurrentZoomLevel(float _zoomLevel);

            /**
             * @brief Gets the speed of camera's zooming.
             * @return float
             */
            [[nodiscard]] float getZoomSpeed() const;

            /**
             * @brief Sets the speed of camera's zooming.
             * @param _zoomSpeed Camera's zooming speed
             */
            void setZoomSpeed(float _zoomSpeed);

            glm::mat4& operator() (const Camera& _camera) {
                return projectionMatrix;
            }

            /**
             * @brief Returns the viewport.
             * @return IViewPort*
             */
            [[nodiscard]] IViewPort* getViewport() const;

            /**
             * @brief Changes the current viewport to a FreeViewport.
             * @param _windowSize Window size
             */
            void setFreeViewport(const Vec2I& _windowSize);

            /**
             * @brief Changes the current viewport to a AdaptiveViewport.
             * @param _virtualDesiredSize  Virtual screen size
             * @param _currentDeviceSize Physical device size
             */
            void setAdaptiveViewport(const Vec2I& _virtualDesiredSize, const Vec2I& _currentDeviceSize);

            /**
             * @brief Returns if an element is inside the camera boundaries.
             * @return bool.
             */
            bool isElementInside(Transform* _transform, const Vec2F& _size) const;

            void update();

        private:
            /**
             * @brief Computes the View Matrix of the camera.
             */
            void recalculateViewMatrix();
    };

}


#endif //GDE_CAMERA_H
