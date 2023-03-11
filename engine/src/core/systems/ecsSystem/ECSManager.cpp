//
// Created by borja on 29/04/22.
//

#include "core/systems/ecsSystem/ECSManager.h"
#include "entt/entt.hpp"
#include "core/util/Functions.h"

namespace RDE {

    SystemRef ECSManager::addSystem(ECSSystem *_system) {
        _system->systemRef = systemRefCounter++;
        _system->updateOrder = _system->systemRef;
        systems.emplace_back(_system);
        return systemRefCounter;
    }

    void ECSManager::removeSystem(SystemRef _systemRef) {
        uint _order = 0;

        auto _found = std::find_if(systems.begin(), systems.end(), [&](const ECSSystem* _system) {
            if(_system->systemRef == _systemRef) {
                _order = _system->updateOrder;
                return true;
            }
            return false;
        });

        if(_found != systems.end()) {
            systems.erase(_found);
            for(auto& _system : systems)
                if(_system->updateOrder > _order) _system->updateOrder--;
        }
    }

    void ECSManager::update(Delta _dt, Graph* _graph) {
        for(auto& system : systems)
            system->onUpdate(_dt, _graph);
    }

    void ECSManager::destroy() {
        Util::Log::debug("Cleaning up ECSManager");
        for(auto* system : systems)
            delete system;
    }


    void ECSManager::swapOrderOfUpdate(SystemRef _systemRef0, SystemRef _systemRef1) {
        ECSSystem* _system0 = nullptr, *_system1 = nullptr;
        auto _found0 = std::find_if(systems.begin(), systems.end(), [&](ECSSystem* _system) {
            _system0 = _system;
            return _system->systemRef == _systemRef0;
        });

        auto _found1 = std::find_if(systems.begin(), systems.end(), [&](ECSSystem* _system) {
            _system1 = _system;
            return _system->systemRef == _systemRef1;
        });

        if(_found0 == systems.end() || _found1 == systems.end()) throw BadECSSystemSwap();

        uint _system0UpdateOrder = _system0->updateOrder;
        _system0->updateOrder = _system1->updateOrder;
        _system1->updateOrder = _system0UpdateOrder;

        std::sort(systems.begin(), systems.end(), [](const ECSSystem* _systemA, const ECSSystem* _systemB) {
            return _systemA->updateOrder > _systemB->updateOrder;
        });
    }

}