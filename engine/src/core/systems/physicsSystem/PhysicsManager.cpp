//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/render/RenderManager.h"
#include "core/util/Mat2.h"

namespace RDE {

    void PhysicsManager::init() {
        space = cpSpaceNew();
        cpSpaceSetGravity(space, { 0, -100 });
        cpSpaceSetCollisionSlop(space, 0.5f);
    }

    void PhysicsManager::destroy() {
        Util::Log::debug("Cleaning up PhysicsManager");
        for(auto* _body : bodies) {
            remove(_body);
        }
        cpSpaceFree(space);
    }

    void PhysicsManager::update(Delta _dt) {
        for (auto* _body: bodies) {
            auto _cpPos = cpBodyGetPosition(_body->body);
            auto _bodyPos = Vec2F { (float)_cpPos.x, (float)_cpPos.y };
            auto _bodyAngle = Util::Math::radiansToDegrees(cpBodyGetAngle(_body->body));
            auto _transformPos = _body->transform->getModelMatrixPosition();
            auto _transformRot = _body->transform->getModelMatrixRotation();

            if(!Util::Math::approximatelyEqual(_transformPos, _bodyPos)) {
                _body->transform->setMatrixModelPosition({(float)_bodyPos.x, (float)_bodyPos.y});
            }

            if(!Util::Math::approximatelyEqual(_transformRot, (float)_bodyAngle)) {
                _body->transform->setMatrixModelRotation((float)_bodyAngle);
            }
        }
    }

    void PhysicsManager::step(Delta _fxDt) {
        if (!simulate) return;

        cpSpaceStep(space, _fxDt);
    }

    void PhysicsManager::setGravity(const Vec2F& _gravity) {
        cpSpaceSetGravity(space, { _gravity.x, _gravity.y });
    }

    Vec2F PhysicsManager::getGravity() {
        auto _gravity = cpSpaceGetGravity(space);
        return { (float)_gravity.x, (float)_gravity.y };
    }

    int PhysicsManager::getStepIterations() {
        return cpSpaceGetIterations(space);
    }

    void PhysicsManager::setStepIterations(int _iterations) {
        cpSpaceSetIterations(space, _iterations);
    }

    void PhysicsManager::drawPolygon(PhysicsBody* _physicsBody, const ShapeConfig& _shapeConfig, RenderManager* _renderManager, const Color& _lineColor, const Color& _radiusColor, bool _showLines, bool _showRadius) {
        if(!_showLines) return;

        Mat2 _rotMatrix(1, _physicsBody->transform->getModelMatrixPosition().x, 0, _physicsBody->transform->getModelMatrixPosition().y);
        _rotMatrix.rotate(Util::Math::radiansToDegrees(cpBodyGetAngle(_physicsBody->body)));

        for (auto _i = 0; _i < _shapeConfig.vertices.size(); _i++) {

            int _next = _i + 1;
            if(_next == _shapeConfig.vertices.size())
                _next = 0;

            Vec2F _p0 = _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _shapeConfig.vertices[_i];
            Vec2F _p1 = _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _shapeConfig.vertices[_next];

            _renderManager->drawLine(_p0, _p1, _lineColor);
        }

        if(!_showRadius || _shapeConfig.vertices.empty()) return;

        _renderManager->drawLine(_physicsBody->transform->getModelMatrixPosition(),
                                 _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _shapeConfig.vertices[0],
                                 _radiusColor);
    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        if(!debugOptions.showGeneralDebug) return;

        for(auto* _body : bodies) {

            if(!_body->isEnabled()) continue;

            for(auto& _physicsShape : _body->physicsShapes) {

                switch (_physicsShape.second.shapeConfig.type) {
                    case PhysicsShapeType::CIRCLE: {
                        if(!debugOptions.showCircleLines) return;

                        const int _segments = 20;
                        float _theta = Util::Math::degreesToRadians(_body->transform->getRotation());
                        float _inc = PI * 2.0f / (float)_segments;
                        Vec2F _points[_segments];

                        for(auto& _point : _points) {
                            _theta += _inc;
                            Vec2F _p(std::cos(_theta), std::sin(_theta) );
                            _p *= _physicsShape.second.shapeConfig.size.x;
                            _p += _body->transform->getModelMatrixPosition();
                            _point = _p;
                        }

                        for (auto _i = 0; _i < _segments; _i++) {
                            int _next = _i + 1;
                            if(_next == _segments)
                                _next = 0;

                            Vec2F _p0 = _points[_i];
                            Vec2F _p1 =_points[_next];

                            _renderManager->drawLine(_p0, _p1, debugOptions.circleLineColor);
                        }

                        if(!debugOptions.showCircleRadius) return;
                        _renderManager->drawLine(_body->transform->getModelMatrixPosition(), _points[0], debugOptions.circleRadiusColor);

                        break;
                    }

                    case PhysicsShapeType::BOX: {
                        drawPolygon(_body, _physicsShape.second.shapeConfig, _renderManager, debugOptions.boxLineColor, debugOptions.boxRadiusColor, debugOptions.showBoxLines, debugOptions.showBoxRadius);
                        break;
                    }
                    case PhysicsShapeType::POLYGON: {
                        drawPolygon(_body, _physicsShape.second.shapeConfig, _renderManager, debugOptions.polygonLineColor, debugOptions.polygonRadiusColor, debugOptions.showPolygonLines, debugOptions.showPolygonRadius);
                        break;
                    }
                    case PhysicsShapeType::SEGMENT: {
                        _renderManager->drawLine({_body->transform->getModelMatrixPosition().x - _physicsShape.second.shapeConfig.size.x / 2.f, _body->transform->getModelMatrixPosition().y},
                                                 {_body->transform->getModelMatrixPosition().x + _physicsShape.second.shapeConfig.size.x / 2.f, _body->transform->getModelMatrixPosition().y},
                                                 Color::Blue);
                        break;
                    }
                    default:
                        break;
                }

            }
        }
    }

