//
// Created by borja on 9/22/22.
//

#include "core/graph/components/UIButton.h"
#include "core/Engine.h"

namespace GDE {


    UIButton::UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture, Font* _font, const std::string& _text) : UI(_scene->getMainGraph()->getComponent<Transform>(_nodeID)) {
        UI::texture = _texture;
        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::texture = _texture;
        UI::batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, _texture);

        auto _textRendererId = _canvas->getGraph()->createNode("Text", _nodeID);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textRendererId, _scene, _font, _text);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
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