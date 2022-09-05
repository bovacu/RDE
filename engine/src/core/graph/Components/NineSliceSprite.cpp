//
// Created by borja on 9/5/22.
//

#include "core/graph/components/NineSliceSprite.h"
#include "core/Engine.h"

namespace GDE {

    NineSliceSprite::NineSliceSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        interaction = _canvas->getGraph()->addComponent<UIInteractable>(_nodeID);
    }

    NineSliceSprite::NineSliceSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        texture = _texture;
        ninePatchSize = _texture->getRegion().size;
        interaction = _canvas->getGraph()->addComponent<UIInteractable>(_nodeID);
    }

    NineSlice& NineSliceSprite::getNineSlice() const {
        return texture->ninePatch;
    }

    void NineSliceSprite::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform) {

    }

}