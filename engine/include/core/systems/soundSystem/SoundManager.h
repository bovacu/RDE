// Created by borja on 30/1/22.


#ifndef ENGINE2_0_SOUND_MANAGER_H
#define ENGINE2_0_SOUND_MANAGER_H


#include "core/util/Util.h"
#include "core/platform/PlatformHeaderSDLMixer.h"

namespace engine {

    class SoundManager;

    struct Sound {
        std::string path, name;
        int repeat = 0;
        float speed = 1;
    };

    struct Music : Sound {
        private:
            friend class SoundManager;
            Mix_Music* musicID = nullptr;
    };

    struct Sfx : Sound {
        int channel;
        private:
            friend class SoundManager;
            Mix_Chunk* sfxID = nullptr;
    };

    class SoundManager {
        private:
            std::unordered_map<std::string, Music> musics;
            std::unordered_map<std::string, Sfx> sfxs;
            Sfx* currentSfx = nullptr;
            Music* currentMusic = nullptr;

        private:
            SoundManager() = default;

        public:
            SoundManager(SoundManager const&) = delete;
            void operator=(SoundManager const&)  = delete;

            static SoundManager& get();

            void init();
            void destroy();

            Music& loadMusic(const std::string& _musicPath);
            Sfx& loadSfx(const std::string& _sfxPath);

            void removeMusic(const std::string& _musicName);
            void removeSfx(const std::string& _sfxName);

            Music& getMusic(const std::string& _musicName);
            Sfx& getSfx(const std::string& _sfxName);

            void playMusic(const std::string& _musicName);
            void playSfx(const std::string& _sfxName);

            void resumeMusic(const std::string& _musicName);
            void resumeSfx(const std::string& _sfxName);

            void pauseMusic(const std::string& _musicName);
            void pauseSfx(const std::string& _sfxName);

            void stopMusic(const std::string& _musicName);
            void stopSfx(const std::string& _sfxName);

            void setMusicVolume(int _volume);
            void setSfxVolume(int _volume);

            void pauseAll();
            void stopAll();
            void resumeAll();
    };

}


#endif //ENGINE2_0_SOUND_MANAGER_H
