//
// Created by borja on 22/04/22.
//

#ifndef GDE_PHYSICS_MANAGER_H
#define GDE_PHYSICS_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/systems/physicsSystem/PhysicsManifold.h"

#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
    typedef unsigned long ulong;
#endif

namespace GDE {

    class RenderManager;

    class PhysicsManager {
        private:
            std::vector<PhysicsBody*> bodies;
            std::vector<PhysicsManifold> contacts;

        public:
            uint32_t steps = 10;
            Vec2F gravity = {0.f, -50.f};

        public:
            void init();
            void destroy();

            void step(Delta _fxDt);
            PhysicsBody* add(PhysicsShape* _physicsShape, const Vec2F& _position);
            void debugRender(RenderManager* _renderManager);

        private:
            void integrateForces(PhysicsBody* _physicsBody, Delta _fxDt);
            void integrateVelocity(PhysicsBody* _physicsBody, Delta _fxDt);
    };
}

#endif //GDE_PHYSICS_MANAGER_H
