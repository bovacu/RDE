//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/render/RenderManager.h"

namespace GDE {

    void PhysicsManager::init() {
        space = cpSpaceNew();
        cpSpaceSetGravity(space, { 0, -100 });
    }

    void PhysicsManager::destroy() {
        LOG_DEBUG("Cleaning up PhysicsManager")
        cpSpaceFree(space);
    }

    void PhysicsManager::step(Delta _fxDt) {
        if (!simulate) return;

        cpSpaceStep(space, _fxDt);

        for (auto* _body: bodies) {
            auto _bodyPos = cpBodyGetPosition(_body->body);
            auto _bodyAngle = radiansToDegrees(cpBodyGetAngle(_body->body));
            _body->transform->setPosition((float) _bodyPos.x, (float) _bodyPos.y);
            _body->transform->setRotation((float) _bodyAngle);
        }
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

    void PhysicsManager::drawPolygon(PhysicsBody* _physicsBody, RenderManager* _renderManager, const Color& _lineColor, const Color& _radiusColor, bool _showLines, bool _showRadius) {
        if(!_showLines) return;

        auto _polygon = _physicsBody->bodyConfig.shapeConfig;
        Mat2 _rotMatrix(1, _physicsBody->transform->getModelMatrixPosition().x, 0, _physicsBody->transform->getModelMatrixPosition().y);
        _rotMatrix.rotate(_physicsBody->transform->getModelMatrixRotation());

        for (auto _i = 0; _i < _polygon.vertices.size(); _i++) {

            int _next = _i + 1;
            if(_next == _polygon.vertices.size())
                _next = 0;

            Vec2F _p0 = _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _polygon.vertices[_i];
            Vec2F _p1 = _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _polygon.vertices[_next];

            _renderManager->drawLine(_p0, _p1, _lineColor);
        }

        if(!_showRadius || _polygon.vertices.empty()) return;

        _renderManager->drawLine(_physicsBody->transform->getModelMatrixPosition(),
                                 _physicsBody->transform->getModelMatrixPosition() + _rotMatrix * _polygon.vertices[0],
                                 _radiusColor);
    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        if(!debugOptions.showGeneralDebug) return;

        for(auto* _body : bodies) {

            switch (_body->bodyConfig.shapeConfig.type) {
                case PhysicsShapeType::CIRCLE: {
                    if(!debugOptions.showCircleLines) return;

                    const int _segments = 20;
                    float _theta = _body->transform->getRotation();
                    float _inc = PI * 2.0f / (float)_segments;
                    Vec2F _points[_segments];

                    for(auto& _point : _points) {
                        _theta += _inc;
                        Vec2F _p(std::cos(_theta), std::sin(_theta) );
                        _p *= _body->bodyConfig.shapeConfig.size.x;
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
                    drawPolygon(_body, _renderManager, debugOptions.boxLineColor, debugOptions.boxRadiusColor, debugOptions.showBoxLines, debugOptions.showBoxRadius);
                    break;
                }
                case PhysicsShapeType::POLYGON: {
                    drawPolygon(_body, _renderManager, debugOptions.polygonLineColor, debugOptions.polygonRadiusColor, debugOptions.showPolygonLines, debugOptions.showPolygonRadius);
                    break;
                }
                case PhysicsShapeType::SEGMENT: {
                    _renderManager->drawLine({_body->transform->getModelMatrixPosition().x - _body->bodyConfig.shapeConfig.size.x / 2.f, _body->transform->getModelMatrixPosition().y},
                                             {_body->transform->getModelMatrixPosition().x + _body->bodyConfig.shapeConfig.size.x / 2.f, _body->transform->getModelMatrixPosition().y},
                                             Color::Blue);
                    break;
                }
                default:
                    break;
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

    cpBool PhysicsManager::onCollisionExit(cpArbiter* _arbiter, cpSpace* _space, void* _data) {
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
            return cpTrue;
        }

        if(PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionExit != nullptr) {
            PhysicsManager::collisionCallbacksTable[_shapeBMask][_shapeAMask].onCollisionExit(_physicsBodyA, _physicsBodyB);
            return cpTrue;
        }

        return cpTrue;
    }

    PhysicsCollisionCallbacks& PhysicsManager::getCollisionCallbacks(uint _maskA, uint _maskB) {
        return PhysicsManager::collisionCallbacksTable[_maskA][_maskB];
    }

    void PhysicsManager::addCollisionCallbacks(uint _maskA, uint _maskB, PhysicsCollisionCallbacks _callbacks) {
        PhysicsManager::collisionCallbacksTable[_maskA][_maskB] = _callbacks;
        auto* _collisionHandler = cpSpaceAddCollisionHandler(space, _maskA, _maskB);
        _collisionHandler->beginFunc = (cpCollisionBeginFunc)onCollisionEnter;
        _collisionHandler->separateFunc = (cpCollisionSeparateFunc)onCollisionExit;
    }

}