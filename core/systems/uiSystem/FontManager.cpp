// Created by borja on 15/1/22.

#include "FontManager.h"
#include "stb_image_writer.h"

namespace engine {

    void Font::init(const std::string& _pathToFont, FT_Library& _ftLibrary) {
        ftLibrary = &_ftLibrary;
        auto _result = FT_New_Face(*ftLibrary, _pathToFont.c_str(), 0, &ftFont);
        if(_result == FT_Err_Unknown_File_Format) {
            LOG_E("Failed to load font at '", _pathToFont, "'!!")
            return;
        }

        FT_Set_Char_Size(ftFont, 0, fontSize << 6, 96, 96);

        int max_dim = (1 + (ftFont->size->metrics.height >> 6)) * ceilf(sqrtf(DEFAULT_NUMBER_OF_CHARS_IN_FONT));
        int tex_width = 1;
        while(tex_width < max_dim) tex_width <<= 1;
        int tex_height = tex_width;

        unsigned char pixels[tex_width * tex_height];
        int pen_x = 0, pen_y = 0;

        for(int i = 32; i < 128; ++i){
            FT_Load_Char(ftFont, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
            FT_Bitmap* bmp = &ftFont->glyph->bitmap;
            FT_Render_Glyph(ftFont->glyph, FT_RENDER_MODE_NORMAL);

            if(pen_x + bmp->width >= tex_width){
                pen_x = 0;
                pen_y += ((ftFont->size->metrics.height >> 6) + 1);
            }

            for(int row = 0; row < bmp->rows; ++row){
                for(int col = 0; col < bmp->width; ++col){
                    int x = pen_x + col;
                    int y = pen_y + row;
                    pixels[y * tex_width + x] = bmp->buffer[row * bmp->pitch + col];
                }
            }

            // this is stuff you'd need when rendering individual glyphs out of the atlas
            char _char = (char)i;
            chars[_char].position = {(float)pen_x, (float)pen_y};
            chars[_char].size = {(float)(ftFont->glyph->metrics.width >> 6), (float)(ftFont->glyph->metrics.height >> 6)};
            chars[_char].bearing = {(float)(ftFont->glyph->bitmap_left), (float)(ftFont->glyph->bitmap_top)};
            chars[_char].advance = (int)(ftFont->glyph->advance.x >> 6);

            if((char)i == 'l') spaceWidth = chars[_char].size.x * 1.5f;

            pen_x += chars[_char].size.x;
            shortestCharHeight = std::min(shortestCharHeight, chars[_char].size.y);
        }

        auto* png_data = new unsigned char[tex_width * tex_height * 4];
        for(int _row = 0; _row < tex_height; _row++) {
            for(int _col = 0; _col < tex_width; _col++) {
                int i = (_row * tex_width + _col);
                png_data[i * 4 + 0] = pixels[i];
                png_data[i * 4 + 1] = pixels[i];
                png_data[i * 4 + 2] = pixels[i];
                png_data[i * 4 + 3] = (int)pixels[i] < 100 ? 0 : 255;
            }
        }

        texture = new Texture;
//        stbi_write_png("font_output.png", tex_width, tex_height, 4, png_data, tex_width * 4);
        texture->loadFromMemory(png_data, tex_width * tex_height * 4);
        delete [] png_data;
    }

    Texture* Font::getTexture() {
        return texture;
    }

    Font::~Font() {
        delete texture;
        LOG_S("Cleaning up Font ", fontName)
        FT_Done_Face(ftFont);
    }

    FontChar& Font::getFontChar(char _c) {
        return chars[_c];
    }

    float Font::getShortestCharHeight() const {
        return shortestCharHeight;
    }

    float Font::getFontSize() const {
        return fontSize;
    }

    float Font::getSpaceWidth() const {
        return spaceWidth;
    }

    float Font::getEnterHeight() const {
        return enterHeight;
    }

    const std::string& Font::getFontName() {
        return fontName;
    }

    float Font::getSpacesBetweenChars() const {
        return spacesBetweenChars;
    }

    //----------------------------- FONT MANAGER -----------------

    FontManager& FontManager::get() {
        static FontManager _fontManager;
        return _fontManager;
    }

    void FontManager::init() {
        if(FT_Init_FreeType(&ftLibrary)) {
            LOG_E("Error initiating FreeType");
            return;
        }

        LOG_S("FontManager loaded")
    }

    FontAtlas* FontManager::loadFont(const std::string& _pathToFont, int _fontSize) {
        if (FT_Init_FreeType(&ftLibrary)) {
            fprintf(stderr, "Could not init freetype library\n");
            return nullptr;
        }

        /* Load a font */
        if (FT_New_Face(ftLibrary, _pathToFont.c_str(), 0, &ftFace)) {
            fprintf(stderr, "Could not open font %s\n", _pathToFont.c_str());
            return nullptr;
        }

        auto* _font = new FontAtlas(ftFace, _fontSize);

        std::string _name = util::getFileNameFromPath(_pathToFont);
        fonts[_name] = _font;

        LOG_S("Load font successfully!!!!");

        return fonts[_name];
    }

    FontAtlas* FontManager::getFont(const std::string& _fontName) {
        return fonts[_fontName];
    }

    FontManager::~FontManager() {
        for(auto& _font : fonts)
            delete _font.second;
        FT_Done_FreeType(ftLibrary);
    }

    std::vector<FontAtlas*> FontManager::getAllFonts() {
        std::vector<FontAtlas*> _fonts;
        for(auto& _font : fonts)
            _fonts.push_back(_font.second);

        return _fonts;
    }
}