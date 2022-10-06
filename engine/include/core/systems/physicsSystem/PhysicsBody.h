//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_BODY_H
#define GDE_PHYSICS_BODY_H


#include "core/render/shapes/DebugShape.h"
#include "core/systems/physicsSystem/PhysicsMath.h"
#include "core/systems/physicsSystem/PhysicsShape.h"
#include "chipmunk/chipmunk.h"

namespace GDE {

    enum PhysicsBodyType {
        STATIC,
        KINEMATIC,
        DYNAMIC
    };

    enum PhysicsShapeType {
        BOX,
        CIRCLE,
        POLYGON,
        SEGMENT
    };

    struct ShapeMaskingConfig {

        /**
         * @brief A value shared across different shapes. All shapes under the same group won't collide.
         */
        uint group = CP_NO_GROUP;

        /**
         * @brief Those are the categories that the shape belongs to. Must be a 2^n.
         */
        uint mask = CP_ALL_CATEGORIES;

        /**
         * @brief Those are the categories the shape will collide with .Must be a 2^n.
         */
        uint toCollideWith = CP_ALL_CATEGORIES;
    };

    struct ShapeConfig {
        ShapeMaskingConfig shapeMaskingConfig;
        PhysicsShapeType type = PhysicsShapeType::BOX;
        Vec2F size = { 32, 32 };
        std::vector<Vec2F> vertices {  };
        bool sensor = false;
        float density = 1.f;
        float friction = 0.3f;
        float restitution = 0.2f;
    };

    typedef ulong PhysicsShapeId;
    struct PhysicsShape {
        ShapeConfig shapeConfig {};
        cpShape* shape = nullptr;
        PhysicsShapeId id;
    };

    struct BodyConfig {
        ShapeConfig shapeConfig {  };
        Vec2F offset = { 0.f, 0.f };
        float mass = 1.0f;
        PhysicsBodyType physicsBodyType = PhysicsBodyType::DYNAMIC;
    };

    FORWARD_DECLARE_CLASS(Scene)

    struct PhysicsBody {

        FRIEND_CLASS(PhysicsManager, Graph)

        private:
            ulong keyCounter = 0;
            cpBody* body = nullptr;
            cpSpace* space = nullptr;
            std::unordered_map<PhysicsShapeId, PhysicsShape> physicsShapes;

        public:
            Transform* transform = nullptr;

        private:
            void calculateDataForBox(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType);
            void calculateDataForCircle(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType);
            void calculateDataForPolygon(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType);
            void calculateDataForSegment(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType);
            void createBody(ShapeConfig& _shapeConfig, float _moment, float _bodyMass, PhysicsBodyType _bodyType);
            void update();

        public:
            PhysicsBody(const NodeID& _id, Scene* _scene, BodyConfig& _bodyConfig);
            ~PhysicsBody();

            PhysicsShapeId addShape(ShapeConfig& _shapeConfig, const Vec2F& _position = {0.f, 0.f}, float _rotation = 0.f);
            bool removeShape(PhysicsShapeId _id);

            void setGroup(PhysicsShapeId _shapeID, uint _group);
            void removeGroup(PhysicsShapeId _shapeID);

            void setMask(PhysicsShapeId _shapeID, uint _mask);
            void removeMask(PhysicsShapeId _shapeID);

            void addMaskFilter(PhysicsShapeId _shapeID, uint _masksToAdd);
            void removeMaskFilter(PhysicsShapeId _shapeID, uint _masksToRemove);
            void resetMaskFilter(PhysicsShapeId _shapeID);

            uint getMasks(PhysicsShapeId _shapeID);
            uint getGroups(PhysicsShapeId _shapeID);
            uint getToCollideWiths(PhysicsShapeId _shapeID);

            void setBodyType(const PhysicsBodyType& _physicsBodyType);
            PhysicsBodyType getBodyType();

            void setMass(float _mass);
            float getMass();

            void setFriction(PhysicsShapeId _shapeID, float _friction);
            float getFriction(PhysicsShapeId _shapeID);

            void setRestitution(PhysicsShapeId _shapeID, float _restitution);
            float getRestitution(PhysicsShapeId _shapeID);

            void setLinearVelocity(const Vec2F& _linearVelocity);
            Vec2F getLinearVelocity();

            void setAngularLinearVelocity(float _linearAngularVelocity);
            float getAngularLinearVelocity();

            void setCenterOfGravity(const Vec2F& _centerOfGravity);
            Vec2F getCenterOfGravity();

            void setGhost(PhysicsShapeId _shapeID, bool _ghost);
            bool isGhost(PhysicsShapeId _shapeID);

            void applyImpulseLocal(const Vec2F& _impulse, const Vec2F& _where);
            void applyImpulseWorld(const Vec2F& _impulse, const Vec2F& _where);

            void applyForceLocal(const Vec2F& _force, const Vec2F& _where);
            void applyForceWorld(const Vec2F& _force, const Vec2F& _where);
    };

}


#endif //GDE_PHYSICS_BODY_H
