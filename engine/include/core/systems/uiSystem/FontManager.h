// Created by borja on 15/1/22.


#ifndef RDE_FONT_MANAGER_H
#define RDE_FONT_MANAGER_H

#include "core/util/Vec.h"
#include <ft2build.h>
#include "freetype/freetype.h"
#include "core/render/elements/Texture.h"
#include <unordered_map>
#include <vector>

#define MAX_WIDTH 512
#define MAX_CHARACTERS 128
#define DEFAULT_FONT_SIZE 36

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

    typedef std::unordered_map<char, CharInfo> CharMap;

    class FontManager;
    class FileManager;

    /**
     * @brief This struct contains all the information needed to render text with this specific font.
     */
    class Font {
        
        friend class FontManager;

        private:
            Texture texture;
            Vec2I atlasSize {0, 0};
            int fontSize = -1;
            std::unordered_map<char, CharInfo> characters;
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
			RDE_FUNC void init(FT_Face face, int _fontSize);

            /**
             * @brief Returns the texture where the font is loaded.
             * @return Texture&
             */
			RDE_FUNC_ND Texture* getTexture();

            /**
             * @brief Returns all of the characters information.
             * @return CharInfo*
             */
			RDE_FUNC_ND CharMap& getChars();

            /**
             * @brief Returns the font name.
             * @return std::string&
             */
			RDE_FUNC_ND std::string& getFontName();

            /**
             * @brief Returns where the font is stored.
             * @return std::string&
             */
			RDE_FUNC_ND std::string& getPath();

            /**
             * @brief Returns the size of the texture.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getSize() const;

            /**
             * @brief Returns the size of the font.
             * @return int
             */
			RDE_FUNC_ND int getFontSize() const;

			RDE_FUNC_ND float getBiggestCharHeight() const;
    };

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class manages all the fonts of the applications. It loads, unloads and gets the fonts of memory.
     */
    class FontManager {
        private:
            /**
             * @brief Freetype library.
             */
            FT_Library ftLibrary{};
            FileManager* fileManager = nullptr;

            /**
             * @brief Map FontName -> std::vector<FontHandler> because a font can have many sub-fonts with different sizes.
             */
            std::unordered_map<std::string, Font*> fonts;

        public:
            /**
             * @brief This is the maximum difference between a registered font size and a requested one.
             */
            int maxDifferenceBetweenFontSizesBeforeCreatingANewOne = 5;

        public:
			RDE_FUNC FontManager() {}

            /**
             * @attention This function should not be called by the end users.
             * @brief Initiates the system.
             */
			RDE_FUNC void init(FileManager* _fileManager);

            /**
             * @attention This function should not be called by the end users.
             * @brief Destroys the system and all loaded fonts.
             */
			RDE_FUNC void destroy();

            /**
             * @brief This function loads a font at _pathFont.
             * @param _pathToFont Path to font 
             * @return Font*
             */
			RDE_FUNC Font* loadFont(FileManager& _fileManager, const std::string& _pathToFont);

            /**
             * @brief Returns the font with the specified name.
             * @param _fontName Font name
             * @return Font*
             */
			RDE_FUNC_ND Font* getFont(const std::string& _fontName);

            /**
             * @brief This function unloads a font with all the sub-fonts created by size.
             * @param _fontName Font name
             */
			RDE_FUNC void unloadFont(const std::string& _fontName);

            /**
             * @brief Returns all of the fonts loaded in memory.
             * @return  std::vector<Font*>
             */
			RDE_FUNC_ND std::vector<Font*> getAllFonts();
    };

}


#endif //RDE_FONT_MANAGER_H