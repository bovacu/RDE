// Created by borja on 24/12/21.


#ifndef ENGINE2_0_SCENECAMERA_H
#define ENGINE2_0_SCENECAMERA_H


#include "core/util/Util.h"
#include "Camera.h"
#include "core/components/Components.h"

namespace engine {
    class SceneCamera : public engine::Camera {
        private:
            float orthographicSize = 10.f, distanceNear = -1.f, distanceFar = 1.f, aspectRatio = 0.f;
            Transform transform;

        private:
            void recalcProjectionMatrix();

        public:
            SceneCamera();
            ~SceneCamera() override = default;

            void setOrthographic(float _size, float _distanceNear, float _distanceFar);
            void setViewportSize(int _width, int _height);

            [[nodiscard]] float getOrthographicSize() const { return this->orthographicSize; }
            [[nodiscard]] float getNear() const { return this->distanceNear; }
            [[nodiscard]] float getFar() const { return this->distanceFar; }
            void setOrthographicSize(float _orthographicSize) { this->orthographicSize = _orthographicSize; }

            Transform& getTransform();
    };
}


#endif //ENGINE2_0_SCENECAMERA_H
