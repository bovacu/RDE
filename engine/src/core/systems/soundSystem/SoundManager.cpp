// Created by borja on 30/1/22.


#include "core/platform/PlatformHeaderSDL.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/util/Functions.h"

namespace GDE {

    void SoundManager::init() {
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            LOG_E("Mix_OpenAudio Error: ", Mix_GetError())
            return;
        }

        int _flags = MIX_INIT_OGG | MIX_INIT_MP3;
        int _initiated = Mix_Init(_flags);

        if((_initiated & _flags) != _flags) {
            LOG_E("Mix_Init Error: ", Mix_GetError())
            return;
        }

        LOG_S("Sound Manager initiated!")
    }

    Music& SoundManager::loadMusic(const std::string& _musicPath) {
        Music _m;
        _m.musicID = Mix_LoadMUS(_musicPath.c_str());
        _m.name = Util::getFileNameFromPath(_musicPath);
        musics[_m.name] = _m;
        LOG_S("Successfully loaded ", _m.name)
        return musics[_m.name];
    }

    Sfx& SoundManager::loadSfx(const std::string& _sfxPath) {
        Sfx _sfx;
        _sfx.sfxID = Mix_LoadWAV(_sfxPath.c_str());
        _sfx.name = Util::getFileNameFromPath(_sfxPath);
        sfxs[_sfx.name] = _sfx;
        return sfxs[_sfx.name];
    }

    void SoundManager::removeMusic(const std::string& _musicName) {
        if(musics.find(_musicName) != musics.end())
            musics.erase(_musicName);
    }

    void SoundManager::removeSfx(const std::string& _sfxName) {
        if(sfxs.find(_sfxName) != sfxs.end())
            sfxs.erase(_sfxName);
    }

    Music& SoundManager::getMusic(const std::string& _musicName) {
        return musics[_musicName];
    }

    Sfx& SoundManager::getSfx(const std::string& _sfxName) {
        return sfxs[_sfxName];
    }

    void SoundManager::playMusic(const std::string& _musicName) {
        Music _m = musics[_musicName];
        Mix_PlayMusic(_m.musicID, _m.repeat);
    }

    void SoundManager::playSfx(const std::string& _sfxName) {
        Sfx _sfx = sfxs[_sfxName];
        // -1 on the channel for the first free channel
        sfxs[_sfxName].channel = Mix_PlayChannel(-1, _sfx.sfxID, _sfx.repeat);
    }

    void SoundManager::resumeMusic(const std::string& _musicName) {
        Mix_ResumeMusic();
    }

    void SoundManager::resumeSfx(const std::string& _sfxName) {
        Mix_Resume(sfxs[_sfxName].channel);
    }

    void SoundManager::pauseMusic(const std::string& _musicName) {
        Mix_PauseMusic();
    }

    void SoundManager::pauseSfx(const std::string& _sfxName) {
        Mix_Pause(sfxs[_sfxName].channel);
    }

    void SoundManager::stopMusic(const std::string& _musicName) {
        Mix_HaltMusic();
    }

    void SoundManager::stopSfx(const std::string& _sfxName) {
        Mix_HaltChannel(sfxs[_sfxName].channel);
    }

    void SoundManager::pauseAll() {
        Mix_PauseMusic();
        Mix_Pause(-1);
    }

    void SoundManager::stopAll() {
        Mix_HaltChannel(-1);
        Mix_HaltMusic();
    }

    void SoundManager::resumeAll() {
        Mix_Resume(-1);
        Mix_ResumeMusic();
    }

    void SoundManager::setMusicVolume(int _volume) {
        Mix_VolumeMusic(_volume);
    }

    void SoundManager::setSfxVolume(int _volume) {
        Mix_Volume(-1, _volume);
    }

    void SoundManager::destroy() {
        LOG_S("Cleaning up Sound Manager")
        stopAll();

        for(auto& _music : musics) {
            LOG_S("     Deleting music: ", _music.first)
            Mix_FreeMusic(_music.second.musicID);
        }

        for(auto& _sfx : sfxs) {
            LOG_S("     Deleting sfx: ", _sfx.first)
            Mix_FreeChunk(_sfx.second.sfxID);
        }

        Mix_CloseAudio();
        Mix_Quit();
    }
}