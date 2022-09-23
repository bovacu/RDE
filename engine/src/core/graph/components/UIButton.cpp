//
// Created by borja on 9/22/22.
//

#include "core/graph/components/UIButton.h"
#include "core/Engine.h"

namespace GDE {


    UIButton::UIButton(const NodeID& _nodeID, Transform* _transform, Canvas* _canvas, Texture* _texture, Font* _font, const std::string& _text) : UI(_transform) {
        UI::texture = _texture;
        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::texture = _texture;
        UI::batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_transform->ID, _transform, _canvas, _texture);

        auto _textRendererId = _canvas->getGraph()->createNode("Text", _transform->ID);
        auto* _textRendererTransform = _canvas->getGraph()->getComponent<Transform>(_textRendererId);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textRendererId, _textRendererTransform, _font, _text);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->pivot = { 0.5f, 0.5f };
    }

    NineSlice& UIButton::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UIButton::getSize() const {
        return nineSliceSprite->getSize();
    }

    void UIButton::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const {

    }
}