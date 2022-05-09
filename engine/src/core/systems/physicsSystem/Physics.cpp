//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/Physics.h"
#include "core/graph/Components.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
namespace GDE {

    void Physics::init() {
        world = new b2World({gravity.x, gravity.y});
        world->SetContactListener(&callback);
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

    Vec2F Physics::getGravity() const {
        return gravity;
    }

    void Physics::setPhysicsActive(bool _active) {
        active = _active;
    }

    bool Physics::isPhysicsActive() const {
        return active;
    }

    void Physics::destroy() {
        for(auto* _body : bodies)
            world->DestroyBody(_body);
        delete world;
    }

    b2Body* Physics::createBody(const b2BodyDef& _bodyDef) {
        auto* _body = world->CreateBody(&_bodyDef);
        bodies.push_back(_body);
        return _body;
    }

    void Physics::destroyBody(b2Body* _body) {
        auto _bodyToDestroy = std::find(bodies.begin(), bodies.end(), _body);
        if(_bodyToDestroy != bodies.end())
            bodies.erase(_bodyToDestroy);
    }

    void Physics::setCollisionMasks(Body* _body, CollisionMask _bodyCollideWith) {
        auto* _filter = reinterpret_cast<b2Filter*>(_body->b2dConfig.body->GetUserData().pointer);
        _filter->maskBits = _bodyCollideWith;
        _body->b2dConfig.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(_body);
    }

    UDelegate<void(b2Contact*)>& Physics::setCallbackForCollision(CollisionMask _colliderA, CollisionMask _colliderB) {
        return callback.masks[{_colliderA, _colliderB}].callback;
    }

    void Physics::CollisionCallback::BeginContact(b2Contact* contact) {
        b2ContactListener::BeginContact(contact);

        auto _a = contact->GetFixtureA()->GetBody();
        auto _b = contact->GetFixtureB()->GetBody();

        auto _aMask = reinterpret_cast<Body*>(_a->GetUserData().pointer);
        auto _bMask = reinterpret_cast<Body*>(_b->GetUserData().pointer);

        if(_aMask == nullptr || _bMask == nullptr) return;

        std::tuple<CollisionMask, CollisionMask> _tupleA = {_aMask->b2dConfig.fixtureDef.filter.categoryBits, _bMask->b2dConfig.fixtureDef.filter.categoryBits};
        std::tuple<CollisionMask, CollisionMask> _tupleB = {_bMask->b2dConfig.fixtureDef.filter.categoryBits, _aMask->b2dConfig.fixtureDef.filter.categoryBits};

        if(masks.find(_tupleA) != masks.end()) {
            masks[_tupleA].callback(contact);
            return;
        }

        if(masks.find(_tupleB) != masks.end()) {
            masks[_tupleB].callback(contact);
        }
    }

    void Physics::CollisionCallback::EndContact(b2Contact* contact) {
        b2ContactListener::EndContact(contact);

        auto _a = contact->GetFixtureA();
        auto _b = contact->GetFixtureB();

        auto _aMask = reinterpret_cast<b2Filter*>(_a->GetUserData().pointer);
        auto _bMask = reinterpret_cast<b2Filter*>(_b->GetUserData().pointer);

        if(_aMask == nullptr || _bMask == nullptr) return;

        std::tuple<CollisionMask, CollisionMask> _tupleA = {_aMask->categoryBits, _bMask->categoryBits};
        std::tuple<CollisionMask, CollisionMask> _tupleB = {_bMask->categoryBits, _aMask->categoryBits};

        if(masks.find(_tupleA) != masks.end()) {
            masks[_tupleA].callback(contact);
            return;
        }

        if(masks.find(_tupleB) != masks.end()) {
            masks[_tupleB].callback(contact);
        }
    }
}
#pragma clang diagnostic pop