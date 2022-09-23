//
// Created by borja on 9/22/22.
//

#include "core/graph/components/UIButton.h"
#include "core/Engine.h"

namespace GDE {


    UIButton::UIButton(const NodeID& _nodeID, Transform* _transform, Scene* _scene, Canvas* _canvas, Texture* _texture, Font* _font, const std::string& _text) : UI(_transform) {
        UI::texture = _texture;
        UI::shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        UI::texture = _texture;
        UI::batchPriority = BatchPriority::SpritePriority;

        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_transform->ID, _transform, _scene, _font, _text);
        textRenderer->batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_transform->ID, _transform, _scene, _canvas, _texture);
    }

    NineSlice& UIButton::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UIButton::getSize() const {
        return nineSliceSprite->getSize();
    }

    void UIButton::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const {
        nineSliceSprite->draw(_vertices, _indices, _transform, _viewport);
        textRenderer->draw(_vertices, _indices, _transform, _viewport);
    }
}