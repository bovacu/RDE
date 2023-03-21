// Created by borja on 9/1/22.


#ifndef RDE_ANIMATION_H
#define RDE_ANIMATION_H


#include <vector>
#include "core/render/elements/Texture.h"

namespace RDE {

    class Animation {
        private:
            std::vector<Texture*> frameTextures;
            int currentFrame = 0;
            float timeBetweenFrames = .1f, currentTime = 0;
            bool canPlay = false;

        public:
			RDE_FUNC Animation() {}
			RDE_FUNC bool addFrame(Texture* _texture);
			RDE_FUNC bool removeFrame(Texture* _texture);
			RDE_FUNC bool removeFrame(int _index);

			RDE_FUNC void setTimeBetweenFrames(float _timeBetweenFrames);
			RDE_FUNC_ND float getTimeBetweenFrames() const;

			RDE_FUNC void play();
			RDE_FUNC void pause();
			RDE_FUNC void stop();
			RDE_FUNC void restart();

			RDE_FUNC void update(float _dt);
			RDE_FUNC_ND Texture* getCurrentFrame();
    };

}


#endif //RDE_ANIMATION_H
