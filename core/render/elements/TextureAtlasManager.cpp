// Created by borja on 6/1/22.

#include "TextureAtlasManager.h"
#include "core/util/Functions.h"

namespace engine {

    TextureAtlasManager TextureAtlasManager::instance;

    bool TextureAtlasManager::addAtlas(int _tileWidth, int _tileHeight, const std::string& _pathToTexture) {
        auto _name = Util::getFileNameFromPath(_pathToTexture);
        LOG_I_TIME("Trying to load '", _pathToTexture, "'...")
        if(instance.atlases.find(_name) != instance.atlases.end()) {
            LOG_E_TIME("Atlas '", _name, "' was already loaded");
            return false;
        }

        auto* _atlas = new Atlas;
        if(!_atlas->texture->loadFromFile(_pathToTexture.c_str())) {
            LOG_E_TIME("Atlas '", _pathToTexture, "' could not be loaded");
            return false;
        }

        _atlas->name = _name;
        _atlas->tileWidth = _tileWidth;
        _atlas->tileHeight = _tileHeight;
        _atlas->textureWidth = _atlas->texture->getSize().x;
        _atlas->textureHeight = _atlas->texture->getSize().y;

        cropTextures(*_atlas);
        instance.atlases[_name] = _atlas;
        LOG_S_TIME("    Load successful!")
        return true;
    }

    Texture* TextureAtlasManager::getTexture(const std::string& _textureName) {
        if(instance.atlases.find(_textureName) == instance.atlases.end()) {
            LOG_E_TIME("Atlas '", _textureName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        if(instance.atlases[_textureName]->subTextures.find(_textureName) == instance.atlases[_textureName]->subTextures.end()) {
            LOG_E_TIME("Texture '", _textureName, "' in '",_textureName ,"' was not found! But tried to be accessed")
            return nullptr;
        }

        return instance.atlases[_textureName]->subTextures[_textureName];
    }

    Texture* TextureAtlasManager::getTile(const std::string& _atlasName, const std::string& _textureName) {
        if(instance.atlases.find(_atlasName) == instance.atlases.end()) {
            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        if(instance.atlases[_atlasName]->subTextures.find(_textureName) == instance.atlases[_atlasName]->subTextures.end()) {
            LOG_E_TIME("Texture '", _textureName, "' in '",_atlasName ,"' was not found! But tried to be accessed")
            return nullptr;
        }

        return instance.atlases[_atlasName]->subTextures[_textureName];
    }

    void TextureAtlasManager::cropTextures(Atlas& _atlas) {

        uint _numOfImagesHorizontal = _atlas.textureWidth / _atlas.tileWidth;
        uint _numOfImagesVertical = _atlas.textureHeight / _atlas.tileHeight;
        uint _currentTexture = 0;

        for(uint _y = 0; _y < _numOfImagesVertical; _y++) {
            for(uint _x = 0; _x < _numOfImagesHorizontal; _x++) {
//                if(isTextureEmpty(_atlas, _x * _atlas.tileWidth, _y * _atlas.tileHeight)) continue;
                IntRect _rect;
                _rect.bottomLeftCorner.x = (int)(_x * _atlas.tileWidth);
                _rect.bottomLeftCorner.y = (int)_atlas.textureHeight - (int)((_y + 1) * _atlas.tileHeight);
                _rect.size.x = (int)_atlas.tileWidth;
                _rect.size.y = (int)_atlas.tileHeight;

                _atlas.subTextures[(_atlas.name + "_" + std::to_string(_currentTexture))] = new Texture{_atlas.texture, _rect};
//                LOG_I("    Loaded sub-texture: ", _atlas.name + "_" + std::to_string(_currentTexture), " -> ", _rect)
                _currentTexture++;
            }
        }

        if(_numOfImagesHorizontal == 0 && _numOfImagesVertical == 1) {
            IntRect _rect;
            _rect.bottomLeftCorner.x = 0;
            _rect.bottomLeftCorner.y = 0;
            _rect.size.x = (int)_atlas.tileWidth;
            _rect.size.y = (int)_atlas.tileHeight;

            _atlas.subTextures[_atlas.name] = new Texture{_atlas.texture, _rect};
        }
    }

    bool TextureAtlasManager::isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const {
        int _transparentPixels = 0;
//        for(auto _h = _x; _h < _x + _atlas.tileWidth; _h++)
//            for(auto _v = _y; _v < _y + _atlas.tileHeight; _v++)
//                if(_atlas.texture.getPixel(_h, _v) == Color::Transparent) _transparentPixels++;

        return _transparentPixels == _atlas.tileWidth * _atlas.tileHeight;
    }

    Atlas* TextureAtlasManager::getAtlas(const std::string& _atlasName) {
        if(instance.atlases.find(_atlasName) == instance.atlases.end()) {
            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        return instance.atlases[_atlasName];
    }

    TextureAtlasManager::~TextureAtlasManager() {
        LOG_S("Cleaning up TextureAtlasManager")
        for(auto& _atlas : instance.atlases)
            delete _atlas.second;
    }

    TextureAtlasManager& TextureAtlasManager::get() {
        return instance;
    }

    std::vector<TextureInfo> TextureAtlasManager::getTexturesInfo() {
        std::vector<TextureInfo> _textures;

        for(auto& _atlas : instance.atlases) {
            _textures.emplace_back(TextureInfo {
                _atlas.second->texture->getKb(),
                (uint)_atlas.second->subTextures.size(),
                _atlas.second->textureWidth,
                _atlas.second->textureHeight,
                _atlas.second->tileWidth,
                _atlas.second->tileHeight,
                _atlas.first.c_str()
            });
        }

        return _textures;
    }

}