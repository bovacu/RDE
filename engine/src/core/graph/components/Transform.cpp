//
// Created by borja on 9/5/22.
//

#include "core/graph/components/Transform.h"
#include "core/graph/Graph.h"

namespace GDE {

    Transform::Transform(const NodeID& _nodeId) {
        ID = _nodeId;
    }

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 _scale = {localScale.x, localScale.y, 1};

        localModelMatrix = glm::translate(glm::mat4(1.0f), {localPosition.x, localPosition.y, 0.0f}) * _rot * glm::scale(glm::mat4(1.0f), _scale);
        return localModelMatrix;
    }

    void Transform::update() {
        if (parent != NODE_ID_NULL) {
            if(!parentTransform->dirty) return;
            modelMatrix = parentTransform->modelMatrix * getLocalModelMatrix();
        } else {
            modelMatrix = getLocalModelMatrix();
        }

        dirty = false;
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        localPosition = glm::vec3 {_x, _y, 0.0f};
        forceUpdate();
        dirty = true;
    }

    Vec2F Transform::getPosition() const {
        return { localModelMatrix[3][0], localModelMatrix[3][1] };
    }

    void Transform::setRotation(float _rotation) {
        localRotation = _rotation;
        forceUpdate();
        dirty = true;
    }

    float Transform::getRotation() const {
        return radiansToDegrees(std::atan2(localModelMatrix[0][1], localModelMatrix[0][0]));
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        localScale = {_x, _y, 1.0f};
        forceUpdate();
        dirty = true;
    }

    Vec2F Transform::getScale() const {
        return { localModelMatrix[0][0], localModelMatrix[1][1] };
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        localPosition.x += _x;
        localPosition.y += _y;
        forceUpdate();
        dirty = true;
    }

    void Transform::rotate(float _amount) {
        localRotation += _amount;
        forceUpdate();
        dirty = true;
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        localScale.x += _x;
        localScale.y += _y;
        forceUpdate();
        dirty = true;
    }

    void Transform::forceUpdate() {
        if (parent != NODE_ID_NULL) {
            modelMatrix = parentTransform->modelMatrix * getLocalModelMatrix();
        } else {
            modelMatrix = getLocalModelMatrix();
        }
    }

    Vec2F Transform::getModelMatrixPosition() const {
        return { modelMatrix[3][0], modelMatrix[3][1] };
    }

    Vec2F Transform::getModelMatrixScale() const {
        return { modelMatrix[0][0], modelMatrix[1][1] };
    }

    float Transform::getModelMatrixRotation() const {
        return radiansToDegrees(std::atan2(modelMatrix[0][1], modelMatrix[0][0]));
    }
}