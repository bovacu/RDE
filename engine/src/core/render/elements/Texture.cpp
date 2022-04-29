#include "core/render/elements/Texture.h"
#include "core/util/Logger.h"

#if !IS_MOBILE()
#include <filesystem>
#endif

#include "core/platform/PlatformHeaderSDLImage.h"
#include "stb/stb_image_writer.h"
#include "core/util/Functions.h"

namespace GDE {

    #define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
    #define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

    Texture::Texture(char* _path) {
        loadFromFile(_path);
    }

    Texture::Texture(Texture* _spriteSheet, const IntRect& _region) {
        openGLTextureID = _spriteSheet->openGLTextureID;
        region = _region;
        refCount = _spriteSheet->refCount;
        width = _spriteSheet->width;
        height = _spriteSheet->height;
        channels = _spriteSheet->channels;
        internalFormat = _spriteSheet->internalFormat;
        dataFormat = _spriteSheet->dataFormat;
        fileSizeKb = _spriteSheet->fileSizeKb;
    }

    Texture::~Texture() {
        glDeleteTextures(1, &openGLTextureID);
    }

    GLuint Texture::getGLTexture() const {
        return openGLTextureID;
    }

    Vec2I Texture::getSize() const {
        return {width, height};
    }

    bool Texture::loadFromFile(const char* _path) {
        SDL_RWops* _imageFile = SDL_RWFromFile(_path, "rb");

        if(_imageFile == nullptr) {
            LOG_E("Couldn't load ", _path)
            return false;
        }

        auto* _image = getSDLSurface(_imageFile, _path);

        if(_image == nullptr) {
            LOG_E("Format of image '", Util::getFileExtension(_path), "' not supported for file ", _path)
            return false;
        }

        invertSDLSurface(_image);
        width = _image->w;
        height = _image->h;
        channels = _image->format->BytesPerPixel;
        auto* _texturePixels = _image->pixels;

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else
            LOG_E("Not supported format image. Channels = ", channels, ", Width = ", width, ", Height = ", height, ", Path = ", _path)

        internalFormat = _internalFormat;
        dataFormat = _dataFormat;

        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, _texturePixels);

        SDL_FreeSurface(_image);
        SDL_RWclose(_imageFile);

        region = {{0, 0}, {width, height}};

        #if !IS_MOBILE()
        fileSizeKb = (float)std::filesystem::file_size(_path) / 1000.f;
        #endif

        return true;
    }

    IntRect& Texture::getRegion() {
        return region;
    }

    float Texture::getKb() const {
        return fileSizeKb;
    }

    bool Texture::loadTextTexture(int _width, int _height) {
        width = _width;
        height = _height;
        fileSizeKb = (float)(_width * _height) / 1024.f;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &openGLTextureID);
        glBindTexture(GL_TEXTURE_2D, openGLTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);

        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        /* Clamping to edges is important to prevent artifacts when scaling */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        /* Linear filtering usually looks best for text */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return true;
    }

    bool Texture::loadTextSubTextures(Vec2I _offset, Vec2I _size, const void* _data) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
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


    // ------------------------------ IMAGE --------------------------


    void Image::init(int _width, int _height, unsigned char* _pixels, const ImageType& _imageType) {
        width = _width;
        height = _height;
        pixels = _pixels;
        channels = getChannels(_imageType);
        imageType = _imageType;
    }

    void Image::init(int _width, int _height, const ImageType& _imageType) {
        width = _width;
        height = _height;
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
                LOG_E("Not supported format image. Channels = ", channels, ", Width = ", width, ", Height = ", height)

            internalFormat = _internalFormat;
            dataFormat = _dataFormat;

            glGenTextures(1, &openGLTextureID);
            glBindTexture(GL_TEXTURE_2D, openGLTextureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
        } else {
            // TODO re-upload to GPU of image
            LOG_E("Re-upload to GPU not implemented!!")
        }
    }

    void Image::saveAs(const std::string& _pathToSave) {
        stbi_flip_vertically_on_write(1);
        switch (imageType) {
            case PNG: stbi_write_png(_pathToSave.c_str(), width, height, channels, pixels, width * channels);
                break;
            case JPG: stbi_write_jpg(_pathToSave.c_str(), width, height, channels, pixels, width * channels);
                break;
            case BMP: stbi_write_bmp(_pathToSave.c_str(), width, height, channels, pixels);
        }
    }

    void Image::setPixel(int _x, int _y, const Color& _color) {
        pixels[_x * channels + _y * width * channels + 0] = _color.r;
        pixels[_x * channels + _y * width * channels + 1] = _color.g;
        pixels[_x * channels + _y * width * channels + 2] = _color.b;
        pixels[_x * channels + _y * width * channels + 3] = _color.a;
    }

    Color Image::getPixel(int _x, int _y) {
        int _base = pixels[_x * channels + _y * width * channels];
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

