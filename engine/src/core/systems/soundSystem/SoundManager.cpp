// Created by borja on 30/1/22.


#include "core/platform/PlatformHeaderSDL.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/util/Functions.h"

namespace RDE {

    void SoundManager::init() {
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            Util::Log::error("Mix_OpenAudio Error: ", Mix_GetError());
            return;
        }

        int _flags = MIX_INIT_OGG;
        
        #if !IS_IOS()
        _flags |= MIX_INIT_MP3;
        #endif
        
        int _initiated = Mix_Init(_flags);

        if((_initiated & _flags) != _flags) {
            Util::Log::error("Mix_Init Error: ", Mix_GetError());
            return;
        }

        Util::Log::debug("Sound Manager loaded successfully!");
    }

    Music& SoundManager::loadMusic(const std::string& _musicPath) {
        Music _m;
        _m.musicID = Mix_LoadMUS(_musicPath.c_str());

        if(_m.musicID == nullptr) {
            throw std::runtime_error(Util::String::appendToString("Couldn't load Music with path ", _musicPath));
        }

        _m.name = Util::String::getFileNameFromPath(_musicPath);
        musics[_m.name] = _m;
        Util::Log::debug("Successfully loaded ", _m.name);
        return musics[_m.name];
    }

    Sfx& SoundManager::loadSfx(const std::string& _sfxPath) {
        Sfx _sfx;

        #if !IS_MOBILE()
        _sfx.sfxID = Mix_LoadWAV(_sfxPath.c_str());
        #else
        // 1 on Mix_LoadWAV_RW is for the function to close the SDL_RWops automatically, whether it fails or success.
        _sfx.sfxID = Mix_LoadWAV_RW(SDL_RWFromFile(_sfxPath.c_str(), "rb"), 1);
        #endif

        if(_sfx.sfxID == nullptr) {
            throw std::runtime_error(Util::String::appendToString("Couldn't load SFX with path ", _sfxPath));
        }

        _sfx.name = Util::String::getFileNameFromPath(_sfxPath);
        sfxs[_sfx.name] = _sfx;
        Util::Log::debug("Loaded sfx: ", _sfx.name);
        return sfxs[_sfx.name];
    }

    void SoundManager::unloadMusic(const std::string& _musicName) {
        if(musics.find(_musicName) != musics.end()) {
            Mix_FreeMusic(musics[_musicName].musicID);
            musics.erase(_musicName);
        } else {
            Util::Log::error("Tried to unload Music '", _musicName, ", but it was not loaded in memory!");
        }
    }

    void SoundManager::unloadSfx(const std::string& _sfxName) {
        if(sfxs.find(_sfxName) != sfxs.end()) {
            Mix_FreeChunk(sfxs[_sfxName].sfxID);
            sfxs.erase(_sfxName);
        } else {
            Util::Log::error("Tried to unload SFX '", _sfxName, ", but it was not loaded in memory!");
        }
    }

    Music& SoundManager::getMusic(const std::string& _musicName) {
        if(musics.find(_musicName) == musics.end()) {
            throw std::runtime_error(Util::String::appendToString("Couldn't get Music '", _musicName, "'"));
        }
        return musics[_musicName];
    }

    Sfx& SoundManager::getSfx(const std::string& _sfxName) {
        if(sfxs.find(_sfxName) == sfxs.end()) {
            throw std::runtime_error(Util::String::appendToString("Couldn't get SFX '", _sfxName, "'"));
        }
        return sfxs[_sfxName];
    }

    void SoundManager::playMusic(const std::string& _musicName) {
        if(musics.find(_musicName) == musics.end()) {
            throw std::runtime_error(Util::String::appendToString("Couldn't get Music '", _musicName, "'"));
        }

        Music _m = musics[_musicName];
        Mix_PlayMusic(_m.musicID, _m.repeat);
    }

    void SoundManager::playSfx(const std::string& _sfxName) {
        if(sfxs.find(_sfxName) == sfxs.end()) {
            throw std::runtime_error(Util::String::appendToString("Couldn't get SFX '", _sfxName, "'"));
        }

        Sfx _sfx = sfxs[_sfxName];

        #if !IS_MOBILE()
        sfxs[_sfxName].channel = Mix_PlayChannel(sfxs[_sfxName].channel, _sfx.sfxID, _sfx.repeat);
        #else
        sfxs[_sfxName].channel = Mix_PlayChannelTimed(sfxs[_sfxName].channel, _sfx.sfxID, _sfx.repeat, -1);
        #endif

        if(sfxs[_sfxName].channel == -1) {
            Util::Log::warn("All of the SFX available channels are in use! SFX may not play correctly");
        }
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
        Util::Log::debug("Cleaning up SoundManager");
        stopAll();

        for(auto& _music : musics) {
            Mix_FreeMusic(_music.second.musicID);
        }

        for(auto& _sfx : sfxs) {
            Mix_FreeChunk(_sfx.second.sfxID);
        }

        Mix_CloseAudio();
        Mix_Quit();
    }
}
