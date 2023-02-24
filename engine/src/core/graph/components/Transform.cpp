//
// Created by borja on 9/5/22.
//

#include "core/graph/components/Transform.h"
#include "core/graph/Graph.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "core/graph/components/Node.h"
#include "core/graph/components/ui/UI.h"
#include "core/util/Functions.h"

namespace RDE {

    Transform::Transform(Graph* _graph) : graph(_graph) {
        worldMatrixCache = recalculateCachedMatrix();
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        if(innerPosition.x == _x && innerPosition.y == _y) return;
        innerPosition = glm::vec3 {_x, _y, 0.0f};
        setDirty(this);
    }

    Vec2F Transform::getPosition() const {
        return { innerPosition.x, innerPosition.y };
    }

    void Transform::setRotation(float _rotation) {
        auto _newRot = glm::quat(glm::vec3(0, 0, glm::radians(_rotation)));
        if(innerRotation == _newRot) return;

        innerRotation = glm::quat(glm::vec3(0, 0, glm::radians(_rotation)));
        setDirty(this);
    }

    float Transform::getRotation() const {
        return Util::Math::radiansToDegrees(glm::eulerAngles(innerRotation).z);
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        innerScale = {_x, _y, 1.0f};
        setDirty(this);
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
        setDirty(this);
    }

    void Transform::rotate(float _amount) {
        glm::vec3 _euler = glm::eulerAngles(innerRotation);
        innerRotation = glm::quat(glm::vec3(_euler.x, _euler.y, _euler.z + Util::Math::degreesToRadians(_amount)));
        setDirty(this);
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        innerScale.x += _x;
        innerScale.y += _y;
        setDirty(this);
    }

    Vec2F Transform::getModelMatrixPosition() {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        auto [_transformMatrix, _] = localToWorld();
        glm::decompose(_transformMatrix, scale, rotation, translation, skew, perspective);
        return { translation.x, translation.y };
    }

    Vec2F Transform::getModelMatrixScale() {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        auto [_transformMatrix, _] = localToWorld();
        glm::decompose(_transformMatrix, scale, rotation, translation, skew, perspective);
        return { scale.x, scale.y };
    }

    float Transform::getModelMatrixRotation() {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        auto [_transformMatrix, _] = localToWorld();
        glm::decompose(_transformMatrix, scale, rotation, translation, skew, perspective);
        glm::vec3 _euler = glm::eulerAngles(rotation);
        return Util::Math::radiansToDegrees(_euler.z);
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
                    glm::vec4(0.0f, 0.0f, 0.0f,        1.0f)
                )

               * glm::mat4_cast(glm::inverse(innerRotation)) //un-rotate

