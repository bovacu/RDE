#include "Texture.h"
#include "core/util/Logger.h"
#include <filesystem>

namespace engine {

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

    void Texture::incRefCount() {
        refCount++;
    }

    void Texture::decRefCount() {
        refCount--;
        if (refCount == 0)
            delete this;
    }

    GLuint Texture::getGLTexture() const {
        return openGLTextureID;
    }

    Vec2I Texture::getSize() const {
        return {width, height};
    }

    bool Texture::loadFromFile(const char* _path, bool _deleteJunkImmediately) {
        // Load the file.
        stbi_set_flip_vertically_on_load(1);

        /// Then we load the image pixels.
        texturePixels = stbi_load(_path, &width, &height, &channels, 0);

        /// We set the basic information.

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else {
            LOG_E("Not supported format image")
        }

        internalFormat = _internalFormat;
        dataFormat = _dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &openGLTextureID);

        /// Then we tell OpenGL that on this buffer we are storing a 2D texture.
        glTextureStorage2D(openGLTextureID, 1, internalFormat, width, height);

        /// We set the up/down resizing algorithms
        glTextureParameteri(openGLTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(openGLTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /// And the up/down wrapping algorithms
        glTextureParameteri(openGLTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(openGLTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Then we specify the data of the texture with SubImage2D, as we already set the basic information on glTextureStorage2D.
        /// glTextureStorage2D could also be used for this task, but is slower.
        glTextureSubImage2D(openGLTextureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, texturePixels);
        // We can unload the images now that the texture data has been buffered with opengl

        if(_deleteJunkImmediately) {
            stbi_image_free(texturePixels);
            texturePixels = nullptr;
        }

        region = {{0, 0}, {width, height}};
        fileSizeKb = (float)std::filesystem::file_size(_path) / 1000.f;
        return true;
    }

    bool Texture::loadFromMemory(unsigned char* _data, int _size) {
        /// Then we load the image pixels.
        texturePixels = _data;
        channels = 4;
        width = height = (int)std::sqrt(_size / 4);

        /// We set the basic information.

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else {
            LOG_E("Not supported format image, channels: ", channels)
        }

        internalFormat = _internalFormat;
        dataFormat = _dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &openGLTextureID);

        /// Then we tell OpenGL that on this buffer we are storing a 2D texture.
        glTextureStorage2D(openGLTextureID, 1, internalFormat, width, height);

        /// We set the up/down resizing algorithms
        glTextureParameteri(openGLTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(openGLTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /// And the up/down wrapping algorithms
        glTextureParameteri(openGLTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(openGLTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Then we specify the data of the texture with SubImage2D, as we already set the basic information on glTextureStorage2D.
        /// glTextureStorage2D could also be used for this task, but is slower.
        glTextureSubImage2D(openGLTextureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, texturePixels);
        // We can unload the images now that the texture data has been buffered with opengl

        region = {{0, 0}, {width, height}};
        fileSizeKb = (float)_size / 1024.f;
        return true;
    }

    void Texture::cleanJunk() {
        if(!texturePixels)
            stbi_image_free(texturePixels);
        texturePixels = nullptr;
    }

    Color Texture::getPixel(uint _x, uint _y) {
        if(!texturePixels) {
            LOG_E("Tried to access the data from a texture that already deleted it!!")
            return Color::Transparent;
        }

        unsigned bytePerPixel = channels;
        unsigned char* pixelOffset = texturePixels + (_x + height * _y) * bytePerPixel;
        unsigned char _r = texturePixels[channels * (_y * width + _x) + 0];
        unsigned char _g = texturePixels[channels * (_y * width + _x) + 1];
        unsigned char _b = texturePixels[channels * (_y * width + _x) + 2];
        unsigned char _a = channels >= 4 ? texturePixels[4 * (_y * width + _x) + 3] : 0xff;

        return Color { _r, _g, _b, _a };
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

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

    bool Texture::loadTextSubTextures(Vec2I _offset, Vec2I _size, const unsigned char* _data) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, _offset.x, _offset.y, _size.x, _size.y, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
        return true;
    }
}

