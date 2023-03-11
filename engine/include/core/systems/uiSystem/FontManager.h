// Created by borja on 15/1/22.


#ifndef RDE_FONT_MANAGER_H
#define RDE_FONT_MANAGER_H

#include "core/util/Vec.h"
#ifndef __EMSCRIPTEN__
#include <ft2build.h>
#include "freetype/freetype.h"
#else
#include <ft2build.h>
#include "freetype.h"
#endif
#include "core/render/elements/Texture.h"
#include <unordered_map>
#include <vector>

#define MAX_WIDTH 512
#define MAX_CHARACTERS 128

namespace RDE {

    /**
     * @brief This struct retains the information of each character.
     */
    struct CharInfo {
        Vec2I  size;
        Vec2I  bearing;
        Vec2I  advance;
        Vec2F  offset;
    };

    class FontManager;

    /**
     * @brief This struct contains all the information needed to render text with this specific font.
     */
    class Font {
        
        friend class FontManager;

        private:
            Texture texture;
			Vec2I atlasSize {0, 0};
            int fontSize = -1;
            CharInfo characters[MAX_CHARACTERS];
            std::string fontName;
            std::string originalPath;
            float biggestCharHeight = 0;

        private:
            Font() {}

        public:
            /**
             * @brief This function should not be called by the end users.
             * @param face FreeTypeFont
             * @param _fontSize Font size
             */
            void init(FT_Face face, int _fontSize);

            /**
             * @brief Returns the texture where the font is loaded.
             * @return Texture&
             */
            Texture& getTexture();

            /**
             * @brief Returns all of the characters information.
             * @return CharInfo*
             */
            CharInfo* getChars();

            /**
             * @brief Returns the font name.
             * @return std::string&
             */
            std::string& getFontName();

            /**
             * @brief Returns where the font is stored.
             * @return std::string&
             */
            std::string& getPath();

            /**
             * @brief Returns the size of the texture.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getSize() const;

            /**
             * @brief Returns the size of the font.
             * @return int
             */
            [[nodiscard]] int getFontSize() const;

            float getBiggestCharHeight() const;
    };

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class manages all the fonts of the applications. It loads, unloads and gets the fonts of memory.
     */
    class FontManager {

        /**
         * @brief Data struct containing a Font and it's size.
         */
        struct FontHandler {
            Font* font = nullptr;
            int fontSize = -1;
        };

        private:
            /**
             * @brief Freetype library.
             */
            FT_Library ftLibrary{};
            FileManager* fileManager = nullptr;

            /**
             * @brief Map FontName -> std::vector<FontHandler> because a font can have many sub-fonts with different sizes.
             */
            std::unordered_map<std::string, std::vector<FontHandler>> fonts;

        public:
            /**
             * @brief This is the maximum difference between a registered font size and a requested one.
             */
            int maxDifferenceBetweenFontSizesBeforeCreatingANewOne = 5;

        public:
            FontManager() {}

            /**
             * @attention This function should not be called by the end users.
             * @brief Initiates the system.
             */
            void init(FileManager* _fileManager);

            /**
             * @attention This function should not be called by the end users.
             * @brief Destroys the system and all loaded fonts.
             */
            void destroy();

            /**
             * @brief This function loads a font with a default font size of 24.
             * @param _pathToFont Path to font file
             * @param _fontSize Font size
             * @return Font*
             */
            Font* loadFont(FileManager& _fileManager, const std::string& _pathToFont, int _fontSize = 24);

            /**
             * @brief Returns the font with the initialized font size
             * @param _fontName Font name
             * @return Font*
             */
            Font* getDefaultFont(const std::string& _fontName);

            /**
             * @brief his function tries to return the font with the specific font size. If the font size is near enough to
             * one already loaded, it will return it. Otherwise it will create a new font with the desired size because
             * if using one too far from the loaded fonts sizes, it will be blurry and bad rendered.
             * @param _fontName Font name
             * @param _fontSize font size
             * @return Font*
             */
            Font* getSpecificFont(const std::string& _fontName, int _fontSize);

            /**
             * @brief This function unloads a font with all the sub-fonts created by size.
             * @param _fontName Font name
             */
            void unloadFullFont(const std::string& _fontName);

            /**
             * @brief This function unloads a specific font size of the full font. If no fonts sizes left when this is called,
             * it will remove the full font from memory.
             * @param _fontName Font name
             * @param _fontSize Font size
             */
            void unloadSpecificFont(const std::string& _fontName, int _fontSize);

            /**
             * @brief Returns all of the fonts loaded in memory.
             * @return  std::vector<Font*>
             */
            std::vector<Font*> getAllFonts();
    };

}


#endif //RDE_FONT_MANAGER_H
