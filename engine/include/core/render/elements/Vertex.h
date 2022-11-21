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
        glm::vec3 position {0, 0, 0};

        /**
         * @brief Color of the vertex.
         */
        glm::vec4 color { 1, 1, 1, 1 };

        /**
         * @brief Coordinate of the texture that this vertex has attached to it.
         */
        glm::vec2 texCoord {0, 0};

        /**
         * @brief Any other data that end-users would like to pass to the vertex shader.
         */
        float* extraData = nullptr;

        OpenGLVertex() {  }
        OpenGLVertex(const glm::vec3& _position, const glm::vec2& _texCoord, const glm::vec4& _color, float* _extraData = nullptr) : position(_position), color(_color), texCoord(_texCoord), extraData(_extraData) {  }
    };

    /**
     * @brief This is the data that is sent to the GPU when rendering default debugging information.
     */
    struct OpenGLVertexDebug {

        /**
         * @brief Position in screen coordinates where the vertex should be placed.
         */
        glm::vec3 position {0, 0, 0};

        /**
         * @brief Color of the vertex.
         */
        glm::vec4 color {1, 1, 1, 1};

        OpenGLVertexDebug(const glm::vec3& _position, const glm::vec4& _color) : position(_position), color(_color) {  }
    };

}

#endif //RDE_VERTEX_H
