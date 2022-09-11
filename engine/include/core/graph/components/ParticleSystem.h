// Created by borja on 31/3/22.


#ifndef ENGINE_PARTICLE_SYSTEM_H
#define ENGINE_PARTICLE_SYSTEM_H

#include <vector>
#include "glm/vec3.hpp"
#include "core/util/Delta.h"
#include "core/util/Color.h"
#include "core/graph/Graph.h"
#include "core/render/elements/Vertex.h"
#include "core/util/Pool.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/IRenderizable.h"

namespace GDE {

    struct ParticleSystemConfig;

    struct ParticleData {
        glm::vec3 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
        Color color;
        float life = -1;

        void reset(const ParticleSystemConfig& _particleSystemConfig, Transform* _parentTransform);
    };

    struct ParticleSystemConfig {
        Color initColor {};
        Color endColor {};
        float lifeTime = -1;
        glm::vec2 initialVelocity;
        float velocity = 50;
        float acceleration = 1;
        int numberOfParticles;
        UDelegate<void(ParticleData&, Delta, const ParticleSystemConfig&)> effectFunction {};
        UDelegate<Color(ParticleData&, Delta, const ParticleSystemConfig&)> colorInterpolationFunction {};
        Texture* texture = nullptr;
        float timeToCreateNewParticleMs = 100;
        bool loop = true;
    };

    class ParticleSystem : public IRenderizable {
        private:
            Pool<ParticleData> pool;
            std::vector<ParticleData> usedParticles {};

        public:
            Transform* transform;
            ParticleSystemConfig particleSystemConfig;

        public:
            ParticleSystem(const NodeID& _nodeID, Scene* _scene, const ParticleSystemConfig& _particleSystemConfig);
            void update(Delta dt);

            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return particleSystemConfig.texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2I getSize() const override { return particleSystemConfig.texture->getSize(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] IntRect getRegion() const override { return particleSystemConfig.texture->getRegion(); }

            /**
             * @see IRenderizable
             */
            void draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const override;

        private:
            ParticleData allocator();
            void defaultEffect(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config);
            Color defaultColorInterpolationFunction(ParticleData& _particleData, Delta _dt, const ParticleSystemConfig& _config);
    };

}

#endif //ENGINE_PARTICLE_SYSTEM_H
