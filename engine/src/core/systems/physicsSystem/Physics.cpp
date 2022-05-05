//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/Physics.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
namespace GDE {

    void Physics::init() {
        world = new b2World({gravity.x, gravity.y});
    }

    Physics& Physics::get() {
        static Physics _physics;
        return _physics;
    }

    void Physics::step(Delta _delta) {
        if(!active) return;
        world->Step(_delta, 6, 2);
    }

    void Physics::setGravity(const Vec2F &_gravity) {
        gravity = _gravity;
        world->SetGravity({gravity.x, gravity.y});
    }

    Vec2F Physics::getGravity() {
        return gravity;
    }

    void Physics::setPhysicsActive(bool _active) {
        active = _active;
    }

    bool Physics::isPhysicsActive() const {
        return active;
    }

    void Physics::destroy() {
        delete world;
    }
}
#pragma clang diagnostic pop