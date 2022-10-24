//
// Created by borja on 10/20/22.
//

#ifndef GDE_UI_SLIDER_H
#define GDE_UI_SLIDER_H

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/NineSliceSprite.h"

namespace GDE {

    struct UISliderConfig : CommonUIConfig {
        Texture* backgroundBarTexture = nullptr;
        Color backgroundBarColor = Color::White;
        Vec2F barSize = { 128, 8 };

        Texture* fillingBarTexture = nullptr;
        Color fillingBarColor = Color::Green;
        float percentageFilled = 0.f;

        Texture* handleTexture = nullptr;
        Color handleColor = Color::White;
    };

    class UISlider : public UI {
        private:
            UISliderConfig config;
            bool mouseDown = false;
            NineSliceSprite* backgroundBarSprite;
            NineSliceSprite* fillBarSprite;
            SpriteRenderer* handleSprite;

            Transform* backgroundBarTransform;
            Transform* fillBarTransform;
            Transform* handleTransform;

        public:
            UISlider(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UISliderConfig& _config);
            ~UISlider() {  }

            [[nodiscard]] Vec2F getSize() const override;

            UISliderConfig getConfig();
            void setConfig(Scene* _scene, const UISliderConfig& _config);

            void setInteractable(bool _disabled) override;
            bool isInteractable() override;

            void setColor(const Color& _color) override;
            Color getColor() override;

            void onUpdate(Delta _dt) override;

        private:
            void onMouseClicked(MouseCode _mouseCode);
            void onMouseReleased(MouseCode _mouseCode);
    };

}

#endif //GDE_UI_SLIDER_H
