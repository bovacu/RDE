// Created by borja on 24/12/21.

#include "VertexArray.h"
#include <glad/glad.h>

namespace engine {

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType _type) {
        switch (_type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:     return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:     return GL_INT;
            case ShaderDataType::Bool:     return GL_BOOL;
            case ShaderDataType::None:
                break;
        }

        ENGINE_ASSERT(false, "Unknown ShaderDataType!")
        return 0;
    }

    VertexArray::VertexArray() {
        /// First, we need to create the buffer in memory for the VAO.
        glCreateVertexArrays(1, &this->rendererID);
    }

    VertexArray::~VertexArray() {
        /// This removes the VAO from memory.
        glDeleteVertexArrays(1, &this->rendererID);
    }

    void VertexArray::bind() const {
        /// Selects this buffer as the one to work with.
        glBindVertexArray(this->rendererID);
    }

    void VertexArray::unbind() const {
        /// Removes this buffer as the one to work with.
        glBindVertexArray(0);
    }

    void VertexArray::addVertexBuffer(const VertexBufferPtr& _vertexBuffer) {
        /// First, the VertexBuffer MUST have a valid layout.
        ENGINE_ASSERT(_vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!")

        /// Then we bind our VAO.
        glBindVertexArray(this->rendererID);
        _vertexBuffer->bind();

        /// For each element of the layout, we set the proper data.
        const auto& _layout = _vertexBuffer->getLayout();
        for (const auto& _element : _layout) {
            switch (_element.type) {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool : {
                    /// This activates the possibility to set the attributes for and specific VB.
                    glEnableVertexAttribArray(this->vertexBufferIndex);

                    /// This method allows us to set the VB layout in a way OpenGL understands.
                    glVertexAttribPointer(this->vertexBufferIndex,
                                          _element.getComponentCount(),
                                          ShaderDataTypeToOpenGLBaseType(_element.type),
                                          _element.normalized ? GL_TRUE : GL_FALSE,
                                          _layout.getStride(),
                                          (const void*)_element.offset);
                    this->vertexBufferIndex++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4 : {
                    /// Same as before, but with multiple values.
                    uint8_t _count = _element.getComponentCount();
                    for (uint8_t _i = 0; _i < _count; _i++) {
                        glEnableVertexAttribArray(this->vertexBufferIndex);
                        glVertexAttribPointer(this->vertexBufferIndex,
                                              _count,
                                              ShaderDataTypeToOpenGLBaseType(_element.type),
                                              _element.normalized ? GL_TRUE : GL_FALSE,
                                              _layout.getStride(),
                                              (const void*)(_element.offset + sizeof(float) * _count * _i));

                        /// This marks a separation between one row or column of data.
                        glVertexAttribDivisor(this->vertexBufferIndex, 1);
                        this->vertexBufferIndex++;
                    }
                    break;
                }
                default: {}
                    ENGINE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }

        this->vertexBuffers.push_back(_vertexBuffer);
    }

    void VertexArray::setIndexBuffer(const IndexBufferPtr& _indexBuffer) {
        /// We need to set the IB to be able to draw correctly.
        glBindVertexArray(this->rendererID);
        _indexBuffer->bind();

        this->indexBuffer = _indexBuffer;
    }

    VertexArrayPtr VertexArray::create() {
        return std::make_shared<VertexArray>();
    }

    const std::vector<VertexBufferPtr>& VertexArray::getVertexBuffers() const {
        return vertexBuffers;
    }

    const IndexBufferPtr& VertexArray::getIndexBuffer() const {
        return indexBuffer;
    }

}