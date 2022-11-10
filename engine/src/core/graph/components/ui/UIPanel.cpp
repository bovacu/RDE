//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UIPanel.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"

namespace GDE {

    static void foo() {

    }

    static void foo2(MouseCode _mouseCode) {

    }

    UIPanel::UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config) : UI(_node) {
        setConfig(_manager, _config);

        UI::texture = config.texture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        ((UITransform*)_node->getTransform())->setSize(config.size);
        UI::interaction->onInnerMouseEntered.bind<&foo>();
        UI::interaction->onInnerMouseExited.bind<&foo>();
        UI::interaction->onInnerClicking.bind<&foo2>();
        UI::interaction->onInnerClickingReleased.bind<&foo2>();

        nineSliceSprite = _node->addComponent<UI9Slice>(config.texture);
        nineSliceSprite->interaction = UI::interaction;
        nineSliceSprite->setSize(config.size);
        nineSliceSprite->setColor(config.color);

        setConfig(_manager, config);
    }

    Vec2F UIPanel::getSize() const {
        return UI::getSize();
    }

    UIPanelConfig UIPanel::getConfig() {
        return config;
    }

    void UIPanel::setConfig(Manager* _manager, const UIPanelConfig& _config) {
        config = _config;

        if(config.texture == nullptr) {
            config.texture = _manager->textureManager.getSubTexture("defaultAssets", "panel");
        }

        if(config.color == Color::NO_COLOR) {
            config.color = Color(22, 29, 34, 255);
        }

        auto* _camera = _manager->sceneManager.getDisplayedScene()->getMainCamera();
        float _aspectRatio = _camera->getAspectRatio();
        bool _isLandscape = _camera->isLandscape();

        if(config.size == -1) {
            config.size = Vec2F { 0.05f * (_isLandscape ? 1.f : _aspectRatio), 0.05f * (_isLandscape ? _aspectRatio : 1.f) };
        }

        auto _parentSize = ((UITransform*)node->getTransform()->parentTransform)->getSize();
        ((UITransform*)node->getTransform())->setSize( {config.size.x * _parentSize.x, config.size.y * _parentSize.y });

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->setSize(config.size);
            nineSliceSprite->setColor(config.color);
            nineSliceSprite->texture = config.texture;
        }
    }

    void UIPanel::setInteractable(bool _interactable) {  }

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