    void PhysicsManager::add(PhysicsBody* _physicsBody) {
        bodies.push_back(_physicsBody);
    }

    void PhysicsManager::remove(PhysicsBody* _physicsBody) {
        auto _it = std::find(bodies.begin(), bodies.end(), _physicsBody);
        if (_it != bodies.end()) {
            bodies.erase(_it);
        }
    }

    cpBool PhysicsManager::onCollisionEnter(cpArbiter* _arbiter, cpSpace* _space, void* _data) {
        cpShape* _shapeA;
        cpShape* _shapeB;
        cpArbiterGetShapes(_arbiter, &_shapeA, &_shapeB);
        auto* _physicsShapeA = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeA));
        auto* _physicsShapeB = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeB));

        cpBody* _bodyA;
        cpBody* _bodyB;
        cpArbiterGetBodies(_arbiter, &_bodyA, &_bodyB);

        auto* _physicsBodyA = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyA));
        auto* _physicsBodyB = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyB));

        auto _shapeAMask = _physicsBodyA->getMasks(_physicsShapeA->id);
        auto _shapeBMask = _physicsBodyB->getMasks(_physicsShapeB->id);

        if(PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionEnter != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionEnter(_physicsBodyA, _physicsBodyB);
            return cpTrue;
        }

        if(PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionEnter != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionEnter(_physicsBodyA, _physicsBodyB);
            return cpTrue;
        }

        return cpTrue;
    }

    void PhysicsManager::onCollisionStay(cpArbiter* _arbiter, cpSpace* _space, void* _data) {
        cpShape* _shapeA;
        cpShape* _shapeB;
        cpArbiterGetShapes(_arbiter, &_shapeA, &_shapeB);
        auto* _physicsShapeA = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeA));
        auto* _physicsShapeB = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeB));

        cpBody* _bodyA;
        cpBody* _bodyB;
        cpArbiterGetBodies(_arbiter, &_bodyA, &_bodyB);

        auto* _physicsBodyA = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyA));
        auto* _physicsBodyB = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyB));

        auto _shapeAMask = _physicsBodyA->getMasks(_physicsShapeA->id);
        auto _shapeBMask = _physicsBodyB->getMasks(_physicsShapeB->id);

        if(PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionStay != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionStay(_physicsBodyA, _physicsBodyB);
        }

        if(PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionStay != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionStay(_physicsBodyA, _physicsBodyB);
        }
    }

    void PhysicsManager::onCollisionExit(cpArbiter* _arbiter, cpSpace* _space, void* _data) {
        cpShape* _shapeA;
        cpShape* _shapeB;
        cpArbiterGetShapes(_arbiter, &_shapeA, &_shapeB);
        auto* _physicsShapeA = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeA));
        auto* _physicsShapeB = static_cast<PhysicsShape*>(cpShapeGetUserData(_shapeB));

        cpBody* _bodyA;
        cpBody* _bodyB;
        cpArbiterGetBodies(_arbiter, &_bodyA, &_bodyB);

        auto* _physicsBodyA = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyA));
        auto* _physicsBodyB = static_cast<PhysicsBody*>(cpBodyGetUserData(_bodyB));

        auto _shapeAMask = _physicsBodyA->getMasks(_physicsShapeA->id);
        auto _shapeBMask = _physicsBodyB->getMasks(_physicsShapeB->id);

        if(PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionExit != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeAMask][_shapeBMask].onCollisionExit(_physicsBodyA, _physicsBodyB);
        }

        if(PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionExit != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionExit(_physicsBodyA, _physicsBodyB);
        }
    }

    PhysicsCollisionCallbacks& PhysicsManager::getCollisionCallbacks(uint _maskA, uint _maskB) {
        return PhysicsManager::collisionCallbacksTable[_maskA][_maskB];
    }

    void PhysicsManager::addCollisionCallbacks(uint _maskA, uint _maskB, PhysicsCollisionCallbacks _callbacks) {
        PhysicsManager::collisionCallbacksTable[_maskA][_maskB] = _callbacks;
        auto* _collisionHandler = cpSpaceAddCollisionHandler(space, _maskA, _maskB);
        _collisionHandler->beginFunc = onCollisionEnter;
        _collisionHandler->separateFunc = onCollisionExit;
        _collisionHandler->postSolveFunc = onCollisionStay;
    }

}