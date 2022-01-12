// Created by borja on 10/1/22.


#ifndef ENGINE2_0_SOUND_BUFFER_H
#define ENGINE2_0_SOUND_BUFFER_H


#include "core/util/Util.h"
#include <AL/al.h>

namespace engine {

    class SoundBuffer {
        private:
            std::vector<ALuint> soundEffectBuffers;

        public:
            static SoundBuffer* get();

            ALuint addSoundEffect(const char* _filename);
            bool removeSoundEffect(const ALuint& _buffer);

        private:
            SoundBuffer();
            ~SoundBuffer();
    };

}


#endif //ENGINE2_0_SOUND_BUFFER_H
