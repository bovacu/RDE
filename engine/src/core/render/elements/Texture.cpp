#include "core/render/elements/Texture.h"

#if !IS_MOBILE()
#include <filesystem>
#endif

#include "core/platform/PlatformHeaderSDLImage.h"
#include "core/render/elements/TextureAtlasManager.h"

namespace RDE {

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 rmask = 0xff000000;
        Uint32 gmask = 0x00ff0000;
        Uint32 bmask = 0x0000ff00;
        Uint32 amask = 0x000000ff;  
    #else
        Uint32 rmask = 0x000000ff;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x00ff0000;
        Uint32 amask = 0xff000000;
    #endif


    Texture::Texture(char* _path) {
        path = std::string(_path);
        loadFromFile(_path);
    }

    Texture::Texture(Atlas* _spriteSheet, const FloatRect& _region) {
        openGLTextureID = _spriteSheet->texture->openGLTextureID;
        region = _region;
        textureSize = _region.size;
        spriteSheetSize = { (int)_spriteSheet->textureWidth, (int)_spriteSheet->textureHeight };
        channels = _spriteSheet->texture->channels;
        internalFormat = _spriteSheet->texture->internalFormat;
        dataFormat = _spriteSheet->texture->dataFormat;
        fileSizeKb = _spriteSheet->texture->fileSizeKb;
    }

    Texture::~Texture() {
        glDeleteTextures(1, &openGLTextureID);
    }

    GLuint Texture::getGLTexture() const {
        return openGLTextureID;
    }

    Vec2F Texture::getSize() const {
        return textureSize;
    }

    Vec2I Texture::getSpriteSheetSize() const {
        return spriteSheetSize;
    }

    bool Texture::loadFromFile(const char* _path) {
        auto* _image = Util::Texture::getSDLSurface(_path);

        if(_image == nullptr) {
            Util::Log::error("Format of image '", Util::String::getFileExtension(_path), "' not supported for file ", _path);
            return false;
        }

        glDeleteTextures(1, &openGLTextureID);

        Util::Texture::invertSDLSurface(_image);
        textureSize.x = (float)_image->w;
        textureSize.y = (float)_image->h;
        channels = _image->format->BytesPerPixel;
        auto* _texturePixels = _image->pixels;

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else {
            Util::Log::error("Not supported format image. Channels = ", channels, ", Width = ", textureSize.x, ", Height = ", textureSize.y, ", Path = ", _path);
            Util::Log::warn("If the sprite sheet is exported from TexturePacker remember to set in advanced options PngOpt Level to 0!!");
        }

        internalFormat = _internalFormat;
        dataFormat = _dataFormat;

        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, (int)textureSize.x, (int)textureSize.y, 0, dataFormat, GL_UNSIGNED_BYTE, _texturePixels);

        #if !IS_MAC() && !IS_WINDOWS()
        SDL_FreeSurface(_image);
        #endif

        region = { { 0, 0 }, { textureSize.x , textureSize.y} };

        #if !IS_MOBILE()
        fileSizeKb = (float)std::filesystem::file_size(_path) / 1000.f;
        #endif

        Util::GL::checkError("Texture loadFromFile");

