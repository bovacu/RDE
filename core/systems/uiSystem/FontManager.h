// Created by borja on 15/1/22.


#ifndef ENGINE2_0_FONT_MANAGER_H
#define ENGINE2_0_FONT_MANAGER_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "core/util/Util.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "core/render/elements/Texture.h"

#define MAX_WIDTH 1024

namespace engine {

    struct CharInfo {
        Vec2F advance;
        Vec2F bitmapSize;
        Vec2F bitmapPos;
        Vec2F offset;
    };

    class FontManager;
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
            void init(FT_Face face, int _fontSize);
            Texture& getTexture();
            CharInfo* getChars();
            std::string& getFontName();
            std::string& getPath();
            [[nodiscard]] Vec2F getSize() const;
            [[nodiscard]] int getFontSize() const;
    };

    class FontManager {

        struct FontHandler {
            Font* font = nullptr;
            int fontSize = -1;
        };

        private:
            FT_Library ftLibrary{};
            std::unordered_map<std::string, std::vector<FontHandler>> fonts;

        private:
            FontManager() = default;

        public:
            static FontManager& get();

            void init();
            Font* loadFont(const std::string& _pathToFont, int _fontSize = 24);
            Font* getDefaultFont(const std::string& _fontName);
            Font* getSpecificFont(const std::string& _fontName, int _fontSize);

            std::vector<Font*> getAllFonts();

            ~FontManager();
    };

}


#endif //ENGINE2_0_FONT_MANAGER_H
