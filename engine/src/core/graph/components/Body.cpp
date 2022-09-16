//
// Created by borja on 9/5/22.
//

#include "core/graph/components/Body.h"
#include "core/graph/components/Transform.h"
#include "core/Engine.h"

namespace GDE {

//    Body::Body(const NodeID& _nodeId, Scene* _scene, const BodyConfig& _config, Transform* _transform) {
//        bodyConfig = _config;
//
//        // This needs to be WorldPosition, because we need to situate the body in world space
//        // The update is because before the first Graph::onUpdate, modelMatrix is the identity matrix, we need
//        // to advance one update step, which doesn't affect at all the simulation.
//        _transform->update();
//        b2dConfig.bodyDefinition.position.Set(_transform->getPositionWorld().x, _transform->getPositionWorld().y);
//        b2dConfig.bodyDefinition.angle = _transform->getRotationLocal();
//        b2dConfig.bodyDefinition.type = gdeBodyTypeToB2dBodyType(_config.bodyType);
//        b2dConfig.body = _scene->engine->manager.physics.createBody(b2dConfig.bodyDefinition);
//
//        switch (_config.bodyShapeType) {
//            case BOX: b2dConfig.polygonShape.SetAsBox(_config.size.x / 2.f, _config.size.y / 2.f); break;
//            case CIRCLE: b2dConfig.circleShape.m_p = {_transform->getPositionLocal().x, _transform->getPositionLocal().y};
//                b2dConfig.circleShape.m_radius = _config.size.x;
//                break;
//            case POLYGON: LOG_W("Polygon shape not yet supported") break;
//        }
//
//        b2dConfig.fixtureDef.friction = _config.friction;
//        b2dConfig.fixtureDef.density = _config.mass;
//        b2dConfig.fixtureDef.restitution = _config.restitution;
//        b2dConfig.fixtureDef.shape = &b2dConfig.polygonShape;
//
//        b2dConfig.fixtureDef.filter.categoryBits = _config.mask;
//
//        b2dConfig.fixture = b2dConfig.body->CreateFixture(&b2dConfig.fixtureDef);
//        b2dConfig.lastPosition = _transform->getPositionLocal();
//        b2dConfig.lastRotation = _transform->getRotationLocal();
//        b2dConfig.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
//    }
//
//    Body::~Body() {  }
//
//    Vec2F Body::getPosition() const {
//        auto _pos = b2dConfig.body->GetTransform().p;
//        return {_pos.x, _pos.y};
//    }
//
//    float Body::getRotation() const {
//        return b2dConfig.body->GetTransform().q.GetAngle() * 180.f / (float)M_PI;
//    }
//
//    b2BodyType Body::gdeBodyTypeToB2dBodyType(const BodyType& _bodyType) {
//        switch (_bodyType) {
//            case DYNAMIC: return b2BodyType::b2_dynamicBody;
//            case STATIC: return b2BodyType::b2_staticBody;
//            case KINEMATIC: return b2BodyType::b2_kinematicBody;
//        }
//
//        return b2BodyType::b2_dynamicBody;
//    }
//
//    void Body::setApplyGravity(bool applyGravity) const {
//        b2dConfig.body->SetGravityScale(applyGravity ? b2dConfig.body->GetGravityScale() : 0);
//    }
//
//    bool Body::isApplyingGravity() const {
//        return b2dConfig.body->GetGravityScale() != 0;
//    }
//
//    void Body::setGravityMultiplier(float _gravityMultiplier) const {
//        b2dConfig.body->SetGravityScale(_gravityMultiplier);
//    }
//
//    float Body::getGravityMultiplayer() const {
//        return b2dConfig.body->GetGravityScale();
//    }
//
//    void Body::setSensor(bool _sensor) const {
//        b2dConfig.fixture->SetSensor(_sensor);
//    }
//
//    bool Body::isSensor() const {
//        return b2dConfig.fixture->IsSensor();
//    }
//
//    void Body::setSelfCollisionMask(CollisionMask _mask) const {
//        auto _filter = reinterpret_cast<b2Filter*>(b2dConfig.body->GetUserData().pointer);
//        _filter->categoryBits = _mask;
//    }

}