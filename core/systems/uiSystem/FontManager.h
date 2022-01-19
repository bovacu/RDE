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
#define MAXWIDTH 1024

namespace engine {

    struct FontAtlas {
        Texture* texture;		// texture object
        GLint uniform_tex{};

        int width;			// width of texture in pixels
        int height{};			// height of texture in pixels

        struct {
            Vec2F advance; // ax, ay
            Vec2F bitmapSize; // bw, bh
            Vec2F bitmapPos; // bl, bt
            Vec2F offset; // tx, ty
        } characters[128];		// character information

        FontAtlas(FT_Face face, int _height) {
            texture = new Texture;

            FT_Set_Pixel_Sizes(face, 0, _height);
            FT_GlyphSlot g = face->glyph;

            int roww = 0;
            int rowh = 0;
            width = 0;
            height = 0;

            memset(characters, 0, sizeof characters);

            /* Find minimum size for a texture holding all visible ASCII characters */
            for (int i = 32; i < 128; i++) {
                if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
                    fprintf(stderr, "Loading character %c failed!\n", i);
                    continue;
                }
                if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
                    width = std::max(width, roww);
                    height += rowh;
                    roww = 0;
                    rowh = 0;
                }
                roww += g->bitmap.width + 1;
                rowh = rowh > g->bitmap.rows ? rowh : g->bitmap.rows;
            }

            width = std::max(width, roww);
            height += rowh;

            /* Create a texture that will be used to hold all ASCII glyphs */
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &texture->openGLTextureID);
            glBindTexture(GL_TEXTURE_2D, texture->openGLTextureID);
//            glUniform1i(uniform_tex, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

            /* We require 1 byte alignment when uploading texture data */
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            /* Clamping to edges is important to prevent artifacts when scaling */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            /* Linear filtering usually looks best for text */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            /* Paste all glyph bitmaps into the texture, remembering the offset */
            int ox = 0;
            int oy = 0;

            rowh = 0;

            for (int i = 32; i < 128; i++) {
                if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
                    fprintf(stderr, "Loading character %c failed!\n", i);
                    continue;
                }

                if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
                    oy += rowh;
                    rowh = 0;
                    ox = 0;
                }

                glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
                characters[i].advance.x = g->advance.x >> 6;
                characters[i].advance.y = g->advance.y >> 6;

                characters[i].bitmapSize.x = g->bitmap.width;
                characters[i].bitmapSize.y = g->bitmap.rows;

                characters[i].bitmapPos.x = g->bitmap_left;
                characters[i].bitmapPos.y = g->bitmap_top;

                characters[i].offset.x = ox / (float)width;
                characters[i].offset.y = oy / (float)height;

                rowh = rowh > g->bitmap.rows ? rowh : g->bitmap.rows;
                ox += g->bitmap.width + 1;
            }
            
            fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", width, height, width * height / 1024);
        }

        ~FontAtlas() {
            glDeleteTextures(1, &texture->openGLTextureID);
            delete texture;
        }
    };

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
            std::unordered_map<std::string, FontAtlas*> fonts;

        private:
            FontManager() = default;

        public:
            static FontManager& get();

            void init();
            FontAtlas* loadFont(const std::string& _pathToFont, int _fontSize = 24);
            FontAtlas* getFont(const std::string& _fontName);

            std::vector<FontAtlas*> getAllFonts();

            ~FontManager();
    };

}


#endif //ENGINE2_0_FONT_MANAGER_H
