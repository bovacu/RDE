// Created by borja on 24/12/21.


#ifndef ENGINE2_0_TEXTURE_H
#define ENGINE2_0_TEXTURE_H

#include <glad/glad.h>
#include "core/util/Util.h"

namespace engine {

    class Texture;
    typedef  std::shared_ptr<Texture> TexturePtr;

    class Texture {
        private:
            std::string path;
            uint32_t width = 0, height = 0;
            uint32_t rendererID = 0;
            GLenum internalFormat, dataFormat;
            GLubyte* bufferData = nullptr;
            int  bpp = 0;

        public:
            explicit Texture(const std::string& _path);
            Texture(uint32_t _width, uint32_t _height, bool _useAlpha);
            ~Texture();

            uint32_t getWidth() const;
            uint32_t getHeight() const;

            void setData(void* _data, uint32_t _size);

            void bind(uint32_t _slot = 0) const;
            virtual bool usesAlpha() const;

            void setPixel(int _x, int _y, unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255);
            void setPixel(int _x, int _y, const Color& _color);
            virtual Color getPixel(int _x, int _y) const;

            unsigned char* getBuffer() const;
            void setBuffer(unsigned char* _buffer);

            int getBPP() const;
            uint32_t getRendererID();

            void updateTexture();

            bool operator==(const Texture& _other) const { return this->rendererID == _other.rendererID; }

            bool LoadTextureFromFile(const char* filename, unsigned int* out_texture, int* out_width, int* out_height) {
                return false;
            }

            static TexturePtr create(uint32_t _width, uint32_t _height, bool _useAlpha = false);
            static TexturePtr create(const std::string& _path);
    };
}


#endif //ENGINE2_0_TEXTURE_H
