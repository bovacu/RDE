// Created by borja on 30/1/22.


#ifndef RDE_SOUND_MANAGER_H
#define RDE_SOUND_MANAGER_H

#include "core/platform/PlatformHeaderSDLMixer.h"
#include <unordered_map>

namespace RDE {

    class SoundManager;

    /**
     * @brief Base class for different types of sounds.
     */
    struct Sound {
        public:
            /**
             * @brief Where the file is located.
             */
            std::string path;

            /**
             * @brief File name.
             */
            std::string name;

            /**
             * @brief If the sound should be played in loop mode.
             */
            int repeat = 0;

            /**
             * @brief Sound speed.
             */
            float speed = 1;
    };

    /**
     * @brief Background music.
     */
    struct Music : Sound {
        friend class SoundManager;
        private:
            /**
             * @brief SDL music.
             */
            Mix_Music* musicID = nullptr;
    };

    /**
     * @brief Sound effects.
     */
    struct Sfx : Sound {
        friend class SoundManager;
        public:
            /**
             * @brief Channel the sound is being played.
             */
            int channel = -1;
        private:
            /**
             * @brief SDL sound effect.
             */
            Mix_Chunk* sfxID = nullptr;
    };

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class is in charge of playing different sound effects and background musics.
     */
    class SoundManager {
        private:
            /**
             * @brief Map MusicName -> Music
             */
            std::unordered_map<std::string, Music> musics;

            /**
             * @brief Map SfxName -> Sfx
             */
            std::unordered_map<std::string, Sfx> sfxs;

            /**
             * @brief Current playing sound effect.
             */
            Sfx* currentSfx = nullptr;

            /**
             * @brief Current playing background music.
             */
            Music* currentMusic = nullptr;

        public:
            SoundManager(SoundManager const&) = delete;
            void operator=(SoundManager const&)  = delete;

            /**
             * @attention This function should not be called by end users.
             * @brief Initiates the sound system.
             */
			RDE_FUNC void init();

            /**
             * @attention This function should not be called by end users.
             * @brief Destroys the sound system and all loaded sounds.
             */
			RDE_FUNC void destroy();

            /**
             * @brief Loads a background music.
             * @param _musicPath File path
             * @return Music&
             */
			RDE_FUNC Music& loadMusic(const std::string& _musicPath);

            /**
             * @brief Loads a sound effect.
             * @param _sfxPath File path
             * @return Sfx&
             */
			RDE_FUNC Sfx& loadSfx(const std::string& _sfxPath);

            /**
             * @brief Unloads a background music.
             * @param _musicName Music name
             */
			RDE_FUNC void unloadMusic(const std::string& _musicName);

            /**
             * @brief Unloads a sound effect.
             * @param _sfxName Sound effect name
             */
			RDE_FUNC void unloadSfx(const std::string& _sfxName);

            /**
             * @brief Gets a background music.
             * @param _musicName Music name
             * @return Music&
             */
			RDE_FUNC_ND Music& getMusic(const std::string& _musicName);

            /**
             * @brief Gets a sound effect.
             * @param _sfxName Sound effect name
             * @return Sfx&
             */
			RDE_FUNC_ND Sfx& getSfx(const std::string& _sfxName);

            /**
             * @brief Plays a background music, only one can be played at a time.
             * @param _musicName Music name
             */
			RDE_FUNC void playMusic(const std::string& _musicName);

            /**
             * @brief Plays a sound effect, multiple sounds effects can be played at a time.
             * @param _sfxName Sound effect name.
             */
			RDE_FUNC void playSfx(const std::string& _sfxName);

            /**
             * @brief Pauses a background music.
             * @param _musicName Music name
             */
			RDE_FUNC void pauseMusic(const std::string& _musicName);

            /**
             * @brief Pauses a sound effect.
             * @param _sfxName Sound effect name
             */
			RDE_FUNC void pauseSfx(const std::string& _sfxName);

            /**
             * @brief Continues a paused background music.
             * @param _musicName Music name
             */
			RDE_FUNC void resumeMusic(const std::string& _musicName);

            /**
             * @brief Continues a paused sound effect.
             * @param _sfxName Sound effect name
             */
			RDE_FUNC void resumeSfx(const std::string& _sfxName);

            /**
             * @brief Stops and sets to its beginning a background music.
             * @param _musicName Music name
             */
			RDE_FUNC void stopMusic(const std::string& _musicName);

            /**
             * @brief Stops and sets to its beginning a sound effect.
             * @param _sfxName Sound effect name
             */
			RDE_FUNC void stopSfx(const std::string& _sfxName);

            /**
             * @brief Sets the volume of a background music.
             * @param _volume Volume value
             */
			RDE_FUNC void setMusicVolume(int _volume);

            /**
             * @brief Sets the volume of a sound effect.
             * @param _volume Volume value
             */
			RDE_FUNC void setSfxVolume(int _volume);

            /**
             * @brief Pauses all of the sounds.
             */
			RDE_FUNC void pauseAll();

            /**
             * @brief Stops and sets to its beginning all of the sounds.
             */
			RDE_FUNC void stopAll();

            /**
             * @brief Continues all of the paused sounds.
             */
			RDE_FUNC void resumeAll();

			RDE_FUNC_ND SoundManager() {}
    };

}


#endif //RDE_SOUND_MANAGER_H