        return true;
    }

    FloatRect& Texture::getRegion() {
        return region;
    }

    float Texture::getKb() const {
        return fileSizeKb;
    }

    bool Texture::loadTextTexture(int _width, int _height) {
        textureSize.x = (float)_width;
        textureSize.y = (float)_height;
        fileSizeKb = (float)(_width * _height) / 1024.f;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        #if IS_MOBILE()
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, textureSize.x, textureSize.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
        #else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        #endif
        
        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        Util::GL::checkError("Texture loadTextTexture UNPACK");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        Util::GL::checkError("Texture loadTextTexture GL_TEXTURE_WRAP_S");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        Util::GL::checkError("Texture loadTextTexture GL_TEXTURE_WRAP_T");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        Util::GL::checkError("Texture loadTextTexture GL_TEXTURE_MIN_FILTER");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Util::GL::checkError("Texture loadTextTexture GL_TEXTURE_MAG_FILTER");

        return true;
    }

    bool Texture::loadTextSubTextures(Vec2I _offset, Vec2I _size, const void* _data) {
        #if IS_MOBILE()
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
        #else
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_RED, GL_UNSIGNED_BYTE, _data);
        #endif
        Util::GL::checkError("Texture loadTextSubTextures");
        return true;
    }

    bool Texture::loadTextureFromMemory(int _width, int _height, const unsigned char* _data) {
        glDeleteTextures(1, &openGLTextureID);

        textureSize.x = (float)_width;
        textureSize.y = (float)_height;

        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        GLint internal_format = GL_RGBA8;
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        region = {{0, 0}, {textureSize.x, textureSize.y}};

        return true;
    }

    std::string Texture::getPath() {
        return path;
    }

    void Texture::setRegion(const FloatRect& _region) {
        region = _region;
    }


    // ------------------------------ IMAGE --------------------------


    void CPUTexture::init(int _width, int _height, unsigned char* _pixels, const ImageType& _imageType) {
        textureSize.x = (float)_width;
        textureSize.y = (float)_height;
        pixels = _pixels;
        channels = getChannels(_imageType);
        imageType = _imageType;
        openGLTextureID = -1;
        region = { { 0, 0 }, { textureSize.x , textureSize.y} };
        spriteSheetSize = { (int)textureSize.x, (int)textureSize.y };
        uploadToGPU();
    }

    void CPUTexture::init(int _width, int _height, const ImageType& _imageType) {
        textureSize.x = (float)_width;
        textureSize.y = (float)_height;
        channels = getChannels(_imageType);
        pixels = new unsigned char[_width * _height * channels];
        for(auto _i = 0; _i < _height; _i++) {
            for(auto _j = 0; _j < _width; _j++) {
                setPixel(_j, _i, Color::Magenta);
            }
        }
        imageType = _imageType;
        openGLTextureID = -1;
        region = { { 0, 0 }, { textureSize.x , textureSize.y} };
        spriteSheetSize = { (int)textureSize.x, (int)textureSize.y };
        uploadToGPU();
    }

    void CPUTexture::uploadToGPU() {
        if(!dirty) return;

        if(openGLTextureID == -1) {
            GLenum _internalFormat = 0, _dataFormat = 0;
            if (channels == 4) {
                _internalFormat = GL_RGBA8;
                _dataFormat = GL_RGBA;
            } else if (channels == 3) {
                _internalFormat = GL_RGB8;
                _dataFormat = GL_RGB;
            } else
                Util::Log::error("Not supported format image. Channels = ", channels, ", Width = ", textureSize.x, ", Height = ", textureSize.y);

            internalFormat = _internalFormat;
            dataFormat = _dataFormat;

            glGenTextures(1, &openGLTextureID);
            glBindTexture(GL_TEXTURE_2D, openGLTextureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, (int)textureSize.x, (int)textureSize.y, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
        } else {
            glBindTexture(GL_TEXTURE_2D, openGLTextureID);
            glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, (int)textureSize.x, (int)textureSize.y, dataFormat, GL_UNSIGNED_BYTE, pixels);
        }
    }

    void CPUTexture::saveAs(const std::string& _pathToSave) {
        SDL_Surface* _surface = nullptr;

        switch (imageType) {
            case PNG: {
                _surface = SDL_CreateRGBSurfaceFrom(pixels, textureSize.x, textureSize.y, 32, 4, rmask, gmask, bmask, amask);
                IMG_SavePNG(_surface, _pathToSave.c_str());
                break;
            }
            case JPG: {
                _surface = SDL_CreateRGBSurfaceFrom(pixels, textureSize.x, textureSize.y, 32, 3, rmask, gmask, bmask, amask);
                IMG_SaveJPG(_surface, _pathToSave.c_str(), 100);
                break;
            }
            case BMP:
                _surface = SDL_CreateRGBSurfaceFrom(pixels, textureSize.x, textureSize.y, 32, 3, rmask, gmask, bmask, amask);
                SDL_SaveBMP(_surface, _pathToSave.c_str());
                break;
            }

        if(_surface) {
            SDL_FreeSurface(_surface);
        }
    }

    void CPUTexture::setPixel(int _x, int _y, const Color& _color) {
        pixels[_x * channels + _y * (int)textureSize.x * channels + 0] = _color.r;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 1] = _color.g;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 2] = _color.b;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 3] = _color.a;
        dirty = true;
    }

    Color CPUTexture::getPixel(int _x, int _y) {
        int _base = pixels[_x * channels + _y * (int)textureSize.x * channels];
        return {pixels[_base], pixels[_base + 1], pixels[_base + 2], pixels[_base + 3]};
    }

    CPUTexture::~CPUTexture() {
        delete [] pixels;
        if(openGLTextureID != -1)
            glDeleteTextures(1, &openGLTextureID);
    }

    int CPUTexture::getChannels(const ImageType& _imageType) {
        switch (_imageType) {
            case PNG:
            case BMP: return 4;
            case JPG: return 3;
        }

        return -1;
    }

    void CPUTexture::resizeImage(const Vec2<uint>& _newSize) {
        Util::Log::warn("Resizing images is not implemented yet!");
        dirty = true;
    }


}

