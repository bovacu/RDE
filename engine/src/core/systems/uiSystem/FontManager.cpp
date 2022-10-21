// Created by borja on 15/1/22.

#include "core/systems/uiSystem/FontManager.h"
#include "core/util/Functions.h"

namespace GDE {

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
            characters[_i].advance.x = (int)g->advance.x >> 6;
            characters[_i].size      = { static_cast<int>(face->glyph->bitmap.width), static_cast<int>(face->glyph->bitmap.rows) };
            characters[_i].bearing   = { face->glyph->bitmap_left, face->glyph->bitmap_top };
            characters[_i].offset    = { (float)_ox / (float)width, (float)_oy / (float)height };
            characters[_i].advance.y = characters[_i].size.y;

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

    void FontManager::init(FileManager* _fileManager) {
        if(FT_Init_FreeType(&ftLibrary)) {
            LOG_E("Error initiating FreeType")
            return;
        }

        loadFont(*_fileManager, "defaultAssets/fonts/MontserratRegular.ttf", 54);
        loadFont(*_fileManager, "defaultAssets/fonts/MontserratItalic.ttf", 54);
        loadFont(*_fileManager, "defaultAssets/fonts/MontserratBold.ttf", 54);
        loadFont(*_fileManager, "defaultAssets/fonts/MontserratBoldItalic.ttf", 54);

        LOG_DEBUG("FontManager loaded successfully")
    }

    Font* FontManager::loadFont(FileManager& _fileManager, const std::string& _pathToFont, int _fontSize) {
        FT_Face _face;
        auto _fileHandler = _fileManager.open(_pathToFont, FileMode::READ);
        auto _data = _fileManager.readFullFile(_fileHandler).content;
        FT_Error _error = FT_New_Memory_Face(ftLibrary, reinterpret_cast<const FT_Byte*>(_data.c_str()), _data.size(), 0, &_face);
        _fileManager.close(_fileHandler);
        if (_error != FT_Err_Ok) {
            LOG_E("Load memory failed with code -> ", _error)
            return nullptr;
        }

        auto* _font = new Font();
        _font->init(_face, _fontSize);

        std::string _name = Util::getFileNameFromPath(_pathToFont);
        _font->fontName = _name;
        _font->originalPath = _pathToFont;
        fonts[_name].emplace_back(FontHandler{ _font, _fontSize });

        FT_Done_Face(_face);

        LOG_DEBUG("Successfully loaded Font ", _name, " with font size ", _fontSize)

        return fonts[_name].back().font;
    }

    Font* FontManager::getDefaultFont(const std::string& _fontName) {
        if(fonts.find(_fontName) == fonts.end()) {
            LOG_E("Font ", _fontName, " is not loaded")
            return nullptr;
        }
        return fonts[_fontName].front().font;
    }

    std::vector<Font*> FontManager::getAllFonts() {
        std::vector<Font*> _fonts;
        for(auto& _fontHandler : fonts)
            for(auto& _font : _fontHandler.second)
                _fonts.push_back(_font.font);

        return _fonts;
    }

    Font* FontManager::getSpecificFont(FileManager& _fileManager, const std::string& _fontName, int _fontSize) {
        if(fonts.find(_fontName) == fonts.end()) {
            LOG_E("Font ", _fontName, " is not loaded, so creating it...")
            return loadFont(_fileManager, _fontName, _fontSize);
        }

        for(auto& _fontHandler : fonts[_fontName]) {
            if(_fontHandler.fontSize == _fontSize || _fontHandler.fontSize - _fontSize <= maxDifferenceBetweenFontSizesBeforeCreatingANewOne)
                return _fontHandler.font;
        }

        loadFont(_fileManager, fonts[_fontName].front().font->originalPath, _fontSize);
        LOG_W("Couldn't find Font ", _fontName, " in size ", _fontSize, " so a new Font in that size was created")
        return fonts[_fontName].back().font;
    }

    void FontManager::destroy() {
        LOG_DEBUG("Cleaning up FontManager")
        for(auto& _fontHandler : fonts)
            for(auto& _font : _fontHandler.second) {
                delete _font.font;
            }
        FT_Done_FreeType(ftLibrary);
    }

    void FontManager::unloadFullFont(const std::string& _fontName) {
        for(auto& _font : fonts[_fontName]) {
            delete _font.font;
        }

        fonts.erase(_fontName);
    }

    void FontManager::unloadSpecificFont(const std::string& _fontName, int _fontSize) {
        for(auto& _font : fonts[_fontName]) {
            if(_font.fontSize == _fontSize) delete _font.font;
        }

        if(fonts[_fontName].empty()) fonts.erase(_fontName);
    }
}