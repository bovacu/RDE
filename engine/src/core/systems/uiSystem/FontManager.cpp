// Created by borja on 15/1/22.

#include "core/systems/uiSystem/FontManager.h"
#include "core/util/Functions.h"
#include "core/systems/fileSystem/FileManager.h"

#define FONT_DPI 96

namespace RDE {

    void Font::init(FT_Face face, int _fontSize)  {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //FT_Set_Pixel_Sizes(face, 0, _fontSize);
		FT_Set_Char_Size(face, 0, _fontSize * 64, FONT_DPI, FONT_DPI);
        FT_GlyphSlot g = face->glyph;

        int _rowWidth = 0;
        int _rowHeight = 0;
        atlasSize.x = 0;
        atlasSize.y = 0;
        fontSize = _fontSize;

        memset(characters, 0, sizeof(characters));

		FT_Int32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
        /* Find minimum size for a texture holding all visible ASCII characters */
        for (int _i = 32; _i < 128; _i++) {
			if (auto errorCode = FT_Load_Char(face, _i, load_flags)) {
                Util::Log::error("Loading character ", (char)_i, " failed! Error code ", errorCode);
                continue;
            }
			atlasSize.x += face->glyph->bitmap.width;
            _rowHeight = _rowHeight > g->bitmap.rows ? _rowHeight : (int)g->bitmap.rows;
        }

        atlasSize.y += _rowHeight;

        /* Create a texture that will be used to hold all ASCII glyphs */
        texture.loadTextTexture(atlasSize.x, atlasSize.y);

        /* Paste all glyph bitmaps into the texture, remembering the offset */
        int _ox = 0;
        int _oy = 0;

        _rowHeight = 0;

        for (int _i = 32; _i < MAX_CHARACTERS; _i++) {
			if (auto errorCode = FT_Load_Char(face, _i, load_flags)) {
                Util::Log::error("Loading character ", (char)_i, " failed! Error code ", errorCode);
                continue;
            }

            texture.loadTextSubTextures({_ox, 0}, {(int)g->bitmap.width, (int)g->bitmap.rows}, g->bitmap.buffer);

            characters[_i].advance.x = (int)g->advance.x >> 6;
            characters[_i].size      = { static_cast<int>(face->glyph->bitmap.width), static_cast<int>(face->glyph->bitmap.rows) };
            characters[_i].bearing   = { face->glyph->bitmap_left, face->glyph->bitmap_top };
            characters[_i].offset    = { (float)_ox / (float)atlasSize.x, (float)_oy / (float)atlasSize.y };
            characters[_i].advance.y = characters[_i].size.y;

            _rowHeight = _rowHeight > g->bitmap.rows ? (int)_rowHeight : (int)g->bitmap.rows;
            _ox += (int)g->bitmap.width;

            biggestCharHeight = biggestCharHeight < characters[_i].size.y ? characters[_i].size.y : biggestCharHeight;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    Texture& Font::getTexture() {
        return texture;
    }

    CharInfo* Font::getChars() {
        return characters;
    }

    Vec2F Font::getSize() const {
        return {(float)atlasSize.x, (float)atlasSize.y};
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

    float Font::getBiggestCharHeight() const {
        return biggestCharHeight;
    }


    //----------------------------- FONT MANAGER -----------------

    void FontManager::init(FileManager* _fileManager) {
        if(FT_Init_FreeType(&ftLibrary)) {
            Util::Log::error("Error initiating FreeType");
            return;
        }

        fileManager = _fileManager;

        loadFont(*_fileManager, "defaultAssets/fonts/MontserratRegular.ttf", 54);
        //loadFont(*_fileManager, "defaultAssets/fonts/MontserratItalic.ttf", 54);
        //loadFont(*_fileManager, "defaultAssets/fonts/MontserratBold.ttf", 54);
        //loadFont(*_fileManager, "defaultAssets/fonts/MontserratBoldItalic.ttf", 54);

		Util::Log::debug("FontManager loaded successfully, atlas size: ", getDefaultFont("MontserratRegular")->getSize());
    }

    Font* FontManager::loadFont(FileManager& _fileManager, const std::string& _pathToFont, int _fontSize) {
        FT_Face _face;
		auto _fileHandler = _fileManager.open(_pathToFont, RDE_FILE_MODE_READ);
        auto _data = _fileManager.readFullFile(_fileHandler).content;
        FT_Error _error = FT_New_Memory_Face(ftLibrary, reinterpret_cast<const FT_Byte*>(_data.c_str()), _data.size(), 0, &_face);
        _fileManager.close(_fileHandler);
        if (_error != FT_Err_Ok) {
            Util::Log::error("Load memory failed with code -> ", _error);
            return nullptr;
        }

        auto* _font = new Font();
        _font->init(_face, _fontSize);

        std::string _name = Util::String::getFileNameFromPath(_pathToFont);
        _font->fontName = _name;
        _font->originalPath = _pathToFont;
        fonts[_name].emplace_back(FontHandler{ _font, _fontSize });

        FT_Done_Face(_face);

        Util::Log::debug("Successfully loaded Font ", _name, " with font size ", _fontSize);

        return fonts[_name].back().font;
    }

    Font* FontManager::getDefaultFont(const std::string& _fontName) {
        if(fonts.find(_fontName) == fonts.end()) {
            Util::Log::error("Font ", _fontName, " is not loaded");
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

    Font* FontManager::getSpecificFont(const std::string& _fontName, int _fontSize) {
        if(fonts.find(_fontName) == fonts.end()) {
            Util::Log::error("Font ", _fontName, " is not loaded, so creating it...");
            return loadFont(*fileManager, _fontName, _fontSize);
        }

        for(auto& _fontHandler : fonts[_fontName]) {
            if(_fontHandler.fontSize == _fontSize || std::abs(_fontHandler.fontSize - _fontSize) <= maxDifferenceBetweenFontSizesBeforeCreatingANewOne)
                return _fontHandler.font;
        }

        loadFont(*fileManager, fonts[_fontName].front().font->originalPath, _fontSize);
        Util::Log::warn("Couldn't find Font ", _fontName, " in size ", _fontSize, " so a new Font in that size was created");
        return fonts[_fontName].back().font;
    }

    void FontManager::destroy() {
        Util::Log::debug("Cleaning up FontManager");
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