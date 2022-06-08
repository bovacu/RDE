// Created by borja on 6/1/22.

#include "core/render/elements/TextureAtlasManager.h"
#include "core/util/Functions.h"

namespace GDE {

    bool TextureAtlasManager::loadSpriteSheet(const YAML::Node& _node) {
        auto _pathToTexture = _node["SpriteSheetSettings"]["Path"].as<std::string>();
        auto _name = Util::getFileNameFromPath(_pathToTexture);
        LOG_I_TIME("Trying to load '", _pathToTexture, "'...")
        if(atlases.find(_name) != atlases.end()) {
            LOG_E_TIME("Atlas '", _name, "' was already loaded");
            return false;
        }

        auto* _atlas = new Atlas;
        if(!_atlas->texture->loadFromFile(_pathToTexture.c_str())) {
            LOG_E_TIME("Atlas '", _pathToTexture, "' could not be loaded");
            return false;
        }

        _atlas->name = _name;
        _atlas->textureWidth = _atlas->texture->getSize().x;
        _atlas->textureHeight = _atlas->texture->getSize().y;

        cropTextures(*_atlas, _node["Sprites"]);
        atlases[_name] = _atlas;
        LOG_S_TIME("    Load successful!")
        return true;
    }

    Texture* TextureAtlasManager::getSubTexture(const std::string& _atlasName, const std::string& _textureName) {
        if(atlases.find(_atlasName) == atlases.end()) {
            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        if(atlases[_atlasName]->subTextures.find(_textureName) == atlases[_atlasName]->subTextures.end()) {
            LOG_E_TIME("Texture '", _textureName, "' in '",_atlasName ,"' was not found! But tried to be accessed")
            return nullptr;
        }

        return atlases[_atlasName]->subTextures[_textureName];
    }

    void TextureAtlasManager::cropTextures(Atlas& _atlas, const YAML::Node& _spritesNode) {

        for(auto& _spriteNode : _spritesNode) {
            IntRect _rect;
            _rect.size.x = _spriteNode["Size"][0].as<int>();
            _rect.size.y = _spriteNode["Size"][1].as<int>();
            _rect.bottomLeftCorner.x = _spriteNode["Origin"][0].as<int>();
            _rect.bottomLeftCorner.y = (int)_atlas.textureHeight - (_spriteNode["Origin"][1].as<int>() + _rect.size.y);

            auto* _texture = new Texture { _atlas.texture, _rect };
            if(_spriteNode["NinePatch"].IsDefined()) {
                _texture->ninePatch.left = _spriteNode["NinePatch"]["Left"].as<int>();
                _texture->ninePatch.top = _spriteNode["NinePatch"]["Top"].as<int>();
                _texture->ninePatch.right = _spriteNode["NinePatch"]["Right"].as<int>();
                _texture->ninePatch.bottom = _spriteNode["NinePatch"]["Bottom"].as<int>();

                cropNinePatchSubTextures(_texture, _spriteNode);
            }

            _atlas.subTextures[_spriteNode["Name"].as<std::string>()] = _texture;
        }
    }

    bool TextureAtlasManager::isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const {
        int _transparentPixels = 0;
        return _transparentPixels == _atlas.tileWidth * _atlas.tileHeight;
    }

    Atlas* TextureAtlasManager::getAtlas(const std::string& _atlasName) {
        if(atlases.find(_atlasName) == atlases.end()) {
            LOG_E_TIME("Atlas '", _atlasName, "' was not loaded! But tried to be accessed")
            return nullptr;
        }

        return atlases[_atlasName];
    }

    std::vector<TextureInfo> TextureAtlasManager::getTexturesInfo() {
        std::vector<TextureInfo> _textures;

        for(auto& _atlas : atlases) {
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

    void TextureAtlasManager::unloadAtlas(const std::string& _atlasName) {
        delete atlases[_atlasName]->texture;
        delete atlases[_atlasName];
        atlases.erase(_atlasName);
    }

    void TextureAtlasManager::destroy() {
        LOG_S("Cleaning up TextureAtlasManager")
        for(auto& _atlas : atlases)
            delete _atlas.second;
    }

    void TextureAtlasManager::cropNinePatchSubTextures(Texture* _texture, const YAML::Node& _spriteNode) {
        auto _origin = Vec2I {_spriteNode["Origin"][0].as<int>(), _texture->getSize().y - (_spriteNode["Origin"][1].as<int>() + _texture->getRegion().size.y)};
        auto _size = Vec2I {_spriteNode["Size"][0].as<int>(), _spriteNode["Size"][1].as<int>()};
        // Bottom row
        _texture->ninePatch.subRects[0] = {
            .bottomLeftCorner = _origin,
            .size = {_texture->ninePatch.left, _texture->getRegion().size.y - (_texture->ninePatch.top + _texture->ninePatch.bottom)}
        };

        _texture->ninePatch.subRects[1] = {
                .bottomLeftCorner = {_texture->ninePatch.left, _origin.y},
                .size = {_texture->ninePatch.right, _size.y - (_texture->ninePatch.top + _texture->ninePatch.bottom)}
        };

        _texture->ninePatch.subRects[2] = {
                .bottomLeftCorner = {_texture->ninePatch.left + _texture->ninePatch.right, _origin.y},
                .size = {_size.x - (_texture->ninePatch.left + _texture->ninePatch.right), _size.y - (_texture->ninePatch.top + _texture->ninePatch.bottom)}
        };



        // Middle row
        _texture->ninePatch.subRects[3] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[0].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[0].size.y},
                .size = {_texture->ninePatch.subRects[0].size.x, _texture->ninePatch.bottom}
        };

        _texture->ninePatch.subRects[4] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[1].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[1].size.y},
                .size = {_texture->ninePatch.subRects[1].size.x, _texture->ninePatch.subRects[3].size.y}
        };

        _texture->ninePatch.subRects[5] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[2].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[2].size.y},
                .size = {_texture->ninePatch.subRects[2].size.x, _texture->ninePatch.subRects[3].size.y}
        };



        // Top row
        _texture->ninePatch.subRects[6] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[0].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[0].size.y + _texture->ninePatch.subRects[3].size.y},
                .size = {_texture->ninePatch.left, _texture->ninePatch.top}
        };

        _texture->ninePatch.subRects[7] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[1].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[1].size.y + _texture->ninePatch.subRects[4].size.y},
                .size = {_texture->ninePatch.subRects[4].size.x, _texture->ninePatch.top}
        };

        _texture->ninePatch.subRects[8] = {
                .bottomLeftCorner = {_texture->ninePatch.subRects[2].bottomLeftCorner.x, _origin.y + _texture->ninePatch.subRects[2].size.y + _texture->ninePatch.subRects[5].size.y},
                .size = {_texture->ninePatch.subRects[5].size.x, _texture->ninePatch.top}
        };
    }

}