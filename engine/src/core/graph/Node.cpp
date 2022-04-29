// Created by borja on 24/3/22.

#include "core/graph/Node.h"

namespace GDE {

    Node::Node(Transform* _transform, Tag* _tag, const NodeID& _nodeID) :transform(_transform), tag(_tag), ID(_nodeID) {  }

    Transform& Node::getTransform() {
        return *transform;
    }

    Tag& Node::getTag() {
        return *tag;
    }

    NodeID Node::getID() {
        return ID;
    }
}
