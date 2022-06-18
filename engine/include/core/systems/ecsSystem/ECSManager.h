//
// Created by borja on 29/04/22.
//

#ifndef ENGINE_GDE_SYSTEM_MANAGER_H
#define ENGINE_GDE_SYSTEM_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/ecsSystem/ECSSystem.h"
#include "core/graph/Graph.h"

namespace GDE {

    /**
     * @brief Custom exception for Bad system swaping error.
     */
    class BadECSSystemSwap : public std::exception {
        public:
            [[nodiscard]] auto what() const noexcept -> const char* override {
                return "Error while swapping ECS systems, one or both of them couldn't be found";
            }
    };


    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class is in charge of updating each of the custom ECS systems created by the end users.
     */
    class ECSManager {
        private:
            int systemRefCounter = 0;
            std::vector<ECSSystem*> systems;

        public:
            /**
             * @brief This function adds a new custom system to the application.
             * @param _system User-defined ECS system
             * @return SystemRef
             */
            SystemRef addSystem(ECSSystem* _system);

            /**
             * @brief This function removes a system from the application.
             * @param _systemRef Ref to the system to delete
             */
            void removeSystem(SystemRef _systemRef);

            /**
             * @brief This function changes the updating order of two systems.
             * @param _systemRef0 System 0
             * @param _systemRef1 System 1
             */
            void swapOrderOfUpdate(SystemRef _systemRef0, SystemRef _systemRef1);

            /**
             * @brief This function should not be called by end users.
             * @param _dt Delta time
             * @param _graph Scene Graph
             */
            void update(Delta _dt, Graph* _graph);

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Destroys all of the ECS systems defined by the user.
             */
            void destroy();
    };

}

#endif //ENGINE_GDE_SYSTEM_MANAGER_H
