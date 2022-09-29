//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/render/RenderManager.h"

namespace GDE {

    void PhysicsManager::init() {
        collisionHandler[PhysicsShape::CIRCLE][PhysicsShape::CIRCLE].bind<&PhysicsManager::collisionCircleCircle>(this);
        collisionHandler[PhysicsShape::CIRCLE][PhysicsShape::POLYGON].bind<&PhysicsManager::collisionCirclePolygon>(this);
        collisionHandler[PhysicsShape::CIRCLE][PhysicsShape::BOX].bind<&PhysicsManager::collisionCirclePolygon>(this);
        collisionHandler[PhysicsShape::POLYGON][PhysicsShape::CIRCLE].bind<&PhysicsManager::collisionPolygonCircle>(this);
        collisionHandler[PhysicsShape::POLYGON][PhysicsShape::POLYGON].bind<&PhysicsManager::collisionPolygonPolygon>(this);
        collisionHandler[PhysicsShape::POLYGON][PhysicsShape::BOX].bind<&PhysicsManager::collisionPolygonPolygon>(this);
        collisionHandler[PhysicsShape::BOX][PhysicsShape::BOX].bind<&PhysicsManager::collisionPolygonPolygon>(this);
        collisionHandler[PhysicsShape::BOX][PhysicsShape::POLYGON].bind<&PhysicsManager::collisionPolygonPolygon>(this);
        collisionHandler[PhysicsShape::BOX][PhysicsShape::CIRCLE].bind<&PhysicsManager::collisionPolygonCircle>(this);

        addOrGetCollisionToTable(0, 0);
    }

    void PhysicsManager::destroy() {
        LOG_DEBUG("Cleaning up PhysicsManager")
//        for(auto _i = 0; _i < MAX_MASKS * 2; _i++) {
//            for(auto _j = 0; _j < MAX_MASKS * 2; _j++) {
//                delete collisionTable[_i][_j];
//            }
//        }
    }

    void PhysicsManager::step(Delta _fxDt) {
        if(!simulate) return;

        contacts.clear();
        for(auto _i = 0; _i < bodies.size(); _i++) {
            PhysicsBody* _a = bodies[_i];

            for(auto _j = _i + 1; _j < bodies.size(); _j++) {
                PhysicsBody* _b = bodies[_j];

                if(_a->inverseMass == 0 && _b->inverseMass == 0 || !hasCollisionInTable(_a->collisionMask, _b->collisionMask))
                    continue;

                PhysicsManifold _m(_a, _b );

                auto _it = std::find_if(collisionStates.begin(), collisionStates.end(), [_a, _b](CollisionState& _cs) {
                    return _cs.A == _a && _cs.B == _b;
                });

                if(collisionHandler[_a->shape.type][_b->shape.type](_m, _a, _b)) {

                    if(_it == collisionStates.end()) {
                        collisionStates.emplace_back(CollisionState { _a, _b });
                        _it = collisionStates.end() - 1;
                    }

                    auto* _physicsCollisionCallback = collisionTable[_a->collisionMask][_b->collisionMask];
                    if(_physicsCollisionCallback != nullptr) {
                        if(_it->state == CollisionState::NONE && _physicsCollisionCallback->onCollisionEnter != nullptr) {
                            _physicsCollisionCallback->onCollisionEnter(_a, _b);
                            _it->state = CollisionState::COLLISION_ENTER;
                        } else if((_it->state == CollisionState::COLLISION_ENTER || CollisionState::COLLISION_STAY)  && _physicsCollisionCallback->onCollisionStay != nullptr) {
                            _physicsCollisionCallback->onCollisionStay(_a, _b);
                            _it->state = CollisionState::COLLISION_STAY;
                        }
                    }

                } else {
                    auto* _physicsCollisionCallback = collisionTable[_a->collisionMask][_b->collisionMask];
                    if(_it != collisionStates.end() && _physicsCollisionCallback != nullptr) {
                        if(_physicsCollisionCallback->onCollisionExit != nullptr) _physicsCollisionCallback->onCollisionExit(_a, _b);
                        _it->state = CollisionState::COLLISION_EXIT;
                        collisionStates.erase(_it);
                    }
                }

                if(_m.contactCount && !_a->ghost && !_b->ghost)
                    contacts.emplace_back(_m);
            }
        }

        // Integrate forces
        for(auto* _body : bodies) {
            if(_body->ignorePhysics) continue;
            integrateForces(_body, _fxDt);
        }

        // Initialize collision
        for(auto& _contact : contacts)
            _contact.initialize(_fxDt, gravity);

        // Solve collisions
        for(auto _i = 0; _i < 1; _i++)
            for(auto& _contact : contacts) {
                _contact.applyImpulse();
            }

        // Integrate velocities
        for(auto* _body : bodies) {
            if(_body->ignorePhysics) continue;
            integrateVelocity(_body, _fxDt);
        }

        // Correct positions
        for(auto& _contact : contacts) {
            _contact.positionalCorrection();
        }

        // Clear all forces
        for(auto* _body : bodies) {
            if(_body->ignorePhysics) continue;
            _body->force.set( 0, 0 );
            _body->torque = 0;
        }
    }

