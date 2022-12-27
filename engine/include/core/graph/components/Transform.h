//
// Created by borja on 9/5/22.
//

#ifndef RDE_TRANSFORM_H
#define RDE_TRANSFORM_H

#include <vector>
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "core/util/Vec.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace RDE {

    FORWARD_DECLARE_STRUCT(Node, Graph)
    /**
     * @brief Component common to every entity that tells the engine where it is, which scale it has and how much it is rotated.
     */
    class Transform {
        FRIEND_CLASS(Graph, Camera, PhysicsBody, SpriteRenderer, DynamicSpriteRenderer, TextRenderer, UIImage, UI9Slice, UIText)
        MAKE_CLASS_ITERABLE(std::vector<Transform*>, children)

        protected:
            glm::vec3 innerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat innerRotation = glm::quat(glm::vec3(0, 0, 0));
            glm::vec3 innerScale = glm::vec3(1.0f, 1.0f, 1.0f);

            glm::mat4 worldMatrixCache { 1.0f };
            bool dirty = false;

            Graph* graph;

        protected:
            glm::mat4 recalculateCachedMatrix();
            void setDirty(Transform* _transform);
            virtual void clearDirty();
            glm::mat4 worldPointToLocalPosition(const Vec2F& _position);
            glm::mat4 worldPointToLocalRotation(float _rotation);

        public:
            explicit Transform(Graph* _graph);
            Node* node;

            /**
             * @brief ID of the direct parent.
             *
             */
            NodeID parent;

            /**
             * @brief Parent Transform
             */
            Transform* parentTransform = nullptr;

            /**
             * @brief All the children that an entity has.
             *
             */
            std::vector<Transform*> children;

            /**
             * @brief Returns the total number of children, from all of the generations.
             * @return int
             */
            int getChildrenCount();

            /**
             * @brief Returns the total number of children, from all of the generations, that are active.
             * @return int
             */
            int getActiveChildrenCount();

            /**
             * @brief Returns the total number of children, from all of the generations, that are enabled.
             * @return int
             */
            int getEnabledChildrenCount();

            /**
             * @brief Returns the total number of children, from all of the generations, that are enabled and active.
             * @return int
             */
            int getEnabledAndActiveChildrenCount();

            /**
             * @brief Sets the position of the object in Local Coordintes, so relative to its parent.
             * @param _position Vector with position.
             */
            void setPosition(const Vec2F& _position);

            /**
             * @brief Sets the position of the object in Local Coordintes, so relative to its parent.
             * @param _x Position x
             * @param _y Position y
             */
            void setPosition(float _x, float _y);

            /**
             * @brief Returns the position relative to its parent.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getPosition() const;

            /**
             * @brief Translates the current position by an amount. It is not the same as setPosition.
             * @param _direction The direction it is moving to.
             */
            void translate(const Vec2F& _direction);

            /**
             * @brief Translates the current position by an amount. It is not the same as setPosition.
             * @param _x Direction in x it is moving to
             * @param _y Direction in y it is moving to
             */
            void translate(float _x, float _y);

            /**
             * @brief Sets the rotation of the entity in Local value.
             * @param _rotation Degrees of rotation.
             */
            void setRotation(float _rotation);

            /**
             * @brief Returns the rotation relative to its parent.
             * @return float
             */
            [[nodiscard]] float getRotation() const;

            /**
             * @brief Rotates the entity by an amount of degrees. It is not the same as setRotation.
             * @param _amount Amount of degrees to rotate.
             */
            void rotate(float _amount);

            /**
             * @brief Sets the scale of the entity in Local value.
             * @param _scale Scale of the entity.
             */
            void setScale(const Vec2F& _scale);

            /**
             * @brief Sets the scale of the entity in Local value.
             * @param _x Scale in x
             * @param _y Scale in y
             */
            void setScale(float _x, float _y);

            /**
             * @brief Returns the scale relative to its parent.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getScale() const;

            /**
             * @brief Adds an amount to the current scale. It is not the same as setScale.
             * @param _scale The amount to increase or decrease the scale.
             */
            void scale(const Vec2F& _scale);

            /**
             * @brief Adds an amount to the current scale. It is not the same as setScale.
             * @param _x The amount to increase/decrease the scale in x
             * @param _y The amount to increase/decrease the scale in y
             */
            void scale(float _x, float _y);

            /**
             * @brief Returns the position of the Node in world coordinates. This is useful when the Node you need the position is a child of other Node and you need
             * the position relative to the world and not the parent (which getPosition() gives).
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getModelMatrixPosition();

            /**
             * @brief Returns the scale of the Node in world units. This is useful when the Node you need the scale is a child of other Node and you need
             * the scale relative to the world and not the parent (which getScale() gives).
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getModelMatrixScale();

            /**
             * @brief Returns the rotation of the Node in world degrees units. This is useful when the Node you need the rotation is a child of other Node and you need
             * the rotation relative to the world and not the parent (which getRotation() gives).
             * @return Vec2F
             */
            [[nodiscard]] float getModelMatrixRotation();

            /**
             * @brief Sets the position of the Node in world coordinates.
             * @param _worldPos Position in world coordinates.
             */
            void setMatrixModelPosition(const Vec2F& _worldPos);

            /**
             * @brief Translates the position of the Node in world coordinates units.
             * @param _worldPos Amount to translate.
             */
            void translateMatrixModelPosition(const Vec2F& _worldPos);

            /**
             * @brief Sets the rotation of the Node in world degrees.
             * @param _rotation Rotation in world degree units.
             */
            void setMatrixModelRotation(float _rotation);

            [[nodiscard]] glm::mat4 localToParent() const;
            [[nodiscard]] glm::mat4 parentToLocal() const;
            [[nodiscard]] virtual std::tuple<glm::mat4, bool> localToWorld();
            [[nodiscard]] glm::mat4 worldToLocal() const;

            glm::mat4 getLocalMatrix() const;
            void setLocalMatrix(const glm::mat4& _matrix);
    };
}

#endif //RDE_TRANSFORM_H
