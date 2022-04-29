// Created by borja on 31/3/22.

#include "core/systems/particleSystem/ParticleSystem.h"
#include "core/graph/Components.h"

namespace GDE {

    void ParticleSystem::update(Delta dt) {
//        for (auto it = particles.begin(); it != particles.end(); ++it) {
//            do {
//                it->life -= dt;
//                if (it->life < 0) {
//                    //particle died, remove from list;
//                    std::swap(*it, particles.back());
//                    particles.pop_back();
//                    if (it == particles.end())return;
//                }
//            } while (it->life < 0);
//
//            //update pos and vel of it*
//            it->pos = {it->pos.x + it->vel * dt, it->pos.y + it->vel * dt};
//            it->speed = {it->speed.x + it->accel * dt, it->speed.y + it->accel * dt};
//        }
    }

    void ParticleSystem::init(Graph* _graph, int _numberOfParticles, const NodeID& _nodeID) {
//        ID = _nodeID;
//
//        auto* _transform = _graph->getComponent<Transform>(ID);
//
//        for(int _i = 0; _i < _numberOfParticles; _i++)
//            particles.emplace_back(ParticleData { _transform->getPositionLocal(), Vec2F{}, Color::White, 1, 0, 1, 5, 50, 50 });
    }

    int ParticleSystem::findUnusedParticle() {
//        for(int i=LastUsedParticle; i < MaxParticles; i++){
//            if (particles[i].life < 0){
//                LastUsedParticle = i;
//                return i;
//            }
//        }
//
//        for(int i=0; i<LastUsedParticle; i++){
//            if (particles[i].life < 0){
//                LastUsedParticle = i;
//                return i;
//            }
//        }

        return 0;
    }
}
