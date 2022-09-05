//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/graph/components/Body.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
namespace GDE {

    void PhysicsManager::init() {
        world = new b2World({gravity.x, gravity.y});
        world->SetContactListener(&callback);
    }

    void PhysicsManager::step(Delta _delta) {
        if(!active) return;
        world->Step(_delta, 6, 2);
    }

    void PhysicsManager::setGravity(const Vec2F &_gravity) {
        gravity = _gravity;
        world->SetGravity({gravity.x, gravity.y});
    }

    Vec2F PhysicsManager::getGravity() const {
        return gravity;
    }

    void PhysicsManager::setPhysicsActive(bool _active) {
        active = _active;
    }

    bool PhysicsManager::isPhysicsActive() const {
        return active;
    }

    void PhysicsManager::destroy() {
        for(auto* _body : bodies)
            world->DestroyBody(_body);
        delete world;
    }

    b2Body* PhysicsManager::createBody(const b2BodyDef& _bodyDef) {
        auto* _body = world->CreateBody(&_bodyDef);
        bodies.push_back(_body);
        return _body;
    }

    void PhysicsManager::destroyBody(b2Body* _body) {
        auto _bodyToDestroy = std::find(bodies.begin(), bodies.end(), _body);
        if(_bodyToDestroy != bodies.end())
            bodies.erase(_bodyToDestroy);
    }

    void PhysicsManager::setWhatBodyCollidesWith(Body* _body, CollisionMask _bodyCollideWith) {
        auto* _filter = reinterpret_cast<b2Filter*>(_body->b2dConfig.body->GetUserData().pointer);
        _filter->maskBits = _bodyCollideWith;
        _body->b2dConfig.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(_body);
    }

    UDelegate<void(b2Contact*)>& PhysicsManager::setCallbackForCollisionBetweenMasks(CollisionMask _colliderA, CollisionMask _colliderB) {
        return callback.masks[_colliderA | _colliderB].callback;
    }

    void PhysicsManager::removeCollisionCallbackBetween(CollisionMask _colliderA, CollisionMask _colliderB) {
        callback.masks.erase(_colliderA | _colliderB);
    }

    void PhysicsManager::removeAllCollisionCallbacksFrom(CollisionMask _collider) {
        std::vector<CollisionMask> _masksToDelete;
        for(auto& _mask : callback.masks)
            if((_mask.first & _collider) != 0) _masksToDelete.push_back(_mask.first);

        for(auto& _mask : _masksToDelete)
            callback.masks.erase(_mask);
    }

    void PhysicsManager::CollisionCallback::BeginContact(b2Contact* contact) {
        b2ContactListener::BeginContact(contact);

        auto _a = contact->GetFixtureA()->GetBody();
        auto _b = contact->GetFixtureB()->GetBody();

        auto _aMask = reinterpret_cast<Body*>(_a->GetUserData().pointer);
        auto _bMask = reinterpret_cast<Body*>(_b->GetUserData().pointer);

        if(_aMask == nullptr || _bMask == nullptr) return;

        auto _aCategory = _aMask->b2dConfig.fixtureDef.filter.categoryBits;
        auto _bCategory = _bMask->b2dConfig.fixtureDef.filter.categoryBits;

        if(masks.find(_aCategory | _bCategory) != masks.end()) {
            masks[_aCategory | _bCategory].callback(contact);
            return;
        }
    }

    void PhysicsManager::CollisionCallback::EndContact(b2Contact* contact) {
        b2ContactListener::EndContact(contact);

        auto _a = contact->GetFixtureA();
        auto _b = contact->GetFixtureB();

        auto _aMask = reinterpret_cast<Body*>(_a->GetUserData().pointer);
        auto _bMask = reinterpret_cast<Body*>(_b->GetUserData().pointer);

        if(_aMask == nullptr || _bMask == nullptr) return;

        auto _aCategory = _aMask->b2dConfig.fixtureDef.filter.categoryBits;
        auto _bCategory = _bMask->b2dConfig.fixtureDef.filter.categoryBits;

        if(masks.find(_aCategory | _bCategory) != masks.end()) {
            masks[_aCategory | _bCategory].callback(contact);
            return;
        }
    }
}
#pragma clang diagnostic pop