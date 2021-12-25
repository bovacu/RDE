// Created by borja on 24/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include <glm/gtx/euler_angles.hpp>
#include <string>
#include "core/util/Util.h"
#include "core/render/camera/SceneCamera.h"

namespace engine {
    struct Transform {
        glm::mat4 transform { 1.0f };

        Transform() = default;
        Transform(const Transform& _transform) = default;
        Transform(const glm::mat4& _transform) : transform(_transform) {  }

        [[nodiscard]] Vec2F getPosition2D() const { return {transform[3][0], transform[3][1]}; }
        [[nodiscard]] float getX() const { return this->getPosition2D().x; }
        [[nodiscard]] float getY() const { return this->getPosition2D().y; }

        Vec2F getScale2D() const { return {transform[0][0], transform[1][1]}; }
        [[nodiscard]] float getScaleX() const { return this->getScale2D().x; }
        [[nodiscard]] float getScaleY() const { return this->getScale2D().y; }

        void setPosition2D(const Vec2F& _position) { this->transform[3][0] = _position.x; this->transform[3][1] = _position.y;}
        void setX(float _x) { this->transform[3][0] = _x; }
        void setY(float _y) { this->transform[3][1] = _y; }

        void setScale2D(const Vec2F& _scale) { this->transform[0][0] = _scale.x; this->transform[1][1] = _scale.y;}
        void setScaleX(float _x) { this->transform[0][0] = _x; }
        void setScaleY(float _y) { this->transform[1][1] = _y; }

        explicit operator glm::mat4& () { return this->transform; }
        explicit operator const glm::mat4& () const { return this->transform; }
    };

    struct Tag {
        std::string tag;

        Tag() = default;
        Tag(const Tag& _tag) = default;
        explicit Tag(const std::string& _tag) : tag(_tag) {  }

        explicit operator std::string& () { return this->tag; }
    };

    struct Active {
        bool active;

        Active() = default;
        Active(const Active& _active) = default;
        Active(bool _active) : active(_active) {  }
    };

/// --------------------------------- RENDERING

    struct CameraComponent {
//        SceneCamera sceneCamera;
        bool primary = true;
        bool fixedAspectRatio = false;
        CameraComponent() = default;
        CameraComponent(const CameraComponent& _camera) = default;
    };

    struct SpriteRenderer {
        Color color = Color::Purple;
        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;
        explicit SpriteRenderer(const Color& _color) : color(_color) {  }
    };
}

#endif //ENGINE2_0_COMPONENTS_H
