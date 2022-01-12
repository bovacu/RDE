// Created by borja on 10/1/22.


#ifndef ENGINE2_0_SOUND_SYSTEM_H
#define ENGINE2_0_SOUND_SYSTEM_H


#include "core/util/Util.h"
#include "external/irrKlang/include/irrKlang.h"

namespace engine {

    // TODO check why irrklang creates memory leaks
    class SoundSystem {
        private:
            irrklang::ISoundEngine* device = nullptr;
            std::unordered_map<std::string, irrklang::ISoundSource*> sounds;
            std::unordered_map<std::string, irrklang::ISoundSource*> playingSounds;

        private:
            SoundSystem() = default;

        public:
            static SoundSystem& get();
            void init();

            void loadSound(const std::string& _soundPath, bool _preload = false);
            void unloadSound(const std::string& _sound);
            void unloadAllSounds();
            [[nodiscard]] irrklang::ISound* play(const std::string& _sound, bool _loop = false);
            void pause(irrklang::ISound* _sound);
            void stop(irrklang::ISound* _sound);
            void restart(irrklang::ISound* _sound);
            void stopAll();
            void stopAll(const std::string& _sound);

            void clean();
    };

}


#endif //ENGINE2_0_SOUND_SYSTEM_H
