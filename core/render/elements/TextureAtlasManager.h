// Created by borja on 6/1/22.


#ifndef ENGINE2_0_TEXTURE_ATLAS_H
#define ENGINE2_0_TEXTURE_ATLAS_H


#include "core/util/Util.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef USE_ZLIB
#include <zlib.h>
#endif

#include "Texture.h"

namespace engine {

    struct Atlas {
        Texture* texture;
        std::string name;
        std::unordered_map<std::string, Texture*> subTextures;
        uint tileWidth = 0, tileHeight = 0;
        uint textureWidth = 0, textureHeight = 0;

        Atlas() {
            texture = new Texture;
        }

        ~Atlas() {
            LOG_I("     Cleaning up Atlas ", name)
            for(auto& _texture : subTextures)
                delete _texture.second;

            delete texture;
        }

        void debugInfo() {
            LOG_I("Atlas: ", name)
            LOG_I("   - Texture: ", &texture)
            LOG_I("   - subTextures size: ", subTextures.size())
            LOG_I("   - tileWidth: ", tileWidth)
            LOG_I("   - tileHeight: ", tileHeight)
            LOG_I("   - textureWidth: ", textureWidth)
            LOG_I("   - textureHeight: ", textureHeight)
        }
    };

    struct TextureInfo {
        float kb;
        uint numberOfTiles;
        uint textureWidth, textureHeight, tileWidth, tileHeight;
        const char* name;
    };


    class TextureAtlasManager {
        private:
            std::unordered_map<std::string, Atlas*> atlases;
            static TextureAtlasManager instance;

        public:
            bool addAtlas(int _tileWidth, int _tileHeight, const std::string& _pathToTexture);
            Texture* getTexture(const std::string& _textureName);
            Texture* getTile(const std::string& _atlasName, const std::string& _textureName);
            Atlas* getAtlas(const std::string& _atlasName);
            static TextureAtlasManager& get();

            // Debug
            std::vector<TextureInfo> getTexturesInfo();

            ~TextureAtlasManager();

        private:
            TextureAtlasManager() = default;
            void cropTextures(Atlas& _atlas);
            bool isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const;
    };

}


#endif //ENGINE2_0_TEXTURE_ATLAS_H
