#include "core/render/elements/Texture.h"
#include "core/util/Logger.h"

#if !IS_MOBILE()
#include <filesystem>
#endif

#include "core/platform/PlatformHeaderSDLImage.h"
#include "core/util/Functions.h"
#include "core/util/GLUtil.h"
#include "core/render/elements/TextureAtlasManager.h"

namespace GDE {

    #define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
    #define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

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
        SDL_RWops* _imageFile = SDL_RWFromFile(_path, "rb");

        if(_imageFile == nullptr) {
            LOG_E("Couldn't load ", _path)
            return false;
        }

        auto* _image = getSDLSurface(_imageFile, _path);
        SDL_RWclose(_imageFile);

        if(_image == nullptr) {
            LOG_E("Format of image '", Util::getFileExtension(_path), "' not supported for file ", _path)
            return false;
        }

        glDeleteTextures(1, &openGLTextureID);

        invertSDLSurface(_image);
        textureSize.x = _image->w;
        textureSize.y = _image->h;
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
            LOG_E("Not supported format image. Channels = ", channels, ", Width = ", textureSize.x, ", Height = ", textureSize.y, ", Path = ", _path)
            LOG_W("If the sprite sheet is exported from TexturePacker remember to set in advanced options PngOpt Level to 0!!")
        }

        internalFormat = _internalFormat;
        dataFormat = _dataFormat;

        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, textureSize.x, textureSize.y, 0, dataFormat, GL_UNSIGNED_BYTE, _texturePixels);

        #if !IS_MAC() && !IS_WINDOWS()
        SDL_FreeSurface(_image);
        #endif

        region = { { 0, 0 }, { textureSize.x , textureSize.y} };

        #if !IS_MOBILE()
        fileSizeKb = (float)std::filesystem::file_size(_path) / 1000.f;
        #endif

        CHECK_GL_ERROR("Texture loadFromFile")

        return true;
    }

    FloatRect& Texture::getRegion() {
        return region;
    }

    float Texture::getKb() const {
        return fileSizeKb;
    }

    bool Texture::loadTextTexture(int _width, int _height) {
        textureSize.x = _width;
        textureSize.y = _height;
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
        CHECK_GL_ERROR("Texture loadTextTexture UNPACK")

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECK_GL_ERROR("Texture loadTextTexture GL_TEXTURE_WRAP_S")
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECK_GL_ERROR("Texture loadTextTexture GL_TEXTURE_WRAP_T")
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECK_GL_ERROR("Texture loadTextTexture GL_TEXTURE_MIN_FILTER")
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_GL_ERROR("Texture loadTextTexture GL_TEXTURE_MAG_FILTER")

        return true;
    }

    bool Texture::loadTextSubTextures(Vec2I _offset, Vec2I _size, const void* _data) {
        #if IS_MOBILE()
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
        #else
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_RED, GL_UNSIGNED_BYTE, _data);
        #endif
        CHECK_GL_ERROR("Texture loadTextSubTextures")
        return true;
    }

    int Texture::invertSDLSurface(SDL_Surface *surface) {
        Uint8 *t;
        Uint8 *a, *b;
        Uint8 *last;
        Uint16 pitch;

        if( SDL_LOCKIFMUST(surface) < 0 )
            return -2;

        /* do nothing unless at least two lines */
        if(surface->h < 2) {
            SDL_UNLOCKIFMUST(surface);
            return 0;
        }

        /* get a place to store a line */
        pitch = surface->pitch;
        t = (Uint8*)malloc(pitch);

        if(t == nullptr) {
            SDL_UNLOCKIFMUST(surface);
            return -2;
        }

        /* get first line; it's about to be trampled */
        memcpy(t,surface->pixels,pitch);

        /* now, shuffle the rest so it's almost correct */
        a = (Uint8*)surface->pixels;
        last = a + pitch * (surface->h - 1);
        b = last;

        while(a < b) {
            memcpy(a,b,pitch);
            a += pitch;
            memcpy(b,a,pitch);
            b -= pitch;
        }

        /* in this shuffled state, the bottom slice is too far down */
        memmove( b, b+pitch, last-b );

        /* now we can put back that first row--in the last place */
        memcpy(last,t,pitch);

        /* everything is in the right place; close up. */
        free(t);
        SDL_UNLOCKIFMUST(surface);

        return 0;
    }

    SDL_Surface* Texture::getSDLSurface(SDL_RWops* _imageFile, const std::string& _pathToFile) {
        auto _extension = Util::getFileExtension(_pathToFile);

        if(strcmp(_extension.c_str(), "png") == 0)
            return IMG_LoadPNG_RW(_imageFile);
        else if(strcmp(_extension.c_str(), "jpg") == 0 || strcmp(_extension.c_str(), "jpeg") == 0)
            return IMG_LoadJPG_RW(_imageFile);
        else if(strcmp(_extension.c_str(), "svg") == 0)
            return IMG_LoadSVG_RW(_imageFile);
        else if(strcmp(_extension.c_str(), "bmp") == 0)
            return IMG_LoadBMP_RW(_imageFile);
        else if(strcmp(_extension.c_str(), "ico") == 0)
            return IMG_LoadICO_RW(_imageFile);

        return nullptr;
    }

    void Texture::checkGLError() {
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR){
            LOG_E("GL_ERROR: ", err)
        }
    }

    bool Texture::loadTextureFromMemory(int _width, int _height, const unsigned char* _data) {
        glDeleteTextures(1, &openGLTextureID);

        textureSize.x = _width;
        textureSize.y = _height;

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


    void Image::init(int _width, int _height, unsigned char* _pixels, const ImageType& _imageType) {
        textureSize.x = _width;
        textureSize.y = _height;
        pixels = _pixels;
        channels = getChannels(_imageType);
        imageType = _imageType;
    }

    void Image::init(int _width, int _height, const ImageType& _imageType) {
        textureSize.x = _width;
        textureSize.y = _height;
        channels = getChannels(_imageType);
        pixels = new unsigned char[_width * _height * channels];
        imageType = _imageType;
    }

    void Image::uploadToGPU() {
        if(openGLTextureID == -1) {
            GLenum _internalFormat = 0, _dataFormat = 0;
            if (channels == 4) {
                _internalFormat = GL_RGBA8;
                _dataFormat = GL_RGBA;
            } else if (channels == 3) {
                _internalFormat = GL_RGB8;
                _dataFormat = GL_RGB;
            } else
                LOG_E("Not supported format image. Channels = ", channels, ", Width = ", textureSize.x, ", Height = ", textureSize.y)

            internalFormat = _internalFormat;
            dataFormat = _dataFormat;

            glGenTextures(1, &openGLTextureID);
            glBindTexture(GL_TEXTURE_2D, openGLTextureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, textureSize.x, textureSize.y, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
        } else {
            // TODO re-upload to GPU of image
            LOG_E("Re-upload to GPU not implemented!!")
        }
    }

    void Image::saveAs(const std::string& _pathToSave) {
        // TODO implement
    }

    void Image::setPixel(int _x, int _y, const Color& _color) {
        pixels[_x * channels + _y * (int)textureSize.x * channels + 0] = _color.r;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 1] = _color.g;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 2] = _color.b;
        pixels[_x * channels + _y * (int)textureSize.x * channels + 3] = _color.a;
    }

    Color Image::getPixel(int _x, int _y) {
        int _base = pixels[_x * channels + _y * (int)textureSize.x * channels];
        return {pixels[_base], pixels[_base + 1], pixels[_base + 2], pixels[_base + 3]};
    }

    Image::~Image() {
        delete [] pixels;
        if(openGLTextureID != -1)
            glDeleteTextures(1, &openGLTextureID);
    }

    int Image::getChannels(const ImageType& _imageType) {
        switch (_imageType) {
            case PNG:
            case BMP: return 4;
            case JPG: return 3;
        }

        return -1;
    }


}

