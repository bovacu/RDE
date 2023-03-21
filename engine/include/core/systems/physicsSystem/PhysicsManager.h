//
// Created by borja on 22/04/22.
//

#ifndef RDE_PHYSICS_MANAGER_H
#define RDE_PHYSICS_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/physicsSystem/PhysicsBody.h"
#include <set>

namespace RDE {

    class RenderManager;

    struct DebugOptions {
        bool showGeneralDebug = true;
        bool showCircleLines = true;
        bool showBoxLines = true;
        bool showPolygonLines = true;
        bool showCircleRadius = true;
        bool showPolygonRadius = true;
        bool showBoxRadius = true;
        Color circleLineColor = Color::Blue;
        Color boxLineColor = Color::Blue;
        Color polygonLineColor = Color::Blue;
        Color circleRadiusColor = Color::Yellow;
        Color boxRadiusColor = Color::Yellow;
        Color polygonRadiusColor = Color::Yellow;
    };

    // This is because uint has a maximum of 32 bits
    #ifndef MAX_MASKS
    #define MAX_MASKS 32
    #endif

    struct PhysicsCollisionCallbacks {
        UniqueDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionEnter;
        UniqueDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionStay;
        UniqueDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionExit;
    };

    class PhysicsManager {
        friend class PhysicsBody;
        friend class Scene;

        private:
            std::vector<PhysicsBody*> bodies;
            cpSpace* space;
            inline static PhysicsCollisionCallbacks collisionCallbacksTable[MAX_MASKS][MAX_MASKS];
            Vec2F gravity = {0.f, -100.f};

        public:
            bool simulate = true;
            DebugOptions debugOptions;

        public:
			RDE_FUNC void init();
			RDE_FUNC void destroy();

			RDE_FUNC void update(Delta _dt);
			RDE_FUNC void step(Delta _fxDt);
			RDE_FUNC void add(PhysicsBody* _physicsBody);
			RDE_FUNC void remove(PhysicsBody* _physicsBody);
			RDE_FUNC void debugRender(RenderManager* _renderManager);

			RDE_FUNC void addCollisionCallbacks(uint _maskA, uint _maskB, PhysicsCollisionCallbacks _callbacks);
			RDE_FUNC_ND PhysicsCollisionCallbacks& getCollisionCallbacks(uint _maskA, uint _maskB);

			RDE_FUNC void setGravity(const Vec2F& _gravity);
			RDE_FUNC_ND Vec2F getGravity();

			RDE_FUNC_ND int getStepIterations();
			RDE_FUNC void setStepIterations(int _iterations);

        private:
            void drawPolygon(PhysicsBody* _physicsBody, const ShapeConfig& _shapeConfig, RenderManager* _renderManager, const Color& _lineColor, const Color& _radiusColor, bool _showLines, bool _showRadius);
            static cpBool onCollisionEnter(cpArbiter* _arbiter, cpSpace* _space, void* _data);
            static void onCollisionExit(cpArbiter* _arbiter, cpSpace* _space, void* _data);
            static void onCollisionStay(cpArbiter* _arbiter, cpSpace* _space, void* _data);
    };
}

#endif //RDE_PHYSICS_MANAGER_H
