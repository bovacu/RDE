//
// Created by borja on 22/04/22.
//

#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "core/util/Util.h"
#include "chipmunk/chipmunk.h"

namespace GDE {

    typedef cpSpace* PhysicsSpace;

    struct Body;
    class Physics {
        private:
            int maskCount = 0;

            Vec2F gravity { 0, -90.8f };
            bool active = true;

            PhysicsSpace physicsSpace = nullptr;
            std::unordered_map<std::string, int> registeredMasks;

        private:
            Physics() = default;

        public:
            static Physics& get();

            void init();
            void step(Delta _delta);

            PhysicsSpace getSpace();

            void setGravity(const Vec2F& _gravity);
            Vec2F getGravity();

            void setPhysicsActive(float _active);
            bool isPhysicsActive();

            void registerCollisionMask(const std::string& _maskName);
            void removeCollisionMask(const std::string& _maskName);
            int getCollisionMask(const std::string& _maskName);

            void createCollisionFilter(Body&, const std::vector<std::string>& _otherMasks);

            void registerOnCollisionStartCallback(const std::string& _mask0, const std::string& _mask1, bool (*_onCollisionStarCallback)(cpArbiter *arb, cpSpace *space, void *data));
            void removeOnCollisionStartCallback(const std::string& _mask0, const std::string& _mask1);

            void registerOnCollisionEndCallback(const std::string& _mask0, const std::string& _mask1, UDelegate<void()> _onCollisionEndCallback);
            void removeOnCollisionEndCallback(const std::string& _mask0, const std::string& _mask1);

            void registerOnCollidingCallback(const std::string& _mask0, const std::string& _mask1, UDelegate<void()> _onCollidingCallback);
            void removeOnCollidingCallback(const std::string& _mask0, const std::string& _mask1);

            void destroy();
    };

}

#endif //ENGINE_PHYSICS_H
