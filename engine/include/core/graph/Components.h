// Created by borja on 27/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include "submodules/glm/glm/ext/matrix_float4x4.hpp"
#include "core/util/Util.h"
#include "core/render/elements/Texture.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/Graph.h"
#include "core/util/Functions.h"
#include "glm/ext/matrix_transform.hpp"

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
        // Local Space
        private:
            glm::vec3 localPosition {0.0f, 0.0f, 0.0f};
            glm::vec3 localScale {1.0f, 1.0f, 1.0f};
            float localRotation = 0.0f;
            bool dirty = true;

        public:
            NodeID parent;
            std::vector<NodeID> children;

            // World Space
            glm::mat4 modelMatrix = glm::mat4(1.0f);

            glm::mat4 getLocalModelMatrix();
            void update(Graph* _graph);

            void setPosition(const Vec2F& _position);
            [[nodiscard]] Vec2F getPosition() const;
            void translate(const Vec2F& _direction);
            void translate(float _x, float _y);

            void setRotation(float _rotation);
            [[nodiscard]] float getRotation() const;
            void rotate(float _amount);

            void setScale(const Vec2F& _scale);
            [[nodiscard]] Vec2F getScale() const;
            void scale(const Vec2F& _scale);
            void scale(float _x, float _y);

            [[nodiscard]] bool isDirty() const;
    };
}

#endif //ENGINE2_0_COMPONENTS_H
