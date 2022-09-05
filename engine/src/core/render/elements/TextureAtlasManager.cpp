// Created by borja on 6/1/22.

#include "core/render/elements/TextureAtlasManager.h"
#include "core/util/Functions.h"

namespace GDE {

    bool TextureAtlasManager::loadSpriteSheet(const nlohmann::json& _spriteSheetNode) {
        auto _pathToTexture = _spriteSheetNode["sprite_sheet_settings"]["path"].get<std::string>();
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

        cropTextures(*_atlas, _spriteSheetNode["sprites"]);
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

    void TextureAtlasManager::cropTextures(Atlas& _atlas, const nlohmann::json& _spritesNode) {

        for(auto& _spriteNode : _spritesNode) {
            IntRect _rect;
            _rect.size.x = _spriteNode["size"][0].get<int>();
            _rect.size.y = _spriteNode["size"][1].get<int>();
            _rect.bottomLeftCorner.x = _spriteNode["origin"][0].get<int>();
            _rect.bottomLeftCorner.y = (int)_atlas.textureHeight - (_spriteNode["origin"][1].get<int>() + _rect.size.y);

            auto* _texture = new Texture { &_atlas, _rect };
            if(_spriteNode.contains("nine_patch")) {
                _texture->nineSlice.left = _spriteNode["nine_patch"]["left"].get<int>();
                _texture->nineSlice.top = _spriteNode["nine_patch"]["top"].get<int>();
                _texture->nineSlice.right = _spriteNode["nine_patch"]["right"].get<int>();
                _texture->nineSlice.bottom = _spriteNode["nine_patch"]["bottom"].get<int>();

                cropNinePatchSubTextures(_texture, _spriteNode);
            }

            _atlas.subTextures[_spriteNode["name"].get<std::string>()] = _texture;
        }
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

    void TextureAtlasManager::cropNinePatchSubTextures(Texture* _texture, const nlohmann::json& _spriteNode) {
        auto _origin = Vec2I {_spriteNode["origin"][0].get<int>(), _texture->getSpriteSheetSize().y - (_spriteNode["origin"][1].get<int>() + _texture->getRegion().size.y)};
        auto _size = Vec2I {_spriteNode["size"][0].get<int>(), _spriteNode["size"][1].get<int>()};
        // Bottom row
        _texture->nineSlice.subRects[0] = {
            .bottomLeftCorner = _origin,
            .size = {_texture->nineSlice.left, _texture->getRegion().size.y - (_texture->nineSlice.top + _texture->nineSlice.bottom)}
        };

        _texture->nineSlice.subRects[1] = {
                .bottomLeftCorner = {_texture->nineSlice.left, _origin.y},
                .size = {_texture->nineSlice.right, _size.y - (_texture->nineSlice.top + _texture->nineSlice.bottom)}
        };

        _texture->nineSlice.subRects[2] = {
                .bottomLeftCorner = {_texture->nineSlice.left + _texture->nineSlice.right, _origin.y},
                .size = {_size.x - (_texture->nineSlice.left + _texture->nineSlice.right), _size.y - (_texture->nineSlice.top + _texture->nineSlice.bottom)}
        };



        // Middle row
        _texture->nineSlice.subRects[3] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[0].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[0].size.y},
                .size = {_texture->nineSlice.subRects[0].size.x, _texture->nineSlice.bottom}
        };

        _texture->nineSlice.subRects[4] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[1].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[1].size.y},
                .size = {_texture->nineSlice.subRects[1].size.x, _texture->nineSlice.subRects[3].size.y}
        };

        _texture->nineSlice.subRects[5] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[2].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[2].size.y},
                .size = {_texture->nineSlice.subRects[2].size.x, _texture->nineSlice.subRects[3].size.y}
        };



        // Top row
        _texture->nineSlice.subRects[6] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[0].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[0].size.y + _texture->nineSlice.subRects[3].size.y},
                .size = {_texture->nineSlice.left, _texture->nineSlice.top}
        };

        _texture->nineSlice.subRects[7] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[1].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[1].size.y + _texture->nineSlice.subRects[4].size.y},
                .size = {_texture->nineSlice.subRects[4].size.x, _texture->nineSlice.top}
        };

        _texture->nineSlice.subRects[8] = {
                .bottomLeftCorner = {_texture->nineSlice.subRects[2].bottomLeftCorner.x, _origin.y + _texture->nineSlice.subRects[2].size.y + _texture->nineSlice.subRects[5].size.y},
                .size = {_texture->nineSlice.subRects[5].size.x, _texture->nineSlice.top}
        };
    }

}