// Created by borja on 6/1/22.

#include "TextureAtlasManager.h"

namespace engine {

    bool TextureAtlasManager::addAtlas(int _tileWidth, int _tileHeight, const std::string& _pathToTexture) {
        auto _name = getTextureName(_pathToTexture);
        LOG_I_TIME("Trying to load '", _pathToTexture, "'...")
        if(atlases.find(_name) != atlases.end()) {
            LOG_E_TIME("Atlas '", _name, "' was already loaded");
            return false;
        }

        Atlas _atlas;
        if(!_atlas.texture->loadFromFile(_pathToTexture.c_str(), false)) {
            LOG_E_TIME("Atlas '", _pathToTexture, "' could not be loaded");
            return false;
        }

        _atlas.name = _name;
        _atlas.tileWidth = _tileWidth;
        _atlas.tileHeight = _tileHeight;
        _atlas.textureWidth = _atlas.texture->getSize().x;
        _atlas.textureHeight = _atlas.texture->getSize().y;

        cropTextures(_atlas);
        atlases[_name] = _atlas;
        _atlas.texture->cleanJunk();
        LOG_S_TIME("Load successful!")
        return true;
    }

    Texture TextureAtlasManager::getTexture(const std::string& _atlasName, const std::string& _textureName) {
        return {nullptr, IntRect()};
//        if(atlases.find(_atlasName) == atlases.end()) {
//            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
//            return {nullptr, IntRect()};
//        }
//
//        Atlas& _atlas = atlases[_atlasName];
//
//        if(_atlas.rects.find(_textureName) == _atlas.rects.end()) {
//            LOG_E_TIME("Texture '", _textureName, "' in '",_atlasName ,"' was not found! But tried to be accessed")
//            return {nullptr, IntRect()};
//        }
//
//        return Texture(&_atlas.texture, _atlas.rects[_textureName]);
    }


    void TextureAtlasManager::cropTextures(Atlas& _atlas) {

        uint _numOfImagesHorizontal = _atlas.textureWidth / _atlas.tileWidth;
        uint _numOfImagesVertical = _atlas.textureHeight / _atlas.tileHeight;
        uint _currentTexture = 0;

        for(uint _x = 0; _x < _numOfImagesHorizontal; _x++) {
            for(uint _y = 0; _y < _numOfImagesVertical; _y++) {
                if(isTextureEmpty(_atlas, _x * _atlas.tileWidth, _y * _atlas.tileHeight)) continue;

                IntRect _rect;
                _rect.bottomLeftCorner.x = (int)(_x * _atlas.tileWidth);
                _rect.bottomLeftCorner.y = (int)(_y * _atlas.tileHeight);
                _rect.size.x = (int)_atlas.tileWidth;
                _rect.size.y = (int)_atlas.tileHeight;

                _atlas.rects[(_atlas.name + "_" + std::to_string(_currentTexture))] = _rect;
                LOG_I("    Loaded sub-texture: ", _atlas.name + "_" + std::to_string(_currentTexture))
                _currentTexture++;
            }
        }
    }

    std::string TextureAtlasManager::getTextureName(const std::string& _path) const {
        std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
        std::string::size_type const p(base_filename.find_last_of('.'));
        return base_filename.substr(0, p);
    }

    bool TextureAtlasManager::isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const {
        int _transparentPixels = 0;
        for(auto _h = _x; _h < _x + _atlas.tileWidth; _h++)
            for(auto _v = _y; _v < _y + _atlas.tileHeight; _v++)
                if(_atlas.texture->getPixel(_h, _v) == Color::Transparent) _transparentPixels++;

        return _transparentPixels == _atlas.tileWidth * _atlas.tileHeight;
    }

    Atlas* TextureAtlasManager::getAtlas(const std::string& _atlasName) {
        if(atlases.find(_atlasName) == atlases.end()) {
            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        return &atlases[_atlasName];
    }

}