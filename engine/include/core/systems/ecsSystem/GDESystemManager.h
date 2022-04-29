//
// Created by borja on 29/04/22.
//

#ifndef ENGINE_GDE_SYSTEM_MANAGER_H
#define ENGINE_GDE_SYSTEM_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/ecsSystem/GDESystem.h"
#include "core/graph/Graph.h"

namespace GDE {

    class BadECSSystemSwap : public std::exception {
        public:
            [[nodiscard]] auto what() const noexcept -> const char* override {
                return "Error while swapping ECS systems, one or both of them couldn't be found";
            }
    };

    class GDESystemManager {
        private:
            int systemRefCounter = 0;
            std::vector<GDESystem*> systems;

        public:
            static GDESystemManager& get();

            SystemRef addSystem(GDESystem* _system);
            void removeSystem(SystemRef _systemRef);
            void swapOrderOfUpdate(SystemRef _systemRef0, SystemRef _systemRef1);

            void update(Delta _dt, Graph* _graph);
            void destroy();
    };

}

#endif //ENGINE_GDE_SYSTEM_MANAGER_H
