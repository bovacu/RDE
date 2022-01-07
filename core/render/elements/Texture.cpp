#include "Texture.h"
#include "core/util/Logger.h"

namespace engine {

    Texture::Texture(char* _path) {
        loadFromFile(_path);
    }

    Texture::Texture(Texture* _spriteSheet, const IntRect& _region) {
        texture = _spriteSheet->texture;
        region = _region;
        refCount = _spriteSheet->refCount;
        width = _spriteSheet->width;
        height = _spriteSheet->height;
        channels = _spriteSheet->channels;
        internalFormat = _spriteSheet->internalFormat;
        dataFormat = _spriteSheet->dataFormat;
    }

    Texture::~Texture() {
        glDeleteTextures(1, &texture);
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
        return texture;
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


        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        /// Then we tell OpenGL that on this buffer we are storing a 2D texture.
        glTextureStorage2D(texture, 1, internalFormat, width, height);

        /// We set the up/down resizing algorithms
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /// And the up/down wrapping algorithms
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Then we specify the data of the texture with SubImage2D, as we already set the basic information on glTextureStorage2D.
        /// glTextureStorage2D could also be used for this task, but is slower.
        glTextureSubImage2D(texture, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, texturePixels);
        // We can unload the images now that the texture data has been buffered with opengl

        if(_deleteJunkImmediately) {
            stbi_image_free(texturePixels);
            texturePixels = nullptr;
        }

        region = {{0, 0}, {width, height}};

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
}

