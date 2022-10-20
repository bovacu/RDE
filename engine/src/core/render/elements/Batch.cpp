//
// Created by borja on 9/5/22.
//

#include "core/render/elements/Batch.h"
#include "core/render/elements/SpriteBatch.h"
#include "core/render/elements/IRenderizable.h"

namespace GDE {

    void Batch::draw(IRenderizable* _renderizable, Transform& _transform) {
        if(textureID < 0)
            textureID = _renderizable->getTexture();

        _renderizable->drawBatched(vertexBuffer, indexBuffer, _transform, *spriteBatch->viewport);
    }

}