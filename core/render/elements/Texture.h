#pragma once

#include <iostream>
#include <GL/glew.h>
#include "core/util/Util.h"
#include "stb_image.h"

namespace engine {
    class Texture {
        private:
            unsigned int refCount = 0;
            int width = -1, height = -1, channels = -1;
            GLenum internalFormat = -1, dataFormat = -1;
            stbi_uc* texturePixels = nullptr;
            IntRect region {};
            float fileSizeKb = -1;

        public:
            GLuint texture{};

        public:
            Texture() = default;
            explicit Texture(char* filePath);
            Texture(Texture* _spriteSheet, const IntRect& _region);
            ~Texture();
            void incRefCount();
            void decRefCount();
            [[nodiscard]] GLuint getGLTexture() const;
            [[nodiscard]] Vec2I getSize() const;
            float getKb() const;

            bool loadFromFile(const char* _path, bool _deleteJunkImmediately = true);
            bool loadFromMemory(unsigned char* _data, int _size);

            void cleanJunk();
            Color getPixel(uint _x, uint _y);

            IntRect& getRegion();
    };
}