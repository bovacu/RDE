//
// Created by borja on 29/04/22.
//

#ifndef ENGINE_GDE_SYSTEM_MANAGER_H
#define ENGINE_GDE_SYSTEM_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/ecsSystem/ECSSystem.h"
#include "core/graph/Graph.h"

namespace GDE {

    class BadECSSystemSwap : public std::exception {
        public:
            [[nodiscard]] auto what() const noexcept -> const char* override {
                return "Error while swapping ECS systems, one or both of them couldn't be found";
            }
    };

    /// This class is in charge of updating each of the custom ECS systems created by the end users.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class ECSManager {
        private:
            int systemRefCounter = 0;
            std::vector<ECSSystem*> systems;

        public:
            /// This function adds a new custom system to the application.
            SystemRef addSystem(ECSSystem* _system);
            /// This function removes a system from the application.
            void removeSystem(SystemRef _systemRef);
            /// This function changes the updating order of two systems.
            void swapOrderOfUpdate(SystemRef _systemRef0, SystemRef _systemRef1);

            /// This function should not be called by end users.
            void update(Delta _dt, Graph* _graph);
            /// This function should not be called by end users.
            void destroy();
    };

}

#endif //ENGINE_GDE_SYSTEM_MANAGER_H
