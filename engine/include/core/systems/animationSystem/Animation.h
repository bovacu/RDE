// Created by borja on 9/1/22.


#ifndef ENGINE2_0_ANIMATION_H
#define ENGINE2_0_ANIMATION_H


#include "core/util/Util.h"
#include "core/render/elements/Texture.h"

namespace GDE {

    class Animation {
        private:
            std::vector<Texture*> frameTextures;
            int currentFrame = 0;
            float timeBetweenFrames = .1f, currentTime = 0;
            bool canPlay = false;

        public:
            Animation() {}
            bool addFrame(Texture* _texture);
            bool removeFrame(Texture* _texture);
            bool removeFrame(int _index);

            void setTimeBetweenFrames(float _timeBetweenFrames);
            float getTimeBetweenFrames() const;

            void play();
            void pause();
            void stop();
            void restart();

            void update(float _dt);
            Texture* getCurrentFrame();
    };

}


#endif //ENGINE2_0_ANIMATION_H
