// Created by borja on 10/1/22.


#include "SoundSystem.h"
#include "core/util/Functions.h"

namespace engine {

    void SoundSystem::init() {
        device = irrklang::createIrrKlangDevice();
        if(!device) {
            LOG_E("Couldn't start sound system")
            return;
        }

    }

    SoundSystem& SoundSystem::get() {
        static SoundSystem instance;
        return instance;
    }

    void SoundSystem::loadSound(const std::string& _soundPath, bool _preload) {
        auto* _ss = device->addSoundSourceFromFile(_soundPath.c_str(), irrklang::ESM_AUTO_DETECT, _preload);
        sounds[Util::getFileNameFromPath(_soundPath)] = _ss;
    }

    irrklang::ISound* SoundSystem::play(const std::string& _sound, bool _loop) {
        auto _s = device->play2D(sounds[_sound], _loop, false, true);
        _s->setIsPaused(false);
        return _s;
    }

    void SoundSystem::pause(irrklang::ISound* _sound) {
        _sound->stop();
    }

    void SoundSystem::stop(irrklang::ISound* _sound) {
        _sound->stop();
        _sound->setPosition({0, 0, 0});
    }

    void SoundSystem::restart(irrklang::ISound* _sound) {
        _sound->setPosition({0, 0, 0});
    }

    void SoundSystem::stopAll() {
        device->stopAllSounds();
    }

    void SoundSystem::stopAll(const std::string& _sound) {
        device->stopAllSoundsOfSoundSource(sounds[_sound]);
    }

    void SoundSystem::unloadSound(const std::string& _sound) {
        stopAll(_sound);
        sounds[_sound]->drop();
        sounds.erase(_sound);
    }

    void SoundSystem::unloadAllSounds() {
        for(auto& _sound : sounds)
            _sound.second->drop();
    }

    void SoundSystem::clean() {
        if(device != nullptr) {
            stopAll();
            device->drop();
            LOG_S("Cleaning up Sound System")
        }
    }
}