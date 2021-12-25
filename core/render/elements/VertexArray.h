// Created by borja on 24/12/21.


#ifndef ENGINE2_0_VERTEX_ARRAY_H
#define ENGINE2_0_VERTEX_ARRAY_H


#include "core/util/Util.h"
#include "Buffer.h"

namespace engine {

    class VertexArray;
    typedef std::shared_ptr<VertexArray> VertexArrayPtr;

    class VertexArray {
        private:
            uint32_t                        rendererID = 0;
            uint32_t                        vertexBufferIndex = 0;
            std::vector<VertexBufferPtr>    vertexBuffers;
            IndexBufferPtr                  indexBuffer;

        public:
            VertexArray();
            ~VertexArray();

            void bind() const;
            void unbind() const;

            void addVertexBuffer(const VertexBufferPtr& vertexBuffer);
            void setIndexBuffer(const IndexBufferPtr& indexBuffer);

            const std::vector<VertexBufferPtr>& getVertexBuffers() const;
            const IndexBufferPtr& getIndexBuffer() const;

        static std::shared_ptr<VertexArray> create();
    };

}


#endif //ENGINE2_0_VERTEX_ARRAY_H