               * glm::mat4( //un-translate
                    glm::vec4(1.0f, 0.0f, 0.0f, 0),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(-innerPosition.x,-innerPosition.y, 0.0f, 1.0f)
                );
    }

    std::tuple<glm::mat4, bool> Transform::localToWorld() {
        return { worldMatrixCache, dirty };
    }

    glm::mat4 Transform::worldToLocal() const {
        if (parentTransform) {
            return parentToLocal() * parentTransform->worldToLocal();
        } else {
            return parentToLocal();
        }
    }

    glm::mat4 Transform::recalculateCachedMatrix() {
        if (parentTransform) {
            auto [_parentMatrix, _dirty] = parentTransform->localToWorld();
            return _parentMatrix * localToParent();
        }

        return localToParent();
    }

    void Transform::setDirty(Transform* _transform) {
        worldMatrixCache = _transform->recalculateCachedMatrix();
        _transform->dirty = true;
        for(auto* _child : children) {
            _child->setDirty(_child);
        }

        graph->renderingTreeData.dirtyTransforms.push_back(_transform);
    }

    void Transform::setMatrixModelPosition(const Vec2F& _worldPos) {
        worldMatrixCache[3][0] = _worldPos.x;
        worldMatrixCache[3][1] = _worldPos.y;
        setLocalMatrix(glm::inverse(parentTransform->worldMatrixCache) * worldMatrixCache);
        dirty = true;
    }

    void Transform::translateMatrixModelPosition(const Vec2F& _worldPos) {
        worldMatrixCache[3][0] += _worldPos.x;
        worldMatrixCache[3][1] += _worldPos.y;
        setLocalMatrix(glm::inverse(parentTransform->worldMatrixCache) * worldMatrixCache);
        dirty = true;
    }

    void Transform::setMatrixModelRotation(float _rotation) {
        glm::vec3 _scale;
        glm::quat _rot;
        glm::vec3 _translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(worldMatrixCache, _scale, _rot, _translation, skew, perspective);

        worldMatrixCache = glm::mat4( //translate
                glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(_translation, 1.0f)
        )
        * glm::mat4_cast(glm::angleAxis(Util::Math::degreesToRadians(_rotation), glm::vec3 { 0, 0, 1 })) //rotate
        * glm::mat4( //scale
                glm::vec4(_scale[0], 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, _scale[1], 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        setLocalMatrix(glm::inverse(parentTransform->worldMatrixCache) * worldMatrixCache);
        dirty = true;
    }

    glm::mat4 Transform::worldPointToLocalPosition(const Vec2F& _position) {
        glm::mat4 _newWorldMatrix = worldMatrixCache;
        _newWorldMatrix[3][0] = _position.x;
        _newWorldMatrix[3][1] = _position.y;

        auto [_mat, _] = parentTransform->localToWorld();
        return _newWorldMatrix * glm::inverse(_mat);
    }

    glm::mat4 Transform::worldPointToLocalRotation(float _rotation) {
        glm::mat4 _newWorldMatrix = worldMatrixCache;
        _newWorldMatrix[0][0] *= std::cos(_rotation);
        _newWorldMatrix[0][1] *= -std::sin(_rotation);
        _newWorldMatrix[1][0] *= std::sin(_rotation);
        _newWorldMatrix[1][1] *= std::cos(_rotation);

        auto [_mat, _] = parentTransform->localToWorld();
        return _newWorldMatrix * glm::inverse(_mat);
    }

    glm::mat4 Transform::getLocalMatrix() const {
        return localToParent();
    }

    void Transform::setLocalMatrix(const glm::mat4& _matrix) {
        glm::vec3 _scale;
        glm::quat _rotation;
        glm::vec3 _translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(_matrix, _scale, _rotation, _translation, skew, perspective);
        glm::vec3 _euler = glm::eulerAngles(_rotation);

        innerPosition = _translation;
        innerRotation = _rotation;
        innerScale = _scale;
    }

    int Transform::getChildrenCount() {
        int _childrenCount = 0;
        for(auto& _child : children) {
            _childrenCount++;
            _childrenCount += _child->getChildrenCount();
        }

        return _childrenCount;
    }

    int Transform::getActiveChildrenCount() {
        int _childrenCount = 0;
        for(auto& _child : children) {
            if(_child->node->isActive()) {
                _childrenCount++;
                _childrenCount += _child->getChildrenCount();
            }
        }

        return _childrenCount;
    }

    int Transform::getEnabledChildrenCount() {
        int _childrenCount = 0;
        for(auto& _child : children) {
            if(!_child->node->hasComponent<DisabledForRender>() && !_child->node->hasComponent<DisabledForUpdate>() &&
                    !_child->node->hasComponent<DisabledForLateUpdate>() && !_child->node->hasComponent<DisabledForFixedUpdate>()
                            && !_child->node->hasComponent<DisabledForLateUpdate>()) {
                _childrenCount++;
            }

            _childrenCount += _child->getChildrenCount();
        }

        return _childrenCount;
    }

    int Transform::getEnabledAndActiveChildrenCount() {
        int _childrenCount = 0;
        for(auto& _child : children) {
            if(node->isActive() && !_child->node->hasComponent<DisabledForRender>() && !_child->node->hasComponent<DisabledForUpdate>() &&
               !_child->node->hasComponent<DisabledForLateUpdate>() && !_child->node->hasComponent<DisabledForFixedUpdate>()
               && !_child->node->hasComponent<DisabledForLateUpdate>()) {
                _childrenCount++;
                _childrenCount += _child->getChildrenCount();
            }
        }

        return _childrenCount;
    }

    void Transform::clearDirty() {
        dirty = false;
    }
}