// Created by borja on 24/12/21.


#ifndef ENGINE2_0_BUFFER_H
#define ENGINE2_0_BUFFER_H


#include "core/util/Util.h"

namespace engine {

    enum class ShaderDataType {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType _type) {
        switch (_type) {
            case ShaderDataType::Float:    return 4;
            case ShaderDataType::Float2:   return 4 * 2;
            case ShaderDataType::Float3:   return 4 * 3;
            case ShaderDataType::Float4:   return 4 * 4;
            case ShaderDataType::Mat3:     return 4 * 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4 * 4;
            case ShaderDataType::Int:      return 4;
            case ShaderDataType::Int2:     return 4 * 2;
            case ShaderDataType::Int3:     return 4 * 3;
            case ShaderDataType::Int4:     return 4 * 4;
            case ShaderDataType::Bool:     return 1;
            case ShaderDataType::None:
                break;
        }

        ENGINE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement {
        std::string name;
        ShaderDataType type = ShaderDataType::None;
        uint32_t size = 0;
        size_t offset = 0;
        bool normalized = false;

        BufferElement() = default;

        BufferElement(ShaderDataType _type, const std::string& _name, bool _normalized = false)
                : name(_name), type(_type), size(ShaderDataTypeSize(_type)), offset(0), normalized(_normalized) {  }

        uint32_t getComponentCount() const {
            switch (this->type) {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3; // 3* float3
                case ShaderDataType::Mat4:    return 4; // 4* float4
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;
                case ShaderDataType::None:
                    break;
            }

            ENGINE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout {
        private:
        std::vector<BufferElement> elements;
        uint32_t stride = 0;

        public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& _elements)
                : elements(_elements) { this->calculateOffsetsAndStride(); }

        [[nodiscard]] uint32_t getStride() const { return this->stride; }
        [[nodiscard]] const std::vector<BufferElement>& getElements() const { return this->elements; }

        std::vector<BufferElement>::iterator begin() { return this->elements.begin(); }
        std::vector<BufferElement>::iterator end() { return this->elements.end(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return this->elements.begin(); }
        [[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return this->elements.end(); }

        private:
            void calculateOffsetsAndStride() {
                size_t _offset = 0;
                this->stride = 0;
                for (auto& _element : this->elements) {
                    _element.offset = _offset;
                    _offset += _element.size;
                    this->stride += _element.size;
                }
            }
    };

    class VertexBuffer;
    typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

    class VertexBuffer {
        private:
            uint32_t            rendererID = 0;
            BufferLayout        layout;

        public:
            explicit VertexBuffer(uint32_t _size);
            VertexBuffer(float* _vertices, uint32_t _size);
            ~VertexBuffer();

            void bind() const;
            void unbind() const;

            void setData(const void* _data, uint32_t _size) const;

            const BufferLayout& getLayout() const;
            void setLayout(const BufferLayout& _layout);

            static VertexBufferPtr create(uint32_t _size);
            static VertexBufferPtr create(float* _vertices, uint32_t _size);
    };

    // Currently the engine only supports 32-bit index buffers
    class IndexBuffer;
    typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

    class IndexBuffer {
        private:
            uint32_t            rendererID = 0;
            uint32_t            count      = 0;
        public:
            IndexBuffer(uint32_t* _indices, uint32_t _count);
            ~IndexBuffer();

            void bind() const;
            void unbind() const;

            uint32_t getCount() const;

            static IndexBufferPtr create(uint32_t* _indices, uint32_t _count);
    };

}


#endif // ENGINE2_0_BUFFER_H
