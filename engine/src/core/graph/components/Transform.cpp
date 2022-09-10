//
// Created by borja on 9/5/22.
//

#include "core/graph/components/Transform.h"
#include "core/graph/Graph.h"

namespace GDE {

    Transform::Transform(const NodeID& _nodeId) {

    }

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 _scale = {localScale.x, localScale.y, 1};

        localModelMatrix = glm::translate(glm::mat4(1.0f), {localPosition.x, localPosition.y, 0.0f}) * _rot * glm::scale(glm::mat4(1.0f), _scale);
        return localModelMatrix;
    }

    void Transform::update() {
        if (parent != NODE_ID_NULL) {
            if(!dirty && !parentTransform->dirty) return;
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
        dirty = true;
    }

    void Transform::setPositionWorld(const Vec2F& _position) {
        setPositionWorld(_position.x, _position.y);
    }

    void Transform::setPositionWorld(float _x, float _y) {
        modelMatrix[3][0] = _x;
        modelMatrix[3][1] = _y;
    }

    Vec2F Transform::getPositionLocal() const {
        return {localPosition.x, localPosition.y};
    }

    void Transform::setRotation(float _rotation) {
        localRotation = _rotation;
        dirty = true;
    }

    float Transform::getRotationLocal() const {
        return localRotation;
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        localScale = {_x, _y, 1.0f};
        dirty = true;
    }

    Vec2F Transform::getScaleLocal() const {
        return {localScale.x, localScale.y};
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        localPosition.x += _x;
        localPosition.y += _y;
        dirty = true;
    }

    void Transform::rotate(float _amount) {
        localRotation += _amount;
        dirty = true;
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        localScale.x += _x;
        localScale.y += _y;
        dirty = true;
    }

    Vec2F Transform::getPositionWorld() const {
        float _x = modelMatrix[3][0], _y =  modelMatrix[3][1];
        return {_x, _y};
    }

    float Transform::getRotationWorld() const {
        return -1;
    }

    Vec2F Transform::getScaleLWorld() const {
        return { modelMatrix[0][0], modelMatrix[1][1] };
    }
}