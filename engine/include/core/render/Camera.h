// Created by borja on 30/12/21.


#ifndef RDE_CAMERA_H
#define RDE_CAMERA_H


#include "core/graph/components/ComponentBase.h"
#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/render/elements/ViewPort.h"
#include "entt/entity/entity.hpp"

namespace RDE {

    typedef entt::entity NodeID;
    FORWARD_DECLARE_CLASS(Transform, SceneManager, Window, Node, Manager, Graph)

    /**
     * @brief This class represents what the engine sees and what is going to be rendered. This is used as a Component of
     * the inner ECS System, so you can add Cameras to any desired Node.
     */
    class Camera : public ComponentBase {
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
            ViewPort* viewport;

        public:
            Node* node;

            Camera(Node* _node, Manager* _manager, Graph* _graph, const Window* _window);
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
            [[nodiscard]] ViewPort* getViewport() const;

            bool isLandscape();

            /**
             * @brief Returns if an element is inside the camera boundaries.
             * @return bool.
             */
            bool isElementInside(Transform* _transform, const Vec2F& _size) const;

            void update();

            void setEnabled(bool _enabled) override;
            bool isEnabled() override;

        private:
            /**
             * @brief Computes the View Matrix of the camera.
             */
            void recalculateViewMatrix();
    };

}


#endif //RDE_CAMERA_H
