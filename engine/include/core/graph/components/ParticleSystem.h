// Created by borja on 31/3/22.


#ifndef RDE_PARTICLE_SYSTEM_H
#define RDE_PARTICLE_SYSTEM_H

#include <vector>
#include "glm/vec3.hpp"
#include "core/util/Delta.h"
#include "core/util/Color.h"
#include "core/render/elements/Vertex.h"
#include "core/util/Pool.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/IRenderizable.h"

namespace RDE {

    struct ParticleSystemConfig;
    class Graph;
    class Manager;

    /**
     * @brief Information about a particle.
     */
    struct ParticleData {
        glm::vec3 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
        Color color;
        float life = -1;

        void reset(const ParticleSystemConfig& _particleSystemConfig, Transform* _parentTransform);
    };

    /**
     * @brief Config for color gradient. TODO: improve this to be able to have 'n' colors and traverse through them.
     */
    struct ParticleSystemColorGradientConfig {
        Color initColor = Color::White;
        Color endColor = Color::NO_COLOR;
    };

    /**
     * @brief Config for function callbacks.
     */
    struct ParticleSystemCallbacksConfig {
        UniqueDelegate<void(ParticleData&, Delta, const ParticleSystemConfig&)> effectFunction {};
        UniqueDelegate<Color(ParticleData&, Delta, const ParticleSystemConfig&)> colorInterpolationFunction {};
    };

    /**
     * @brief Config with data related to each particle state.
     */
    struct ParticleSystemDataConfig {
        float lifeTime = -1;
        glm::vec2 initialVelocity;
        int numberOfParticles;
        Texture* texture = nullptr;
        float timeToCreateNewParticleMs = 100;
        bool loop = true;
        ShaderID shader = 0;
    };

    /**
     * @brief Initial configuration of the particle system.
     */
    struct ParticleSystemConfig {
        ParticleSystemColorGradientConfig colorGradientConfig;
        ParticleSystemCallbacksConfig callbacksConfig;
        ParticleSystemDataConfig dataConfig;
    };

    /**
     * @brief Component that emits particles in a customizable way.
     */
    class ParticleSystem {
        private:
            Pool<ParticleData> pool;
            std::vector<ParticleData> usedParticles {};
            bool isPlaying = false;

        public:
            ParticleSystemConfig particleSystemConfig;

        RENDERIZABLE_BASIC_PROPERTIES()

        public:
			RDE_FUNC ParticleSystem(Node* _node, Scene* _scene, const ParticleSystemConfig& _particleSystemConfig);
			RDE_FUNC ParticleSystem(Node* _node, Manager* _manager, Graph* _graph, const ParticleSystemConfig& _particleSystemConfig);
			RDE_FUNC void update(Delta dt);


            RENDERIZABLE_BASIC_METHODS()

            /**
             * @brief Starts the particle system. By default the particle system won't start until this method is called.
             */
			RDE_FUNC void play();

            /**
             * @brief Pauses the emission of particles, but the current emitted particles will continue their life cycle.
             */
			RDE_FUNC void pause();

            /**
             * @brief Pauses the emission of particles, removing all particles and resetting the particle system to config values.
             */
			RDE_FUNC void stop();

            /**
             * @brief Resets the particle system to config values, but does not stop the emission.
             */
			RDE_FUNC void reset();

			RDE_FUNC void drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform* _transform, const ViewPort* _viewport);

        private:
            /**
             * @brief Allocator for ParticleData, needed by the pool system.
             * @return ParticleData
             */
            ParticleData allocator();

            /**
             * @brief Default effect to for the particles, it is like a smokey fire.
             * @param _particleData ParticleData to modify.
             * @param _dt Delta time.
             * @param _config Initial configuration of the system.
             */
            void defaultEffect(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config);

            /**
             * @brief Default color interpolation function.
             * @param _particleData ParticleData to modify.
             * @param _dt Delta time.
             * @param _config Initial configuration of the system.
             * @returns Color
             */
            Color defaultColorInterpolationFunction(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config);
    };

}

#endif //RDE_PARTICLE_SYSTEM_H
