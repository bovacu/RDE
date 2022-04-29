//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/Physics.h"
#include "core/graph/Components.h"

namespace GDE {

    void Physics::init() {
        physicsSpace = cpSpaceNew();
        cpSpaceSetGravity(physicsSpace, {gravity.x, gravity.y});
    }

    Physics& Physics::get() {
        static Physics _physics;
        return _physics;
    }

    void Physics::step(Delta _delta) {
        cpSpaceStep(physicsSpace, _delta);
    }

    PhysicsSpace Physics::getSpace() {
        return physicsSpace;
    }

    void Physics::setGravity(const Vec2F &_gravity) {
        gravity = _gravity;
        cpSpaceSetGravity(physicsSpace, {gravity.x, gravity.y});
    }

    Vec2F Physics::getGravity() {
        return gravity;
    }

    void Physics::setPhysicsActive(float _active) {
        active = _active;
    }

    bool Physics::isPhysicsActive() {
        return active;
    }

    void Physics::registerCollisionMask(const std::string& _maskName) {
        registeredMasks[_maskName] = 1 << maskCount++;
    }

    void Physics::removeCollisionMask(const std::string& _maskName) {

    }

    int Physics::getCollisionMask(const std::string &_maskName) {
        return registeredMasks[_maskName];
    }

    void Physics::registerOnCollisionStartCallback(const std::string& _mask0, const std::string &_mask1, bool (*_onCollisionStarCallback)(cpArbiter *arb, cpSpace *space, void *data) ) {
        auto* _handler = cpSpaceAddCollisionHandler(physicsSpace, registeredMasks[_mask0], registeredMasks[_mask1]);
        _handler->beginFunc = reinterpret_cast<cpCollisionBeginFunc>(_onCollisionStarCallback);
    }

    void Physics::removeOnCollisionStartCallback(const std::string& _mask0, const std::string& _mask1) {

    }

    void Physics::registerOnCollisionEndCallback(const std::string& _mask0, const std::string& _mask1, UDelegate<void()> _onCollisionEndCallback) {

    }

    void Physics::removeOnCollisionEndCallback(const std::string& _mask0, const std::string& _mask1) {

    }

    void Physics::registerOnCollidingCallback(const std::string& _mask0, const std::string& _mask1, UDelegate<void()> _onCollidingCallback) {

    }

    void Physics::removeOnCollidingCallback(const std::string& _mask0, const std::string& _mask1) {

    }

    void Physics::destroy() {
        cpSpaceFree(physicsSpace);
    }

    void Physics::createCollisionFilter(Body& _body, const std::vector<std::string>& _otherMasks) {
        cpShapeFilter _filter;
        _filter.categories = _body.getCollisionMask();
        _filter.mask = ~CP_ALL_CATEGORIES;
        for(auto& _mask : _otherMasks) {
            _filter.mask |= registeredMasks[_mask];
        }

        cpShapeSetFilter((&_body)->shape, _filter);
    }
}