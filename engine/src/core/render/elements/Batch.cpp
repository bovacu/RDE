//
// Created by borja on 9/5/22.
//

#include "core/render/elements/Batch.h"
#include "core/render/elements/SpriteBatch.h"
#include "core/render/elements/IRenderizable.h"

namespace GDE {

    void Batch::draw(const IRenderizable* _renderizable, const Transform& _transform) {
        if(indexBuffer.size() + 6 >= spriteBatch->maxIndicesPerDrawCall)
            spriteBatch->flush();

        if(textureID < 0)
            textureID = _renderizable->getTexture();

        _renderizable->draw(vertexBuffer, indexBuffer, _transform, *spriteBatch->viewport);
    }

}