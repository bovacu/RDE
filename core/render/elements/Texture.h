#pragma once

#include <iostream>
#include <GL/glew.h>

namespace engine {
    class Texture
    {
        private:
        GLuint texture;
        unsigned int m_refCount = 0;

        public:
        explicit Texture(char* filePath);
        ~Texture();
        void IncRefCount();
        void DecRefCount();
        GLuint GetGLTexture() const;
    };
}