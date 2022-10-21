//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UIPanel.h"
#include "core/Engine.h"

namespace GDE {

    static void foo() {

    }

    static void foo2(MouseCode _mouseCode) {

    }

    UIPanel::UIPanel(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIPanelConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.texture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.size;
        UI::interaction->onInnerMouseEntered.bind<&foo>();
        UI::interaction->onInnerMouseExited.bind<&foo>();
        UI::interaction->onInnerClicking.bind<&foo2>();
        UI::interaction->onInnerClickingReleased.bind<&foo2>();

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.texture);
        nineSliceSprite->nineSliceSize = _config.size;
        nineSliceSprite->color = _config.color;

        setConfig(_scene, _config);
    }

    Vec2F UIPanel::getSize() const {
        return UI::getSize();
    }

    UIPanelConfig UIPanel::getConfig() {
        return config;
    }

    void UIPanel::setConfig(Scene* _scene, const UIPanelConfig& _config) {
        config = _config;

        if(config.texture == nullptr) {
            config.texture = _scene->engine->manager.textureManager.getSubTexture("assets", "panel");
        }

        if(config.color == Color::NO_COLOR) {
            config.color = Color(22, 29, 34, 255);
        }

        UI::interaction->sizeOfInteraction = config.size;

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->nineSliceSize = config.size;
            nineSliceSprite->color = config.color;
            nineSliceSprite->texture = config.texture;
        }
    }

    void UIPanel::setInteractable(bool _disabled) {  }

    bool UIPanel::isInteractable() { return false; }

    void UIPanel::setColor(const Color& _color) {
        if(nineSliceSprite != nullptr) {
            nineSliceSprite->color = _color;
        }
    }

    Color UIPanel::getColor() {
        if(nineSliceSprite != nullptr) {
           return  nineSliceSprite->color;
        }

        return Color::White;
    }
}