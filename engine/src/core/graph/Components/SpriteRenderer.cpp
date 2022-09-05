//
// Created by borja on 9/5/22.
//

#include "core/graph/components/SpriteRenderer.h"
#include "core/render/Camera.h"
#include "core/util/Functions.h"

namespace GDE {

    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Scene* _scene, Texture* _texture) : texture(_texture) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        viewport = _scene->getMainCamera()->getViewport();
    }

    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Scene* _scene) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        viewport = _scene->getMainCamera()->getViewport();
    }

    std::string SpriteRenderer::getTexturePath() {
        return texture->getPath();
    }

    std::string SpriteRenderer::getTextureName() {
        return Util::getFileNameFromPath(texture->getPath());
    }

    std::string SpriteRenderer::getTextureExtension() {
        return Util::getFileExtension(texture->getPath());
    }

    void SpriteRenderer::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform) {

    }

}