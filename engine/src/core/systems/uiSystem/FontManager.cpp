// Created by borja on 15/1/22.

#include "core/systems/uiSystem/FontManager.h"
#include "stb/stb_image_writer.h"
#include "core/util/Functions.h"

namespace engine {

    void Font::init(FT_Face face, int _fontSize)  {
        FT_Set_Pixel_Sizes(face, 0, _fontSize);
        FT_GlyphSlot g = face->glyph;

        int _rowWidth = 0;
        int _rowHeight = 0;
        width = 0;
        height = 0;
        fontSize = _fontSize;

        memset(characters, 0, sizeof(characters));

        /* Find minimum size for a texture holding all visible ASCII characters */
        for (int _i = 32; _i < 128; _i++) {
            if (FT_Load_Char(face, _i, FT_LOAD_RENDER)) {
                LOG_E("Loading character", _i, " failed!")
                continue;
            }
            if (_rowWidth + g->bitmap.width + 1 >= MAX_WIDTH) {
                width = std::max(width, _rowWidth);
                height += _rowHeight;
                _rowWidth = 0;
                _rowHeight = 0;
            }
            _rowWidth += (int)g->bitmap.width + 1;
            _rowHeight = _rowHeight > g->bitmap.rows ? _rowHeight : (int)g->bitmap.rows;
        }

        width = std::max(width, _rowWidth);
        height += _rowHeight;

        /* Create a texture that will be used to hold all ASCII glyphs */
        texture.loadTextTexture(width, height);

        /* Paste all glyph bitmaps into the texture, remembering the offset */
        int _ox = 0;
        int _oy = 0;

        _rowHeight = 0;

        for (int _i = 32; _i < 128; _i++) {
            if (FT_Load_Char(face, _i, FT_LOAD_RENDER)) {
                LOG_E("Loading character", _i, " failed!")
                continue;
            }

            if (_ox + g->bitmap.width + 1 >= MAX_WIDTH) {
                _oy += _rowHeight;
                _rowHeight = 0;
                _ox = 0;
            }

            texture.loadTextSubTextures({_ox, _oy}, {(int)g->bitmap.width, (int)g->bitmap.rows}, g->bitmap.buffer);
            characters[_i].advance.x = (float)(g->advance.x >> 6);
            characters[_i].advance.y = (float)(g->advance.y >> 6);

            characters[_i].bitmapSize.x = (float)g->bitmap.width;
            characters[_i].bitmapSize.y = (float)g->bitmap.rows;

            characters[_i].bitmapPos.x = (float)g->bitmap_left;
            characters[_i].bitmapPos.y = (float)g->bitmap_top;

            characters[_i].offset.x = (float)_ox / (float)width;
            characters[_i].offset.y = (float)_oy / (float)height;

            characters[_i].bearing.x = (float)(g->metrics.width - g->metrics.horiBearingX);
            characters[_i].bearing.y = (float)(g->metrics.height - g->metrics.horiBearingY);

            _rowHeight = _rowHeight > g->bitmap.rows ? (int)_rowHeight : (int)g->bitmap.rows;
            _ox += (int)g->bitmap.width + 1;
        }
    }

    Texture& Font::getTexture() {
        return texture;
    }

    CharInfo* Font::getChars() {
        return characters;
    }

    Vec2F Font::getSize() const {
        return {(float)width, (float)height};
    }

    int Font::getFontSize() const {
        return fontSize;
    }

    std::string& Font::getFontName() {
        return fontName;
    }

    std::string& Font::getPath() {
        return originalPath;
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

    Font* FontManager::loadFont(const std::string& _pathToFont, int _fontSize) {
        FT_Face _face;

        SDL_RWops* _fontFile = SDL_RWFromFile(_pathToFont.c_str(), "rb");

        if(_fontFile == nullptr) {
            LOG_E("Couldn't open font file ", _pathToFont)
        }

        off_t fontDataSize = SDL_RWsize(_fontFile);

        auto* fontData = new FT_Byte[fontDataSize];
        SDL_RWseek(_fontFile, 0, RW_SEEK_END);
        SDL_RWseek(_fontFile, 0, RW_SEEK_SET);
        SDL_RWread(_fontFile, &fontData[0], fontDataSize, fontDataSize);
        SDL_RWclose(_fontFile);

        if (FT_New_Memory_Face(ftLibrary, (const FT_Byte*)fontData, (FT_Long)fontDataSize, 0, &_face)) {
            LOG_E("Load memory failed");
            return nullptr;
        }

        auto* _font = new Font();
        _font->init(_face, _fontSize);

        std::string _name = Util::getFileNameFromPath(_pathToFont);
        _font->fontName = _name;
        _font->originalPath = _pathToFont;
        fonts[_name].emplace_back(FontHandler{ _font, _fontSize });

        FT_Done_Face(_face);
        delete [] fontData;

        LOG_S("Successfully loaded Font ", _name, " with font size ", _fontSize)

        return fonts[_name].back().font;
    }

    Font* FontManager::getDefaultFont(const std::string& _fontName) {
        return fonts[_fontName].front().font;
    }

    std::vector<Font*> FontManager::getAllFonts() {
        std::vector<Font*> _fonts;
        for(auto& _fontHandler : fonts)
            for(auto& _font : _fontHandler.second)
                _fonts.push_back(_font.font);

        return _fonts;
    }

    Font* FontManager::getSpecificFont(const std::string& _fontName, int _fontSize) {
        if(fonts.find(_fontName) == fonts.end()) {
            LOG_E("Font ", _fontName, " is not loaded")
            return nullptr;
        }

        for(auto& _fontHandler : fonts[_fontName]) {
            if(_fontHandler.fontSize == _fontSize)
                return _fontHandler.font;
        }

        loadFont(fonts[_fontName].front().font->originalPath, _fontSize);
        LOG_W("Couldn't find Font ", _fontName, " in size ", _fontSize, " so a new Font in that size was created")
        return fonts[_fontName].back().font;
    }

    void FontManager::destroy() {
        LOG_S("Cleaning up FontManager")
        for(auto& _fontHandler : fonts)
            for(auto& _font : _fontHandler.second) {
                LOG_I("     Cleaning Font ", _font.font->fontName, " of size ", _font.font->fontSize)
                delete _font.font;
            }
        FT_Done_FreeType(ftLibrary);
    }
}