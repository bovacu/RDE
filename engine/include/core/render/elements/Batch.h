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
enum RDE_BATCH_PRIORITY_ {
	RDE_BATCH_PRIORITY_SPRITE = 0,
	RDE_BATCH_PRIORITY_TEXT = 1
};

namespace RDE {

    class SpriteBatch;

    /**
     * @brief This class represents a group of geometry that shares common elements and therefor can be sent all
     * together to the GPU so the number of Draw Calls is minimized as much as possible.
     */
    class Batch {
        friend class SpriteBatch;
        friend class Canvas;

        public:
            /**
             * @see SpriteBatch
             */
            SpriteBatch* spriteBatch = nullptr;

            /**
             * @see BatchPriority
             */
			RDE_BATCH_PRIORITY_ priority = RDE_BATCH_PRIORITY_SPRITE;

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
             * @brief Amount of vertices to be sent to the GPU.
             */
            int vertexCount = 0;

            /**
             * @brief Texture to be enabled on the GPU to be drawn by the vertices.
             */
            GLuint textureID = -1;

            bool alreadyDrawnThisFrame = false;
    };

}

#endif //RDE_BATCH_H
