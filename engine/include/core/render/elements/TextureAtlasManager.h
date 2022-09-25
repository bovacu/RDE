// Created by borja on 6/1/22.


#ifndef GDE_TEXTURE_ATLAS_H
#define GDE_TEXTURE_ATLAS_H


#include "nlohmann/json.hpp"
#include "core/util/Util.h"

#ifdef USE_ZLIB
#include <zlib.h>
#endif

#include "Texture.h"
#include "core/systems/fileSystem/FileManager.h"

namespace GDE {

    /**
     * @brief An atlas is a group of images normally related between them, or as also known, a Sprite Sheet. This struct holds
     * all the information related to this Sprite Sheet and how the are separated.
     */
    struct Atlas {
        /**
         * @brief Texture containing the Sprite Sheet.
         */
        Texture* texture;

        /**
         * @brief Name of the Sprite Sheet.
         */
        std::string name;

        /**
         * @brief Map that holds each sub-sprite and its name.
         */
        std::unordered_map<std::string, Texture*> subTextures;

        /**.
         * @brief Width of the Sprite Sheet
         */
        uint textureWidth = 0;

        /**
         * @brief Height of the Sprite Sheet.
         */
        uint textureHeight = 0;

        Atlas() {
            texture = new Texture;
        }

        ~Atlas() {
            for(auto& _texture : subTextures)
                delete _texture.second;

            delete texture;
        }

        /**
         * @brief Prints info about the Sprite Sheet.
         */
        void debugInfo() {
            LOG_I("Atlas: ", name)
            LOG_I("   - Texture: ", &texture)
            LOG_I("   - subTextures size: ", subTextures.size())
            LOG_I("   - textureWidth: ", textureWidth)
            LOG_I("   - textureHeight: ", textureHeight)
        }
    };

    /**
     * @brief Data struct use to render debug info with the ImGuiScene.
     */
    struct TextureInfo {
        /**
         * @brief Size of the Sprite Sheet in Kilobytes.
         */
        float kb;

        /**
         * @brief Number of sub-sprites that the Sprite Sheet has.
         */
        uint numberOfTiles;

        /**
         * @brief Width of the Sprite Sheet.
         */
        uint textureWidth;

        /**
         * @brief Height of the Sprite Sheet.
         */
        uint textureHeight;

        /**
         * @brief Name of the Sprite Sheet.
         */
        const char* name;
    };

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This manager holds all the atlases and stores them in memory.
     */
    class TextureAtlasManager {
        private:
            std::unordered_map<std::string, Atlas*> atlases;
            FileManager* fileManager;

        public:
            /**
             * @attention This function should not be called by end users.
             * @brief Initiates the sound system.
             */
            void init(FileManager* _fileManager);

            bool loadSpriteSheet(const std::string& _spriteSheetPath);

            /**
             * @brief This function returns a sub-texture inside the Sprite Sheet.
             * @param _atlasName the sprite sheet name.
             * @param _textureName it is the name defined in the YAML generated by TexturePacker.
             */
            Texture* getSubTexture(const std::string& _atlasName, const std::string& _textureName);

            /**
             * @brief Returns the Sprite Sheet.
             * @param _atlasName Name of the Atlas
             * @return Atlas*
             */
            Atlas* getAtlas(const std::string& _atlasName);

            /**
             * @brief This functions unloads from memory an specific Sprite Sheet. Useful when switching between scenes and
             * some Sprite Sheet are not more in need.
             * @param _atlasName Name of the Altas
             */
            void unloadAtlas(const std::string& _atlasName);

            /**
             * @brief Debug function.
             * @return std::vector<TextureInfo>
             */
            std::vector<TextureInfo> getTexturesInfo();

            /**
             * @attention This is not meant to be called by end-users.
             * @brief This function cleans all of the Sprite Sheets loaded in memory.
             */
            void destroy();

            TextureAtlasManager() {};

        private:
            /**
             * @brief Divides the Sprite Sheet into sub-sprites.
             * @param _atlas Atlas to be divided.
             * @param _spritesNode TexturePacker information about how it is sub-divided
             */
            void cropTextures(Atlas& _atlas, const nlohmann::json& _spritesNode);

            /**
             * @brief Divides a Sprite into 9 sub-sprites to render the NineSlice.
             * @param _texture Texture to sub-divide
             * @param _spriteNode TexturePacker information about how it is sub-divided
             */
            void cropNinePatchSubTextures(Texture* _texture, const nlohmann::json& _spriteNode);
    };

}


#endif //GDE_TEXTURE_ATLAS_H
