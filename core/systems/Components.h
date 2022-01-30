// Created by borja on 27/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include <glm/ext/matrix_float4x4.hpp>
#include "core/util/Util.h"
#include "core/render/elements/Texture.h"

namespace engine {

    struct Transform {
        glm::mat4 transformMatrix {1.0f };
        float rotation = 0.f;

        Transform() = default;
        Transform(const Transform& _transform) = default;
        Transform(const glm::mat3& _transform) : transformMatrix(_transform) {  }

        [[nodiscard]] Vec2F getPosition() const { return {transformMatrix[3][0], transformMatrix[3][1]}; }
        [[nodiscard]] float getX() const { return getPosition().x; }
        [[nodiscard]] float getY() const { return getPosition().y; }

        Vec2F getScale() const { return {transformMatrix[0][0], transformMatrix[1][1]}; }
        [[nodiscard]] float getScaleX() const { return getScale().x; }
        [[nodiscard]] float getScaleY() const { return getScale().y; }

        [[nodiscard]] float getRotation() const { return rotation; }

        void setPosition(const Vec2F& _position) { transformMatrix[3][0] = _position.x; transformMatrix[3][1] = _position.y;}
        void setX(float _x) { transformMatrix[3][0] = _x; }
        void setY(float _y) { transformMatrix[3][1] = _y; }

        void setScale(const Vec2F& _scale) { transformMatrix[0][0] = _scale.x; transformMatrix[1][1] = _scale.y;}
        void setScaleX(float _x) { transformMatrix[0][0] = _x; }
        void setScaleY(float _y) { this->transformMatrix[1][1] = _y; }

        void setRotation(float _rotation) { rotation = _rotation; }

        explicit operator glm::mat4& () { return this->transformMatrix; }
        explicit operator const glm::mat4& () const { return this->transformMatrix; }
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
        explicit Active(bool _active) : active(_active) {  }
    };

    struct SpriteRenderer {
        Color color = Color::White;
        Texture* texture = nullptr;
        GLuint shaderID = -1;
        int layer = 0;

        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;
        SpriteRenderer(Texture* _texture, const Color& _color) : texture(_texture), color(_color) {  }
    };

}

#endif //ENGINE2_0_COMPONENTS_H
