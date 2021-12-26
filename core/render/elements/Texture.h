#pragma once

#include <iostream>
#include <GL/glew.h>
#include "core/util/Util.h"

namespace engine {
    class Texture {
        private:
            GLuint texture{};
            unsigned int refCount = 0;
            int width, height;

        public:
            explicit Texture(char* filePath);
            ~Texture();
            void incRefCount();
            void decRefCount();
            [[nodiscard]] GLuint getGLTexture() const;
            Vec2I getSize() const;
    };
}