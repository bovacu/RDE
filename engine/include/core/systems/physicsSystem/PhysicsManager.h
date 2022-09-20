//
// Created by borja on 22/04/22.
//

#ifndef GDE_PHYSICS_MANAGER_H
#define GDE_PHYSICS_MANAGER_H

#include "core/util/Util.h"
#include "box2d/box2d.h"
#include "PhysicsBody.h"
#include "PhysicsManifold.h"
#include "core/physics/Precompiled.h"

#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
    typedef unsigned long ulong;
#endif

namespace GDE {

    class RenderManager;

    class PhysicsManager {
        private:
            std::vector<Physics::Body*> bodies;
            std::vector<Physics::Manifold> contacts;

        public:
            uint32 steps = 10;
            Vec2F gravity;

        public:
            void init();
            void destroy();

            void step(Delta _fxDt);
        Physics::Body* add(Physics::Shape* _physicsShape, const Vec2F& _position);
            void debugRender(RenderManager* _renderManager);

        private:
//            void integrateForces( PhysicsBody *b, Delta _fxDt);
//            void integrateVelocity( PhysicsBody *b, Delta _fxDt);
    };

//    typedef ulong CollisionMask;
//    typedef UDelegate<void(b2Contact*)> CollisionMaskCallback;
//
//    enum BodyType {
//        DYNAMIC,
//        STATIC,
//        KINEMATIC
//    };
//
//    /**
//     * @brief Configuration of the Body. This includes the physic properties that a body should have.
//     *
//     */
//    struct BodyConfig {
//        /**
//         * @brief The mass of the entity.
//         *
//         */
//        float mass = 1;
//
//        /**
//         * @brief The size of the entity. If rectanlge, both values are used, if a polygon, both values are used and if circle, only x is used.
//         *
//         */
//        Vec2F size = { 64, 64 };
//
//        /**
//         * @brief Friction of the entity.
//         *
//         */
//        float friction = 0;
//
//        /**
//         * @brief Restitution of the entity.
//         *
//         */
//        float restitution = 0;
//
//        /**
//         * @brief Sets with what objects the entity can collide or trigger a ghost collision.
//         *
//         */
//        CollisionMask mask = 0;
//
//        /**
//         * @brief Type of body: dynamic, kinematic or static.
//         *
//         */
//        BodyType bodyType = BodyType::DYNAMIC;
//
//        /**
//         * @brief Type of shape: Box, Circle or Polygon.
//         *
//         */
//        BodyShapeType bodyShapeType = BodyShapeType::BOX;
//    };

//    /**
//     * @brief A wrapper around Box2D information.
//     *
//     */
//    struct B2DConfig {
//
//        /**
//         * @brief Box2D BodyDef.
//         *
//         */
//        b2BodyDef bodyDefinition;
//
//        /**
//         * @brief Box2D PolygonShape.
//         *
//         */
//        b2PolygonShape polygonShape;
//
//        /**
//         * @brief Box2D CircleShape.
//         *
//         */
//        b2CircleShape circleShape;
//
//        /**
//         * @brief Box2D FixtureDef.
//         *
//         */
//        b2FixtureDef fixtureDef;
//
//        /**
//         * @brief Box2D Body.
//         *
//         */
//        b2Body* body = nullptr;
//
//        /**
//         * @brief Box2D Fixture.
//         *
//         */
//        b2Fixture* fixture = nullptr;
//
//        /**
//         * @brief Position to compare the amount of space that the body should be transalated since the last update in case the body
//         * is moved by position and not by forces. This is not recomended at all.
//         *
//         */
//        Vec2F lastPosition;
//
//        float lastRotation;
//    };
//
//    struct Body;
//
//    /**
//     * @brief This is the class that allocates and deallocats all the physic objects, sets the physics configuration
//     * and manages all the collisions and its callbacks.
//     *
//     */
//    class PhysicsManager {
//        private:
//
//            /**
//             * @brief Data struct that contains which elements collide and what is the response on that collision.
//             *
//             */
//            struct Collision_CallbackFunction {
//                CollisionMask collisions;
//                UDelegate<void(b2Contact*)> callback;
//            };
//
//            /**
//             * @brief Box2D interface implementation to answer to collisions.
//             *
//             */
//            class CollisionCallback : public b2ContactListener {
//                friend class PhysicsManager;
//
//                /**
//                 * @brief Map that contains which entity collides with which other entity and the callback attach to them.
//                 *
//                 */
//                private:
//                    std::unordered_map<CollisionMask , Collision_CallbackFunction> masks;
//
//                private:
//                    /**
//                     * @see Box2D documentation
//                     */
//                    void BeginContact(b2Contact* contact) override;
//
//                     /**
//                     * @see Box2D documentation
//                     */
//                    void EndContact(b2Contact* contact) override;
//            };
//
//        private:
//            /**
//             * @brief Gravity applied to all of the entities with a Body component.
//             * @see Body
//             */
//            Vec2F gravity { 0, -90.8f };
//
//            /**
//             * @brief If the physics system is running or not.
//             *
//             */
//            bool active = true;
//
//            /**
//             * @brief Box2D world in charge of doing the physics simulation.
//             *
//             */
//            b2World* world = nullptr;
//
//            /**
//             * @brief All of the bodies that participate in the simulation.
//             *
//             */
//            std::vector<b2Body*> bodies;
//
//            /**
//             * @brief The implementation of Box2D's interface to collide with other bodies.
//             *
//             */
//            CollisionCallback callback;
//
//        public:
//            bool drawDebugInfo = true;
//
//        public:
//            PhysicsManager() = default;
//
//            /**
//             * @brief Set up everything needed to run the simulation.
//             *
//             */
//            void init();
//
//            /**
//             * @brief Advances the simulation.
//             *
//             * @param _delta Fixed amount of time passed from one frame to another.
//             */
//            void step(Delta _delta);
//
//            /**
//             * @brief Creates a Box2D body and adds it to the simulation.
//             *
//             * @param _bodyDef Definition of the Body.
//             * @return b2Body*
//             */
//            b2Body* createBody(const b2BodyDef& _bodyDef);
//
//            /**
//             * @brief Destroys a body from the simulation.
//             *
//             * @param _body Body to destroy.
//             */
//            void destroyBody(b2Body* _body);
//
//            /**
//             * @brief Sets the gravity
//             *
//             * @param _gravity Gravity
//             */
//            void setGravity(const Vec2F& _gravity);
//
//            /**
//             * @brief Returns the gravity
//             *
//             * @return Vec2F
//             */
//            [[nodiscard]] Vec2F getGravity() const;
//
//            /**
//             * @brief Enables or disables the system.
//             *
//             * @param _active
//             */
//            void setPhysicsActive(bool _active);
//
//            /**
//             * @brief Returns if the system is active or not.
//             *
//             * @return true
//             * @return false
//             */
//            [[nodiscard]] bool isPhysicsActive() const;
//
//            /**
//             * @brief Sets what body can collide with what other bodies.
//             *
//             * @param _body
//             * @param _bodyCollideWith
//             */
//            void setWhatBodyCollidesWith(Body* _body, CollisionMask _bodyCollideWith);
//
//            /**
//             * @brief Set the Callback For Collision Between Masks
//             *
//             * @param _colliderA
//             * @param _colliderB
//             * @return CollisionMaskCallback&
//             */
//            CollisionMaskCallback& setCallbackForCollisionBetweenMasks(CollisionMask _colliderA, CollisionMask _colliderB);
//
//            /**
//             * @brief Removes the collision detection callback between 2 bodies.
//             *
//             * @param _colliderA
//             * @param _colliderB
//             */
//            void removeCollisionCallbackBetween(CollisionMask _colliderA, CollisionMask _colliderB);
//
//            /**
//             * @brief Removes all collision detectin callback of a body with all the others.
//             *
//             * @param _collider
//             */
//            void removeAllCollisionCallbacksFrom(CollisionMask _collider);
//
//            /**
//             * @brief Called to clean up the system and shut it down. MUSN'T be called by end-user.
//             *
//             */
//            void destroy();
//    };

}

#endif //GDE_PHYSICS_MANAGER_H
