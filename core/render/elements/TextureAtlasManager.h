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
        std::unordered_map<std::string, IntRect> rects;
        uint tileWidth, tileHeight;
        uint textureWidth, textureHeight;

        void debugInfo() {
            LOG_I("Atlas: ", name)
            LOG_I("   - Texture: ", &texture)
            LOG_I("   - rects size: ", rects.size())
            LOG_I("   - tileWidth: ", tileWidth)
            LOG_I("   - tileHeight: ", tileHeight)
            LOG_I("   - textureWidth: ", textureWidth)
            LOG_I("   - textureHeight: ", textureHeight)
        }
    };


    class TextureAtlasManager {
        private:
            std::unordered_map<std::string, Atlas> atlases;

        public:
            bool addAtlas(int _tileWidth, int _tileHeight, const std::string& _pathToTexture);
            Texture getTexture(const std::string& _atlasName, const std::string& _textureName);
            Atlas* getAtlas(const std::string& _atlasName);

        private:
            void cropTextures(Atlas& _atlas);
            std::string getTextureName(const std::string& _path) const;
            bool isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const;
    };

}


#endif //ENGINE2_0_TEXTURE_ATLAS_H
