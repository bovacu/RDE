//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"
#include "core/graph/components/Node.h"

namespace RDE {

    PhysicsBody::PhysicsBody(Node* _node, Manager* _manager, Graph* _graph, PhysicsBodyConfig& _bodyConfig) {
        space = _manager->physics.space;
        transform = _node->getTransform();

        switch (_bodyConfig.shapeConfig.type) {
            case PhysicsShapeType::CIRCLE:
                calculateDataForCircle(_bodyConfig.shapeConfig, _bodyConfig.mass, _bodyConfig.physicsBodyType);
                break;
            case PhysicsShapeType::POLYGON:
                calculateDataForPolygon(_bodyConfig.shapeConfig, _bodyConfig.mass, _bodyConfig.physicsBodyType);
                break;
            case PhysicsShapeType::BOX:
                calculateDataForBox(_bodyConfig.shapeConfig, _bodyConfig.mass, _bodyConfig.physicsBodyType);
                break;
            case PhysicsShapeType::SEGMENT:
                calculateDataForSegment(_bodyConfig.shapeConfig, _bodyConfig.mass, _bodyConfig.physicsBodyType);
                break;
        }

        cpBodySetUserData(body, this);
        _manager->physics.add(this);

    }

    void PhysicsBody::createBody(ShapeConfig& _shapeConfig, float _moment, float _bodyMass, PhysicsBodyType _bodyType) {
        switch (_bodyType) {
            case STATIC:
                body = cpBodyNewStatic();
                break;
            case KINEMATIC:
                body = cpBodyNewKinematic();
                break;
            case DYNAMIC:
                body = cpBodyNew(_bodyMass, _moment);
                break;
        }

        cpBodySetPosition(body, { transform->getModelMatrixPosition().x, transform->getModelMatrixPosition().y });
        auto _rotation = transform->getModelMatrixRotation();
        auto _radians = Util::Math::degreesToRadians(_rotation);
        cpBodySetAngle(body, _radians);
        cpSpaceReindexShapesForBody(space, body);
        cpSpaceAddBody(space, body);
    }

