// Created by borja on 6/1/22.


#ifndef ENGINE2_0_TEXTURE_ATLAS_H
#define ENGINE2_0_TEXTURE_ATLAS_H


#include "core/util/Util.h"

#ifdef USE_ZLIB
#include <zlib.h>
#endif

#include "Texture.h"

namespace GDE {

    /// An atlas is a group of images normally related between them, or as also known, a spritesheet. This struct holds
    /// all the information related to this spritesheet and how the are separated.
    /// @version 0.5
    /// As for version 0.5 and below, it currently only allows the spritesheet to be divided into peaces of the same size.
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

    /// This manager holds all the atlases and stores them in memory.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class TextureAtlasManager {
        private:
            std::unordered_map<std::string, Atlas*> atlases;

        public:

            /// Load a spritesheet in memory with it's subdivisions.
            /// @param _tileWidth the width of each of the individual sub-sprites.
            /// @param _tileHeight the height of each of the individual sub-sprites.
            /// @param _pathToTexture path to the texture to be loaded.
            bool loadAtlas(int _tileWidth, int _tileHeight, const std::string& _pathToTexture);

            /// This function is used to get the whole spritesheet. Useful when a spritesheet is just a whole sprite.
            Texture* getTile(const std::string& _atlasName);

            /// This function returns a sub-sprite inside the spritesheet.
            /// @param _atlasName the spritesheet name.
            /// @param _textureName it is _atlasName_X, with X the index of subdivided sprites starting at top left.
            Texture* getTile(const std::string& _atlasName, const std::string& _textureName);

            /// Returns the spritesheet.
            Atlas* getAtlas(const std::string& _atlasName);

            /// This functions unloads from memory an specific spritesheet. Useful when switching between scenes and
            /// some spritesheets are not more in need
            void unloadAtlas(const std::string& _atlasName);

            // Debug
            std::vector<TextureInfo> getTexturesInfo();

            /// This function cleans all of the spritesheets loaded in memory, but should not be called by users, it is
            /// called internally when needed.
            void destroy();

            TextureAtlasManager() = default;

        private:
            void cropTextures(Atlas& _atlas);
            bool isTextureEmpty(const Atlas& _atlas, uint _x, uint _y) const;
    };

}


#endif //ENGINE2_0_TEXTURE_ATLAS_H
