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
        PhysicsShapeType type = PhysicsShapeType::POLYGON;
        Vec2F size = { 32, 32 };
        std::vector<Vec2F> vertices {  };
        bool sensor = false;
    };

    typedef ulong PhysicsShapeId;
    struct PhysicsShape {
        ShapeConfig shapeConfig {};
        cpShape* shape = nullptr;
    };

    struct BodyConfig {
        ShapeConfig shapeConfig {  };
        bool isStatic = false;
        float dynamicFriction = 0.3f;
        float staticFriction = 0.5f;
        float restitution = 0.2f;
        float density = 1.0f;
        ulong collisionMask = 0;
        bool ignorePhysics = false;
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
            std::unordered_map<PhysicsShapeId, PhysicsShape> physicsShapes;
            cpSpace* space = nullptr;
            BodyConfig bodyConfig;

        public:
            Transform* transform = nullptr;

        private:
            void calculateDataForBox(const ShapeConfig& _shapeConfig);
            void calculateDataForCircle(const ShapeConfig& _shapeConfig);
            void calculateDataForPolygon(const ShapeConfig& _shapeConfig);
            void calculateDataForSegment(const ShapeConfig& _shapeConfig);
            void update();

        public:
            PhysicsBody(const NodeID& _id, Scene* _scene, const BodyConfig& _bodyConfig);
            ~PhysicsBody();

            PhysicsShapeId addShape(const ShapeConfig& _shapeConfig, const Vec2F& _position = {0.f, 0.f}, float _rotation = 0.f);
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
    };

}


#endif //GDE_PHYSICS_BODY_H
