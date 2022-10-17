//
// Created by borja on 9/22/22.
//

#include "core/graph/components/UIButton.h"
#include "core/Engine.h"

namespace GDE {


    UIButton::UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config) : UI(_nodeID, _canvas) {
        Texture* _texture = nullptr;
        if(_config.texture == nullptr) {
            _texture = _scene->engine->manager.textureManager.getSubTexture("assets", "buttonDark");
        } else {
            _texture = _config.texture;
        }

        Font* _font = nullptr;
        if(_config.font == nullptr) {
            _font = _scene->engine->manager.fontManager.getDefaultFont("MontserratRegular");
        } else {
            _font = _config.font;
        }

        UI::texture = _texture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, _texture);
        nineSliceSprite->interaction = UI::interaction;

        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_nodeID, _scene, _canvas, _config.text, _font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite->nineSliceSize = _config.size;
        nineSliceSprite->color = _config.color;
    }

    NineSlice& UIButton::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UIButton::getSize() const {
        return nineSliceSprite->getSize();
    }

    void UIButton::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) {

    }
}