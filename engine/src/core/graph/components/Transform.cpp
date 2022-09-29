//
// Created by borja on 9/5/22.
//

#include "core/graph/components/Transform.h"
#include "core/graph/Graph.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GDE {

    Transform::Transform(const NodeID& _nodeId) {
        ID = _nodeId;
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        innerPosition = glm::vec3 {_x, _y, 0.0f};
    }

    Vec2F Transform::getPosition() const {
        return { innerPosition.x, innerPosition.y };
    }

    void Transform::setRotation(float _rotation) {
        innerRotation = glm::quat(glm::vec3(0, 0, glm::radians(_rotation)));
    }

    float Transform::getRotation() const {
        return radiansToDegrees(glm::eulerAngles(innerRotation).z);
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        innerScale = {_x, _y, 1.0f};
    }

    Vec2F Transform::getScale() const {
        return { innerScale.x, innerScale.y };
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        innerPosition.x += _x;
        innerPosition.y += _y;
    }

    void Transform::rotate(float _amount) {
        glm::vec3 _euler = glm::eulerAngles(innerRotation);
        innerRotation = glm::quat(glm::vec3(_euler.x, _euler.y, _euler.z + degreesToRadians(_amount)));
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        innerScale.x += _x;
        innerScale.y += _y;
    }

    Vec2F Transform::getModelMatrixPosition() const {
        auto _worldModelMatrix = localToWorld();
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(_worldModelMatrix, scale, rotation, translation, skew, perspective);
        return { translation.x, translation.y };
    }

    Vec2F Transform::getModelMatrixScale() const {
        auto _worldModelMatrix = localToWorld();
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(_worldModelMatrix, scale, rotation, translation, skew, perspective);
        return { scale.x, scale.y };
    }

    float Transform::getModelMatrixRotation() const {
        auto _worldModelMatrix = localToWorld();
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(_worldModelMatrix, scale, rotation, translation, skew, perspective);
        glm::vec3 _euler = glm::eulerAngles(rotation);
        return radiansToDegrees(_euler.z);
    }

    glm::mat4 Transform::localToParent() const{
        return glm::mat4( //translate
                glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(innerPosition, 1.0f)
        )
        * glm::mat4_cast(innerRotation) //rotate
        * glm::mat4( //scale
                glm::vec4(innerScale.x, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, innerScale.y, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    glm::mat4 Transform::parentToLocal() const {
        glm::vec3 inv_scale;
        inv_scale.x = (innerScale.x == 0.0f ? 0.0f : 1.0f / innerScale.x);
        inv_scale.y = (innerScale.y == 0.0f ? 0.0f : 1.0f / innerScale.y);
        inv_scale.z = 1.0f;
        return glm::mat4( //un-scale
                glm::vec4(inv_scale.x, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, inv_scale.y, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, inv_scale.z, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        )
               * glm::mat4_cast(glm::inverse(innerRotation)) //un-rotate
               * glm::mat4( //un-translate
                glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(-innerPosition, 1.0f)
        );
    }

    glm::mat4 Transform::localToWorld() const {
        if (parentTransform) {
            auto _parentMatrix = parentTransform->localToWorld();
            return _parentMatrix * localToParent();
        } else {
            return localToParent();
        }
    }

    glm::mat4 Transform::worldToLocal() const {
        if (parentTransform) {
            return parentToLocal() * parentTransform->worldToLocal();
        } else {
            return parentToLocal();
        }
    }
}