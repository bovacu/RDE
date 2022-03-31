// Created by borja on 31/3/22.


#ifndef ENGINE_PARTICLE_SYSTEM_H
#define ENGINE_PARTICLE_SYSTEM_H

#include <vector>
#include "glm/vec3.hpp"
#include "core/util/Delta.h"
#include "core/util/Color.h"
#include "core/graph/Graph.h"

namespace engine {

    struct ParticleData {
        Vec2F pos, speed;
        Color color; // Color
        float size, angle, weight;
        float life, vel, accel;
    };

    class ParticleSystem {
        private:
            const int MaxParticles = 100;
            ParticleData particles[100];
            int LastUsedParticle = 0;
            NodeID ID = NODE_ID_NULL;

        private:
            int findUnusedParticle();

        public:
            void init(Graph* _graph, int _numberOfParticles, const NodeID& _nodeID);
            void update(Delta dt);
    };

}

#endif //ENGINE_PARTICLE_SYSTEM_H
