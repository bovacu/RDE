#include "texture.h"
#include "stb_image.h"

namespace engine {
    Texture::Texture(char* _path) {
        int _width, _height, _channels;
        // Load the file.
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* _data;

        /// Then we load the image pixels.
        _data = stbi_load(_path, &_width, &_height, &_channels, 0);

        /// We set the basic information.

        GLenum _internalFormat = 0, _dataFormat = 0;
        if (_channels == 4) {
            _internalFormat = GL_RGBA8;
            _dataFormat = GL_RGBA;
        } else if (_channels == 3) {
            _internalFormat = GL_RGB8;
            _dataFormat = GL_RGB;
        } else {

        }

        GLenum internalFormat = _internalFormat;
        GLenum dataFormat = _dataFormat;


        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        /// Then we tell OpenGL that on this buffer we are storing a 2D texture.
        glTextureStorage2D(texture, 1, internalFormat, _width, _height);

        /// We set the up/down resizing algorithms
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /// And the up/down wrapping algorithms
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Then we specify the data of the texture with SubImage2D, as we already set the basic information on glTextureStorage2D.
        /// glTextureStorage2D could also be used for this task, but is slower.
        glTextureSubImage2D(texture, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, _data);
        // We can unload the images now that the texture data has been buffered with opengl
        stbi_image_free(_data);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &texture);
    }

    void Texture::IncRefCount() {
        m_refCount++;
    }

    void Texture::DecRefCount() {
        m_refCount--;
        if (m_refCount == 0)
        {
            delete this;
        }
    }

    GLuint Texture::GetGLTexture() const {
        return texture;
    }
}

