#pragma once

#include <iostream>
#include <GL/glew.h>
#include "core/util/Util.h"
#include "stb_image.h"

namespace engine {
    class Texture {
        private:
            GLuint texture{};
            unsigned int refCount = 0;
            int width, height, channels;
            GLenum internalFormat, dataFormat;
            stbi_uc* texturePixels;
            IntRect region;

        public:
            Texture() = default;
            explicit Texture(char* filePath);
            Texture(Texture* _spriteSheet, const IntRect& _region);
            ~Texture();
            void incRefCount();
            void decRefCount();
            [[nodiscard]] GLuint getGLTexture() const;
            [[nodiscard]] Vec2I getSize() const;

            bool loadFromFile(const char* _path, bool _deleteJunkImmediately = true);

            void cleanJunk();
            Color getPixel(uint _x, uint _y);
    };
}