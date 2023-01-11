//
// Created by borja on 9/5/22.
//

#ifndef RDE_VERTEX_H
#define RDE_VERTEX_H

#include <glm/glm.hpp>

namespace RDE {

    /**
     * @brief This is the data that is sent to the GPU when rendering default vertices.
     */
    struct OpenGLVertex {

        /**
         * @brief Position in screen coordinates where the vertex should be placed.
         */
        glm::vec2 position {0, 0};

        /**
         * @brief Color of the vertex.
         */
        uint32_t color = 0xFFFFFF;

        /**
         * @brief Coordinate of the texture that this vertex has attached to it.
         */
        glm::vec2 texCoord {0, 0};

        /**
         * @brief Any other data that end-users would like to pass to the vertex shader.
         */
        float* extraData = nullptr;

        OpenGLVertex() {  }
        OpenGLVertex(const glm::vec2& _position, const glm::vec2& _texCoord, uint32_t _color, float* _extraData = nullptr) : position(_position), color(_color), texCoord(_texCoord), extraData(_extraData) {  }
    };

    /**
     * @brief This is the data that is sent to the GPU when rendering default debugging information.
     */
    struct OpenGLVertexDebug {

        /**
         * @brief Position in screen coordinates where the vertex should be placed.
         */
        glm::vec2 position {0, 0};

        /**
         * @brief Color of the vertex.
         */
        uint32_t color = 0xFFFFFF;

        OpenGLVertexDebug(const glm::vec2& _position, uint32_t _color) : position(_position), color(_color) {  }
    };

}

#endif //RDE_VERTEX_H
