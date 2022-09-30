//
// Created by borja on 9/5/22.
//

#ifndef GDE_TRANSFORM_H
#define GDE_TRANSFORM_H

#include <vector>
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "core/util/Vec.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace GDE {

    /**
     * @brief Component common to every entity that tells the engine where it is, which scale it has and how much it is rotated.
     */
    class Transform {

        private:
            glm::vec3 innerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat innerRotation = glm::quat(glm::vec3(0, 0, 0));
            glm::vec3 innerScale = glm::vec3(1.0f, 1.0f, 1.0f);

            glm::mat4 worldMatrixCache { 1.0f };
            bool dirty = false;

        public:
            bool staticTransform = false;

        private:
            glm::mat4 recalculateCachedMatrix();
            void setDirty();

        public:
            explicit Transform(const NodeID& _nodeId);

            /**
             * @brief Entity's ID.
             */
            NodeID ID;

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

            [[nodiscard]] Vec2F getModelMatrixPosition();
            [[nodiscard]] Vec2F getModelMatrixScale();
            [[nodiscard]] float getModelMatrixRotation();

            [[nodiscard]] glm::mat4 localToParent() const;
            [[nodiscard]] glm::mat4 parentToLocal() const;
            [[nodiscard]] glm::mat4 localToWorld();
            [[nodiscard]] glm::mat4 worldToLocal() const;

            void setMatrix(const glm::mat4& _matrix);
    };

}

#endif //GDE_TRANSFORM_H
