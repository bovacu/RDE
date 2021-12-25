// Created by borja on 24/12/21.

#include "Buffer.h"

#include <glad/glad.h>

namespace engine {

    VertexBuffer::VertexBuffer(uint32_t _size) {
        /// First the buffer to store the data is created.
        glCreateBuffers(1, &this->rendererID);

        /// Second, we bind the buffer to be able to work with it.
        glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);

        /// Third, we tell OpenGL what we need to store and the size, but as we don't know the vertices to draw, we need
        /// to use GL_DYNAMIC_DRAW, and that's why we set nullptr.
        glBufferData(GL_ARRAY_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::VertexBuffer(float* _vertices, uint32_t _size) {
        /// First the buffer to store the data is created.
        glCreateBuffers(1, &this->rendererID);

        /// Second, we bind the buffer to be able to work with it.
        glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);

        /// Third, we tell OpenGL what we need to store and the size and as we know the vertices, we pass them and we
        /// set GL_STATIC_DRAW.
        glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer() {
        /// This deletes the stored buffer from memory.
        glDeleteBuffers(1, &this->rendererID);
    }

    void VertexBuffer::bind() const {
        /// This sets this current buffer as the one to be using.
        glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);
    }

    void VertexBuffer::unbind() const {
        /// This removes the buffer as the one to be using.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::setData(const void* _data, uint32_t _size) const {
        /// First we bind the buffer.
        glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);

        /// Then we set the data we want to update. We use BufferSubData as we already previously set the data first.
        glBufferSubData(GL_ARRAY_BUFFER, 0, _size, _data);
    }

    const BufferLayout& VertexBuffer::getLayout() const {
        return layout;
    }

    void VertexBuffer::setLayout(const BufferLayout& _layout) {
        layout = _layout;
    }

    VertexBufferPtr VertexBuffer::create(uint32_t _size) {
        return std::make_shared<VertexBuffer>(_size);
    }

    VertexBufferPtr VertexBuffer::create(float* _vertices, uint32_t _size) {
        return std::make_shared<VertexBuffer>(_vertices, _size);
    }


    // ----------------------------------- INDEX BUFFER ----------------------------------


    IndexBuffer::IndexBuffer(uint32_t* _indices, uint32_t _count)
            : count(_count) {
        /// First the buffer to store the data is created.
        glCreateBuffers(1, &this->rendererID);

        /// GL_ELEMENT_ARRAY_BUFFER is the correct one to use, but it is not valid without an actively bound VAO, so
        /// initially we can't use it until the VAO is built.
        /// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        /// Second, we bind the buffer to be able to work with it.
        glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);

        /// Third, we tell OpenGL what we need to store and the size and as we know the indices, we pass them and we
        /// set GL_STATIC_DRAW.
        glBufferData(GL_ARRAY_BUFFER, _count * sizeof(uint32_t), _indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer() {
        /// This deletes the stored buffer from memory.
        glDeleteBuffers(1, &this->rendererID);
    }

    void IndexBuffer::bind() const {
        /// This sets this current buffer as the one to be using. Now we can use and we use GL_ELEMENT_ARRAY_BUFFER.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->rendererID);
    }

    void IndexBuffer::unbind() const {
        /// This removes the buffer as the one to be using. Now we can use and we use GL_ELEMENT_ARRAY_BUFFER.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    uint32_t IndexBuffer::getCount() const {
        return count;
    }

    IndexBufferPtr IndexBuffer::create(uint32_t* _indices, uint32_t _count) {
        return std::make_shared<IndexBuffer>(_indices, _count);
    }

}
