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

    FORWARD_DECLARE_CLASS(RenderManager)

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
        UDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionEnter;
        UDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionStay;
        UDelegate<void(PhysicsBody* bodyA, PhysicsBody* bodyB)> onCollisionExit;
    };

    class PhysicsManager {
        FRIEND_CLASS(PhysicsBody, Graph)

        private:
            std::vector<PhysicsBody*> bodies;
            cpSpace* space;
            inline static PhysicsCollisionCallbacks collisionCallbacksTable[MAX_MASKS][MAX_MASKS];
            Vec2F gravity = {0.f, -100.f};

        public:
            bool simulate = true;
            DebugOptions debugOptions;

        public:
            void init();
            void destroy();

            void update(Delta _dt);
            void step(Delta _fxDt);
            void add(PhysicsBody* _physicsBody);
            void remove(PhysicsBody* _physicsBody);
            void debugRender(RenderManager* _renderManager);

            void addCollisionCallbacks(uint _maskA, uint _maskB, PhysicsCollisionCallbacks _callbacks);
            PhysicsCollisionCallbacks& getCollisionCallbacks(uint _maskA, uint _maskB);

            void setGravity(const Vec2F& _gravity);
            Vec2F getGravity();

            int getStepIterations();
            void setStepIterations(int _iterations);

        private:
            void drawPolygon(PhysicsBody* _physicsBody, const ShapeConfig& _shapeConfig, RenderManager* _renderManager, const Color& _lineColor, const Color& _radiusColor, bool _showLines, bool _showRadius);
            static cpBool onCollisionEnter(cpArbiter* _arbiter, cpSpace* _space, void* _data);
            static void onCollisionExit(cpArbiter* _arbiter, cpSpace* _space, void* _data);
            static void onCollisionStay(cpArbiter* _arbiter, cpSpace* _space, void* _data);
    };
}

#endif //GDE_PHYSICS_MANAGER_H
