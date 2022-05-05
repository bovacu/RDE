//
// Created by borja on 22/04/22.
//

#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include "core/util/Util.h"
#include "box2d/box2d.h"

namespace GDE {

    class Physics {
        public:
            Vec2F gravity { 0, -90.8f };
            bool active = true;
            b2World* world = nullptr;

        private:
            Physics() = default;

        public:
            static Physics& get();

            void init();
            void step(Delta _delta);

            void setGravity(const Vec2F& _gravity);
            Vec2F getGravity();

            void setPhysicsActive(bool _active);
            [[nodiscard]] bool isPhysicsActive() const;

            void destroy();
    };

}

#endif //ENGINE_PHYSICS_H
