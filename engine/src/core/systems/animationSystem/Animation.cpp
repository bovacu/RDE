// Created by borja on 9/1/22.


#include "core/systems/animationSystem/Animation.h"
#include <algorithm>

namespace RDE {


    bool Animation::addFrame(Texture* _texture) {
        if(std::find(frameTextures.begin(), frameTextures.end(), _texture) != frameTextures.end()) return false;
        frameTextures.push_back(_texture);
        return true;
    }

    bool Animation::removeFrame(Texture* _texture) {
        std::vector<Texture*>::iterator _it;
        if((_it = std::find(frameTextures.begin(), frameTextures.end(), _texture)) == frameTextures.end()) return false;
        frameTextures.erase(_it);
        return true;
    }

    bool Animation::removeFrame(int _index) {
        if(_index < 0 || _index > frameTextures.size()) return false;
        if(std::find(frameTextures.begin(), frameTextures.end(), frameTextures[_index]) != frameTextures.end()) return false;

        frameTextures.erase(frameTextures.begin() + _index);

        return true;
    }

    void Animation::play() {
        canPlay = true;
    }

    void Animation::pause() {
        canPlay = false;
    }

    void Animation::stop() {
        canPlay = false;
        currentFrame = 0;
        currentTime = 0;
    }

    void Animation::restart() {
        canPlay = true;
        currentFrame = 0;
        currentTime = 0;
    }

    void Animation::update(float _dt) {
        if(!canPlay) return;

        currentTime += _dt;
        if(currentTime >= timeBetweenFrames) {
            currentTime = 0;
            currentFrame++;
            if(currentFrame == frameTextures.size() - 1) currentFrame = 0;
        }
    }

    Texture* Animation::getCurrentFrame() {
        return frameTextures[currentFrame];
    }

    void Animation::setTimeBetweenFrames(float _timeBetweenFrames) {
        timeBetweenFrames = _timeBetweenFrames;
    }

    float Animation::getTimeBetweenFrames() const {
        return timeBetweenFrames;
    }
}
