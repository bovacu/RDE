// Created by borja on 9/1/22.


#ifndef ENGINE2_0_ANIMATION_H
#define ENGINE2_0_ANIMATION_H


#include "core/util/Util.h"
#include "core/render/elements/Texture.h"

namespace engine {

    class Animation {
        private:
            std::vector<Texture*> frameTextures;
            int currentFrame = 0;
            float timeBetweenFrames = .1f, currentTime;
            bool canPlay = false;

        public:
            Animation() = default;
            bool addFrame(Texture* _texture);
            bool removeFrame(Texture* _texture);
            bool removeFrame(int _index);

            void play();
            void update(float _dt);
            void stop();
            void restart();

            Texture* getCurrentFrame();
    };

}


#endif //ENGINE2_0_ANIMATION_H
