// Created by borja on 24/3/22.


#ifndef ENGINE_NODE_H
#define ENGINE_NODE_H

#include "entt/entity/entity.hpp"

namespace GDE {

    typedef entt::entity NodeID;

    class Graph; struct Transform; struct Tag;
    class Node {
        private:
            Transform* transform;
            Tag* tag;
            Graph* graph;
            NodeID ID;

        public:
            Node(Transform* _transform, Tag* _tag, const NodeID& _nodeID);

            Transform& getTransform();
            Tag& getTag();
            NodeID getID();
    };

}

#endif //ENGINE_NODE_H
