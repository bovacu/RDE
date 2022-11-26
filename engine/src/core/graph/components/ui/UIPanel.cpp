//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UIPanel.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIImage.h"

namespace RDE {

    static void foo() {

    }

    static void foo2(MouseCode _mouseCode) {

    }

    UIPanel::UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config) : UI(_node, &_config) {
        UI::texture = _config.texture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "panel") :
                      _config.texture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        ((UITransform*)_node->getTransform())->setSize(_config.size);
        UI::interaction->onInnerMouseEntered.bind<&foo>();
        UI::interaction->onInnerMouseExited.bind<&foo>();
        UI::interaction->onInnerClicking.bind<&foo2>();
        UI::interaction->onInnerClickingReleased.bind<&foo2>();

        nineSliceSprite = _node->addComponent<UIImage>(UIImageConfig {
            .size = UI::getSize(),
            .texture = UI::texture,
            .color = _config.color,
            .imageRenderingType = ImageRenderingType::NINE_SLICE
        });
        nineSliceSprite->interaction = UI::interaction;
    }

    Vec2F UIPanel::getSize() const {
        return UI::getSize();
    }

    void UIPanel::setInteractable(bool _interactable) {  }

    bool UIPanel::isInteractable() { return false; }

    void UIPanel::setColor(const Color& _color) {
        if(nineSliceSprite != nullptr) {
            nineSliceSprite->setColor(_color);
        }
    }

    Color UIPanel::getColor() {
        if(nineSliceSprite != nullptr) {
           return nineSliceSprite->getColor();
        }

        return Color::White;
    }
}