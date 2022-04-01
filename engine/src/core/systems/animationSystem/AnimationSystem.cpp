// Created by borja on 9/1/22.

#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/render/elements/TextureAtlasManager.h"

namespace engine {

    Animation* AnimationSystem::createAnimation(const std::string& _animName, const std::string& _atlas, const std::vector<int>& _indices) {
        Animation _animation;

        for(auto& _index : _indices) {
            _animation.addFrame(TextureAtlasManager::get().getTile(_atlas, _atlas + "_" + std::to_string(_index)));
        }

        animations[_animName] = AnimationNode { _animation, {  } };
        return &animations[_animName].animation;
    }

    void AnimationSystem::setInitialAnimation(const std::string& _animationName) {
        currentNode = &animations[_animationName];
    }

    void AnimationSystem::setAnimationTimeBetweenFrames(const std::string& _animation, float _timeBetweenFrames) {
        if(_timeBetweenFrames <= 0) return;
        animations[_animation].animation.setTimeBetweenFrames(_timeBetweenFrames);
    }

    float AnimationSystem::getAnimationTimeBetweenFrames(const std::string& _animation) {
        return animations[_animation].animation.getTimeBetweenFrames();
    }

    void AnimationSystem::start() {
        currentNode->animation.play();
    }

    void AnimationSystem::pause() {
        currentNode->animation.pause();
    }

    void AnimationSystem::stop() {
        currentNode->animation.stop();
    }

    void AnimationSystem::restart() {
        currentNode->animation.restart();
    }

    void AnimationSystem::update(float _dt, SpriteRenderer& _spriteRenderer, TransitionParams _params) {
        AnimationNode* _nextAnimation = nullptr;
        for(auto& _transition : currentNode->transitions) {
            if(bus.dispatch(_transition.transitionCode, _params)) {
                _nextAnimation = _transition.nextAnimation;
                currentNode->animation.stop();
                _nextAnimation->animation.play();
            }
        }

        if(_nextAnimation != nullptr) currentNode = _nextAnimation;
        currentNode->animation.update(_dt);

        if(_spriteRenderer.texture != getCurrentAnimation()->animation.getCurrentFrame())
            _spriteRenderer.texture = getCurrentAnimation()->animation.getCurrentFrame();
    }

    AnimationNode* AnimationSystem::getCurrentAnimation() {
        return currentNode;
    }
}