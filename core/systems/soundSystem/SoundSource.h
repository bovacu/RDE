// Created by borja on 10/1/22.


#ifndef ENGINE2_0_SOUND_SOURCE_H
#define ENGINE2_0_SOUND_SOURCE_H


#include <AL/al.h>
#include "core/util/Util.h"
#include <thread>

namespace engine {

    class SoundSource {
        public:
            SoundSource();
            ~SoundSource();

            void Play(ALuint buffer_to_play);
            void init();

        private:
            ALuint p_Source;
            float p_Pitch = 1.f;
            float p_Gain = 1.f;
            float p_Position[3] = { 0,0,0 };
            float p_Velocity[3] = { 0,0,0 };
            bool p_LoopSound = false;
            ALuint p_Buffer = 0;

            void asyncPlay(ALuint _buffer);
            std::thread playThread;
    };

}


#endif //ENGINE2_0_SOUND_SOURCE_H
