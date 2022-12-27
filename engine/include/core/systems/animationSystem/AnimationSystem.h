// Created by borja on 9/1/22.


#ifndef RDE_ANIMATION_SYSTEM_H
#define RDE_ANIMATION_SYSTEM_H


#include "core/util/Util.h"
#include "Animation.h"
#include "core/systems/eventSystem/EventBus.h"
#include "core/graph/components/SpriteRenderer.h"

namespace RDE {

    FORWARD_DECLARE_CLASS(Manager)

    FORWARD_DECLARE_STRUCT(AnimationNode)
    struct AnimationTransition {
        std::string transitionCode;
        AnimationNode* nextAnimation;
    };

    struct AnimationNode {
        Animation animation;
        std::vector<AnimationTransition> transitions;
    };

    class TransitionParams {
        // when assigning this, it must be the reference to that variable (&)
        private:
            void* whatEver = nullptr;

        public:
            template<typename T>
            void setValue(T& _value) {
                whatEver = &_value;
            }

            template<typename T>
            T& getValue() {
                ENGINE_ASSERT(whatEver, "There are no transition parameters, but you are trying to get them!")
                return *(T*)whatEver;
            }
    };

    typedef MDelegate<bool(const TransitionParams&)> transitionFunc;

    class AnimationSystem {
        private:
            std::unordered_map<std::string, AnimationNode> animations;
            AnimationNode* currentNode;
            EventBus<std::string, TransitionParams> bus;
            Manager* manager;

        public:
            explicit AnimationSystem(Manager* _manager);
            Animation* createAnimation(const std::string& _animName, const std::string& _atlas, const std::vector<int>& _indices);

            void setAnimationTimeBetweenFrames(const std::string& _animation, float _timeBetweenFrames);
            float getAnimationTimeBetweenFrames(const std::string& _animation);

            void setInitialAnimation(const std::string& _animationName);

            template<auto TransitionFunc, typename Class>
            void createTransition(const std::string& _initialAnimation, const std::string& _finalAnimation, Class* _class);

            AnimationNode* getCurrentAnimation();

            void start();
            void pause();
            void stop();
            void restart();

            void update(float _dt, SpriteRenderer* _spriteRenderer, TransitionParams _params = {});
    };

    template<auto TransitionFunc, typename Class>
    void AnimationSystem::createTransition(const std::string& _initialAnimation, const std::string& _finalAnimation, Class* _class) {
        std::string _transitionCode = _initialAnimation + "_" + _finalAnimation;
        bus.subscribe<TransitionFunc>(_transitionCode, _class);
        AnimationTransition _transition { _transitionCode, &animations[_finalAnimation] };
        animations[_initialAnimation].transitions.push_back(_transition);
    }
}


#endif //RDE_ANIMATION_SYSTEM_H
