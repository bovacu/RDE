//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"

namespace GDE {

    PhysicsBody::PhysicsBody(const NodeID& _id, Scene* _scene, const BodyConfig& _bodyConfig) {
        space = _scene->engine->manager.physics.space;
        transform = _scene->getMainGraph()->getComponent<Transform>(_id);
        bodyConfig = _bodyConfig;

        switch (_bodyConfig.shapeConfig.type) {
            case PhysicsShapeType::CIRCLE:
                calculateDataForCircle(_bodyConfig.shapeConfig);
                break;
            case PhysicsShapeType::POLYGON:
                calculateDataForPolygon(_bodyConfig.shapeConfig);
                break;
            case PhysicsShapeType::BOX:
                calculateDataForBox(_bodyConfig.shapeConfig);
                break;
            case PhysicsShapeType::SEGMENT:
                calculateDataForSegment(_bodyConfig.shapeConfig);
                break;
        }

        cpBodySetUserData(body, this);
        _scene->engine->manager.physics.add(this);
    }

    void PhysicsBody::calculateDataForBox(const ShapeConfig& _shapeConfig) {
        auto _size = Vec2F { _shapeConfig.size.x * transform->getModelMatrixScale().x, _shapeConfig.size.y * transform->getModelMatrixScale().y };

        bodyConfig.shapeConfig.vertices.emplace_back( -_size.x / 2.f, -_size.y / 2.f);
        bodyConfig.shapeConfig.vertices.emplace_back(  _size.x / 2.f, -_size.y / 2.f);
        bodyConfig.shapeConfig.vertices.emplace_back(  _size.x / 2.f,  _size.y / 2.f);
        bodyConfig.shapeConfig.vertices.emplace_back( -_size.x / 2.f,  _size.y / 2.f);

        if(body == nullptr) {
            auto _moment = cpMomentForBox(bodyConfig.mass, _size.x, _size.y);
            switch (bodyConfig.physicsBodyType) {
                case STATIC:
                    body = cpBodyNewStatic();
                    break;
                case KINEMATIC:
                    body = cpBodyNewKinematic();
                    break;
                case DYNAMIC:
                    body = cpBodyNew(bodyConfig.mass, _moment);
                    break;
            }

            cpBodySetPosition(body, { transform->getModelMatrixPosition().x, transform->getModelMatrixPosition().y });
            auto _rotation = transform->getModelMatrixRotation();
            auto _radians = degreesToRadians(_rotation);
            cpBodySetAngle(body, _radians);
            cpSpaceReindexShapesForBody(space, body);
            cpSpaceAddBody(space, body);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpBoxShapeNew(body, _shapeConfig.size.x, _shapeConfig.size.y, 0.f), keyCounter };
        cpSpaceAddShape(space, physicsShapes[keyCounter].shape);
        cpShapeSetUserData(physicsShapes[keyCounter].shape, &physicsShapes[keyCounter]);
        cpShapeSetSensor(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.sensor);

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.toCollideWith, _shapeConfig.shapeMaskingConfig.mask);
            cpShapeSetFilter(physicsShapes[keyCounter].shape, _filter);
            if(_shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES) {
                cpShapeSetCollisionType(physicsShapes[keyCounter].shape, _shapeConfig.shapeMaskingConfig.mask);
            }
        }

        keyCounter++;
    }

    void PhysicsBody::calculateDataForCircle(const ShapeConfig& _shapeConfig) {
        if(body == nullptr) {
            auto _moment = cpMomentForCircle(bodyConfig.mass, 0, _shapeConfig.size.x / 2.f * transform->getModelMatrixScale().x, cpvzero);
            switch (bodyConfig.physicsBodyType) {
                case STATIC:
                    body = cpBodyNewStatic();
                    break;
                case KINEMATIC:
                    body = cpBodyNewKinematic();
                    break;
                case DYNAMIC:
                    body = cpBodyNew(bodyConfig.mass, _moment);
                    break;
            }

            cpBodySetPosition(body, { transform->getModelMatrixPosition().x, transform->getModelMatrixPosition().y });
            cpBodySetAngle(body, degreesToRadians(transform->getModelMatrixRotation()));
            cpSpaceAddBody(space, body);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpCircleShapeNew(body, bodyConfig.shapeConfig.size.x / 2.f, cpvzero), keyCounter };
        cpSpaceAddShape(space, physicsShapes[keyCounter].shape);
        cpShapeSetUserData(physicsShapes[keyCounter].shape, &physicsShapes[keyCounter]);
        cpShapeSetSensor(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.sensor);

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.toCollideWith, _shapeConfig.shapeMaskingConfig.mask);
            cpShapeSetFilter(physicsShapes[keyCounter].shape, _filter);
            if(_shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES) {
                cpShapeSetCollisionType(physicsShapes[keyCounter].shape, _shapeConfig.shapeMaskingConfig.mask);
            }
        }

        keyCounter++;
    }

    void PhysicsBody::calculateDataForPolygon(const ShapeConfig& _shapeConfig) {
        const auto _numOfVertices = bodyConfig.shapeConfig.vertices.size();
        cpVect _vertices[_numOfVertices];

        for(auto _i = 0; _i < _numOfVertices; _i++) {
            auto& _vertex = bodyConfig.shapeConfig.vertices[_i];
            _vertices[_i] = { _vertex.x * transform->getModelMatrixScale().x, _vertex.y * transform->getModelMatrixScale().y };
        }

        if(body == nullptr) {
            auto _moment = cpMomentForPoly(bodyConfig.mass, (int)_numOfVertices, _vertices, cpvzero, 0.f);
            switch (bodyConfig.physicsBodyType) {
                case STATIC:
                    body = cpBodyNewStatic();
                    break;
                case KINEMATIC:
                    body = cpBodyNewKinematic();
                    break;
                case DYNAMIC:
                    body = cpBodyNew(bodyConfig.mass, _moment);
                    break;
            }

            cpBodySetPosition(body, { transform->getModelMatrixPosition().x, transform->getModelMatrixPosition().y });
            cpBodySetAngle(body, degreesToRadians(transform->getModelMatrixRotation()));
            cpSpaceAddBody(space, body);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpPolyShapeNew(body, (int)_numOfVertices, _vertices, cpTransformIdentity, 0.f), keyCounter };
        cpSpaceAddShape(space, physicsShapes[keyCounter].shape);
        cpShapeSetUserData(physicsShapes[keyCounter].shape, &physicsShapes[keyCounter]);
        cpShapeSetSensor(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.sensor);

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.toCollideWith, _shapeConfig.shapeMaskingConfig.mask);
            cpShapeSetFilter(physicsShapes[keyCounter].shape, _filter);
            if(_shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES) {
                cpShapeSetCollisionType(physicsShapes[keyCounter].shape, _shapeConfig.shapeMaskingConfig.mask);
            }
        }

        keyCounter++;
    }

    void PhysicsBody::calculateDataForSegment(const ShapeConfig& _shapeConfig) {
        auto _halfWidth = _shapeConfig.size.x / 2.f * transform->getModelMatrixScale().x;
        auto _a = cpVect { -_halfWidth, 0 };
        auto _b = cpVect { _halfWidth, 0 };

        if(body == nullptr) {
            auto _moment = cpMomentForSegment(bodyConfig.mass, _a, _b, 0.f);
            switch (bodyConfig.physicsBodyType) {
                case STATIC:
                    body = cpBodyNewStatic();
                    break;
                case KINEMATIC:
                    body = cpBodyNewKinematic();
                    break;
                case DYNAMIC:
                    body = cpBodyNew(bodyConfig.mass, _moment);
                    break;
            }

            cpBodySetPosition(body, { transform->getModelMatrixPosition().x, transform->getModelMatrixPosition().y });
            cpBodySetAngle(body, degreesToRadians(transform->getModelMatrixRotation()));
            cpSpaceAddBody(space, body);
        }

        physicsShapes[keyCounter] = { _shapeConfig, cpSegmentShapeNew(body, _a, _b, 0.f), keyCounter };
        cpSpaceAddShape(space, physicsShapes[keyCounter].shape);
        cpShapeSetUserData(physicsShapes[keyCounter].shape, &physicsShapes[keyCounter]);
        cpShapeSetSensor(physicsShapes[keyCounter].shape, physicsShapes[keyCounter].shapeConfig.sensor);

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.toCollideWith, _shapeConfig.shapeMaskingConfig.mask);
            cpShapeSetFilter(physicsShapes[keyCounter].shape, _filter);
            if(_shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES) {
                cpShapeSetCollisionType(physicsShapes[keyCounter].shape, _shapeConfig.shapeMaskingConfig.mask);
            }
        }

        keyCounter++;
    }

    PhysicsBody::~PhysicsBody() {
        for(auto& _shape : physicsShapes) {
            cpShapeFree(_shape.second.shape);
        }
        cpBodyFree(body);
    }

    PhysicsShapeId PhysicsBody::addShape(const ShapeConfig& _shapeConfig, const Vec2F& _position, float _rotation) {
        switch (_shapeConfig.type) {
            case PhysicsShapeType::CIRCLE:
                calculateDataForCircle(_shapeConfig);
                break;
            case PhysicsShapeType::POLYGON:
                calculateDataForPolygon(_shapeConfig);
                break;
            case PhysicsShapeType::BOX:
                calculateDataForBox(_shapeConfig);
                break;
            case PhysicsShapeType::SEGMENT:
                calculateDataForSegment(_shapeConfig);
                break;
        }

        if(_shapeConfig.shapeMaskingConfig.group != CP_NO_GROUP || _shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES || _shapeConfig.shapeMaskingConfig.toCollideWith != CP_ALL_CATEGORIES) {
            auto _filter = cpShapeFilterNew(_shapeConfig.shapeMaskingConfig.group, _shapeConfig.shapeMaskingConfig.mask, _shapeConfig.shapeMaskingConfig.toCollideWith);
            cpShapeSetFilter(physicsShapes[keyCounter - 1].shape, _filter);
            if(_shapeConfig.shapeMaskingConfig.mask != CP_ALL_CATEGORIES) {
                cpShapeSetCollisionType(physicsShapes[keyCounter - 1].shape, _shapeConfig.shapeMaskingConfig.mask);
            }
        }

        return keyCounter;
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
        cpShapeSetCollisionType(physicsShapes[_shapeID].shape, _mask);
    }

    void PhysicsBody::removeMask(PhysicsShapeId _shapeID) {
        auto _currentFilter = cpShapeGetFilter(physicsShapes[_shapeID].shape);
        _currentFilter.mask = CP_ALL_CATEGORIES;
        cpShapeSetFilter(physicsShapes[_shapeID].shape, _currentFilter);
        cpShapeSetCollisionType(physicsShapes[_shapeID].shape, 0);
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
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).categories;
    }

    uint PhysicsBody::getGroups(PhysicsShapeId _shapeID) {
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).group;
    }

    uint PhysicsBody::getToCollideWiths(PhysicsShapeId _shapeID) {
        return cpShapeGetFilter(physicsShapes[_shapeID].shape).mask;
    }

    void PhysicsBody::update() {
        Vec2F _transformPos = transform->getModelMatrixPosition();
        float _transformRot = transform->getModelMatrixRotation();

        Vec2F _bodyPosition = { (float)cpBodyGetPosition(body).x, (float)cpBodyGetPosition(body).y };
        float _bodyRot = radiansToDegrees(cpBodyGetAngle(body));
        bool _dirtyPos = false;
        bool _dirtyRot = false;

        if(_transformPos.x != _bodyPosition.x || _transformPos.y != _bodyPosition.y) {
            cpBodySetPosition(body, { _transformPos.x, _transformPos.y });
            _dirtyPos = true;
        }

        if(_transformRot != _bodyRot) {
            cpBodySetAngle(body, degreesToRadians(_transformRot));
            _dirtyRot = true;
        }

        if(_dirtyPos || _dirtyRot) {
            if(_dirtyPos) {
                cpBodySetVelocity(body, cpvzero);
            }
            cpSpaceReindexShapesForBody(space, body);
        }
    }


}