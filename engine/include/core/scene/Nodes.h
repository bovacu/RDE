// Created by borja on 9/3/22.


#ifndef ENGINE_NODES_H
#define ENGINE_NODES_H

#include "core/scene/Scene.h"
#include <entt/entt.hpp>

namespace engine {

    class Node {
        friend class Scene;
        private:
        Scene* scene = nullptr;
        entt::entity id {};
        typename std::vector<Node*>::iterator iter;

        public:
            template<typename Component, typename... Args>
            Component& addComponent(Args... _args) {
                return scene->registry.template emplace<Component>(id, _args...);
            }

            template<typename Component>
            void removeComponent() {
                scene->registry.template remove<Component>(id);
            }

            template<typename Component>
            Component& getComponent() {
                return scene->registry.template get<Component>(id);
            }
    };

}

#endif //ENGINE_NODES_H
