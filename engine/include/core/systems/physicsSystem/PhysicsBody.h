//
// Created by borja on 9/16/22.
//

#ifndef RDE_PHYSICS_BODY_H
#define RDE_PHYSICS_BODY_H


#include "core/render/shapes/DebugShape.h"
#include "chipmunk/chipmunk.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/ComponentBase.h"
#include "core/Enums.h"

#include <unordered_map>

namespace RDE {

    #ifdef __EMSCRIPTEN__
    typedef unsigned long ulong;
    #endif

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
		RDE_PHYSICS_SHAPE_TYPE_ type = RDE_PHYSICS_SHAPE_TYPE_BOX;
        Vec2F offset = { 0.f, 0.f };
        Vec2F size = { 32, 32 };
        std::vector<Vec2F> vertices {  };
        bool sensor = false;
        float friction = 0.4f;
        float restitution = 0.f;
    };

    typedef ulong PhysicsShapeId;
    struct PhysicsShape {
        ShapeConfig shapeConfig {};
        cpShape* shape = nullptr;
        PhysicsShapeId id = 0;
    };

    struct PhysicsBodyConfig {
        ShapeConfig shapeConfig {  };
        float mass = 1.0f;
		RDE_PHYSICS_BODY_TYPE_ physicsBodyType = RDE_PHYSICS_BODY_TYPE_DYNAMIC;
    };

    class Scene;
    class Manager;
    class Graph;

    struct Node;

    struct PhysicsBody : public ComponentBase {

        friend class PhysicsManager;
        friend class Graph;

        private:
            ulong keyCounter = 0;
            cpBody* body = nullptr;
            cpSpace* space = nullptr;
			std::unordered_map<PhysicsShapeId, PhysicsShape> physicsShapes;

        public:
            Transform* transform = nullptr;

        private:
			void calculateDataForBox(ShapeConfig& _shapeConfig, float _bodyMass, RDE_PHYSICS_BODY_TYPE_ _bodyType);
			void calculateDataForCircle(ShapeConfig& _shapeConfig, float _bodyMass, RDE_PHYSICS_BODY_TYPE_ _bodyType);
			void calculateDataForPolygon(ShapeConfig& _shapeConfig, float _bodyMass, RDE_PHYSICS_BODY_TYPE_ _bodyType);
			void calculateDataForSegment(ShapeConfig& _shapeConfig, float _bodyMass, RDE_PHYSICS_BODY_TYPE_ _bodyType);
			void createBody(ShapeConfig& _shapeConfig, float _moment, float _bodyMass, RDE_PHYSICS_BODY_TYPE_ _bodyType);
            void setupShape(ShapeConfig& _shapeConfig);

        public:
			RDE_FUNC PhysicsBody(Node* _node, Manager* _manager, Graph* _graph, PhysicsBodyConfig& _bodyConfig);
			RDE_FUNC ~PhysicsBody();
			RDE_FUNC void update();

			RDE_FUNC PhysicsShapeId addShape(ShapeConfig& _shapeConfig, const Vec2F& _position = {0.f, 0.f}, float _rotation = 0.f);
			RDE_FUNC bool removeShape(PhysicsShapeId _id);

			RDE_FUNC void setGroup(PhysicsShapeId _shapeID, uint _group);
			RDE_FUNC void removeGroup(PhysicsShapeId _shapeID);

			RDE_FUNC void setMask(PhysicsShapeId _shapeID, uint _mask);
			RDE_FUNC void removeMask(PhysicsShapeId _shapeID);

			RDE_FUNC void setToCollideWiths(PhysicsShapeId _shapeID, uint _toCollideWiths);
			RDE_FUNC void removeToCollideWiths(PhysicsShapeId _shapeID);

			RDE_FUNC void addMaskFilter(PhysicsShapeId _shapeID, uint _masksToAdd);
			RDE_FUNC void removeMaskFilter(PhysicsShapeId _shapeID, uint _masksToRemove);
			RDE_FUNC void resetMaskFilter(PhysicsShapeId _shapeID);

			RDE_FUNC_ND uint getMasks(PhysicsShapeId _shapeID);
			RDE_FUNC_ND uint getGroups(PhysicsShapeId _shapeID);
			RDE_FUNC_ND uint getToCollideWiths(PhysicsShapeId _shapeID);

			RDE_FUNC void setBodyType(const RDE_PHYSICS_BODY_TYPE_& _physicsBodyType);
			RDE_FUNC_ND RDE_PHYSICS_BODY_TYPE_ getBodyType();

			RDE_FUNC void setMass(float _mass);
			RDE_FUNC_ND float getMass();

			RDE_FUNC void setFriction(PhysicsShapeId _shapeID, float _friction);
			RDE_FUNC_ND float getFriction(PhysicsShapeId _shapeID);

			RDE_FUNC void setRestitution(PhysicsShapeId _shapeID, float _restitution);
			RDE_FUNC_ND float getRestitution(PhysicsShapeId _shapeID);

			RDE_FUNC void setLinearVelocity(const Vec2F& _linearVelocity);
			RDE_FUNC_ND Vec2F getLinearVelocity();

			RDE_FUNC void setAngularLinearVelocity(float _linearAngularVelocity);
			RDE_FUNC_ND float getAngularLinearVelocity();

			RDE_FUNC void setCenterOfGravity(const Vec2F& _centerOfGravity);
			RDE_FUNC_ND Vec2F getCenterOfGravity();

			RDE_FUNC void setGhost(PhysicsShapeId _shapeID, bool _ghost);
			RDE_FUNC_ND bool isGhost(PhysicsShapeId _shapeID);

			RDE_FUNC void applyImpulseLocal(const Vec2F& _impulse, const Vec2F& _where);
			RDE_FUNC void applyImpulseWorld(const Vec2F& _impulse, const Vec2F& _where);

			RDE_FUNC void applyForceLocal(const Vec2F& _force, const Vec2F& _where);
			RDE_FUNC void applyForceWorld(const Vec2F& _force, const Vec2F& _where);

			RDE_FUNC void setEnabled(bool _enabled) override;
			RDE_FUNC_ND bool isEnabled() override;
    };

}


#endif //RDE_PHYSICS_BODY_H
