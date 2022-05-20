// Created by borja on 15/1/22.


#ifndef ENGINE2_0_FONT_MANAGER_H
#define ENGINE2_0_FONT_MANAGER_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "core/util/Util.h"
#include <ft2build.h>
#include "freetype/freetype.h"
#include "core/render/elements/Texture.h"

#define MAX_WIDTH 1024

namespace GDE {

    /// This struct retains the information of each character.
    struct CharInfo {
        Vec2F advance;
        Vec2F bitmapSize;
        Vec2F bitmapPos;
        Vec2F offset;
        Vec2F bearing;
    };

    class FontManager;
    /// This struct contains all the information needed to render text with this specific font.
    class Font {
        private:
            Texture texture;
            int width = -1;
            int height = -1;
            int fontSize = -1;
            CharInfo characters[128];
            std::string fontName;
            std::string originalPath;

        private:
            friend class FontManager;
            Font() = default;

        public:
            /// This function should not be called by the end users.
            void init(FT_Face face, int _fontSize);

            /// Returns the texture where the font is loaded.
            Texture& getTexture();

            /// Returns all of the characters information.
            CharInfo* getChars();

            /// Returns the font name.
            std::string& getFontName();

            /// Returns where the font is stored.
            std::string& getPath();

            /// Returns the size of the texture.
            [[nodiscard]] Vec2F getSize() const;

            /// Returns the size of the font.
            [[nodiscard]] int getFontSize() const;
    };

    /// This class manages all the fonts of the applications. It loads, unloads and gets the fonts of memory.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class FontManager {

        struct FontHandler {
            Font* font = nullptr;
            int fontSize = -1;
        };

        private:
            FT_Library ftLibrary{};
            std::unordered_map<std::string, std::vector<FontHandler>> fonts;

        public:
            int maxDifferenceBetweenFontSizesBeforeCreatingANewOne = 5;

        public:
            FontManager() = default;

            /// This function should not be called by the end users.
            void init();

            /// This function should not be called by the end users.
            void destroy();

            /// This function loads a font with a default font size of 24.
            Font* loadFont(const std::string& _pathToFont, int _fontSize = 24);

            /// Returns the font with the initialized font size
            Font* getDefaultFont(const std::string& _fontName);

            /// This function tries to return the font with the specific font size. If the font size is near enough to
            /// one already loaded, it will return it. Otherwise it will create a new font with the desired size because
            /// if using one too far from the loaded fonts sizes, it will be blurry and bad rendered.
            Font* getSpecificFont(const std::string& _fontName, int _fontSize);

            /// This function unloads a font with all the sub-fonts created by size.
            void unloadFullFont(const std::string& _fontName);

            /// This function unloads a specific font size of the full font. If no fonts sizes left when this is called,
            /// it will remove the full font from memory.
            void unloadSpecificFont(const std::string& _fontName, int _fontSize);

            /// Returns all of the fonts loaded in memory.
            std::vector<Font*> getAllFonts();
    };

}


#endif //ENGINE2_0_FONT_MANAGER_H
