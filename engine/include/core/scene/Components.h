// Created by borja on 27/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include "submodules/glm/glm/ext/matrix_float4x4.hpp"
#include "core/util/Util.h"
#include "core/render/elements/Texture.h"
#include "core/render/elements/ShaderManager.h"
#include "core/scene/Scene.h"
#include "core/util/Functions.h"
#include "glm/ext/matrix_transform.hpp"
//#include "core/Engine.h"

namespace engine {

    struct Tag {
        std::string tag;

        Tag() = default;
        Tag(const Tag& _tag) = default;
        explicit Tag(const std::string& _tag) : tag(_tag) {  }

        explicit operator std::string& () { return tag; }
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
        GLuint shaderID = ShaderManager::get().getShader("basic");
        int layer = 0;

        SpriteRenderer() = default;
        SpriteRenderer(const SpriteRenderer&) = default;
        explicit SpriteRenderer(Texture* _texture) : texture(_texture) {  }
    };

    struct Hierarchy {
        std::size_t children{};
        NodeID parent       { NODE_ID_NULL };
        NodeID prevBrother  { NODE_ID_NULL };
        NodeID nextBrother  { NODE_ID_NULL };
        NodeID firstChild   { NODE_ID_NULL };
        NodeID lastChild    { NODE_ID_NULL };

        Hierarchy() = default;
        Hierarchy(const Hierarchy&) = default;
        Hierarchy(const NodeID& _firstChild, const NodeID& _prevBrother, const NodeID& _nextBrother, const NodeID& _parent)
        : firstChild(_firstChild), prevBrother(_prevBrother), nextBrother(_nextBrother), parent(_parent) {  }
    };

    struct Transform {
        Scene* scene = nullptr;
        NodeID id;
        glm::mat4 transformMatrix { 1.0f };
        float rotation = 0.f;

        Transform() = default;
        ~Transform() = default;
        Transform(const Transform& _transform) = default;
        Transform(Scene* _scene, const NodeID& _nodeID) : scene(_scene), id(_nodeID) {  }

        [[nodiscard]] Vec2F getPosition() const;
        [[nodiscard]] float getX() const { return getPosition().x; }
        [[nodiscard]] float getY() const { return getPosition().y; }

        [[nodiscard]] Vec2F getScale() const { return { transformMatrix[0][0], transformMatrix[1][1] }; }
        [[nodiscard]] float getScaleX() const { return getScale().x; }
        [[nodiscard]] float getScaleY() const { return getScale().y; }

        [[nodiscard]] float getRotation() const { return rotation; }

        void setPosition(const Vec2F& _position);
        void setX(float _x);
        void setY(float _y);

        void setScale(const Vec2F& _scale);
        void setScaleX(float _x) { transformMatrix[0][0] = _x; }
        void setScaleY(float _y) { transformMatrix[1][1] = _y; }

        void setRotation(float _rotation);

        explicit operator glm::mat4& () { return transformMatrix; }
        explicit operator const glm::mat4& () const { return transformMatrix; }

        private:
            glm::vec4 transformPosition(const Vec2F& _position);
            void traverseChildrenPosition(const Vec2F& _distance, const NodeID& _node);
            void traverseChildrenRotation(float _difference, const NodeID& _node, const NodeID& _parent);
            void traverseChildrenScale(const Vec2F& _scale, const NodeID& _node);
    };

}

#endif //ENGINE2_0_COMPONENTS_H
