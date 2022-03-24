// Created by borja on 11/3/22.

#include "core/graph/Components.h"
#include "core/graph/Graph.h"
#include "core/graph/Node.h"
#include "core/util/Functions.h"

namespace engine {

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        return glm::translate(glm::mat4(1.0f), localPosition) * _rot * glm::scale(glm::mat4(1.0f), localScale);
    }

    void Transform::update(Graph* _graph) {
        if (parent != NODE_ID_NULL)
            modelMatrix = _graph->getComponent<Transform>(parent)->modelMatrix * getLocalModelMatrix();
        else
            modelMatrix = getLocalModelMatrix();

        for (auto& _child : children) {
            _graph->getComponent<Transform>(_child)->update(_graph);
        }

        dirty = false;
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        Util::worldToScreenSize(_x, _y);
        localPosition = glm::vec3 {_x, _y, 0.0f};
        dirty = true;
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
        Util::worldToScreenSize(_x, _y);
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

    bool Transform::isDirty() const {
        return dirty;
    }

    Vec2F Transform::getPositionWorld() const {
        return { modelMatrix[3][0], modelMatrix[3][1] };
    }

    float Transform::getRotationWorld() const {
        return -1;
    }

    Vec2F Transform::getScaleLWorld() const {
        return { modelMatrix[0][0], modelMatrix[1][1] };
    }

}
