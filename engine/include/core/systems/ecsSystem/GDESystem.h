//
// Created by borja on 29/04/22.
//

#ifndef ENGINE_GDE_SYSTEM_H
#define ENGINE_GDE_SYSTEM_H

#include "core/util/Util.h"
#include "core/graph/Graph.h"
#include <type_traits>

namespace GDE {

    typedef unsigned int SystemRef;

    class GDESystem {

        friend class GDESystemManager;

        private:
            uint updateOrder = -1;
            SystemRef systemRef = -1;

        public:

            virtual void onCreate() {  };
            virtual void onUpdate(Delta _dt, Graph* _graph) = 0;
            virtual void onDestroy() {  };
            virtual ~GDESystem() {  }
    };

//    EXAMPLE OF USE
//    class Test : public GDESystem {
//
//        public:
//            void onUpdate(Delta _dt, Graph* _graph) override {
//                auto _query0 = _graph->query<int, float>();
//                for(auto&& [entity, pos, vel]: _query0) updateArchetype0(entity, pos, vel);
//            }
//
//            void updateArchetype0(const entt::entity& _entity, int& _a, float& _b) {
//
//            }
//    };
//
//    GDESystemManager::get().addSystem(new Test());
}

#endif //ENGINE_GDE_SYSTEM_H
