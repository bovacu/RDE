//
// Created by borja on 22/04/22.
//

#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "core/util/Util.h"
#include "box2d/box2d.h"

namespace GDE {

    typedef uint16 CollisionMask;

    enum BodyType {
        DYNAMIC,
        STATIC,
        KINEMATIC
    };

    enum BodyShapeType {
        BOX,
        CIRCLE,
        POLYGON
    };

    struct BodyConfig {
        float mass = 1;
        Vec2F size = { 64, 64 };
        float friction = 0;
        float restitution = 0;
        CollisionMask mask = 0;

        BodyType bodyType = BodyType::DYNAMIC;
        BodyShapeType bodyShapeType = BodyShapeType::BOX;
    };

    struct B2DConfig {
        b2BodyDef bodyDefinition;
        b2PolygonShape polygonShape;
        b2CircleShape circleShape;
        b2FixtureDef fixtureDef;
        b2Body* body{};
        b2Fixture* fixture;
        Vec2F lastPosition;
    };

    typedef std::tuple<CollisionMask, CollisionMask> MaskToMask;
    typedef std::tuple<CollisionMask, CollisionMask> key_t;
    struct key_hash : public std::unary_function<key_t, std::size_t> {
        std::size_t operator()(const key_t& k) const
        {
            return std::get<0>(k) ^ std::get<1>(k);
        }
    };

    class Body;
    class Physics {
        private:
            struct Collision_CallbackFunction {
                CollisionMask collisions;
                UDelegate<void(b2Contact*)> callback;
            };

            class CollisionCallback : public b2ContactListener {
                friend class Physics;

                private:
                    std::unordered_map<MaskToMask, Collision_CallbackFunction, key_hash> masks;

                private:
                    void BeginContact(b2Contact* contact) override;
                    void EndContact(b2Contact* contact) override;
            };

        private:
            Vec2F gravity { 0, -90.8f };
            bool active = true;
            b2World* world = nullptr;
            std::vector<b2Body*> bodies;
            CollisionCallback callback;

        private:
            Physics() = default;

        public:
            static Physics& get();

            void init();
            void step(Delta _delta);

            b2Body* createBody(const b2BodyDef& _bodyDef);
            void destroyBody(b2Body* _body);

            void setGravity(const Vec2F& _gravity);
            [[nodiscard]] Vec2F getGravity() const;

            void setPhysicsActive(bool _active);
            [[nodiscard]] bool isPhysicsActive() const;

            void setCollisionMasks(Body* _body, CollisionMask _bodyCollideWith);
            UDelegate<void(b2Contact*)>& setCallbackForCollision(CollisionMask _colliderA, CollisionMask _colliderB);

            void destroy();
    };

}

#endif //ENGINE_PHYSICS_H