    void PhysicsBody::setupShape(ShapeConfig& _shapeConfig) {
        cpSpaceAddShape(space, physicsShapes[keyCounter].shape);
        cpShapeSetUserData(physicsShapes[keyCounter].shape, &physicsShapes[keyCounter]);
        cpShapeSetSensor(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.sensor);
        cpShapeSetElasticity(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.restitution);
        cpShapeSetFriction(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.friction);

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.toCollideWith, _shapeConfig.shapeMaskingConfig.mask);
            cpShapeSetFilter(physicsShapes[keyCounter].shape, _filter);
            cpShapeSetCollisionType(physicsShapes[keyCounter].shape, _shapeConfig.shapeMaskingConfig.mask);
        }
    }

    void PhysicsBody::calculateDataForBox(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType) {
        auto _size = Vec2F { _shapeConfig.size.x * transform->getModelMatrixScale().x, _shapeConfig.size.y * transform->getModelMatrixScale().y };

        _shapeConfig.vertices.emplace_back( -_size.x / 2.f, -_size.y / 2.f);
        _shapeConfig.vertices.emplace_back(  _size.x / 2.f, -_size.y / 2.f);
        _shapeConfig.vertices.emplace_back(  _size.x / 2.f,  _size.y / 2.f);
        _shapeConfig.vertices.emplace_back( -_size.x / 2.f,  _size.y / 2.f);

        if(body == nullptr) {
            auto _moment = (float)cpMomentForBox(_bodyMass, _size.x, _size.y);
            createBody(_shapeConfig, _moment, _bodyMass, _bodyType);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpBoxShapeNew(body, _shapeConfig.size.x, _shapeConfig.size.y, 0.f), keyCounter };

        setupShape(_shapeConfig);

        keyCounter++;
    }

    void PhysicsBody::calculateDataForCircle(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType) {
        if(body == nullptr) {
            auto _moment = (float)cpMomentForCircle(_bodyMass, 0, _shapeConfig.size.x * transform->getModelMatrixScale().x, cpvzero);
            createBody(_shapeConfig, _moment, _bodyMass, _bodyType);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpCircleShapeNew(body, _shapeConfig.size.x, cpvzero), keyCounter };
        setupShape(_shapeConfig);
        keyCounter++;
    }

    void PhysicsBody::calculateDataForPolygon(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType) {
        // TODO (RDE): If physics act weird or stop working check this.
        const auto _numOfVertices = _shapeConfig.vertices.size();
        std::vector<cpVect> _vertices;
        _vertices.resize(_numOfVertices);

        for(auto _i = 0; _i < _numOfVertices; _i++) {
            auto& _vertex = _shapeConfig.vertices[_i];
            _vertices[_i] = { _vertex.x * transform->getModelMatrixScale().x, _vertex.y * transform->getModelMatrixScale().y };
        }

        if(body == nullptr) {
            auto _moment = (float)cpMomentForPoly(_bodyMass, (int)_numOfVertices, &_vertices[0], cpvzero, 0.f);
            createBody(_shapeConfig, _moment, _bodyMass, _bodyType);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpPolyShapeNew(body, (int)_numOfVertices, &_vertices[0], cpTransformIdentity, 0.f), keyCounter };
        setupShape(_shapeConfig);
        keyCounter++;
    }

    void PhysicsBody::calculateDataForSegment(ShapeConfig& _shapeConfig, float _bodyMass, PhysicsBodyType _bodyType) {
        auto _halfWidth = _shapeConfig.size.x / 2.f * transform->getModelMatrixScale().x;
        auto _a = cpVect { -_halfWidth, 0 };
        auto _b = cpVect { _halfWidth, 0 };

        if(body == nullptr) {
            auto _moment = (float)cpMomentForSegment(_bodyMass, _a, _b, 0.f);
            createBody(_shapeConfig, _moment, _bodyMass, _bodyType);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpSegmentShapeNew(body, _a, _b, 0.f), keyCounter };
        setupShape(_shapeConfig);
        keyCounter++;
    }

    PhysicsBody::~PhysicsBody() {
        for(auto& _shape : physicsShapes) {
            if(cpSpaceContainsShape(space, _shape.second.shape)) {
                cpSpaceRemoveShape(space, _shape.second.shape);
                cpShapeFree(_shape.second.shape);
            }
        }

        if(cpSpaceContainsBody(space, body)) {
            cpSpaceRemoveBody(space, body);
            cpBodyFree(body);
        }
    }

    PhysicsShapeId PhysicsBody::addShape(ShapeConfig& _shapeConfig, const Vec2F& _position, float _rotation) {
        switch (_shapeConfig.type) {
            case PhysicsShapeType::CIRCLE:
                calculateDataForCircle(_shapeConfig, getMass(), getBodyType());
                break;
            case PhysicsShapeType::POLYGON:
                calculateDataForPolygon(_shapeConfig, getMass(), getBodyType());
                break;
            case PhysicsShapeType::BOX:
                calculateDataForBox(_shapeConfig, getMass(), getBodyType());
                break;
            case PhysicsShapeType::SEGMENT:
                calculateDataForSegment(_shapeConfig, getMass(), getBodyType());
                break;
        }

        return keyCounter - 1;
    }

    bool PhysicsBody::removeShape(PhysicsShapeId _id) {
        auto _it = physicsShapes.find(_id);

        if(_it == physicsShapes.end()) {
            return false;
        }

        cpShapeFree(physicsShapes[_id].shape);
        physicsShapes.erase(_it);
        return true;
    }

    void PhysicsBody::setGroup(PhysicsShapeId _shapeID, uint _group) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.group = _group;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::removeGroup(PhysicsShapeId _shapeID) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.group = CP_NO_GROUP;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::setMask(PhysicsShapeId _shapeID, uint _mask) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.mask = _mask;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::removeMask(PhysicsShapeId _shapeID) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.mask = CP_ALL_CATEGORIES;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::setToCollideWiths(PhysicsShapeId _shapeID, uint _toCollideWiths) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.categories = _toCollideWiths;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::removeToCollideWiths(PhysicsShapeId _shapeID) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.categories = CP_ALL_CATEGORIES;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::addMaskFilter(PhysicsShapeId _shapeID, uint _masksToAdd) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.categories |= _masksToAdd;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::removeMaskFilter(PhysicsShapeId _shapeID, uint _masksToRemove) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.categories &= ~_masksToRemove;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    void PhysicsBody::resetMaskFilter(PhysicsShapeId _shapeID) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.categories = CP_ALL_CATEGORIES;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
    }

    uint PhysicsBody::getMasks(PhysicsShapeId _shapeID) {
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).mask;
    }

    uint PhysicsBody::getGroups(PhysicsShapeId _shapeID) {
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).group;
    }

    uint PhysicsBody::getToCollideWiths(PhysicsShapeId _shapeID) {
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).categories;
    }

    void PhysicsBody::update() {
        Vec2F _transformPos = transform->getModelMatrixPosition();
        float _transformRot = transform->getModelMatrixRotation();

        Vec2F _bodyPosition = { (float)cpBodyGetPosition(body).x, (float)cpBodyGetPosition(body).y };
        float _bodyRot = Util::Math::radiansToDegrees(cpBodyGetAngle(body));
        bool _dirtyPos = false;
        bool _dirtyRot = false;

        if(!Util::Math::approximatelyEqual(_bodyRot, _transformRot)) {
            cpBodySetAngle(body, Util::Math::degreesToRadians(_transformRot));
            _dirtyRot = true;
        }

        if(!Util::Math::approximatelyEqual(_transformPos, _bodyPosition)) {
            cpBodySetPosition(body, { _transformPos.x, _transformPos.y });
            _dirtyPos = true;
        }

        if(_dirtyPos || _dirtyRot) {
            cpSpaceReindexShapesForBody(space, body);
        }
    }

    void PhysicsBody::setBodyType(const PhysicsBodyType& _physicsBodyType) {
        if(getBodyType() == _physicsBodyType) {
            return;
        }

        switch (_physicsBodyType) {
            case STATIC:
                cpBodySetType(body, cpBodyType::CP_BODY_TYPE_STATIC);
                break;
            case KINEMATIC:
                cpBodySetType(body, cpBodyType::CP_BODY_TYPE_KINEMATIC);
                break;
            case DYNAMIC:
                cpBodySetType(body, cpBodyType::CP_BODY_TYPE_DYNAMIC);
                break;
        }
    }

    PhysicsBodyType PhysicsBody::getBodyType() {
        switch (cpBodyGetType(body)) {
            case cpBodyType::CP_BODY_TYPE_STATIC:
                return PhysicsBodyType::STATIC;
            case cpBodyType::CP_BODY_TYPE_KINEMATIC:
                return PhysicsBodyType::KINEMATIC;
            case cpBodyType::CP_BODY_TYPE_DYNAMIC:
                return PhysicsBodyType::DYNAMIC;
        }

        return PhysicsBodyType::STATIC;
    }

    void PhysicsBody::setMass(float _mass) {
        cpBodySetMass(body, _mass);
    }

    float PhysicsBody::getMass() {
        return (float)cpBodyGetMass(body);
    }

    void PhysicsBody::setFriction(PhysicsShapeId _shapeID, float _friction) {
        cpShapeSetFriction(physicsShapes[_shapeID].shape, _friction);
    }

    float PhysicsBody::getFriction(PhysicsShapeId _shapeID) {
        return (float)cpShapeGetFriction(physicsShapes[_shapeID].shape);
    }

    void PhysicsBody::setRestitution(PhysicsShapeId _shapeID, float _restitution) {
        cpShapeSetElasticity(physicsShapes[_shapeID].shape, _restitution);
    }

    float PhysicsBody::getRestitution(PhysicsShapeId _shapeID) {
        return (float)cpShapeGetElasticity(physicsShapes[_shapeID].shape);
    }

    void PhysicsBody::setLinearVelocity(const Vec2F& _linearVelocity) {
        cpBodySetVelocity(body, { _linearVelocity.x, _linearVelocity.y });
    }

    Vec2F PhysicsBody::getLinearVelocity() {
        auto _linearVelocity = cpBodyGetVelocity(body);
        return { (float)_linearVelocity.x, (float)_linearVelocity.y };
    }

    void PhysicsBody::setAngularLinearVelocity(float _linearAngularVelocity) {
        cpBodySetAngularVelocity(body, _linearAngularVelocity);
    }

    float PhysicsBody::getAngularLinearVelocity() {
        return (float)cpBodyGetAngularVelocity(body);
    }

    void PhysicsBody::setCenterOfGravity(const Vec2F& _centerOfGravity) {
        cpBodySetCenterOfGravity(body, { _centerOfGravity.x, _centerOfGravity.y });
    }

    Vec2F PhysicsBody::getCenterOfGravity() {
        auto _centerOfGravity = cpBodyGetCenterOfGravity(body);
        return { (float)_centerOfGravity.x, (float)_centerOfGravity.y };
    }

    void PhysicsBody::setGhost(PhysicsShapeId _shapeID, bool _ghost) {
        cpShapeSetSensor(physicsShapes[_shapeID].shape, _ghost);
    }

    bool PhysicsBody::isGhost(PhysicsShapeId _shapeID) {
        return cpShapeGetSensor(physicsShapes[_shapeID].shape);
    }

    void PhysicsBody::applyImpulseLocal(const Vec2F& _impulse, const Vec2F& _where) {
        cpBodyApplyImpulseAtLocalPoint(body, { _impulse.x, _impulse.y }, { _where.x, _where.y });
    }

    void PhysicsBody::applyImpulseWorld(const Vec2F& _impulse, const Vec2F& _where) {
        cpBodyApplyImpulseAtWorldPoint(body, { _impulse.x, _impulse.y }, { _where.x, _where.y });
    }

    void PhysicsBody::applyForceLocal(const Vec2F& _force, const Vec2F& _where) {
        cpBodyApplyForceAtLocalPoint(body, { _force.x, _force.y }, { _where.x, _where.y });
    }

    void PhysicsBody::applyForceWorld(const Vec2F& _force, const Vec2F& _where) {
        cpBodyApplyForceAtWorldPoint(body, { _force.x, _force.y }, { _where.x, _where.y });
    }

    void PhysicsBody::setEnabled(bool _enabled) {
        if(_enabled && transform->node->hasComponent<DisabledForFixedUpdate>()) {
            transform->node->removeComponent<DisabledForFixedUpdate>();
            return;
        }

        if(!_enabled && !transform->node->hasComponent<DisabledForFixedUpdate>()) {
            transform->node->addComponent<DisabledForFixedUpdate>();
        }
    }

    bool PhysicsBody::isEnabled() {
        return !transform->node->hasComponent<DisabledForFixedUpdate>();
    }
}