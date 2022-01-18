// Created by borja on 15/1/22.


#ifndef ENGINE2_0_FONT_MANAGER_H
#define ENGINE2_0_FONT_MANAGER_H


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "core/util/Util.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <glm/ext/vector_int2.hpp>
#include "core/render/elements/Texture.h"

#define DEFAULT_NUMBER_OF_CHARS_IN_FONT 96

namespace engine {

    struct FontChar {
        Vec2F position;
        Vec2F size;
        Vec2F bearing;
        int advance;
    };

    class FontManager;
    class Font {
        private:
            Texture* texture;
            FT_Face ftFont;
            FT_Library* ftLibrary = nullptr;
            std::unordered_map<char, FontChar> chars;
            float shortestCharHeight = MAXFLOAT;
            int fontSize;
            float spaceWidth;
            float enterHeight = 3.f;
            float spacesBetweenChars = 1.f;
            std::string fontName;

            friend class FontManager;

        public:
            void init(const std::string& _pathToFont, FT_Library& _ftLibrary);
            FontChar& getFontChar(char _c);
            Texture* getTexture();
            float getShortestCharHeight() const;
            float getFontSize() const;
            float getSpaceWidth() const;
            float getEnterHeight() const;
            float getSpacesBetweenChars() const;
            const std::string& getFontName();

            ~Font();
    };

    class FontManager {
        private:
            FT_Library ftLibrary;
            std::unordered_map<std::string, Font*> fonts;

        private:
            FontManager() = default;

        public:
            static FontManager& get();

            void init();
            Font* loadFont(const std::string& _pathToFont, int _fontSize = 24);
            Font* getFont(const std::string& _fontName);

            std::vector<Font*> getAllFonts();

            ~FontManager();
    };

}


#endif //ENGINE2_0_FONT_MANAGER_H
