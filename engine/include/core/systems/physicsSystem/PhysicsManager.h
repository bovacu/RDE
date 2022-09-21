//
// Created by borja on 22/04/22.
//

#ifndef GDE_PHYSICS_MANAGER_H
#define GDE_PHYSICS_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/systems/physicsSystem/PhysicsManifold.h"
#include <set>

namespace GDE {

    class RenderManager;

    typedef std::pair<ulong, ulong> CollisionMaskPair;
    typedef std::set<CollisionMaskPair> CollisionTable;

    class PhysicsManager {

        private:
            std::vector<PhysicsBody*> bodies;
            std::vector<PhysicsManifold> contacts;
            UDelegate<bool (PhysicsManifold& manifold, PhysicsBody* bodyA, PhysicsBody* bodyB)> collisionHandler[PhysicsShape::MAX][PhysicsShape::MAX];
            CollisionTable collisionTable;

        public:
            uint32_t steps = 10;
            Vec2F gravity = {0.f, -50.f};
            bool simulate = true;

        public:
            void init();
            void destroy();

            void step(Delta _fxDt);
            void add(PhysicsBody* _physicsBody);
            void debugRender(RenderManager* _renderManager);

            [[nodiscard]] CollisionTable getCollisionTable() const;
            bool addCollisionToTable(ulong _bodyMaskA, ulong _bodyMaskB);
            bool removeCollisionToTable(ulong _bodyMaskA, ulong _bodyMaskB);
            bool hasCollisionInTable(ulong _bodyMaskA, ulong _bodyMaskB);

        private:
            void integrateForces(PhysicsBody* _physicsBody, Delta _fxDt);
            void integrateVelocity(PhysicsBody* _physicsBody, Delta _fxDt);
            bool collisionCircleCircle(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
            bool collisionCirclePolygon(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
            bool collisionPolygonCircle(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
            bool collisionPolygonPolygon(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
            float findAxisWithLeastPenetration(uint32_t* _faceIndex, PhysicsShape* _shapeA, PhysicsShape* _shapeB);
            void findIncidentFace(Vec2F* _vec, PhysicsShape* _refPoly, PhysicsShape* _incPoly, uint32_t _referenceIndex);
            int clip(Vec2F _n, float _c, Vec2F* _face);
    };
}

#endif //GDE_PHYSICS_MANAGER_H