    void PhysicsManager::integrateForces(PhysicsBody* _physicsBody, Delta _fixedDelta) {
        if(_physicsBody->inverseMass == 0.0f)
            return;

        _physicsBody->velocity += (_physicsBody->force * _physicsBody->inverseMass + gravity) * (_fixedDelta / 2.0f);
        _physicsBody->angularVelocity += _physicsBody->torque * _physicsBody->inverseInertia * (_fixedDelta / 2.0f);
    }

    void PhysicsManager::integrateVelocity(PhysicsBody* _physicsBody, Delta _fxDt) {
        if(_physicsBody->inverseMass == 0.0f)
            return;

        _physicsBody->transform->translate(_physicsBody->velocity * _fxDt);

        _physicsBody->rotate(radiansToDegrees(_physicsBody->angularVelocity * _fxDt));
        integrateForces(_physicsBody, _fxDt);
    }

    void PhysicsManager::drawPolygon(PhysicsBody* _physicsBody, RenderManager* _renderManager, const Color& _lineColor, const Color& _radiusColor, bool _showLines, bool _showRadius) {
        if(!_showLines) return;

        auto& _polygon = _physicsBody->shape;
        for (auto _i = 0; _i < _polygon.vertices.size(); _i++) {

            int _next = _i + 1;
            if(_next == _polygon.vertices.size())
                _next = 0;

            Vec2F _p0 = (_physicsBody->transform->getModelMatrixPosition()) + _polygon.getRotationMatrix() * (_polygon.vertices[_i] + _physicsBody->offset);
            Vec2F _p1 = (_physicsBody->transform->getModelMatrixPosition()) + _polygon.getRotationMatrix() * (_polygon.vertices[_next] + _physicsBody->offset);

            _renderManager->drawLine(_p0, _p1, _lineColor);
        }

        if(!_showRadius) return;

        _renderManager->drawLine(_physicsBody->transform->getModelMatrixPosition() + _polygon.getRotationMatrix() * _physicsBody->offset,
                                 _physicsBody->transform->getModelMatrixPosition() + _polygon.getRotationMatrix() * (_polygon.vertices[0] + _physicsBody->offset),
                                 _radiusColor);
    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        if(!debugOptions.showGeneralDebug) return;

        for(auto* _body : bodies) {

            switch (_body->shape.type) {
                case PhysicsShape::Type::CIRCLE: {
                    if(!debugOptions.showCircleLines) return;

                    const int _segments = 20;
                    float _theta = _body->transform->getRotation();
                    float _inc = PI * 2.0f / (float)_segments;
                    Vec2F _points[_segments];

                    for(auto& _point : _points) {
                        _theta += _inc;
                        Vec2F _p(std::cos(_theta), std::sin(_theta) );
                        _p *= _body->shape.size.x;
                        _p += (_body->transform->getModelMatrixPosition() + _body->shape.getRotationMatrix() * _body->offset);
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
                    _renderManager->drawLine((_body->transform->getModelMatrixPosition() + _body->shape.getRotationMatrix() * _body->offset), _points[0], debugOptions.circleRadiusColor);

                    break;
                }

                case PhysicsShape::Type::BOX:
                    drawPolygon(_body, _renderManager, debugOptions.boxLineColor, debugOptions.boxRadiusColor, debugOptions.showBoxLines, debugOptions.showBoxRadius);
                    break;
                case PhysicsShape::Type::POLYGON: {
                    drawPolygon(_body, _renderManager, debugOptions.polygonLineColor, debugOptions.polygonRadiusColor, debugOptions.showPolygonLines, debugOptions.showPolygonRadius);
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
        if(_it != bodies.end()) {
            bodies.erase(_it);
        }

        auto _it1 = std::find_if(contacts.begin(), contacts.end(), [_physicsBody](PhysicsManifold& _contact) {
            return _contact.A == _physicsBody || _contact.B == _physicsBody;
        });
        if(_it1 != contacts.end()) {
            contacts.erase(_it1);
        }

        auto _it2 = std::find_if(collisionStates.begin(), collisionStates.end(), [_physicsBody](CollisionState& _collisionState) {
            return _collisionState.A == _physicsBody || _collisionState.B == _physicsBody;
        });
        if(_it2 != collisionStates.end()) {
            collisionStates.erase(_it2);
        }
    }

    bool PhysicsManager::collisionCircleCircle(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto& _a = _bodyA->shape;
        auto& _b = _bodyB->shape;

        // Calculate translational vector, which is _normal
        Vec2F _normal = (_bodyB->transform->getModelMatrixPosition() + _bodyB->offset) - (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);

        float _distSqr = _normal.magnitudeSqr();
        float _radius = _a.size.x + _b.size.x;

        // Not in contact
        if (_distSqr >= _radius * _radius) {
            _manifold.contactCount = 0;
            return false;
        }

        float _distance = std::sqrt(_distSqr);

        _manifold.contactCount = 1;

        if (_distance == 0.0f) {
            _manifold.penetration = _a.size.x;
            _manifold.normal = GDE::Vec2F(1, 0);
            _manifold.contacts[0] = (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);
        } else {
            _manifold.penetration = _radius - _distance;
            _manifold.normal = _normal / _distance; // Faster than using Normalized since we already performed sqrt
            _manifold.contacts[0] = _manifold.normal * _a.size.x + (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);
        }

        return true;
    }

    bool PhysicsManager::collisionCirclePolygon(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto& _a = _bodyA->shape;
        auto& _b = _bodyB->shape;

        _manifold.contactCount = 0;

        // Transform circle _center to Polygon model space
        Vec2F _center = (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);
        _center = _b.getRotationMatrix().transpose() * (_center - (_bodyB->transform->getModelMatrixPosition() + _bodyB->offset));

        // Find edge with minimum penetration
        // Exact concept as using support points in Polygon vs Polygon
        float _separation = -FLT_MAX;
        auto _faceNormal = 0;
        for (auto _i = 0; _i < _b.vertexCount; ++_i) {
            float _s = _b.normals[_i].dotProduct(_center - _b.vertices[_i]);

            if (_s > _a.size.x)
                return false;

            if (_s > _separation) {
                _separation = _s;
                _faceNormal = _i;
            }
        }

        // Grab face's vertices
        Vec2F _v1 = _b.vertices[_faceNormal];
        auto _i2 = _faceNormal + 1 < _b.vertexCount ? _faceNormal + 1 : 0;
        Vec2F _v2 = _b.vertices[_i2];

        // Check to see if _center is within polygon
        if (_separation < EPSILON) {
            _manifold.contactCount = 1;
            _manifold.normal = -(_b.getRotationMatrix() * _b.normals[_faceNormal]);
            _manifold.contacts[0] = _manifold.normal * _a.size.x + (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);
            _manifold.penetration = _a.size.x;
            return false;
        }

        // Determine which voronoi region of the edge _center of circle lies within
        float _dot1 = (_center - _v1).dotProduct(_v2 - _v1);
        float _dot2 = (_center - _v2).dotProduct(_v1 - _v2);
        _manifold.penetration = _a.size.x - _separation;

        // Closest to _v1
        if (_dot1 <= 0.0f) {
            if (_center.distanceSqr(_v1) > _a.size.x * _a.size.x)
                return false;

            _manifold.contactCount = 1;
            Vec2F _n = _v1 - _center;
            _n = _b.getRotationMatrix() * _n;
            _n.normalize();
            _manifold.normal = _n;
            _v1 = _b.getRotationMatrix() * _v1 + (_bodyB->transform->getModelMatrixPosition() + _bodyB->offset);
            _manifold.contacts[0] = _v1;
        }

            // Closest to _v2
        else if (_dot2 <= 0.0f) {
            if (_center.distance(_v2) > _a.size.x * _a.size.x)
                return false;

            _manifold.contactCount = 1;
            Vec2F _n = _v2 - _center;
            _v2 = _b.getRotationMatrix() * _v2 + (_bodyB->transform->getModelMatrixPosition() + _bodyB->offset);
            _manifold.contacts[0] = _v2;
            _n = _b.getRotationMatrix() * _n;
            _n.normalize();
            _manifold.normal = _n;
        }

            // Closest to face
        else {
            Vec2F _n = _b.normals[_faceNormal];
            if ((_center - _v1).dotProduct(_n) > _a.size.x)
                return false;

            _n = _b.getRotationMatrix() * _n;
            _manifold.normal = -_n;
            _manifold.contacts[0] = _manifold.normal * _a.size.x + (_bodyA->transform->getModelMatrixPosition() + _bodyA->offset);
            _manifold.contactCount = 1;
        }

        return true;
    }

    bool PhysicsManager::collisionPolygonCircle(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto _collision = collisionCirclePolygon(_manifold, _bodyB, _bodyA);
        _manifold.normal = -_manifold.normal;
        return _collision;
    }

    bool PhysicsManager::collisionPolygonPolygon(PhysicsManifold& _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto& _a = _bodyA->shape;
        auto& _b = _bodyB->shape;
        _manifold.contactCount = 0;

        // Check for _bodyA separating axis with _a's face planes
        uint32_t _faceA;
        float _penetrationA = findAxisWithLeastPenetration(&_faceA, &_a, &_b);
        if (_penetrationA >= 0.0f)
            return false;

        // Check for _bodyA separating axis with _b's face planes
        uint32_t _faceB;
        float _penetrationB = findAxisWithLeastPenetration(&_faceB, &_b, &_a);
        if (_penetrationB >= 0.0f)
            return false;

        uint32_t _referenceIndex;
        bool _flip; // Always point from _bodyA to _bodyB

        PhysicsShape* _refPoly; // Reference
        PhysicsShape* _incPoly; // Incident

        // Determine which shape contains reference face
        if (PhysicsMath::biasGreaterThan(_penetrationA, _penetrationB)) {
            _refPoly = &_a;
            _incPoly = &_b;
            _referenceIndex = _faceA;
            _flip = false;
        } else {
            _refPoly = &_b;
            _incPoly = &_a;
            _referenceIndex = _faceB;
            _flip = true;
        }

        // World space incident face
        Vec2F _incidentFace[2];
        findIncidentFace(_incidentFace, _refPoly, _incPoly, _referenceIndex);

        //        y
        //        ^  ->n       ^
        //      +---c ------posPlane--
        //  x < | i |\
        //      +---+ c-----negPlane--
        //             \       v
        //              r
        //
        //  r : reference face
        //  i : incident poly
        //  c : clipped point
        //  n : incident normal

        // Setup reference face vertices
        Vec2F _v1 = _refPoly->vertices[_referenceIndex];
        _referenceIndex = _referenceIndex + 1 == _refPoly->vertexCount ? 0 : _referenceIndex + 1;
        Vec2F _v2 = _refPoly->vertices[_referenceIndex];

        // Transform vertices to world space
        _v1 = _refPoly->getRotationMatrix() * _v1 + (_refPoly->physicsBody->transform->getModelMatrixPosition() + _refPoly->physicsBody->offset);
        _v2 = _refPoly->getRotationMatrix() * _v2 + (_refPoly->physicsBody->transform->getModelMatrixPosition() + _refPoly->physicsBody->offset);

        // Calculate reference face side normal in world space
        Vec2F _sidePlaneNormal = (_v2 - _v1);
        _sidePlaneNormal.normalize();

        // Orthogonalize
        Vec2F _refFaceNormal(_sidePlaneNormal.y, -_sidePlaneNormal.x);

        // ax + by = c
        // c is distance from origin
        float _refC = _refFaceNormal.dotProduct(_v1);
        float _negSide = -_sidePlaneNormal.dotProduct(_v1);
        float _posSide = _sidePlaneNormal.dotProduct(_v2);

        // Clip incident face to reference face side planes
        if (clip(-_sidePlaneNormal, _negSide, _incidentFace) < 2)
            return false; // Due to floating point error, possible to not have required points

        if (clip(_sidePlaneNormal, _posSide, _incidentFace) < 2)
            return false; // Due to floating point error, possible to not have required points

        // Flip
        _manifold.normal = _flip ? -_refFaceNormal : _refFaceNormal;

        // Keep points behind reference face
        auto _cp = 0; // clipped points behind reference face
        float _separation = _refFaceNormal.dotProduct(_incidentFace[0]) - _refC;
        if (_separation <= 0.0f) {
            _manifold.contacts[_cp] = _incidentFace[0];
            _manifold.penetration = -_separation;
            ++_cp;
        } else
            _manifold.penetration = 0;

        _separation = _refFaceNormal.dotProduct(_incidentFace[1]) - _refC;
        if (_separation <= 0.0f) {
            _manifold.contacts[_cp] = _incidentFace[1];

            _manifold.penetration += -_separation;
            ++_cp;

            // Average penetration
            _manifold.penetration /= (float) _cp;
        }

        _manifold.contactCount = _cp;

        return true;
    }

    float PhysicsManager::findAxisWithLeastPenetration(uint32_t* _faceIndex, PhysicsShape* _shapeA, PhysicsShape* _shapeB) {
        float _bestDistance = -FLT_MAX;
        uint32_t _bestIndex;

        for (auto _i = 0; _i < _shapeA->vertexCount; ++_i) {
            // Retrieve a face normal from _shapeA
            Vec2F _n = _shapeA->normals[_i];
            Vec2F _nw = _shapeA->getRotationMatrix() * _n;

            // Transform face normal into _shapeB'_s model space
            Mat2 _buT = _shapeB->getRotationMatrix().transpose();
            _n = _buT * _nw;

            // Retrieve support point from _shapeB along -_n
            Vec2F _s = _shapeB->getSupport(-_n);

            // Retrieve vertex on face from _shapeA, transform into
            // _shapeB'_s model space
            Vec2F _v = _shapeA->vertices[_i];
            _v = _shapeA->getRotationMatrix() * _v + (_shapeA->physicsBody->transform->getModelMatrixPosition() + _shapeA->physicsBody->offset);
            _v -= (_shapeB->physicsBody->transform->getModelMatrixPosition() + _shapeB->physicsBody->offset);
            _v = _buT * _v;

            // Compute penetration distance (in _shapeB'_s model space)
            float _d = _n.dotProduct(_s - _v);

            // Store greatest distance
            if (_d > _bestDistance) {
                _bestDistance = _d;
                _bestIndex = _i;
            }
        }

        *_faceIndex = _bestIndex;
        return _bestDistance;
    }

    void PhysicsManager::findIncidentFace(Vec2F* _vec, PhysicsShape* _refPoly, PhysicsShape* _incPoly, uint32_t _referenceIndex) {
        Vec2F _referenceNormal = _refPoly->normals[_referenceIndex];

        // Calculate normal in incident's frame of reference
        _referenceNormal = _refPoly->getRotationMatrix() * _referenceNormal; // To world space
        _referenceNormal = _incPoly->getRotationMatrix().transpose() * _referenceNormal; // To incident's model space

        // Find most anti-normal face on incident polygon
        auto _incidentFace = 0;
        float _minDot = FLT_MAX;
        for (auto _i = 0; _i < _incPoly->vertexCount; ++_i) {
            float _dot = _referenceNormal.dotProduct(_incPoly->normals[_i]);
            if (_dot < _minDot) {
                _minDot = _dot;
                _incidentFace = _i;
            }
        }

        // Assign face vertices for _incidentFace
        _vec[0] = _incPoly->getRotationMatrix() * _incPoly->vertices[_incidentFace] +
                (_incPoly->physicsBody->transform->getModelMatrixPosition() + _incPoly->physicsBody->offset);
        _incidentFace = _incidentFace + 1 >= (int32_t) _incPoly->vertexCount ? 0 : _incidentFace + 1;
        _vec[1] = _incPoly->getRotationMatrix() * _incPoly->vertices[_incidentFace] +
                (_incPoly->physicsBody->transform->getModelMatrixPosition() + _incPoly->physicsBody->offset);
    }

    int PhysicsManager::clip(Vec2F _n, float _c, Vec2F* _face) {
        auto _sp = 0;
        Vec2F _out[2] = { _face[0],_face[1] };

        // Retrieve distances from each endpoint to the line
        // d = ax + by - _c
        float _d1 = _n.dotProduct(_face[0]) - _c;
        float _d2 = _n.dotProduct(_face[1]) - _c;

        // If negative (behind plane) clip
        if (_d1 <= 0.0f) _out[_sp++] = _face[0];
        if (_d2 <= 0.0f) _out[_sp++] = _face[1];

        // If the points are on different sides of the plane
        if (_d1 * _d2 < 0.0f) {  // less than to ignore -0.0f
            // Push interesection point
            float _alpha = _d1 / (_d1 - _d2);
            _out[_sp] = _face[0] + _alpha * (_face[1] - _face[0]);
            ++_sp;
        }

        // Assign our new converted values
        _face[0] = _out[0];
        _face[1] = _out[1];

        return _sp;
    }

    PhysicsCollisionCallbacks* PhysicsManager::addOrGetCollisionToTable(ulong _bodyMaskA, ulong _bodyMaskB) {
        ENGINE_ASSERT(_bodyMaskA >= 0 && _bodyMaskA < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS,")")
        ENGINE_ASSERT(_bodyMaskB >= 0 && _bodyMaskB < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS, ")")

        if(collisionTable[_bodyMaskA][_bodyMaskB] == nullptr && collisionTable[_bodyMaskA][_bodyMaskB] == nullptr) {
            collisionTable[_bodyMaskA][_bodyMaskB] = new PhysicsCollisionCallbacks;
            collisionTable[_bodyMaskB][_bodyMaskA] = collisionTable[_bodyMaskA][_bodyMaskB];
        }

        return collisionTable[_bodyMaskA][_bodyMaskB];
    }

    bool PhysicsManager::removeCollisionToTable(ulong _bodyMaskA, ulong _bodyMaskB) {
        ENGINE_ASSERT(_bodyMaskA >= 0 && _bodyMaskA < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS,")")
        ENGINE_ASSERT(_bodyMaskB >= 0 && _bodyMaskB < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS,")")

        if(collisionTable[_bodyMaskA][_bodyMaskB] != nullptr) {
            delete collisionTable[_bodyMaskA][_bodyMaskB];
            collisionTable[_bodyMaskA][_bodyMaskB] = nullptr;
            collisionTable[_bodyMaskB][_bodyMaskA] = nullptr;
            return true;
        }

        if(collisionTable[_bodyMaskB][_bodyMaskA] != nullptr) {
            delete collisionTable[_bodyMaskB][_bodyMaskA];
            collisionTable[_bodyMaskB][_bodyMaskA] = nullptr;
            collisionTable[_bodyMaskA][_bodyMaskB] = nullptr;
            return true;
        }

        return false;
    }

    bool PhysicsManager::hasCollisionInTable(ulong _bodyMaskA, ulong _bodyMaskB) {
        ENGINE_ASSERT(_bodyMaskA >= 0 && _bodyMaskA < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS,")")
        ENGINE_ASSERT(_bodyMaskB >= 0 && _bodyMaskB < MAX_MASKS, "Collision masks must be defined in the range of [0,", MAX_MASKS,")")

        return collisionTable[_bodyMaskA][_bodyMaskB] != nullptr;
    }




}