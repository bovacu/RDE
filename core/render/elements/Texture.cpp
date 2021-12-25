// Created by borja on 24/12/21.

#include "Texture.h"
#include "stb_image.h"

namespace engine {

    Texture::Texture(uint32_t _width, uint32_t _height, bool _useAlpha) : width(_width), height(_height) {
        /// This constructor is to create a texture and modify it 100% through code.
        this->bpp = _useAlpha ? 4 : 3;
        this->internalFormat = _useAlpha ? GL_RGBA8 : GL_RGB8;
        this->dataFormat = _useAlpha ? GL_RGBA : GL_RGB;

        /// Same process as the below constructor.
        glCreateTextures(GL_TEXTURE_2D, 1, &this->rendererID);
        glTextureStorage2D(this->rendererID, 1, this->internalFormat, this->width, this->height);

        glTextureParameteri(this->rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(this->rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(this->rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(this->rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// In this case we store in memory the data.
        this->bufferData = new GLubyte[_width * _height * (_useAlpha ? 4 : 3)];
    }

    Texture::Texture(const std::string& _path) : path(_path) {
        int _width, _height, _channels;

        /// As for how stb_img loads the image and how OpenGL renders them, we need to flip it vertically.
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* _data;

        /// Then we load the image pixels.
        _data = stbi_load(_path.c_str(), &_width, &_height, &_channels, 0);

        /// We set the basic information.
        ENGINE_ASSERT(_data, "Failed to load image!")
        this->width = _width;
        this->height = _height;
        this->bpp = _channels;

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (_channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (_channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else {
            LOG_E("Images must have 3 or 4 channels! Image's channels not supported");
        }

        this->internalFormat = _internalFormat;
        this->dataFormat = _dataFormat;

        /// Check if the image format is or not supported.
        ENGINE_ASSERT(_internalFormat & _dataFormat, "Format not supported!")

        /// With the data of the image loaded, we can proceed to create the memory buffer.
        glCreateTextures(GL_TEXTURE_2D, 1, &this->rendererID);

        /// Then we tell OpenGL that on this buffer we are storing a 2D texture.
        glTextureStorage2D(this->rendererID, 1, this->internalFormat, this->width, this->height);

        /// We set the up/down resizing algorithms
        glTextureParameteri(this->rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(this->rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /// And the up/down wrapping algorithms
        glTextureParameteri(this->rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(this->rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Then we specify the data of the texture with SubImage2D, as we already set the basic information on glTextureStorage2D.
        /// glTextureStorage2D could also be used for this task, but is slower.
        glTextureSubImage2D(this->rendererID, 0, 0, 0, this->width, this->height, this->dataFormat, GL_UNSIGNED_BYTE, _data);

        /// Free the memory not needed anymore.
        stbi_image_free(_data);
    }

    Texture::~Texture() {
        /// Freeing the texture buffer.
        glDeleteTextures(1, &this->rendererID);
        delete this->bufferData;
    }

    void Texture::setData(void* _data, uint32_t _size) {
        uint32_t _bpp = this->dataFormat == GL_RGBA ? 4 : 3;
        ENGINE_ASSERT(_size == this->width * this->height * _bpp, "Data must be entire texture!")
        glTextureSubImage2D(this->rendererID, 0, 0, 0, this->width, this->height, this->dataFormat, GL_UNSIGNED_BYTE, _data);
    }

    void Texture::bind(uint32_t _slot) const {
        glBindTextureUnit(_slot, this->rendererID);
    }

    bool Texture::usesAlpha() const {
        return this->bpp == 4;
    }

    void Texture::setPixel(int _x, int _y, unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) {
        int _pixel = this->bpp * _x + (this->bpp * (int)this->width * _y);
        this->bufferData[_pixel] = _r;
        this->bufferData[_pixel + 1] = _g;
        this->bufferData[_pixel + 2] = _b;
        if(this->bpp == 4) this->bufferData[_pixel + 3] = _a;
    }

    void Texture::setPixel(int _x, int _y, const Color& _color) {
        this->setPixel(_x, _y, _color.r, _color.g, _color.b, _color.a);
    }

    Color Texture::getPixel(int _x, int _y) const {
        Color _color;
        int _pixel = this->bpp * _x + (this->bpp * (int)this->width * _y);
        _color.r = this->bufferData[_pixel];
        _color.g = this->bufferData[_pixel + 1];
        _color.b = this->bufferData[_pixel + 2];
        if(usesAlpha()) _color.a = this->bufferData[_pixel + 3];
        return _color;
    }

    void Texture::updateTexture() {
        /// If the pixels are modified in a single way, then the texture needs to be reloaded.
        glTextureSubImage2D(this->rendererID, 0, 0, 0, this->width, this->height, this->dataFormat, GL_UNSIGNED_BYTE, this->bufferData);
    }

    unsigned char* Texture::getBuffer() const {
        return bufferData;
    }

    void Texture::setBuffer(unsigned char* _buffer) {
        bufferData = _buffer;
    }

    int Texture::getBPP() const {
        return bpp;
    }

    uint32_t Texture::getRendererID() {
        return rendererID;
    }

    TexturePtr Texture::create(uint32_t _width, uint32_t _height, bool _useAlpha) {
        return std::make_shared<Texture>(_width, _height, _useAlpha);
    }

    TexturePtr Texture::create(const std::string& _path) {
        return std::make_shared<Texture>(_path);
    }

    uint32_t Texture::getWidth() const {
        return width;
    }

    uint32_t Texture::getHeight() const {
        return height;
    }
}
