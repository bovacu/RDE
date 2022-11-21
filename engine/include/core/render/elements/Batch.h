//
// Created by borja on 9/5/22.
//

#ifndef RDE_BATCH_H
#define RDE_BATCH_H

#include <vector>
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Shader.h"
#include "core/graph/components/Transform.h"


/**
 * @brief This is the order of rendering of different Renderizable elements.
 */
enum BatchPriority {
    SpritePriority = 0,
    TextPriority = 1
};

namespace RDE {

    FORWARD_DECLARE_CLASS(SpriteBatch, IRenderizable)

    /**
     * @brief This class represents a group of geometry that shares common elements and therefor can be sent all
     * together to the GPU so the number of Draw Calls is minimized as much as possible.
     */
    class Batch {
        FRIEND_CLASS(SpriteBatch, Canvas)

        private:

        /**
         * @see SpriteBatch
         */
        SpriteBatch* spriteBatch = nullptr;

        /**
         * @see BatchPriority
         */
        BatchPriority priority = BatchPriority::SpritePriority;

        int ID = -1;

        /**
         * @brief How far or near it should be rendered on the Z-Axis.
         */
        int layer = 0;

        /**
         * @brief Shader that is going to be used to render the whole batch.
         */
        Shader* shader = nullptr;

        /**
         * @brief Data struct that contains all of the info of the vertices to be sent to the GPU.
         */
        std::vector<OpenGLVertex> vertexBuffer {};

        /**
         * @brief Indices of the vertices to be sent to the GPU. We use indices as it is cheaper to send individual
         * uint32_t values that repeated OpenGLVertex.
         */
        std::vector<uint32_t> indexBuffer {};

        /**
         * @brief Amount of vertices to be sent to the GPU.
         */
        int vertexCount = 0;

        /**
         * @brief Texture to be enabled on the GPU to be drawn by the vertices.
         */
        GLuint textureID = -1;

        bool alreadyDrawnThisFrame = false;

        void draw(IRenderizable* _renderizable, Transform& _transform);
    };

}

#endif //RDE_BATCH_H
