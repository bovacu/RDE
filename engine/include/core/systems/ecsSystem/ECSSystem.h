//
// Created by borja on 29/04/22.
//

#ifndef RDE_SYSTEM_H
#define RDE_SYSTEM_H

#include "core/util/Delta.h"
#include "core/graph/Graph.h"
#include <type_traits>

namespace RDE {

    /** This class is the base that every custom ECS system should inherit from. The only method that is mandatory to
     * implement is the update method.
     * The systemRef is the way to locate this specific system inside the ECSManager.
     * The updateOrder allows us to set in which order we want to update some systems. By default they will be updated
     * in the same order as they've been added to the manager.
     *
     * An example of use would be:
     *
     * |-----------------------------------------------------------------------------------------------|
     * |   class Test : public ECSSystem {                                                             |
     * |                                                                                               |
     * |       public:                                                                                 |
     * |           void onUpdate(Delta _dt, Graph* _graph) override {                                  |
     * |               auto _query0 = _graph->query<int, float>();                                     |
     * |               for(auto&& [entity, pos, vel]: _query0) updateArchetype0(entity, pos, vel);     |
     * |           }                                                                                   |
     * |                                                                                               |
     * |           void updateArchetype0(const entt::entity& _entity, int& _a, float& _b) {            |
     * |                                                                                               |
     * |           }                                                                                   |
     * |   };                                                                                          |
     * |-----------------------------------------------------------------------------------------------|
     * |   // In some scene...                                                                         |
     * |                                                                                               |
     * |   engine->manager.ecsSystem.addSystem(new Test());                                            |
     * |                                                                                               |
     * ------------------------------------------------------------------------------------------------|
     *
     * As you can see in the update method, we create an Archetype of the custom query we want to iterate at. This is an
     * example, but you can store this query so you don't need to create it every frame the update method is called.
     * Once the archetype of the query is ready, we just need to iterate over the components of the query and operate over them.
     */


    typedef int SystemRef;

    /**
     * @brief Interface that EVERY user-defined ECS system MUST follow.
     */
    class ECSSystem {

        friend class ECSManager;

        private:
            /**
             * @brief It sets the order of updating the system.
             */
            int updateOrder = -1;

            /**
             * @brief Reference of the system.
             */
            SystemRef systemRef = -1;

        public:

            /**
             * @brief Called only once, when the system is created.
             */
            virtual void onCreate() {  };

            /**
             * @brief Called every frame, in Graph::update(_dt)
             * @param _dt Delta time
             * @param _graph Scene Graph
             */
            virtual void onUpdate(Delta _dt, Graph* _graph) = 0;

            /**
             * @brief Called only once, when the system is destroyed.
             */
            virtual void onDestroy() {  };


            virtual ~ECSSystem() {  }
    };
}

#endif //RDE_SYSTEM_H
